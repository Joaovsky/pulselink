#include "processor.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("classify by thresholds", "[processor]") {
    REQUIRE(Processor::classify(2000, 500)  == Health::NORMAL);
    REQUIRE(Processor::classify(6500, 500)  == Health::WARNING);
    REQUIRE(Processor::classify(2000, 3500) == Health::WARNING);
    REQUIRE(Processor::classify(8500, 500)  == Health::FAULT);
    REQUIRE(Processor::classify(2000, 7000) == Health::FAULT);
}
