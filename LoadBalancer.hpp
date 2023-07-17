#include <functional>
#include <tuple>
#include <vector>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
template<class Yin, class Yang, class Key>
struct Loadbalancer {
    typedef std::function<void(const Yin&)> YinFunc;
    typedef std::function<void(const Yang&)> YangFunc;
    typedef std::tuple<YinFunc, YangFunc> ListenerFunctions;
    Loadbalancer(const size_t& numPartitions,
                std::function<Key(const Yin&)> keyFromYin,
                std::function<Key(const Yang&)> keyFromYang,
                std::function<Yin(const Key&)> yinFromKey,
                std::function<Yang(const Key&)> yangFromKey,
                std::function<size_t(const Key&)> keyGen)
        : m_numPartitions(numPartitions),
          m_activeKeys(numPartitions),
          m_partitionToListener(numPartitions, 0),
          m_listenerIdToListener(),
          m_keyFromYin(keyFromYin), m_keyFromYang(keyFromYang),
          m_yinFromKey(yinFromKey), m_yangFromKey(yangFromKey), m_keyGen(keyGen) {}

    void onData(const Yin& yin) {
        Key key = m_keyFromYin(yin);
        size_t partition = m_keyGen(key) % m_numPartitions;
        m_activeKeys[partition].insert(key);
        if (m_listenerIdToListener.empty()) {
            return;
        }

        const auto& [yinFunc, yangFunc] = m_listenerIdToListener[m_partitionToListener[partition]];
        yinFunc(yin);
    }

    void onData(const Yang& yang) {
        Key key = m_keyFromYang(yang);
        size_t partition = m_keyGen(key) % m_numPartitions;
        m_activeKeys[partition].erase(key);
        if (m_listenerIdToListener.empty()) {
            return;
        }

        const auto& [yinFunc, yangFunc] = m_listenerIdToListener[m_partitionToListener[partition]];
        yinFunc(yang);
    }

    bool onListenerAdded(const size_t& listenerId, const ListenerFunctions& listenerFunctions) {       
        if (0 == m_listenerIdToListener.size()) {
            const auto onFirstListenerAdded  = [this](const size_t& listenerId, const ListenerFunctions& listenerFunctions) {
                m_listenerIdToListener[listenerId] = listenerFunctions;
                for( size_t partition = 0; partition < m_numPartitions; ++partition) {
                    m_partitionToListener[partition] = listenerId;
                }

                auto& partitionList = m_listenerIdToPartitions[listenerId];
                for (size_t i = 0; i < m_numPartitions; i++) {
                    partitionList.insert(i);
                }

                m_partitionDensityBook[m_numPartitions].insert(listenerId);

                for (const auto& keysForThisPartition: m_activeKeys) {
                    for (const auto& key : keysForThisPartition) {
                        std::get<0>(listenerFunctions)(m_yinFromKey(key));
                    }
                }
            };
            onFirstListenerAdded(listenerId, listenerFunctions);
            printAllBooks();
            return true;
        } else if (m_listenerIdToListener.size() == m_numPartitions) {
            m_reserveListeners[listenerId] = listenerFunctions;
            printAllBooks();
            return true;
        } else if (m_listenerIdToListener.find(listenerId) != m_listenerIdToListener.end()) {
            printAllBooks();
            return false;
        } else if (!(std::get<0>(listenerFunctions) && std::get<1>(listenerFunctions))) {
            printAllBooks();
            return false;
        }

        m_listenerIdToListener[listenerId] = listenerFunctions;
        std::unordered_map<size_t, size_t> listenerIdToNumPartitionsToBeSnatched;

        for (size_t numPartitionsToBeReassigned = m_numPartitions / m_listenerIdToListener.size();
            numPartitionsToBeReassigned > 0;
            --numPartitionsToBeReassigned)
        {
            auto currDensityIterator = m_partitionDensityBook.begin();
            const size_t& currDensity = currDensityIterator->first;
            auto& listenersWithHighestDensity = currDensityIterator->second;
            size_t listenerIdToBeSnatchedFrom = *listenersWithHighestDensity.begin();
            ++listenerIdToNumPartitionsToBeSnatched[listenerIdToBeSnatchedFrom];
            listenersWithHighestDensity.erase(listenersWithHighestDensity.begin());
            if (listenersWithHighestDensity.empty()) {
                m_partitionDensityBook.erase(currDensityIterator);
            }

            m_partitionDensityBook[currDensity-1].insert(listenerIdToBeSnatchedFrom);
        }

        for (const auto& [existingListenerId, numPartitionsToBeRemoved] : listenerIdToNumPartitionsToBeSnatched) {
            auto& partitionRemovalList = m_listenerIdToPartitions[existingListenerId];
            std::for_each(  partitionRemovalList.begin(), 
                            std::next(partitionRemovalList.begin(), numPartitionsToBeRemoved),
                            [this, existingListenerId, &listenerFunctions, listenerId](size_t partitionToBeRemovedFromExistingListener) {
                                auto& [currYinFunc, currYangFunc] = m_listenerIdToListener[existingListenerId];
                                auto& [listenerYinFunc, listenerYangFunc] = listenerFunctions;
                                const auto& currActiveKeys = m_activeKeys[partitionToBeRemovedFromExistingListener];
                                for (const auto& key : currActiveKeys) {
                                    currYangFunc(m_yangFromKey(key));
                                    listenerYinFunc(m_yinFromKey(key));
                                }
                                m_partitionToListener[partitionToBeRemovedFromExistingListener] = listenerId;
                                m_listenerIdToPartitions[listenerId].insert(partitionToBeRemovedFromExistingListener);
                            }
            );

            auto end = partitionRemovalList.begin();
            std::advance(end, numPartitionsToBeRemoved);
            partitionRemovalList.erase(partitionRemovalList.begin(), end);
        }

        size_t numPartitionsToBeReassigned = m_numPartitions / (m_listenerIdToListener.size());
        m_partitionDensityBook[numPartitionsToBeReassigned].insert(listenerId);

        printAllBooks();
        return true;
    }

