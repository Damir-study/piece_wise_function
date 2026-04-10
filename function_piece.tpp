#include "function_piece.h"
#include <stdexcept>

template <typename X, typename Y>
function_piece<X,Y>::function_piece(const function_piece<X, Y>& other) {
    inter = other.get_interval();
    func = other.get_function().clone();
}

template <typename X, typename Y>
function_piece<X, Y>& function_piece<X,Y>::operator=(const function_piece<X, Y>& other) {
    if (this == &other) {
        return *this;
    }

    function<X,Y>* new_func = other.get_function().clone();
    inter = other.get_interval();
    delete func;
    func = new_func;
    return *this;
}

template <typename X, typename Y>
function_piece<X,Y>::~function_piece() {
    delete func;
}


template <typename X, typename Y>
bool function_piece<X,Y>::contains(const X& x) const{
    return inter.contains(x);
}

template <typename X, typename Y>
Y function_piece<X,Y>::calculate(const X& x) const {
    if (func == nullptr) throw std::logic_error("function_piece is empty");
    if (!contains(x)) throw std::invalid_argument("x is not defined in this piece");
    return func->calculate(x);
}
