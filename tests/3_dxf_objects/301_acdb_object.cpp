// Copyright (c) 2021, Manfred Moitzi
// License: MIT License
//
#include <catch2/catch.hpp>
#include "ezdxf/acdb/object.hpp"

using ezdxf::acdb::Object;

TEST_CASE("Testing ezdxf::acdb::Object", "[acdb, object]") {

    SECTION("Test default constructor.") {
        auto o = Object();

        REQUIRE(o.get_handle() == 0);
        REQUIRE(o.get_owner() == 0);
        REQUIRE(o.dxf_type() == "OBJECT");
        REQUIRE(o.is_erased() == false);
        REQUIRE(o.is_alive() == true);
    }

    SECTION("Construct object with handle.") {
        auto o = Object(1);
        REQUIRE(o.get_handle() == 1);
        REQUIRE(o.get_owner() == 0);
    }

    SECTION("Construct object with handle and owner.") {
        auto o = Object(1, 2);
        REQUIRE(o.get_handle() == 1);
        REQUIRE(o.get_owner() == 2);
    }

    SECTION("Can set handle if unset.") {
        auto o = Object();
        o.set_handle(1);
        REQUIRE(o.get_handle() == 1);
    }

    SECTION("Can set handle 0 if unset.") {
        auto o = Object();
        o.set_handle(0);
        REQUIRE(o.get_handle() == 0);
        // therefore handle is still unset:
        o.set_handle(1);
        REQUIRE(o.get_handle() == 1);
    }

    SECTION("Can not change handle if already set.") {
        // Object handle is immutable ofer the lifetime of the DXF document.
        auto o = Object(1);
        REQUIRE_THROWS_AS(o.set_handle(1), std::invalid_argument);
    }

    SECTION("Can change owner handle.") {
        auto o = Object(1, 2);
        o.set_owner(3);
        REQUIRE(o.get_owner() == 3);
    }

    SECTION("Can set owner handle to 0.") {
        auto o = Object(1, 2);
        o.set_owner(0);
        REQUIRE(o.get_owner() == 0);  // which means no owner
    }

    SECTION("Erasing the object does not destroy the object.") {
        auto o = Object(1, 2);
        o.erase();
        REQUIRE(o.is_erased() == true);
        REQUIRE(o.is_alive() == false);

        // All properties are still available:
        REQUIRE(o.get_handle() == 1);
        REQUIRE(o.get_owner() == 2);
    }
}
