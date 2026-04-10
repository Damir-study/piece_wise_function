#include "power_function.h"
#include <stdexcept>

template <typename X, typename Y>
power_function<X, Y>::power_function(int exponent) : exponent(exponent) {
    if (exponent < 0) {
        throw std::invalid_argument("Exponent must be non-negative");
    }
}

template <typename X, typename Y>
Y power_function<X, Y>::calculate(const X& x) const {
    Y result = Y(1);
    Y value = static_cast<Y>(x);

    for (int i = 0; i < exponent; ++i) {
        result = result * value;
    }

    return result;
}

template <typename X, typename Y>
function<X, Y>* power_function<X, Y>::clone() const {
    return new power_function<X, Y>(*this);
}

template <typename X, typename Y>
bool power_function<X, Y>::is_continuous_on(const interval<X>&) const {
    return true;
}

template <typename X, typename Y>
monotonicity_type power_function<X, Y>::get_monotonicity_on(const interval<X>& inter) const {
    const X zero = X(0);

    if (inter.get_left() == inter.get_right()) {
        return monotonicity_type::constant;
    }

    if (exponent == 0) {
        return monotonicity_type::constant;
    }

    if (exponent % 2 == 1) {
        return monotonicity_type::increasing;
    }

    if (inter.get_right() <= zero) {
        return monotonicity_type::decreasing;
    }

    if (zero <= inter.get_left()) {
        return monotonicity_type::increasing;
    }

    return monotonicity_type::not_monotonic;
}
