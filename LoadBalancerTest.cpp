#include <string>
#include <set>
#include "LoadBalancer.hpp"
#include <cassert>
#include <array>

struct Yin : std::string
{
    Yin(const std::string& str) : std::string(str.c_str()) {}
};
typedef std::string Yang;
int main() {
    typedef Loadbalancer<Yin, Yang, std::string, size_t> Lb;
    const size_t numPartitions = 10; 
    
    std::function<std::string(const Yin&)> keyFromYin = [](const Yin& yin){ return yin; };
    std::function<Yin(const std::string&)> yinFromKey = [](const std::string& key){ return key;};

    std::function<std::string(const Yang&)> keyFromYang = [](const Yang& yang){ return yang; };
    std::function<Yang(const std::string&)> yangFromKey = [](const std::string& key){ return key;};

    std::function<size_t(const std::string&)> indexGen = [](const std::string& key){ return (size_t)std::atoll(key.c_str()); };
    std::function<size_t(const std::string&)> partitionGen = [&numPartitions, indexGen = std::cref(indexGen)](const std::string& key) { return (size_t)(indexGen(key) % numPartitions); }; 

    std::map<size_t, std::set<size_t>> partitionToListeners;
    std::map<std::string, std::set<size_t>> keyToListeners;
    std::map<size_t, std::set<std::string>> listenerIdToPartitions;
    std::map<size_t, std::set<std::string>> listenerIdToKeysToKeys;
    std::set<size_t> deletedListenerIds;


    Lb lb(  numPartitions,
            keyFromYin,
            keyFromYang,
            yinFromKey,
            yangFromKey,
            indexGen);

    for (size_t i = 0; i < numPartitions; i++) {
        lb.onListenerAdded(i, {[i,
                                partitionGen,
                                keyFromYin,
                                &listenerIdToPartitions,
                                &keyToListeners,
                                &partitionToListeners](const Yin& yin) {
                                    auto partition = partitionGen(yin);
                                    keyToListeners[yin].insert(i);
                                    partitionToListeners[partition].insert(i);
                                },
                                [](const Yang& yang) { }});
    }
    //std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    //for(size_t i = numPartitions; i < 2*numPartitions; i++) {
    //    lb.onListenerRemoved(i);
    //}cd /mn   
    //for(size_t i = 0; i < numPartitions; i++) {
    //    lb.onListenerRemoved(i);
    //}

    //for(size_t i = numPartitions; i < 2*  numPartitions; i++) {
    //    lb.onListenerRemoved(i);
    //}

    //for(size_t i = 0; i < numPartitions; i++) {
    //    lb.onListenerRemoved(i);
    //}

    const size_t numKeys = 100;
    std::cout << std::endl;
    for(size_t i = 0; i < numKeys; i++) {
        lb.onData(Yin(std::to_string(i)));
    }

    

    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::array<size_t, 2> lisnetersToBeremoved{2, 9}; 
    for(const auto& listener : lisnetersToBeremoved) {
        lb.onListenerRemoved(listener);
        deletedListenerIds.insert(listener);
    }


    for(size_t i = 0; i < numKeys; i++) {
        for (const auto& listenerId : deletedListenerIds) {
            const std::string key = std::to_string(i);
            keyToListeners[key].erase(listenerId);
            partitionToListeners[partitionGen(key)].erase(listenerId);
        }
    }

    for (const auto& [key, listeners] : keyToListeners) {
        assert(1 == listeners.size());
    }

    for (const auto& [partition, listeners] : partitionToListeners) {
        assert(1 == listeners.size());
    }

    assert(numKeys == keyToListeners.size());

    return 0;
}