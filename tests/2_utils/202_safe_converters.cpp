// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include <catch2/catch.hpp>
#include <limits>
#include "ezdxf/utils.hpp"
#include "ezdxf/tag/tag.hpp"


using namespace ezdxf::utils;
using namespace ezdxf::tag;

// Important note:
// This converters are optimized to load DXF tags and nothing else,
// they are not meant as general purpose functions.

TEST_CASE("Test VALID group codes converted by safe_group_code()",
          "[utils][safe]") {
    SECTION("Test without whitespaces") {
        std::string s = GENERATE("10", "18", "210", "1010", "1013");
        REQUIRE(safe_group_code(s) == std::stoi(s));
    }
        // The line endings are not really a problem, in reality they are
        // removed as its loads.
    SECTION("Test whitespace in the back") {
        std::string s = GENERATE("10 ", "10\r", "10\n", "10\t");
        REQUIRE(safe_group_code(s) == 10);
    }

    SECTION("Test whitespaces in the front") {
        std::string s = GENERATE(" 10", "\r10", "\n10", "\t10");
        REQUIRE(safe_group_code(s) == 10);
    }

    SECTION("Test whitespaces on both sides ") {
        std::string s = GENERATE(" 10 ", "\r10\r", "\n10\n", "\t10\t");
        REQUIRE(safe_group_code(s) == 10);
    }

    SECTION("Test read as much as possible") {
        // This should be errors, but the converters from the stdlib are very
        // forgiving -- I can live with that results, because further checks
        // just cost time (for every case) and DXF files containing such a mess
        // will fail by additional flaws:
        std::string s = GENERATE("10a", "10#", "10!");
        REQUIRE(safe_group_code(s) == 10);
    }
}

TEST_CASE("Test INVALID group codes converted by safe_group_code()",
          "[utils][safe]") {
    SECTION("Test values out of range") {
        std::string s = GENERATE("-1", "1072", "-1234567890", "1234567890");

        REQUIRE(safe_group_code(s) == GroupCode::kError);
    }

    SECTION("Test invalid literals in front") {
        std::string s = GENERATE("a", "#1", "!1");
        REQUIRE(safe_group_code(s) == GroupCode::kError);
    }

    SECTION("Test just whitespace") {
        std::string s = GENERATE(" ", "\n", "\r", "\t");
        REQUIRE(safe_group_code(s) == GroupCode::kError);
    }
}

// safe_str_to_int64() returns an optional<int64_t> because there is no legit
// error code.
TEST_CASE("Test VALID int64 converted by safe_str_to_int64()",
          "[utils][safe]") {
    SECTION("Test valid values without whitespace") {
        std::string s = GENERATE("0", "1", "+1", "-1");
        REQUIRE(safe_str_to_int64(s).value() == std::stoll(s));
    }

    SECTION("Test int64 limits") {
        std::string s = GENERATE("9223372036854775807", "-9223372036854775808");
        REQUIRE(safe_str_to_int64(s).has_value() == true);
    }

    SECTION("Test valid values with whitespace in the front") {
        std::string s = GENERATE(" 99", " +99", "\t99", "\t+99", "\r99",
                                 "\r+99");
        REQUIRE(safe_str_to_int64(s).value() == 99);
        s = GENERATE(" -99", " -99", "\t-99", "\t-99", "\r-99", "\r-99");
        REQUIRE(safe_str_to_int64(s).value() == -99);
    }

    SECTION("Test valid values with whitespace on both sides") {
        std::string s = GENERATE(" 99 ", "\t99\t", "\r99\r");
        REQUIRE(safe_str_to_int64(s).value() == 99);
    }
    SECTION("Test read integer part from decimals literals") {
        std::string s = GENERATE("99.1", "99.1e2");
        REQUIRE(safe_str_to_int64(s).value() == 99);

        s = GENERATE("-99.1", "-99.1e2");
        REQUIRE(safe_str_to_int64(s).value() == -99);
    }

    SECTION("Test invalid literals after legit digits") {
        std::string s = GENERATE("1a", "1e2", "1E2", "1+", "1-");
        // All this is accepted by AutoCAD -- read int until first non-digit
        // AutoCAD uses most likely then same C/C++ stdlib function!
        REQUIRE(safe_str_to_int64(s).value() == 1);
    }

}

TEST_CASE("Test INVALID int64 converted by safe_str_to_int64()",
          "[utils][safe]") {

    SECTION("Test int64 beyond the limits") {
        std::string s = GENERATE("9223372036854775808", "-9223372036854775809");
        REQUIRE(safe_str_to_int64(s).has_value() == false);
    }

    SECTION("Test invalid literals in front") {
        std::string s = GENERATE("a", "a", "#1", " #1", "!1", " !1" );
        REQUIRE(safe_str_to_int64(s).has_value() == false);
    }

}
