#include "cosine_function.h"
#include <cmath>

template <typename X, typename Y>
Y cosine_function<X, Y>::calculate(const X& x) const {
    return static_cast<Y>(std::cos(static_cast<long double>(x)));
}

template <typename X, typename Y>
function<X, Y>* cosine_function<X, Y>::clone() const {
    return new cosine_function<X, Y>;
}

template <typename X, typename Y>
bool cosine_function<X, Y>::is_continuous_on(const interval<X>&) const {
    return true;
}

template <typename X, typename Y>
bool cosine_function<X, Y>::has_critical_point_inside(const interval<X>& inter) {
    const long double left = static_cast<long double>(inter.get_left());
    const long double right = static_cast<long double>(inter.get_right());

    long long start_k = static_cast<long long>(std::ceil(left / pi));
    long long end_k = static_cast<long long>(std::floor(right / pi));

    for (long long k = start_k; k <= end_k; ++k) {
        long double critical_point = static_cast<long double>(k) * pi;

        if (left < critical_point && critical_point < right) {
            return true;
        }
    }

    return false;
}

template <typename X, typename Y>
monotonicity_type cosine_function<X, Y>::get_monotonicity_on(const interval<X>& inter) const {
    if (inter.get_left() == inter.get_right()) {
        return monotonicity_type::constant;
    }

    if (has_critical_point_inside(inter)) {
        return monotonicity_type::not_monotonic;
    }

    auto left_value = calculate(inter.get_left());
    auto right_value = calculate(inter.get_right());

    if (left_value < right_value) return monotonicity_type::increasing;
    if (left_value > right_value) return monotonicity_type::decreasing;
    return monotonicity_type::constant;
}
