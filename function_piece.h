#ifndef FUNCTION_PIECE_H
#define FUNCTION_PIECE_H

#include "interval.h"
#include "function.h"

template <typename X, typename Y>
class function_piece {
public:
    function_piece() : inter(), func(nullptr) {}
    function_piece(const interval<X>& inter, const function<X, Y>& func) : inter(inter), func(func.clone()) {};
    function_piece(const function_piece<X, Y>& other);
    function_piece<X, Y>& operator=(const function_piece<X, Y>& other);
    ~function_piece();

    bool contains(const X& x) const;
    Y calculate(const X& x) const;

    const interval<X>& get_interval() const {return inter;}
    const function<X, Y>& get_function() const {return *func;}
    const X& get_left() const {return inter.get_left();}
    const X& get_right() const {return inter.get_right();}
    bool has_function() const {return func != nullptr;}
    bool left_included() const {return inter.left_included();}
    bool right_included() const {return inter.right_included();}

private:
    interval<X> inter;
    function<X,Y>* func;
};

#include "function_piece.tpp"

#endif
