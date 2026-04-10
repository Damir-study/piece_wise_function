#ifndef COSINE_FUNCTION_H
#define COSINE_FUNCTION_H

#include "../function.h"
#include "../interval.h"

template <typename X, typename Y>
class cosine_function : public function<X, Y> {
public:
    ~cosine_function() override = default;

    Y calculate(const X& x) const override;
    function<X, Y>* clone() const override;
    bool is_continuous_on(const interval<X>& inter) const override;
    monotonicity_type get_monotonicity_on(const interval<X>& inter) const override;

private:
    static bool has_critical_point_inside(const interval<X>& inter);
    static constexpr long double pi = 3.141592653589793238462643383279502884L;
};

#include "cosine_function.tpp"

#endif
