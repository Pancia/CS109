#include <cstdlib>
#include <exception>
#include <limits>
#include <stack>
#include <stdexcept>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/foreach.hpp>
#include <boost/range/combine.hpp>

#include "bigint.h"
#include "debug.h"

using boost::adaptors::reverse;

bigint::bigint(long that) {
    bigint(std::to_string(that));
}

bigint::bigint(const std::string& that) {
    this->negative = false;
    for (char val : reverse(that)) {
        if (val == '_') {
            this->negative = true;
        } else {
            this->big_value.push_back(val);
        }
    };
}

bigint::operator std::string() const {
    std::string ret;
    if (this->negative) {
        ret += "-";
    }
    for (char val : reverse(this->big_value)) {
        ret += val;
    }
    return ret;
}

bigint operator+(const bigint& left, const bigint& right) {
    DEBUGF('b', "LEFT:" << left << "RIGHT:" << right);
    bigint sum;
    if (left.negative == right.negative) {
        sum.big_value = bigint::do_bigadd
            (left.big_value, right.big_value);
        sum.negative = left.negative;
    } else {
        //else do_bigless -> do_bigsub (big - small)
        sum.big_value = bigint::do_bigadd
            (left.big_value, right.big_value);
        sum.negative = true;
    }
    return sum;
}

bigint operator-(const bigint& left, const bigint& right) {
    DEBUGF('b', "LEFT:" << left << "RIGHT:" << right);
    throw std::runtime_error("-2 STUB");
}

bigint operator+(const bigint& right) {
    DEBUGF('b', "RIGHT:" << right);
    throw std::runtime_error("+1 STUB");
}

bigint operator-(const bigint& right) {
    DEBUGF('b', "RIGHT:" << right);
    throw std::runtime_error("-1 STUB");
}

bigint::bigvalue_t bigint::do_bigadd(
        const bigvalue_t& left,
        const bigvalue_t& right) {
    bigint::bigvalue_t padded_left = left;
    bigint::bigvalue_t padded_right = right;
    if (left.size() < right.size()) {
        padded_left.resize(right.size(), '0');
    } else if (right.size() < left.size()) {
        padded_right.resize(left.size(), '0');
    }
    bigvalue_t sum;
    char l, r; int carry {0};
    BOOST_FOREACH(boost::tie(l, r),
            boost::combine(padded_left, padded_right)) {
        char res = l + r - '0' + carry;
        carry = 0;
        if (res - '0' > 9) {
            res -= 10;
            carry = 1;
        }
        sum.push_back(res);
    }
    return bigint::trim_zeros(sum);
}

bigint::bigvalue_t bigint::do_bigsub(
        const bigvalue_t& left,
        const bigvalue_t& right) {
    (void) left; (void) right;
    throw std::runtime_error("do_bigsub STUB");
}

bigint::bigvalue_t bigint::trim_zeros(bigint::bigvalue_t val) {
    for (char c : reverse(val)) {
        if (c == '0')
            val.pop_back();
        else break;
    }
    return val;
}

long bigint::to_long() const {
    DEBUGF('b', "bigint::to_long()");
    throw std::runtime_error("to_long STUB");
    //if (*this <= bigint(std::numeric_limits<long>::min())
    //        or *this > bigint(std::numeric_limits<long>::max()))
    //    throw std::range_error("bigint__to_long: out of range");
    //return long_value;
}

bool abs_less(const long& left, const long& right) {
    return left < right;
}

//
// Multiplication algorithm.
//
bigint operator*(const bigint& left, const bigint& right) {
    DEBUGF('b', "LEFT:" << left << "RIGHT:" << right);
    throw std::runtime_error("*2 STUB");
}

//
// Division algorithm.
//
void multiply_by_2(bigint::unumber& unumber_value) {
    unumber_value *= 2;
}

void divide_by_2(bigint::unumber& unumber_value) {
    unumber_value /= 2;
}

bigint::quot_rem divide(const bigint& left, const bigint& right) {
    DEBUGF('b', "LEFT:" << left << "RIGHT:" << right);
    throw std::runtime_error("/2 STUB");
    //if (right == 0)
    //    throw std::domain_error("divide by 0");
    //using unumber = bigint::unumber;
    //static unumber zero = 0;
    //if (right == 0)
    //    throw std::domain_error("bigint::divide");
    //unumber divisor = right;
    //unumber quotient = 0;
    //unumber remainder = left;
    //unumber power_of_2 = 1;
    //while (abs_less(divisor, remainder)) {
    //    multiply_by_2(divisor);
    //    multiply_by_2(power_of_2);
    //}
    //while (abs_less(zero, power_of_2)) {
    //    if (not abs_less(remainder, divisor)) {
    //        remainder = remainder - divisor;
    //        quotient = quotient + power_of_2;
    //    }
    //    divide_by_2(divisor);
    //    divide_by_2(power_of_2);
    //}
    //return {quotient, remainder};
}

bigint operator/(const bigint& left, const bigint& right) {
    return divide(left, right).first;
}

bigint operator%(const bigint& left, const bigint& right) {
    return divide(left, right).second;
}

bool operator==(const bigint& left, const bigint& right) {
    DEBUGF('b', "LEFT:" << left << "RIGHT:" << right);
    return (left.negative == right.negative)
        && (left.big_value == right.big_value);
}

bool operator<(const bigint& left, const bigint& right) {
    DEBUGF('b', "LEFT:" << left << "RIGHT:" << right);
    throw std::runtime_error("< STUB");
}

std::ostream& operator<<(std::ostream& out, const bigint& that) {
    out << std::string(that);
    return out;
}

bigint pow(const bigint& base, const bigint& exponent) {
    DEBUGF('^', "base = " << base << ", exponent = " << exponent);
    if (base == 0)
        return 0;
    bigint base_copy = base;
    long expt = exponent.to_long();
    bigint result = 1;
    if (expt < 0) {
        base_copy = 1 / base_copy;
        expt = - expt;
    }
    while (expt > 0) {
        if (expt & 1) { //odd
            result = result * base_copy;
            --expt;
        } else { //even
            base_copy = base_copy * base_copy;
            expt /= 2;
        }
    }
    DEBUGF('^', "result = " << result);
    return result;
}
