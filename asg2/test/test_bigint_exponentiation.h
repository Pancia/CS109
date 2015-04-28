#include "../bigint.h"

namespace test_bigint_exponentiation {
    bigint x, y, result, expected;
    TEST_CASE("two bigints can be exponentiated") {
        SECTION("small values") {
            x = {"18"};
            y = {"19"};
            result = pow(x, y);
            expected = {"708235345355337676357632"};
            REQUIRE(result == expected);
        }

        SECTION("medium values") {
            x = {"8888"};
            y = {"19"};
            result = pow(x, y);
            expected = {"1064821650178425363971534624689408719801243970561469706573552210737786519552"};
            REQUIRE(result == expected);
        }

        SECTION("large values") {
            x = {"82379279"};
            y = {"23"};
            result = pow(x, y);
            expected = {"115828035858209300941750454485085588138929064896190090242409960653814427661340565157835360614130540104600598386151235853898178472929495642060487954948028584004260672281097775205360239"};
            REQUIRE(result == expected);
        }
    }
}
