#include "protocol.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("encode->decode round-trips", "[protocol]") {
    Frame in{42, -1550, 1234, Health::WARNING};
    auto bytes = encode(in);
    auto out = decode(bytes.data(), bytes.size());
    REQUIRE(out.has_value());
    REQUIRE(out->node_id == 42);
    REQUIRE(out->temperature == -1550);
    REQUIRE(out->vibration == 1234);
    REQUIRE(out->health == Health::WARNING);
}

TEST_CASE("corrupted byte fails CRC", "[protocol]") {
    Frame in{7, 2000, 500, Health::NORMAL};
    auto bytes = encode(in);
    bytes[2] ^= 0xFF;                         // corrompe um byte
    REQUIRE_FALSE(decode(bytes.data(), bytes.size()).has_value());
}

TEST_CASE("wrong length is rejected", "[protocol]") {
    std::vector<uint8_t> tooShort(5, 0);
    REQUIRE_FALSE(decode(tooShort.data(), tooShort.size()).has_value());
}
