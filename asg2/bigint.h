#ifndef __BIGINT_H__
#define __BIGINT_H__

#include <exception>
#include <iostream>
#include <utility>

#include "debug.h"

//
// Define class bigint
//
class bigint {
    friend std::ostream& operator<< (std::ostream&, const bigint&);
private:
    using digit_t = unsigned char;
    using bigvalue_t = std::vector<digit_t>;
    bool negative;
    bigvalue_t big_value;
    static bigvalue_t do_bigadd(const bigvalue_t&, const bigvalue_t&);
    static bigvalue_t do_bigsub(const bigvalue_t&, const bigvalue_t&);
    static bigvalue_t do_bigmul(const bigvalue_t&, const bigvalue_t&);
    static bool do_bigless(const bigvalue_t&, const bigvalue_t&);
    static bigint::bigvalue_t trim_zeros(bigvalue_t val);

    using quot_rem = std::pair<bigint,bigint>;
    friend quot_rem divide(const bigint&, const bigint&);
public:
    static bigint::bigvalue_t multiply_by_2(bigint::bigvalue_t& val);
    static bigint::bigvalue_t divide_by_2(bigint::bigvalue_t& val);
    operator std::string() const;

    //
    // Ensure synthesized members are genrated.
    //
    bigint() = default;
    bigint(const bigint&) = default;
    bigint(bigint&&) = default;
    bigint& operator=(const bigint&) = default;
    bigint& operator=(bigint&&) = default;
    ~bigint() = default;

    //
    // Extra ctors to make bigints.
    //
    bigint(const long);
    bigint(const std::string&);

    // To string helpers
    static std::string to_string(bigvalue_t);

    //
    // Basic add/sub operators.
    //
    friend bigint operator+(const bigint&, const bigint&);
    friend bigint operator-(const bigint&, const bigint&);
    friend bigint operator+(const bigint&);
    friend bigint operator-(const bigint&);
    long to_long() const;

    //
    // Extended operators implemented with add/sub.
    //
    friend bigint operator*(const bigint&, const bigint&);
    friend bigint operator/(const bigint&, const bigint&);
    friend bigint operator%(const bigint&, const bigint&);

    //
    // Comparison operators.
    //
    friend bool operator==(const bigint&, const bigint&);
    friend bool operator<(const bigint&, const bigint&);
};

//
// The rest of the operators do not need to be friends.
// Make the comparisons inline for efficiency.
//
bigint pow(const bigint& base, const bigint& exponent);

inline bool operator!=(const bigint &left, const bigint &right) {
    return not (left == right);
}
inline bool operator>(const bigint &left, const bigint &right) {
    return right < left;
}
inline bool operator<=(const bigint &left, const bigint &right) {
    return not (right < left);
}
inline bool operator>=(const bigint &left, const bigint &right) {
    return not (left < right);
}

#endif
