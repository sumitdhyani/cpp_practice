#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <variant>
#include <memory>
#include <functional>
#include <unordered_set>

struct DataFetcher
{
    using std::function<std::string (const std::string& ipPort, const std::string& table)> = ReaderFunc;
    using std::function<std::string(const std::string& ipPort, const std::string& data)> =  WriterFunc;
    using std::function<std::string(const std::string& ipPort, 
                              const std::string& olddata,
                              const std::string& newData)>  = UpdaterFunc;
    DataFetcher(const ReaderFunc & readerFunc)
    {

    }

    DataFetcher(std::function<std::string (const std::string& ipPort)> readerFunc,
                WriterFunc writerFunc,
                UpdaterFunc uodaterFunc):
                DataFetcher([](const std::string& ipPort, const std::string& table){ readerFunc();})
    {

    }



    std::function<std::string (const std::string& ipPort)> reader;
    std::function<std::string(const std::string& ipPort, const std::string& data)>  writer;
    std::function<std::string(const std::string& ipPort, 
                              const std::string& olddata,
                              const std::string& newData)> updater;
}


Sqlite sqlite(conn params);
/*conn params*/*/



WebSocket ws("127.0.0.1:2920")

auto networkRead = [&ws](const std::string& ipPort){ nf->fetch(ipPport)}
auto networkRead = [&ws](const std::string& ipPort){ nf->fetch(ipPport)}

DataFetcher datafetcher([](){return "jgfhgfh"}, 
                        [&sqlite](const std::string& ipPort, const std::string& data){ assert(data.com)}))

template <class T>
T sum(const std::vector<T>& vec, const T start, const std::function<const T&>)
{
    
    for(auto const& elem : vec)
    {
        start += elem;
    }

    return start;
}

struct Employee
{
    std::unordered_set<std::string> skills;
    uint8_t epxInYears;

    const Employee& operaor +=(const Employee& other)
    {
        for(auto const& skill : other.skills)
        {
            skills.insert(skill);
        }
    }
}

int main() {
    std::function<void()> f1 = [addr = &f1, n = std::make_shared<int>(5)]() {
        std::cout << "f1 : " << *n << std::endl;
        if ((*n)--) {
            (*addr)();
        }
    };

    return 0;
}