#ifndef POWER_FUNCTION_H
#define POWER_FUNCTION_H

#include "../function.h"
#include "../interval.h"

template <typename X, typename Y>
class power_function : public function<X, Y> {
public:
    power_function(int exponent);
    ~power_function() override = default;

    Y calculate(const X& x) const override;
    function<X, Y>* clone() const override;
    bool is_continuous_on(const interval<X>& inter) const override;
    monotonicity_type get_monotonicity_on(const interval<X>& inter) const override;

    int get_exponent() const {return exponent;}

private:
    int exponent;
};

#include "power_function.tpp"

#endif
