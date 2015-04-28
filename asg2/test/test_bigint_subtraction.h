#include "../bigint.h"

namespace test_bigint_subtraction {
        bigint x, y, expected;
    TEST_CASE("two bigints can be subtracted") {
        SECTION("no carrying") {
            x = {"0238502938"};
            y = {"0126301815"};
            expected = {"112201123"};
            REQUIRE((x - y) == expected);
        }

        SECTION("carrying") {
            x = {"8923765823723986"};
            y = {"2938579089235987"};
            expected = {"5985186734487999"};
            REQUIRE((x - y) == expected);
        }

        SECTION("different lengths") {
            x = {"203950398278735"};
            y = {"26301815"};
            expected = {"203950371976920"};
            REQUIRE((x - y) == expected);
        }

        SECTION("one negative") {
            x = {"945659238502938"};
            y = {"_982387235"};
            expected = {"945660220890173"};
            REQUIRE((x - y) == expected);
        }

        SECTION("both negative") {
            x = {"_292038502938"};
            y = {"_273917591759"};
            expected = {"_18120911179"};
            REQUIRE((x - y) == expected);
        }

        SECTION("test3-sub.ydc") {
            x = {"999999999999999999999"};
            y = {"88888888888888888888888888888888888888"};
            expected = {"-88888888888888887888888888888888888889"};
            REQUIRE((x - y) == expected);
        }
    }
}
