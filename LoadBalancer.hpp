#include <functional>
#include <tuple>
#include <vector>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
template<class Data, class ListenerId>
struct Loadbalancer {
    typedef std::function<void(const Data&)> ListenerFunction;
    Loadbalancer(const size_t& numPartitions,
                std::function<size_t(const Data&)> idxGen)
        : m_numPartitions(numPartitions),
          m_partitionToListener(numPartitions, 0),
          m_idxGen(idxGen) {
            if (!idxGen) {
                throw std::runtime_error("Invalid idxGen passed");
            } else if (0 == numPartitions) {
                throw std::runtime_error("Num partitions can't be 0");
            }
        }

    void onData(const Data& data) {
        size_t partition = m_idxGen(data) % m_numPartitions;
        if (m_activeListeners.empty()) {
            return;
        }

        m_activeListeners[m_partitionToListener[partition]](data);
    }

    void onListenerAdded(const ListenerId& listenerId, const ListenerFunction& listenerFunction) {       
        if (0 == m_activeListeners.size()) {
            const std::function<void(const size_t&, const ListenerFunction&)> onFirstListenerAdded  = [this](const size_t& listenerId, const ListenerFunction& listenerFunction) {
                m_activeListeners[listenerId] = listenerFunction;
                auto& partitionList = m_listenerIdToPartitions[listenerId];
                for (size_t partition = 0; partition < m_numPartitions; ++partition) {
                    m_partitionToListener[partition] = listenerId;
                    partitionList.insert(partition);
                }

                m_partitionDensityBook[m_numPartitions].insert(listenerId);
            };
            onFirstListenerAdded(listenerId, listenerFunction);
        } else if (m_activeListeners.size() == m_numPartitions) {
            m_reserveListeners[listenerId] = listenerFunction;
        } else if (m_activeListeners.find(listenerId) != m_activeListeners.end()) {
            throw std::runtime_error("Duplicate listener Id provided!");
        } else if (!listenerFunction) {
            throw std::runtime_error("Null listener provided!");
        } else {
            m_activeListeners[listenerId] = listenerFunctions;
            std::unordered_map<size_t, size_t> listenerIdToNumReassignedPartitions;

            for (size_t numPartitionsToBeReassigned = m_numPartitions / m_activeListeners.size();
                numPartitionsToBeReassigned > 0;
                --numPartitionsToBeReassigned)
            {
                auto currDensityIterator = m_partitionDensityBook.begin();
                const size_t currDensity = currDensityIterator->first;
                auto& listenersWithHighestDensity = currDensityIterator->second;
                size_t listenerIdToBeSnatchedFrom = *listenersWithHighestDensity.begin();
                ++listenerIdToNumReassignedPartitions[listenerIdToBeSnatchedFrom];
                listenersWithHighestDensity.erase(listenersWithHighestDensity.begin());
                if (listenersWithHighestDensity.empty()) {
                    m_partitionDensityBook.erase(currDensityIterator);
                }

                m_partitionDensityBook[currDensity-1].insert(listenerIdToBeSnatchedFrom);
            }

            for (const auto& [existingListenerId, numPartitionsToBeRemoved] : listenerIdToNumReassignedPartitions) {
                auto& partitionRemovalList = m_listenerIdToPartitions[existingListenerId];
                std::for_each(  partitionRemovalList.begin(), 
                                std::next(partitionRemovalList.begin(), numPartitionsToBeRemoved),
                                [this, &existingListenerId, &listenerId, &listenerFunction](size_t partitionToBeRemovedFromExistingListener) {
                                    m_partitionToListener[partitionToBeRemovedFromExistingListener] = listenerId;
                                    m_listenerIdToPartitions[listenerId].insert(partitionToBeRemovedFromExistingListener);
                                }
                );

                auto end = partitionRemovalList.begin();
                std::advance(end, numPartitionsToBeRemoved);
                partitionRemovalList.erase(partitionRemovalList.begin(), end);
            }

            size_t numReassignedPartitions = m_numPartitions / (m_activeListeners.size());
            m_partitionDensityBook[numReassignedPartitions].insert(listenerId);
        }
    }

    void onListenerRemoved(const ListenerId& listenerId) {
        if (auto it = m_reserveListeners.find(listenerId); it != m_reserveListeners.end()) {
            m_reserveListeners.erase(it);
        } else if (m_activeListeners.find(listenerId) == m_activeListeners.end()) {
            throw std::runtime_error("Spurious listnerId provided!");
        } else if (!(m_reserveListeners.empty())) {
            const std::function<void(const size_t&)> transferPartitionsToNextAvailableReservedListener = [this](const size_t& listenerId) {
                auto [reservedListenerId, reservedListenerFunctions] = *m_reserveListeners.begin();
                auto& [reservedYinFunc, reservedYangFunc] = reservedListenerFunctions;
                m_reserveListeners.erase(m_reserveListeners.begin());
                for (const auto& partition : m_listenerIdToPartitions[listenerId]) {
                    for (const auto& key : m_activeKeys[partition]) {
                        reservedYinFunc(m_yinFromKey(key));
                    }
                }

                m_activeListeners[reservedListenerId] = reservedListenerFunctions;
                const auto& existingPartitionAssignmentSet = m_listenerIdToPartitions[listenerId];
                m_partitionDensityBook[existingPartitionAssignmentSet.size()].insert(reservedListenerId);
                for (const auto& partition : existingPartitionAssignmentSet) {
                    m_partitionToListener[partition] = reservedListenerId;
                }
                m_listenerIdToPartitions[reservedListenerId] = std::move(existingPartitionAssignmentSet);
                auto removeFromAllTables = [this](const size_t& listenerId) {
                    m_activeListeners.erase(listenerId);
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
        } else {
            auto transferPartitionsToSiblingListeners = [this](const size_t& listenerIdToBeRemoved) {
                std::unordered_set<size_t> orphannedPartitions(std::move(m_listenerIdToPartitions[listenerIdToBeRemoved]));
                m_activeListeners.erase(listenerIdToBeRemoved);
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
                size_t numSiblingsToShareOrphannedPartitions = std::min(numOrphannedPartitions, m_activeListeners.size());
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
                            const auto& [yinFunc, yangFunc] = m_activeListeners[listenerId];
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
        }
    }

private:
    size_t m_numPartitions;
    std::unordered_map<ListenerId, ListenerFunction> m_activeListeners;
    std::unordered_map<size_t, std::unordered_set<size_t>> m_listenerIdToPartitions;
    std::vector<size_t> m_partitionToListener;
    std::map<size_t, std::unordered_set<size_t>, std::greater<size_t>> m_partitionDensityBook;

    std::unordered_map<size_t, ListenerFunction> m_reserveListeners;

    std::function<size_t(const Key&)> m_idxGen;
};
