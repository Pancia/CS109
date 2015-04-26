#include "../bigint.h"

namespace test_bigint_addition {
    bigint x, y, expected;
    TEST_CASE("two positive bigints can be added together") {
        SECTION("no carrying") {
            x = {"100000000000000"};
            y = {"222222222222222"};
            expected = {"322222222222222"};
            REQUIRE((x + y) == expected);
        }

        SECTION("some carrying") {
            x = {"444444444444444"};
            y = {"100000000654321"};
            expected = {"544444445098765"};
            REQUIRE((x + y) == expected);
        }

        SECTION("different lengths, no carrying") {
            x = {        "10000000"};
            y = {"2222222222222222"};
            expected = {"2222222232222222"};
            REQUIRE((x + y) == expected);
        }

        SECTION("should trim") {
            x = {"000444444444440"};
            y = {  "0009876543210"};
            expected = {"454320987650"};
            REQUIRE((x + y) == expected);
        }
    }
    TEST_CASE("1 pos and 1 neg bigints can be added together") {
        SECTION("no carrying") {
            x = { "21516326"};
            y = {"_66666666"};
            expected = {"_45150340"};
            REQUIRE((x + y) == expected);
        }

        SECTION("carrying") {
            x = { "35098236"};
            y = {"_19283523"};
            expected = {"15814713"};
            REQUIRE((x + y) == expected);
        }

        SECTION("different lengths") {
            x = {     "20509235"};
            y = {"_989285135987"};
            expected = {"_989264626752"};
            REQUIRE((x + y) == expected);
        }
    }
    TEST_CASE("2 negative bigints can be added together") {
        SECTION("no carrying") {
            x = {"_12035023"};
            y = {"_35921633"};
            expected = {"_47956656"};
            REQUIRE((x + y) == expected);
        }
        SECTION("carrying") {
            x = {"_87079860987"};
            y = {"_09709680909"};
            expected = {"_96789541896"};
            REQUIRE((x + y) == expected);
        }
        SECTION("different lengths") {
            x = {"_87079860987"};
            y = {    "_7480909"};
            expected = {"_87087341896"};
            REQUIRE((x + y) == expected);
        }
    }
}
