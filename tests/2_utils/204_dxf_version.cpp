// Copyright (c) 2021, Manfred Moitzi
// License: MIT License
//

#include <catch2/catch.hpp>
#include <ezdxf/utils.hpp>

using namespace ezdxf::utils;

TEST_CASE("Test DXF version.", "[utils][version]") {
    SECTION("Test string to DXF version") {
        REQUIRE(str_to_dxf_version("AC1004") == ezdxf::Version::R9);
        REQUIRE(str_to_dxf_version("AC1032") == ezdxf::Version::R2018);
        // Invalid DXF version defaults to DXF R12
        REQUIRE(str_to_dxf_version("XXX") == ezdxf::Version::R12);
    }

    SECTION("Test DXF version to string") {
        REQUIRE(dxf_version_to_str(ezdxf::Version::R9) == "AC1004");
        REQUIRE(dxf_version_to_str(ezdxf::Version::R12) == "AC1009");
        REQUIRE(dxf_version_to_str(ezdxf::Version::R2018) == "AC1032");
    }
}