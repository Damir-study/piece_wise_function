#ifndef FUNCTION_H
#define FUNCTION_H

template <typename X>
class interval;

enum class monotonicity_type {
    increasing,
    decreasing,
    constant,
    not_monotonic,
    undefined
};

template <typename X, typename Y>
class function {
public:
    virtual Y calculate(const X&) const = 0;
    virtual function<X,Y>* clone() const = 0;
    virtual bool is_continuous_on(const interval<X>& inter) const = 0;
    virtual monotonicity_type get_monotonicity_on(const interval<X>& inter) const = 0;
    virtual ~function() {}
};


#endif
