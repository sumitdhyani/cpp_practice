#include <string>
#include "LoadBalancer.hpp"

int main() {
    typedef Loadbalancer<std::string, size_t, size_t> Lb;
    size_t numPartitions = 10; 
    std::function<size_t(const std::string&)> keyFromYin = [](const std::string&){ return 0;};
    std::function<size_t(const size_t&)> keyFromYang = [](const size_t&){ return 0;};
    std::function<std::string(const size_t&)> yinFromKey = [](const size_t&){ return "";};
    std::function<size_t(const size_t&)> yangFromKey = [](const size_t&){ return 0;};
    std::function<size_t(const size_t&)> keyGen = [](const size_t&){ return 0;};
    Lb lb(  numPartitions,
            keyFromYin,
            keyFromYang,
            yinFromKey,
            yangFromKey,
            keyGen);

    for (size_t i = 0; i < numPartitions; i++) {
        std::cout << lb.onListenerAdded(i, {[](const std::string& str) {}, [](const size_t& str) {}});
    }

    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    //for(size_t i = numPartitions; i < 2*numPartitions; i++) {
    //    lb.onListenerRemoved(i);
    //}
    //for(size_t i = 0; i < numPartitions; i++) {
    //    lb.onListenerRemoved(i);
    //}

    //for(size_t i = numPartitions; i < 2*  numPartitions; i++) {
    //    lb.onListenerRemoved(i);
    //}

    for(size_t i = 0; i < numPartitions; i++) {
        lb.onListenerRemoved(i);
    }

    return 0;
}