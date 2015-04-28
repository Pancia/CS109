#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <exception>
#include <limits>
#include <stack>
#include <stdexcept>

#include "bigint.h"
#include "debug.h"

bigint::bigint(long that) {
    bigint(std::to_string(that));
}

bigint::bigint(const std::string& that) {
    this->negative = false;
    std::string reversed = that;
    std::reverse(reversed.begin(), reversed.end());
    for (char val : reversed) {
        if (val == '_') {
            this->negative = true;
        } else {
            this->big_value.push_back(val);
        }
    }
}

bigint::operator std::string() const {
    std::string ret;
    if (this->big_value.size() == 0) {
        ret = "0";
        return ret;
    }
    if (this->negative) {
        ret += "-";
    }
    bigint::bigvalue_t reversed = this->big_value;
    std::reverse(reversed.begin(), reversed.end());
    for (char val : reversed) {
        ret += val;
    }
    return ret;
}

std::string bigint::to_string(bigint::bigvalue_t vec) {
    std::string ret;
    bigint::bigvalue_t reversed = vec;
    std::reverse(reversed.begin(), reversed.end());
    for (char val : reversed) {
        ret += val;
    }
    return ret;
}

bigint operator +(const bigint& left, const bigint& right) {
    DEBUGF('b', "LEFT:" << left << "RIGHT:" << right);
    bigint sum;
    if (left.negative == right.negative) {
        sum.big_value = bigint::do_bigadd
            (left.big_value, right.big_value);
        sum.negative = left.negative;
    } else {
        if (bigint::do_bigless(left.big_value, right.big_value)) {
            sum.big_value = bigint::do_bigsub
                (left.big_value, right.big_value);
            sum.negative = left.negative;
        } else {
            sum.big_value = bigint::do_bigsub
                (right.big_value, left.big_value);
            sum.negative = right.negative;
        }
    }
    return sum;
}

bigint operator -(const bigint& left, const bigint& right) {
    DEBUGF('b', "LEFT:" << left << "RIGHT:" << right);
    bigint sub;
    if (left.negative == right.negative) {
        sub.big_value = bigint::do_bigsub
            (left.big_value, right.big_value);
        sub.negative = left.negative;
    } else {
        if (bigint::do_bigless(left.big_value, right.big_value)) {
            sub.big_value = bigint::do_bigadd
                (left.big_value, right.big_value);
            sub.negative = left.negative;
        } else {
            sub.big_value = bigint::do_bigadd
                (right.big_value, left.big_value);
            sub.negative = right.negative;
        }
    }
    return sub;
}

bigint operator +(const bigint& right) {
    DEBUGF('b', "RIGHT:" << right);
    bigint copy = right;
    copy.negative = false;
    return copy;
}

bigint operator -(const bigint& right) {
    DEBUGF('b', "RIGHT:" << right);
    bigint copy = right;
    copy.negative = true;
    return copy;
}

bigint::bigvalue_t bigint::do_bigadd(
        const bigvalue_t& left,
        const bigvalue_t& right) {
    bigint::bigvalue_t padded_left = left;
    bigint::bigvalue_t padded_right = right;
    if (left.size() < right.size()+1) {
        padded_left.resize(right.size(), '0');
    } else if (right.size() < left.size()+1) {
        padded_right.resize(left.size(), '0');
    }
    bigvalue_t sum; int carry {0};
    for (size_t i = 0; i < padded_left.size(); i++) {
        char l = padded_left[i];
        char r = padded_right[i];
        char res = l + r - '0' + carry;
        carry = 0;
        if (res - '0' > 9) {
            res -= 10;
            carry = 1;
        }
        sum.push_back(res);
    }
    sum.push_back(carry + '0');
    return bigint::trim_zeros(sum);
}

