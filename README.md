# ticker-plant

solo project to learn how real market data infrastructure works from scratch. Ticks.cpp generates random tick data using a simplified gbm price walk, works out bid and ask from a spread, then writes each tick to a binary file field by field. next step is reusing the same encoding logic to send ticks over udp between a server and client on lan. long term goal is feeding a live tick stream into an order matching engine im also building.
