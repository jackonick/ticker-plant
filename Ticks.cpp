#include <iostream>
#include <fstream>
#include <cstdint>
#include <random>
#include <unordered_map>
#include <string>
#include <cmath>
#include <chrono>

struct QuoteTick {
    int64_t timestamp_ns;
    int32_t symbol_id;
    int32_t seq_number; //per stream, counts packets: last_seq + 1
    int32_t bid_price; //highest buy
    int32_t ask_price; //highest sell
    int32_t bid_size; //volume available at bid 
    int32_t ask_size; //vol at ask
    int32_t quote_condition;
};


QuoteTick createTicks(std::pair<const int32_t, int32_t>& pair, std::mt19937& gen, int32_t& seq_number, int64_t& ns) { //last_prices is passed by reference
    QuoteTick tick{};

    int32_t drift{ 0 };
    double volatility{ 0.20 };
    double dt{ 1.0 / (252.0 * 6.5 * 3600.0) };
    std::normal_distribution<double> zDist(0.0, 1.0);
    double z = zDist(gen);


    double old_price = pair.second;

    double new_price = old_price * exp((drift - 0.5 * (volatility * volatility)) //GBM price movement
        * dt + volatility * sqrt(dt) * z);

    pair.second = new_price; //write new GBM price into old price on map
    tick.symbol_id = pair.first;

    double spread{ new_price * 0.005 }; //calc spread for bidask prices in tick struct

    double bid_price = static_cast<int32_t>(new_price - spread / 2);
    double ask_price = static_cast<int32_t>(new_price + spread / 2);

    std::uniform_int_distribution<int32_t> randomSize(1, 10000);
    int32_t bid_size = randomSize(gen); //calculate size randomly from 1 to 10000 lots for tick struct
    int32_t ask_size = randomSize(gen);

    ++seq_number;

    std::uniform_int_distribution<int32_t> randomNs(1000, 1000000);
    //increment timestamp from 1 micro to 1 millisecond
    ns += randomNs(gen);

    tick.bid_price = bid_price;
    tick.ask_price = ask_price;
    tick.bid_size = bid_size; //store info back into tick struct
    tick.ask_size = ask_size;
    tick.seq_number = seq_number;
    tick.timestamp_ns = ns;
    tick.quote_condition = 0;

    return tick;
}

void writeTicks() {

}

std::vector<int32_t> loadIds() {
    std::ifstream file("tickerIds.jack");

    if (!file.is_open()) {
        std::cerr << "file could not be opened.\n";
        return {};
    }

    std::vector<int32_t> tickers;
    std::string line;

    while (std::getline(file, line)) {

        size_t commaPos = line.find(',');
        std::string numPart = line.substr(0, commaPos);
        int value = std::stoi(numPart);
        tickers.push_back(value);
    }

    return tickers;
}


int main() {
    std::vector<int32_t> ids = loadIds();
    if (ids.empty()) {
        std::cerr << "failed to load id's\n";
        return -1;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int32_t> priceDist(50.0, 500.0);
    std::unordered_map<int32_t, int32_t> last_prices; //ex: xxx, 50025

    for (const auto& id : ids) {
        double p = priceDist(gen);
        p = static_cast<int32_t>(p * 100);
        last_prices[id] = p;
    }

  
    for (const auto& pair : last_prices) {
        std::cout << pair.first << " ," << pair.second << "\n";
    }

    std::vector<QuoteTick> newTicks;
    int32_t seq_number{ 1 };

    auto now = std::chrono::system_clock::now();
    int64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

    for (auto& pair : last_prices) {
        newTicks.push_back(createTicks(pair, gen, seq_number, ns));
    }

    for (const auto& tick : newTicks) {
        std::cout << "symbol id: " << tick.symbol_id << " \n";
        std::cout << "sequence number: " << tick.seq_number << " \n";
        std::cout << "bid price: " << tick.bid_price << " \n";
        std::cout << "ask price: " << tick.ask_price << " \n";
        std::cout << "bid size: " << tick.bid_size << " \n";
        std::cout << "ask size: " << tick.ask_size << " \n";
        std::cout << "timestamp: " << tick.timestamp_ns << " \n";
        std::cout << "quote condition: " << tick.quote_condition << " \n";
        std::cout << " \n";

    }

    std::cout << sizeof(QuoteTick);

    writeTicks();

    return 0;
}