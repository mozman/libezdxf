// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include <catch2/catch.hpp>
#include <ezdxf/utils.hpp>
#include <ezdxf/tag/tag.hpp>

using namespace ezdxf::utils;
using namespace ezdxf::tag;

TEST_CASE("Test VALID group codes converted by safe_group_code().",
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

TEST_CASE("Test INVALID group codes converted by safe_group_code().",
          "[utils][safe]") {
    SECTION("Test values out of range") {
        std::string s = GENERATE("-1", "1072", "-1234567890", "1234567890");

        REQUIRE(safe_group_code(s) == GroupCode::kError);
    }SECTION("Test invalid literals in front") {
        // Function is based on safe_str_to_int64()
        // exhaustive testing can be found there.
        std::string s = GENERATE("a", "#1", "!1");
        REQUIRE(safe_group_code(s) == GroupCode::kError);
    }
}
