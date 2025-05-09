#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <random>
#include <cmath>

// Helper function to format a time_point as "YYYY-MM-DD HH:MM:SS.mmm"
std::string formatTimestamp(const std::chrono::system_clock::time_point &tp) {
    using namespace std::chrono;
    // Extract milliseconds from the time_point
    auto ms = duration_cast<milliseconds>(tp.time_since_epoch()) % 1000;
    // Convert time_point to time_t (seconds since epoch)
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    // Convert time_t to local tm structure
    std::tm tm = *std::localtime(&t);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <output_file>\n";
        return EXIT_FAILURE;
    }
    
    std::string outputFile = argv[1];
    std::ofstream ofs(outputFile);
    if (!ofs) {
        std::cerr << "Error: Cannot open output file " << outputFile << "\n";
        return EXIT_FAILURE;
    }
    
    // Write header line.
    ofs << "Timestamp, Price, Size, Exchange, Type\n";
    
    const int NUM_RECORDS = 100000;
    
    // Setup random generators.
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Distribution for milliseconds increment (1 to 50 ms).
    std::uniform_int_distribution<> msIncrementDist(1, 50);
    // Distribution for price between 50.0 and 250.0.
    std::uniform_real_distribution<> priceDist(50.0, 250.0);
    // Distribution for size between 10 and 1000.
    std::uniform_int_distribution<> sizeDist(10, 1000);
    
    // Predefined arrays for exchanges and types.
    std::vector<std::string> exchanges = {"NYSE", "NASDAQ", "NYSE_ARCA", "NSX"};
    std::vector<std::string> types = {"Ask", "Bid", "TRADE"};
    std::uniform_int_distribution<> exchDist(0, exchanges.size() - 1);
    std::uniform_int_distribution<> typeDist(0, types.size() - 1);
    
    // Set base time to "2021-03-05 10:00:00.000"
    std::tm base_tm = {};
    base_tm.tm_year = 2021 - 1900;  // years since 1900
    base_tm.tm_mon = 2;             // March (0-indexed: 0=Jan, 1=Feb, 2=Mar)
    base_tm.tm_mday = 5;
    base_tm.tm_hour = 10;
    base_tm.tm_min = 0;
    base_tm.tm_sec = 0;
    std::time_t base_time_t = std::mktime(&base_tm);
    if (base_time_t == -1) {
        std::cerr << "Error converting base time\n";
        return EXIT_FAILURE;
    }
    
    std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::from_time_t(base_time_t);
    
    // Generate records.
    for (int i = 0; i < NUM_RECORDS; ++i) {
        // Increase the current time by a random number of milliseconds.
        int increment = msIncrementDist(gen);
        currentTime += std::chrono::milliseconds(increment);
        std::string timestamp = formatTimestamp(currentTime);
        
        // Generate random price and round it to one decimal.
        double price = priceDist(gen);
        price = std::round(price * 10) / 10.0;
        
        int size = sizeDist(gen);
        std::string exch = exchanges[exchDist(gen)];
        std::string typ = types[typeDist(gen)];
        
        // Write record to file.
        ofs << timestamp << ", " << price << ", " << size << ", " << exch << ", " << typ << "\n";
    }
    
    ofs.close();
    std::cout << "Generated " << NUM_RECORDS << " records in " << outputFile << std::endl;
    return EXIT_SUCCESS;
}
