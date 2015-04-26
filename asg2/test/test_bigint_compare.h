#include "../bigint.h"

namespace test_bigint_compare {
    bigint x, y, z;
    TEST_CASE("two bigints can be compared") {
        SECTION("bigints can be equaled") {
            x = {"29386323906"};
            y = {"29386323906"};
            z = {     "999999"};
            REQUIRE(x == y);
            REQUIRE(x == x);
            REQUIRE(x != z);
        }

        SECTION("bigints can be compared") {
            x = {"23982623"};
            y = {"31111111"};
            z = {     "999"};
            REQUIRE(x < y);
            REQUIRE(z < x);
            REQUIRE(z < y);
        }
    }
}
