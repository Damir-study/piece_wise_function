#include "logarithm_function.h"
#include <cmath>
#include <stdexcept>

template <typename X, typename Y>
logarithm_function<X, Y>::logarithm_function(const X& base) : base(base) {
    if (base <= X(0)) {
        throw std::invalid_argument("Logarithm base must be positive");
    }

    if (base == X(1)) {
        throw std::invalid_argument("Logarithm base must not be equal to one");
    }
}

template <typename X, typename Y>
Y logarithm_function<X, Y>::calculate(const X& x) const {
    if (x <= X(0)) {
        throw std::invalid_argument("Logarithm is undefined for non-positive arguments");
    }

    return static_cast<Y>(std::log(static_cast<long double>(x)) /
                          std::log(static_cast<long double>(base)));
}

template <typename X, typename Y>
function<X, Y>* logarithm_function<X, Y>::clone() const {
    return new logarithm_function<X, Y>(*this);
}

template <typename X, typename Y>
bool logarithm_function<X, Y>::is_continuous_on(const interval<X>& inter) const {
    if (inter.get_left() < X(0)) {
        return false;
    }

    if (inter.get_left() == X(0) && inter.left_included()) {
        return false;
    }

    return true;
}

template <typename X, typename Y>
monotonicity_type logarithm_function<X, Y>::get_monotonicity_on(const interval<X>& inter) const {
    if (!is_continuous_on(inter)) {
        return monotonicity_type::undefined;
    }

    if (inter.get_left() == inter.get_right()) {
        return monotonicity_type::constant;
    }

    if (base > X(1)) {
        return monotonicity_type::increasing;
    }

    return monotonicity_type::decreasing;
}
