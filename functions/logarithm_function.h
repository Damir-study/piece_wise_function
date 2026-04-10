#ifndef LOGARITHM_FUNCTION_H
#define LOGARITHM_FUNCTION_H

#include "../function.h"
#include "../interval.h"

template <typename X, typename Y>
class logarithm_function : public function<X, Y> {
public:
    logarithm_function(const X& base);
    ~logarithm_function() override = default;

    Y calculate(const X& x) const override;
    function<X, Y>* clone() const override;
    bool is_continuous_on(const interval<X>& inter) const override;
    monotonicity_type get_monotonicity_on(const interval<X>& inter) const override;

    const X& get_base() const {return base;}

private:
    X base;
};

#include "logarithm_function.tpp"

#endif
