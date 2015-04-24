#include "../bigint.h"

SCENARIO("two bigints can be added together", "[bigint]") {
    bigint::bigint x, y, expected;
    GIVEN("same length") {
        WHEN("no carrying") {
            x = {"100000000000000"};
            y = {"222222222222222"};
            THEN("their sum is correct") {
                expected = {"322222222222222"};
                REQUIRE((x + y) == expected);
            }
        }

        WHEN("carrying") {
            x = {"444444444444444"};
            y = {"100000000654321"};
            THEN("their sum is correct") {
                expected = {"544444445098765"};
                REQUIRE((x + y) == expected);
            }
        }
    }

    GIVEN("different lengths") {
        WHEN("no carrying") {
            x = {        "10000000"};
            y = {"2222222222222222"};
            THEN("their sum is correct") {
                expected = {"2222222232222222"};
                REQUIRE((x + y) == expected);
            }
        }

        WHEN("carrying") {
            AND_WHEN("result could have trailing zeros") {
                x = {"000444444444440"};
                y = {  "0009876543210"};
                THEN("it should trim it") {
                    expected = {"454320987650"};
                    REQUIRE((x + y) == expected);
                }
            }
        }
    }
}
