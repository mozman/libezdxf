// Copyright (c) 2021, Manfred Moitzi
// License: MIT License
//
#include <catch2/catch.hpp>
#include <ezdxf/utils.hpp>

using namespace ezdxf::utils;

// Utility functions to manage binary data in binary tags with
// group codes 310-319 & 1004.

// Convert data 0xfefe to a string "FEFE"
TEST_CASE("Test hexlify binary data", "[utils][hex]") {
    SECTION("Convert no data") {
        auto data = ezdxf::Bytes{};
        auto result = hexlify(data);
        REQUIRE(result.empty() == true);
    }

    SECTION("Convert zero bytes") {
        auto data = ezdxf::Bytes{0, 0, 0, 0};
        auto result = hexlify(data);
        REQUIRE(result == "00000000");
    }

    SECTION("Convert all chars") {
        auto data = ezdxf::Bytes{0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd,
                                 0xef};
        auto result = hexlify(data);
        REQUIRE(result == "0123456789ABCDEF");
    }
}

// Convert a hex strings like  "FEFE" into binary data {0xfe, 0xfe}
TEST_CASE("Test unhexlify binary data", "[utils][hex]") {
    SECTION("Convert no data") {
        auto result = unhexlify("");
        REQUIRE(result.has_value() == true);
        auto value = result.value();
        REQUIRE(value.empty() == true);
    }

    SECTION("Test invalid input data") {
        auto s = GENERATE("x", "00%", "gg", "GG");
        REQUIRE(unhexlify(s).has_value() == false);
    }

    SECTION("Convert zero data") {
        auto result = unhexlify("00000000");
        REQUIRE(result.has_value() == true);
        auto value = result.value();
        REQUIRE(value == ezdxf::Bytes{0, 0, 0, 0});
    }

    SECTION("Test all hex char cases.") {
        auto s = GENERATE("FEFE", "fefe", "FEfe", "feFE", "FeFe", "fEfE");
        auto result = unhexlify(s);
        REQUIRE(result.value() == ezdxf::Bytes{0xfe, 0xfe});
    }

    SECTION("Test all chars.") {
        auto result = unhexlify("0123456789abcdef");
        REQUIRE(result.value() == ezdxf::Bytes{0x01, 0x23, 0x45, 0x67,
                                               0x89, 0xab, 0xcd, 0xef});
    }

    SECTION("Test if last nibble is ignored for uneven string length.") {
        REQUIRE(unhexlify("0").value().empty() == true);
        REQUIRE(unhexlify("FEFE0").value() == ezdxf::Bytes{0xfe, 0xfe});
    }

}