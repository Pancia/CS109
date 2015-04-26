#include "../bigint.h"

namespace test_bigint_multiplication {
    bigint x, y, expected;
    TEST_CASE("two positive bigints can be multiplied") {
        SECTION("small and simple values") {
            x = {"1234"};
            y = {"4321"};
            expected = {"5332114"};
            REQUIRE((x * y) == expected);
        }

        SECTION("big values") {
            x = {"509328509132560236"};
            y = {"029385602938223908"};
            expected = {"14966925334486964630286426665322288"};
            REQUIRE((x * y) == expected);
        }

        SECTION("one negative") {
            x = {"092350983108722352"};
            y = {"_029385602982735209"};
            expected = {"_2713789324698200451081634315691568"};
            REQUIRE((x * y) == expected);
        }

        SECTION("both negative") {
            x = {"_29035702936"};
            y = {"_09235987236"};
            expected = {"268173381705183724896"};
            REQUIRE((x * y) == expected);
        }
    }
}
