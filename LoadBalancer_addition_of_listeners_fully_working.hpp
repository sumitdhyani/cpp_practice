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

    Loadbalancer(size_t numPartitions,
                std::function<Key(const Yin&)> keyFromYin,
                std::function<Key(const Yang&)> keyFromYang,
                std::function<Yin(const Key&)> yinFromKey,
                std::function<Yang(const Key&)> yangFromKey,
                std::function<size_t(const Key&)> keyGen)
        : m_numPartitions(numPartitions),
          m_activeKeys(numPartitions),
          m_partitionToListener(numPartitions, std::make_tuple(YinFunc{}, YangFunc{})),
          m_listenerIdToListener(),
          m_keyFromYin(keyFromYin), m_keyFromYang(keyFromYang),
          m_yinFromKey(yinFromKey), m_yangFromKey(yangFromKey), m_keyGen(keyGen) {}

    void onData(Yin yin) {
        Key key = m_keyFromYin(yin);
        size_t partition = m_keyGen(key) % m_numPartitions;

        m_activeKeys[partition].insert(key);

        if (std::get<0>(m_partitionToListener[partition]) != nullptr) {
            auto listenerFuncs = m_partitionToListener[partition];
            std::get<0>(listenerFuncs)(yin);
        }
    }

    void onData(Yang yang) {
        Key key = m_keyFromYang(yang);
        size_t partition = m_keyGen(key) % m_numPartitions;

        m_activeKeys[partition].erase(key);

        if (std::get<1>(m_partitionToListener[partition]) != nullptr) {
            auto listenerFuncs = m_partitionToListener[partition];
            std::get<1>(listenerFuncs)(yang);
        }
    }

    bool onListenerAdded(size_t listenerId, ListenerFunctions listenerFunctions) {
        if (0 == m_listenerIdToListener.size()) {
            onFirstListenerAdded(listenerId, listenerFunctions);
            printAllBooks();
            return true;
        } else if (m_listenerIdToListener.size() == m_numPartitions) {
            m_reserveListeners.push_back({listenerId, listenerFunctions});
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
        size_t numPartitionsToBeReassigned = m_numPartitions / (m_listenerIdToListener.size());

        for (size_t numPartitionsToBeReassigned = m_numPartitions / (m_listenerIdToListener.size());
            numPartitionsToBeReassigned > 0;
            --numPartitionsToBeReassigned)
        {
            auto currDensityIterator = m_numPartitionshandledToListenerIds.begin();
            size_t currDensity = currDensityIterator->first;
            auto& listenersWithHighestDensity  = currDensityIterator->second;
            size_t listenerIdToBeSnatchedFrom = *listenersWithHighestDensity.begin();
            ++listenerIdToNumPartitionsToBeSnatched[listenerIdToBeSnatchedFrom];
            listenersWithHighestDensity.erase(listenersWithHighestDensity.begin());
            if (listenersWithHighestDensity.empty()) {
                m_numPartitionshandledToListenerIds.erase(currDensityIterator);
            }

            m_numPartitionshandledToListenerIds[currDensity-1].insert(listenerIdToBeSnatchedFrom);
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
                                m_partitionToListener[partitionToBeRemovedFromExistingListener] = listenerFunctions;
                                m_listenerIdToPartitions[listenerId].insert(partitionToBeRemovedFromExistingListener);
                            }
            );

            auto end = partitionRemovalList.begin();
            std::advance(end, numPartitionsToBeRemoved);
            partitionRemovalList.erase(partitionRemovalList.begin(), end);
        }

        m_numPartitionshandledToListenerIds[numPartitionsToBeReassigned].insert(listenerId);

        printAllBooks();
        return true;
    }

    void onListenerRemoved(size_t listenerId) {
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

        for ( const auto& [numPartitionshandled, listenerds] : m_numPartitionshandledToListenerIds) {
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

    std::vector<ListenerFunctions> m_partitionToListener;
    std::unordered_map<size_t, ListenerFunctions> m_listenerIdToListener;
    std::unordered_map<size_t, std::unordered_set<size_t>> m_listenerIdToPartitions;

    std::map<size_t, std::unordered_set<size_t>, std::greater<size_t>> m_numPartitionshandledToListenerIds;
    std::vector<std::tuple<size_t, ListenerFunctions>> m_reserveListeners;

    std::function<Key(const Yin&)> m_keyFromYin;
    std::function<Key(const Yang&)> m_keyFromYang;
    std::function<Yin(const Key&)> m_yinFromKey;
    std::function<Yang(const Key&)> m_yangFromKey;
    std::function<size_t(const Key&)> m_keyGen;

    void onFirstListenerAdded(size_t listenerId, ListenerFunctions listenerFunctions) {
        m_listenerIdToListener[listenerId] = listenerFunctions;
        for( size_t partition = 0; partition < m_numPartitions; ++partition) {
            m_partitionToListener[partition] = listenerFunctions;
        }

        auto& partitionList = m_listenerIdToPartitions[listenerId];
        for (size_t i = 0; i < m_numPartitions; i++) {
            partitionList.insert(i);
        }

        m_numPartitionshandledToListenerIds[m_numPartitions].insert(listenerId);

        for (const auto& keysForThisPartition: m_activeKeys) {
            for (const auto& key : keysForThisPartition) {
                std::get<0>(listenerFunctions)(m_yinFromKey(key));
            }
        }
    }
};
