// Copyright (c) 2021, Manfred Moitzi
// License: MIT License
//

#include <catch2/catch.hpp>
#include "ezdxf/type.hpp"
#include "ezdxf/simple_set.hpp"

using namespace ezdxf;

TEST_CASE("Test SimpleSet.", "[utils][set]") {
    SECTION("Create a string set.") {
        auto const set = SimpleSet<std::string>{"AA", "BB", "CC"};
        REQUIRE(set.has("AA") == true);
        REQUIRE(set.has("XX") == false);
    }

    SECTION("Init a unique data set.") {
        auto const set = SimpleSet<std::string>{"AA", "BB", "CC", "AA"};
        REQUIRE(set.size() == 3);
    }

    SECTION("Add only unique data to set.") {
        auto set = SimpleSet<std::string>{"AA", "BB", "CC"};
        REQUIRE(set.size() == 3);
        set.add("BB");
        REQUIRE(set.size() == 3);
        set.add("DD");
        REQUIRE(set.size() == 4);
    }

    SECTION("Create a DXF Version set.") {
        auto const set = SimpleSet<ezdxf::Version>{
                ezdxf::Version::R12,
                ezdxf::Version::R2000,
                ezdxf::Version::R2004,
        };
        REQUIRE(set.has(ezdxf::Version::R2004) == true);
        REQUIRE(set.has(ezdxf::Version::R2018) == false);
    }
}