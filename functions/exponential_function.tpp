#include "exponential_function.h"
#include <cmath>
#include <stdexcept>

template <typename X, typename Y>
exponential_function<X, Y>::exponential_function(const X& base) : base(base) {
    if (base <= X(0)) {
        throw std::invalid_argument("Exponential base must be positive");
    }
}

template <typename X, typename Y>
Y exponential_function<X, Y>::calculate(const X& x) const {
    return static_cast<Y>(std::pow(static_cast<long double>(base), static_cast<long double>(x)));
}

template <typename X, typename Y>
function<X, Y>* exponential_function<X, Y>::clone() const {
    return new exponential_function<X, Y>(*this);
}

template <typename X, typename Y>
bool exponential_function<X, Y>::is_continuous_on(const interval<X>&) const {
    return true;
}

template <typename X, typename Y>
monotonicity_type exponential_function<X, Y>::get_monotonicity_on(const interval<X>& inter) const {
    if (inter.get_left() == inter.get_right()) {
        return monotonicity_type::constant;
    }

    if (base == X(1)) {
        return monotonicity_type::constant;
    }

    if (base > X(1)) {
        return monotonicity_type::increasing;
    }

    return monotonicity_type::decreasing;
}
