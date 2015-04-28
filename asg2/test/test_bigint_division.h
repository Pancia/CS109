#include "../bigint.h"

namespace test_bigint_division {
    bigint x, y, result, expected;

    TEST_CASE("two bigints can be divided") {
        SECTION("small and simple values") {
            x = {"12"};
            y = { "3"};
            result = (x / y);
            expected = {"4"};
            REQUIRE(result == expected);
            result = (x % y);
            expected = {""};
            REQUIRE(result == expected);
        }

        SECTION("larger values") {
            x = {"1241351"};
            y = {    "234"};
            result = (x / y);
            expected = {"5304"};
            REQUIRE(result == expected);
            result = (x % y);
            expected = {"215"};
            REQUIRE(result == expected);
        }

        SECTION("big and complex values") {
            x = {"987329587109715087135"};
            y = {        "0398105178325"};
            result = (x / y);
            expected = {"2480072204"};
            REQUIRE(result == expected);
            result = (x % y);
            expected = {"77419308835"};
            REQUIRE(result == expected);
        }

        SECTION("enormous values") {
            x = {"587295701928365012937932865702610923841982750129653"};
            y = {"2799831461"};
            result = (x / y);
            expected = {"209761091018887230418878726108653767942635"};
            REQUIRE(result == expected);
            result = (x % y);
            expected = {"33889918"};
            REQUIRE(result == expected);
        }

        SECTION("ginormous values") {
            x = {"58729570192836501293793286570261092384198275012965358729570192836501293793"};
            y = {"2582739"};
            result = (x / y);
            expected = {"22739258667963158992756638038245867036583361699716989881505716542206"};
            REQUIRE(result == expected);
            result = (x % y);
            expected = {"711559"};
            REQUIRE(result == expected);
        }
    }
}