bigint::bigvalue_t bigint::do_bigsub(
        const bigvalue_t& left,
        const bigvalue_t& right) {
    bigint::bigvalue_t padded_left = left;
    bigint::bigvalue_t padded_right = right;
    if (left.size() < right.size()) {
        padded_left.resize(right.size(), '0');
    } else if (right.size() < left.size()) {
        padded_right.resize(left.size(), '0');
    }
    bigint::bigvalue_t sub;
    int borrow {0};
    for (size_t i = 0; i < padded_left.size(); i++) {
        int l = padded_left[i] - '0';
        int r = padded_right[i] - '0';
        char res = l - r + '0' - borrow;
        if (res - '0' < 0) {
            res += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        sub.push_back(res);
    }
    return bigint::trim_zeros(sub);
}

bigint::bigvalue_t bigint::trim_zeros(bigint::bigvalue_t val) {
    bigint::bigvalue_t reversed = val;
    std::reverse(reversed.begin(), reversed.end());
    for (char c : reversed) {
        if (c == '0')
            val.pop_back();
        else break;
    }
    return val;
}

long bigint::to_long() const {
    DEBUGF('b', "bigint::to_long()");
    if (*this >= bigint{"2147483647"})
        throw std::range_error("bigint::to_long: out of range");
    long long_bigint = 0;
    for (size_t i = 0; i < this->big_value.size(); i++) {
        if (this->negative) {
            long_bigint -= ((this->big_value[i]-'0') * std::pow(10, i));
        } else {
            long_bigint += ((this->big_value[i]-'0') * std::pow(10, i));
        }
    }
    return long_bigint;
}

//
// Returns true if left is bigger (or eq?) than right
//
bool bigint::do_bigless(
        const bigvalue_t& left,
        const bigvalue_t& right) {
    if (left.size() < right.size())
        return false;
    if (left.size() > right.size())
        return true;
    for (size_t i = right.size(); i > 0; i--) {
        if (left.at(i-1) < right.at(i-1))
            return false;
        if (left.at(i-1) > right.at(i-1))
            return true;
    }

    return true;
}

//
// Multiplication algorithm.
//
bigint operator *(const bigint& left, const bigint& right) {
    DEBUGF('b', "LEFT:" << left << "RIGHT:" << right);
    bigint mul;
    mul.big_value = bigint::do_bigmul(left.big_value, right.big_value);
    mul.negative = (left.negative == right.negative ? false : true);
    return mul;
}

bigint::bigvalue_t bigint::do_bigmul(
        const bigvalue_t& left,
        const bigvalue_t& right) {
    DEBUGF('b', "LEFT:" << to_string(left)
            << "RIGHT:" << to_string(right));
    bigvalue_t product(left.size()+right.size(), '0');
    for (size_t i = 0; i < left.size(); i++) {
        int c = 0;
        for (size_t j = 0; j < right.size(); j++) {
            int d = (product.at(i+j)-'0')
                + ((left.at(i)-'0')*(right.at(j)-'0')) + c;
            product[i+j] = (d % 10) + '0';
            c = d / 10;
        }
        product[i + right.size()] = c + '0';
    }
    return bigint::trim_zeros(product);
}

//
// Division algorithm.
//
bigint::bigvalue_t bigint::multiply_by_2(bigint::bigvalue_t& val) {
    val = do_bigmul(val, bigint::bigvalue_t{'2'});
    return val;
}

bigint::bigvalue_t bigint::divide_by_2(bigvalue_t& val) {
    bigvalue_t tmp = val;
    bigvalue_t size_one {'1'};
    bigvalue_t size_two {'2'};
    bigvalue_t quotient {'0'};
    bigvalue_t pow_ten = bigint("1024").big_value;
    bigvalue_t pow_nine = bigint("512").big_value;
    bigvalue_t pow_eight = bigint("256").big_value;

    for (;!tmp.empty() && tmp != size_one;) {
        // Inflate value until pow_big
        //   is bigger than tmp
        bigvalue_t pow_big = pow_ten;
        bigvalue_t pow_mid = pow_nine;
        bigvalue_t pow_low = pow_eight;
        while (do_bigless(tmp, pow_big)) {
            pow_big = trim_zeros(pow_big);
            pow_mid = trim_zeros(pow_mid);
            pow_low = trim_zeros(pow_low);
            pow_big = multiply_by_2(pow_big);
            pow_mid = multiply_by_2(pow_mid);
            pow_low = multiply_by_2(pow_low);
        }

        // If we can subtract tmp by pow_mid
        //   we can also add to quotient by pow_low
        if (do_bigless(tmp, pow_mid)) {
            tmp = do_bigsub(tmp, pow_mid);
            quotient = do_bigadd(quotient, pow_low);
            // Otherwise do small sub
        } else if (do_bigless(tmp, size_one)) {
            tmp = do_bigsub(tmp, size_two);
            quotient = do_bigadd(quotient, size_one);
        }
    }

    val = quotient;
    return quotient;
}

bigint::quot_rem divide(const bigint& left, const bigint& right) {
    DEBUGF('b', "LEFT:" << left << "RIGHT:" << right);
    if (right == bigint{"0"})
        throw std::domain_error("divide by 0");
    static bigint zero {"0"};
    if (right == zero)
        throw std::domain_error("bigint::divide");
    bigint divisor = right;
    bigint quotient {"0"};
    bigint remainder = left;
    bigint power_of_2 {"1"};
    while (not bigint::do_bigless
            (divisor.big_value, remainder.big_value)) {
        bigint::multiply_by_2(divisor.big_value);
        bigint::multiply_by_2(power_of_2.big_value);
    }
    while (not bigint::do_bigless
            (zero.big_value, power_of_2.big_value)) {
        if (not bigint::do_bigless
                (divisor.big_value, remainder.big_value)) {
            remainder = remainder - divisor;
            quotient = quotient + power_of_2;
        }
        bigint::divide_by_2(divisor.big_value);
        bigint::divide_by_2(power_of_2.big_value);
    }
    if (remainder == right) {
        quotient = quotient + bigint{"1"};
        remainder = remainder - right;
    }
    return {quotient, remainder};
}

bigint operator /(const bigint& left, const bigint& right) {
    return divide(left, right).first;
}

bigint operator %(const bigint& left, const bigint& right) {
    return divide(left, right).second;
}

bool operator ==(const bigint& left, const bigint& right) {
    DEBUGF('b', "LEFT:" << left << "RIGHT:" << right);
    return (left.negative == right.negative)
        && (left.big_value == right.big_value);
}

bool operator <(const bigint& left, const bigint& right) {
    DEBUGF('b', "LEFT:" << left << "RIGHT:" << right);
    if (left.negative != right.negative) {
        return left.negative;
    } else {
        return !bigint::do_bigless(left.big_value, right.big_value);
    }
}

std::ostream& operator <<(std::ostream& out, const bigint& that) {
    out << std::string(that);
    return out;
}

bigint pow(const bigint& base, const bigint& exponent) {
    DEBUGF('^', "base = " << base << ", exponent = " << exponent);
    bigint zero = bigint{""};
    bigint one = bigint{"1"};
    if (base == zero)
        return zero;
    bigint base_copy = base;
    long expt = exponent.to_long();
    DEBUGF('^', "expt = " << expt);
    bigint result = one;
    if (expt < zero) {
        base_copy = one / base_copy;
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
        DEBUGF('^', "expt = " << expt);
        DEBUGF('^', "result = " << result);
        DEBUGF('^', "base_copy = " << base_copy);
    }
    DEBUGF('^', "result = " << result);
    return result;
}

//Exponent algorithm
//gint pow (const bigint& base, const bigint& exponent) {
// DEBUGF ('^', "base = " << base << ", exponent = " << exponent);
// bigint zero = bigint("0");
// bigint one = bigint("1");
// zero = zero.zero_clear(zero);
// if (base == zero){
//    return zero;
// }
// bigint base_copy = base;
// long expt = exponent.to_long();
// bigint result = bigint("1");
// if (expt < 0) {
//    base_copy = one / base_copy;
//    expt = - expt;
// }
// while (expt > 0) {
//    if (expt & 1) { //odd
//       result = result * base_copy;
//       --expt;
//    } else { //even
//       base_copy = base_copy * base_copy;
//       expt /= 2;
//    }
// }
// DEBUGF ('^', "result = " << result);
// return result;
