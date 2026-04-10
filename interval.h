#ifndef INTERVAL_H
#define INTERVAL_H

template <typename X>
class interval {
public:
    interval();
    interval(const X& left, const X& right, bool is_left_included = true, bool is_right_included = true);

    bool contains(const X& x) const;
    bool intersects(const interval& other) const;
    interval<X> get_intersection(const interval<X>& other) const;

    void set_left(const X& x);
    void set_right(const X& x);

    const X& get_left() const {return left;}
    const X& get_right() const {return right;}
    bool left_included() const {return is_left_included;}
    bool right_included() const {return is_right_included;}
private:
    X left;
    X right;
    bool is_left_included;
    bool is_right_included;
};

#include "interval.tpp"

#endif