    bool onListenerRemoved(const size_t& listenerId) {
        if (auto it = m_reserveListeners.find(listenerId); it != m_reserveListeners.end()) {
            m_reserveListeners.erase(it);
            printAllBooks();
            return true;
        } else if (auto it = m_listenerIdToListener.find(listenerId); it == m_listenerIdToListener.end()) {
            printAllBooks();
            return false;
        } else if (!(m_reserveListeners.empty())) {
            auto transferPartitionsToNextAvailableReservedListener = [this](const size_t& listenerId) {
                auto [reservedListenerId, reservedListenerFunctions] = *m_reserveListeners.begin();
                auto& [reservedYinFunc, reservedYangFunc] = reservedListenerFunctions;
                m_reserveListeners.erase(m_reserveListeners.begin());
                for (const auto& partition : m_listenerIdToPartitions[listenerId]) {
                    for (const auto& key : m_activeKeys[partition]) {
                        reservedYinFunc(m_yinFromKey(key));
                    }
                }

                m_listenerIdToListener[reservedListenerId] = reservedListenerFunctions;
                const auto& existingPartitionAssignmentSet = m_listenerIdToPartitions[listenerId];
                m_partitionDensityBook[existingPartitionAssignmentSet.size()].insert(reservedListenerId);
                for (const auto& partition : existingPartitionAssignmentSet) {
                    m_partitionToListener[partition] = reservedListenerId;
                }
                m_listenerIdToPartitions[reservedListenerId] = std::move(existingPartitionAssignmentSet);
                auto removeFromAllTables = [this](const size_t& listenerId) {
                    m_listenerIdToListener.erase(listenerId);
                    m_listenerIdToPartitions.erase(listenerId);
                    for (auto it = m_partitionDensityBook.begin();;) {
                        if (it->second.erase(listenerId) == 1) {
                            if (it->second.empty()) {
                                m_partitionDensityBook.erase(it);
                            }
                            break;
                        } else {
                            it++;
                        }
                    }
                };

                removeFromAllTables(listenerId);
            };
            transferPartitionsToNextAvailableReservedListener(listenerId);
            printAllBooks();
            return false;
        } else {
            auto transferPartitionsToSiblingListeners = [this](const size_t& listenerIdToBeRemoved) {
                std::unordered_set<size_t> orphannedPartitions(std::move(m_listenerIdToPartitions[listenerIdToBeRemoved]));
                m_listenerIdToListener.erase(listenerIdToBeRemoved);
                m_listenerIdToPartitions.erase(listenerIdToBeRemoved);
                const auto it = m_partitionDensityBook.find(orphannedPartitions.size());

                {
                    auto& listenerIdSet = it->second;
                    listenerIdSet.erase(listenerIdToBeRemoved);
                    if (listenerIdSet.empty()) {
                        m_partitionDensityBook.erase(it);
                    }
                }

                size_t numOrphannedPartitions = orphannedPartitions.size();
                size_t numSiblingsToShareOrphannedPartitions = std::min(numOrphannedPartitions, m_listenerIdToListener.size());
                if (0 == numSiblingsToShareOrphannedPartitions) {
                    return;
                }

                std::vector<size_t> siblingsToShareOrphannedPartitions;

                {
                    size_t remainingOrphannedPartitions = numOrphannedPartitions;
                    auto it = m_partitionDensityBook.rbegin();
                    for(; remainingOrphannedPartitions > 0; it++) {
                        const auto& currDensitySet = it->second;
                        std::for_each(currDensitySet.begin(), 
                                    std::next(currDensitySet.begin(), std::min(remainingOrphannedPartitions, currDensitySet.size())),
                                    [this, &siblingsToShareOrphannedPartitions, &remainingOrphannedPartitions](size_t listenerId) {
                                        siblingsToShareOrphannedPartitions.push_back(listenerId);
                                        --remainingOrphannedPartitions;                
                                    });
                    }
                }

                {
                    size_t currSiblingIndex = 0;
                    for (auto it = orphannedPartitions.begin(); it != orphannedPartitions.end(); ++it) {
                        size_t currReassignedPartition = *it;
                        const auto& listenerId = siblingsToShareOrphannedPartitions[currSiblingIndex];

                        {
                            const auto& [yinFunc, yangFunc] = m_listenerIdToListener[listenerId];
                            for (const auto& key : m_activeKeys[currReassignedPartition]) {
                                yinFunc(m_yinFromKey(key));
                            }
                        }

                        size_t numPreviouslyHandledPartitionsbyThisListener = m_listenerIdToPartitions[listenerId].size();
                        auto itDensitySet = m_partitionDensityBook.find(numPreviouslyHandledPartitionsbyThisListener);
                        auto& prevDensitySet = itDensitySet->second;
                        prevDensitySet.erase(listenerId);
                        if (prevDensitySet.empty()) {
                            m_partitionDensityBook.erase(itDensitySet);
                        }

                        m_partitionToListener[currReassignedPartition] = listenerId;
                        m_listenerIdToPartitions[listenerId].insert(currReassignedPartition);
                        m_partitionDensityBook[m_listenerIdToPartitions[listenerId].size()].insert(listenerId);

                        currSiblingIndex = (currSiblingIndex + 1) % numSiblingsToShareOrphannedPartitions;
                    }
                }
            };
            transferPartitionsToSiblingListeners(listenerId);
            printAllBooks();
            return true;
        }
    }

private:
    void printAllBooks() {
        std::cout << "==========================================================" << std::endl;
        for ( const auto& [listenerId, partitions] : m_listenerIdToPartitions) {
            std::cout << "Listener Id: " << listenerId << ", partitions: [";
            for (const auto& partition : partitions) {
                std::cout << partition << ", ";
            }
            std::cout << "]" << std::endl;
        }

        for ( const auto& [numPartitionshandled, listenerds] : m_partitionDensityBook) {
            std::cout << "NumPartitionshandled: " << numPartitionshandled << ", listenerds: [";
            for (const auto& listenerd : listenerds) {
                std::cout << listenerd << ", ";
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "Reserved listeners: [";
        for ( const auto& [listenerId, listenerFunctions] : m_reserveListeners) {
            std::cout << listenerId << ", ";
        }
        std::cout << "]" << std::endl;

        std::cout << "==========================================================" << std::endl;
    }

    size_t m_numPartitions;
    std::vector<std::unordered_set<Key>> m_activeKeys;

    std::unordered_map<size_t, ListenerFunctions> m_listenerIdToListener;
    std::unordered_map<size_t, std::unordered_set<size_t>> m_listenerIdToPartitions;
    std::vector<size_t> m_partitionToListener;
    std::map<size_t, std::unordered_set<size_t>, std::greater<size_t>> m_partitionDensityBook;

    std::unordered_map<size_t, ListenerFunctions> m_reserveListeners;

    std::function<Key(const Yin&)> m_keyFromYin;
    std::function<Key(const Yang&)> m_keyFromYang;
    std::function<Yin(const Key&)> m_yinFromKey;
    std::function<Yang(const Key&)> m_yangFromKey;
    std::function<size_t(const Key&)> m_keyGen;

    void onFirstListenerAdded(size_t listenerId, ListenerFunctions listenerFunctions) {
        m_listenerIdToListener[listenerId] = listenerFunctions;
        for( size_t partition = 0; partition < m_numPartitions; ++partition) {
            m_partitionToListener[partition] = listenerId;
        }

        auto& partitionList = m_listenerIdToPartitions[listenerId];
        for (size_t i = 0; i < m_numPartitions; i++) {
            partitionList.insert(i);
        }

        m_partitionDensityBook[m_numPartitions].insert(listenerId);

        for (const auto& keysForThisPartition: m_activeKeys) {
            for (const auto& key : keysForThisPartition) {
                std::get<0>(listenerFunctions)(m_yinFromKey(key));
            }
        }
    }
};
