#include "../bigint.h"

namespace test_bigint_subtraction {
        bigint x, y, z;
    TEST_CASE("two bigints can be subtracted") {
        SECTION("foobar") {
            x = {"3"};
            y = {"3"};
            REQUIRE(x == y);
        }
    }
}
