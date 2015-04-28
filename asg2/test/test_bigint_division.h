#include <ctime>

#include "../bigint.h"

namespace test_bigint_division {
    bigint x, y, result, expected;
    std::time_t start, end;
    TEST_CASE("two bigvalue_t can be divided") {
        //FIXME: Remove this section,
        // change stuff back to private
        std::vector<unsigned char> x, expected;
        SECTION("do_bigadd") {
            const bigint::bigvalue_t x {'9'};
            const bigint::bigvalue_t y {'1'};
            const bigint::bigvalue_t expected {'0', '1'};
            REQUIRE(bigint::do_bigadd(x, y) == expected);
        }

        SECTION("small values") {
            x = {'4', '2'};
            expected = {'2', '1'};
            REQUIRE(bigint::divide_by_2(x) == expected);
        }

        SECTION("small values") {
            x = {'3', '2'};
            expected = {'1', '1'};
            REQUIRE(bigint::divide_by_2(x) == expected);
        }

        SECTION("small values") {
            x = {'1'};
            expected = {'0'};
            REQUIRE(bigint::divide_by_2(x) == expected);
        }
    }

    TEST_CASE("two bigints can be divided") {
        SECTION("small and simple values") {
            x = {"12"};
            y = { "3"};
            expected = {"4"};
            REQUIRE((x / y) == expected);
            REQUIRE((x % y) == bigint{""});
        }

        SECTION("larger values") {
            x = {"1241351"};
            y = {    "234"};
            result = (x / y);
            expected = {"5304"};
            REQUIRE(result == expected);
        }

        SECTION("big and complex values") {
            start = std::time(nullptr);
            x = {"987329587109715087135"};
            y = {        "0398105178325"};
            result = (x / y);
            end = std::time(nullptr);
            std::cout << "TIME:" << (end - start) << std::endl;
            expected = {"2480072204"};
            REQUIRE(result == expected);
        }

        SECTION("enormous values") {
            start = std::time(nullptr);
            x = {"587295701928365012937932865702610923841982750129653"};
            y = {"2799831461"};
            result = (x / y);
            end = std::time(nullptr);
            std::cout << "TIME:" << (end - start) << std::endl;
            expected = {"209761091018887230418878726108653767942635"};
            REQUIRE(result == expected);
        }

        SECTION("ginormous values") {
            start = std::time(nullptr);
            x = {"58729570192836501293793286570261092384198275012965358729570192836501293793"};
            y = {"2582739"};
            result = (x / y);
            end = std::time(nullptr);
            std::cout << "TIME:" << (end - start) << std::endl;
            expected = {"22739258667963158992756638038245867036583361699716989881505716542206"};
            REQUIRE(result == expected);
        }
    }
}
