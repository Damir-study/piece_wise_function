#ifndef PIECEWISE_FUNCTION
#define PIECEWISE_FUNCTION

#include "function.h"
#include "sequence/sequence.h"
#include "function_piece.h"
#include "interval.h"
#include "sequence/IEnumerator.h"

template <typename X, typename Y>
class piecewise_function : public function<X,Y>, public IEnumerable<function_piece<X,Y>> {
public:
    piecewise_function();
    piecewise_function(sequence<function_piece<X,Y>>* pieces);
    piecewise_function(const piecewise_function<X,Y>& other);
    piecewise_function<X,Y>& operator=(const piecewise_function<X,Y>& other);
    ~piecewise_function() {delete pieces;}

    Y calculate(const X& x) const override;
    function<X,Y>* clone() const override {return new piecewise_function(*this);}

    int get_piece_count() const;
    function_piece<X,Y> get_piece(int index) const;
    function<X,Y>* get_function(int index) const;
    interval<X> get_interval(int index) const;

    void override_on(const interval<X>& interval, const function<X,Y>& func);

    bool is_defined_on(const interval<X>& inter) const;
    bool is_continuous_on(const interval<X>& inter) const override;
    monotonicity_type get_monotonicity_on(const interval<X>& inter) const;

    IEnumerator<function_piece<X,Y>>* get_enumerator() const override;

private:
    static void validate_pieces(const sequence<function_piece<X,Y>>* pieces);
    sequence<function_piece<X,Y>>* pieces;
};


#include "piecewise_function.tpp"


#endif
