#include "interval.h"
#include <stdexcept>

template <typename X>
interval<X>::interval() : left(), right(), is_left_included(true), is_right_included(true) {}

template <typename X>
interval<X>::interval(const X& left, const X& right, bool is_left_included, bool is_right_included)
    : left(left), right(right), is_left_included(is_left_included), is_right_included(is_right_included) {
    if (right < left) {
        throw std::invalid_argument("Left bound must not be greater than right bound");
    }
}

template <typename X>
void interval<X>::set_left(const X& x) {
    if (right < x) {
        throw std::invalid_argument("Left bound must not be greater than right bound");
    }
    left = x;
}

template <typename X>
void interval<X>::set_right(const X& x) {
    if (x < left) {
        throw std::invalid_argument("Right bound must not be less than left bound");
    }
    right = x;
}

template <typename X>
bool interval<X>::contains(const X& x) const{
    if (x > left && x < right) return true;
    if (x == left and is_left_included) return true;
    if (x == right and is_right_included) return true;
    return false;
}

template <typename X>
bool interval<X>::intersects(const interval& other) const {
    if (right < other.get_left() || other.get_right() < left) {
        return false;
    }

    if (right == other.get_left()) {
        return is_right_included && other.left_included();
    }

    if (other.get_right() == left) {
        return other.right_included() && is_left_included;
    }

    return true;
}

template <typename X>
interval<X> interval<X>::get_intersection(const interval<X>& other) const {
    if (!intersects(other)) {
        throw std::invalid_argument("Intervals do not intersect");
    }

    X new_left = left < other.get_left() ? other.get_left() : left;
    X new_right = right < other.get_right() ? right : other.get_right();

    bool new_left_included;
    if (left == other.get_left()) {
        new_left_included = is_left_included && other.left_included();
    } else if (new_left == left) {
        new_left_included = is_left_included;
    } else {
        new_left_included = other.left_included();
    }

    bool new_right_included;
    if (right == other.get_right()) {
        new_right_included = is_right_included && other.right_included();
    } else if (new_right == right) {
        new_right_included = is_right_included;
    } else {
        new_right_included = other.right_included();
    }

    return interval<X>(new_left, new_right, new_left_included, new_right_included);
}
