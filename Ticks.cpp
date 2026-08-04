#include <iostream>
#include <fstream>
#include <cstdint>
#include <random>
#include <unordered_map>


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


void createTicks()	{
	
	//increment seq_number
}

void writeTicks() {

}

std::vector<int32_t> loadIds() {
	std::ifstream file("tickerIds.jack");

	if (!file.is_open()) {
		std::cerr << "file could not be opened.\n";
		return;
	}

	std::vector<int32_t> tickers{
		std::istream_iterator<int32_t>(file),
		std::istream_iterator<int32_t>()
	};
	
	return tickers;
}





int main() {
	loadIds();
	createTicks();
	writeTicks();

	return 0;
}