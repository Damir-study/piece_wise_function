#include "piecewise_function.h"
#include "sequence/mutable_list_sequence.h"
#include <stdexcept>

template <typename X, typename Y>
void append_piece_to_sequence(sequence<function_piece<X,Y>>*& result, const function_piece<X,Y>& piece) {
    sequence<function_piece<X,Y>>* old_result = result;
    result = result->append(piece);
    if (result != old_result) {
        delete old_result;
    }
}

template <typename X, typename Y>
void piecewise_function<X,Y>::validate_pieces(const sequence<function_piece<X,Y>>* pieces) {
    if (pieces == nullptr) {
        throw std::invalid_argument("Pieces sequence must not be nullptr");
    }

    IEnumerator<function_piece<X,Y>>* it = pieces->get_enumerator();

    try {
        if (!it->move_next()) {
            delete it;
            return;
        }

        function_piece<X,Y> previous_piece = it->get_current();

        if (!previous_piece.has_function()) {
            throw std::invalid_argument("Each function piece must contain a function");
        }

        while (it->move_next()) {
            const function_piece<X,Y>& current_piece = it->get_current();

            if (!current_piece.has_function()) {
                throw std::invalid_argument("Each function piece must contain a function");
            }

            if (current_piece.get_left() < previous_piece.get_left()) {
                throw std::invalid_argument("Pieces must be sorted by left bound");
            }

            if (previous_piece.get_right() > current_piece.get_left()) {
                throw std::invalid_argument("Pieces must not overlap");
            }

            if (previous_piece.get_right() == current_piece.get_left()) {
                if (previous_piece.right_included()) {
                    throw std::invalid_argument("Right boundary of the left piece must be open at a junction");
                }

                if (!current_piece.left_included()) {
                    throw std::invalid_argument("Left boundary of the right piece must be closed at a junction");
                }
            }

            previous_piece = current_piece;
        }

        delete it;
    }
    catch (...) {
        delete it;
        throw;
    }
}

template <typename X, typename Y>
piecewise_function<X,Y>::piecewise_function(const piecewise_function<X,Y>& other) {
    pieces = other.pieces->clone();
    try {
        validate_pieces(pieces);
    }
    catch (...) {
        delete pieces;
        throw;
    }
}

template <typename X, typename Y>
piecewise_function<X,Y>::piecewise_function(sequence<function_piece<X,Y>>* pieces) : pieces(pieces) {
    try {
        validate_pieces(this->pieces);
    }
    catch (...) {
        delete this->pieces;
        throw;
    }
}

template <typename X, typename Y>
piecewise_function<X,Y>::piecewise_function() {
    pieces = new mutable_list_sequence<function_piece<X,Y>>();
}

template <typename X, typename Y>
piecewise_function<X,Y>& piecewise_function<X,Y>::operator=(const piecewise_function<X,Y>& other) {
    if (this == &other) {
        return *this;
    }

    sequence<function_piece<X,Y>>* new_pieces = other.pieces->clone();
    try {
        validate_pieces(new_pieces);
    }
    catch (...) {
        delete new_pieces;
        throw;
    }
    delete pieces;
    pieces = new_pieces;
    return *this;
}

template <typename X, typename Y>
IEnumerator<function_piece<X,Y>>* piecewise_function<X,Y>::get_enumerator() const {
    return pieces->get_enumerator();
}

template <typename X, typename Y>
int piecewise_function<X,Y>::get_piece_count() const {
    return pieces->get_length();
}

template <typename X, typename Y>
function_piece<X,Y> piecewise_function<X,Y>::get_piece(int index) const {
    if (index < 0 || index >= get_piece_count()) {
        throw std::out_of_range("IndexOutOfRange");
    }

    IEnumerator<function_piece<X,Y>>* it = get_enumerator();

    for (int i = -1; i < index; ++i) {
        it->move_next();
    }

    function_piece<X,Y> res = it->get_current();

    delete it;
    return res;
}

template <typename X, typename Y>
function<X,Y>* piecewise_function<X,Y>::get_function(int index) const {
    function_piece<X,Y> piece = get_piece(index);
    return piece.get_function().clone();
}

template <typename X, typename Y>
interval<X> piecewise_function<X,Y>::get_interval(int index) const {
    function_piece<X,Y> piece = get_piece(index);
    return piece.get_interval();
}

template <typename X, typename Y>
Y piecewise_function<X,Y>::calculate(const X& x) const {
    IEnumerator<function_piece<X,Y>>* it = get_enumerator();

    while (it->move_next() && !it->get_current().contains(x)) {}

    try {
        function_piece<X,Y> correct_piece = it->get_current();
        Y res = correct_piece.calculate(x);
        delete it;
        return res;
    }
    catch (...) {
        delete it;
        throw;
    }
}

template <typename X, typename Y>
void piecewise_function<X,Y>::override_on(const interval<X>& new_interval, const function<X,Y>& new_func) {
    IEnumerator<function_piece<X,Y>>* it = get_enumerator();
    sequence<function_piece<X,Y>>* result = pieces->create_empty();
    bool inserted = false;
    bool new_piece_has_right_neighbor = false;

    // проверяем, есть ли у нового куска сосед справа
    while (it->move_next()) {
        const function_piece<X,Y>& old_piece = it->get_current();

        if (!old_piece.get_interval().intersects(new_interval)) {
            if (old_piece.get_left() == new_interval.get_right()) {
                new_piece_has_right_neighbor = true;
                break;
            }
            continue;
        }

        if (new_interval.get_right() < old_piece.get_right()) {
            new_piece_has_right_neighbor = true;
            break;
        }
    }

    delete it;
    it = get_enumerator();

    while (it->move_next()) {
        const function_piece<X,Y>& old_piece = it->get_current();

        // Если текущий участок и тот, который вставляем не пересекаются
        if (!old_piece.get_interval().intersects(new_interval)) {

            // Если новый участок вообще ни с кем не пересекается
            if (!inserted && new_interval.get_right() <= old_piece.get_left()) {
                interval<X> inserted_interval(
                    new_interval.get_left(),
                    new_interval.get_right(),
                    true,
                    new_piece_has_right_neighbor ? false : new_interval.right_included()
                );
                append_piece_to_sequence(result, function_piece<X,Y>(inserted_interval, new_func));
                inserted = true;
            }
            append_piece_to_sequence(result, old_piece);
            continue;
        }

        // если левая часть находится внутри текущего отрезка
        if (old_piece.get_left() < new_interval.get_left()) {
            interval<X> left_part(
                old_piece.get_left(),
                new_interval.get_left(),
                old_piece.left_included(),
                false
            );
            append_piece_to_sequence(result, function_piece<X,Y>(left_part, old_piece.get_function()));
        }

        if (!inserted) {
            interval<X> inserted_interval(
                new_interval.get_left(),
                new_interval.get_right(),
                true,
                new_piece_has_right_neighbor ? false : new_interval.right_included()
            );
            append_piece_to_sequence(result, function_piece<X,Y>(inserted_interval, new_func));
            inserted = true;
        }

        // если правая часть находится внутри текущего отрезка
        if (new_interval.get_right() < old_piece.get_right()) {
            interval<X> right_part(
                new_interval.get_right(),
                old_piece.get_right(),
                true,
                old_piece.right_included()
            );
            append_piece_to_sequence(result, function_piece<X,Y>(right_part, old_piece.get_function()));
        }
    }

    if (!inserted) {
        interval<X> inserted_interval(
            new_interval.get_left(),
            new_interval.get_right(),
            true,
            new_piece_has_right_neighbor ? false : new_interval.right_included()
        );
        append_piece_to_sequence(result, function_piece<X,Y>(inserted_interval, new_func));
    }

    delete it;
    try {
        validate_pieces(result);
    }
    catch (...) {
        delete result;
        throw;
    }
    delete pieces;
    pieces = result;
}

template <typename X, typename Y>
bool piecewise_function<X,Y>::is_defined_on(const interval<X>& inter) const {
    IEnumerator<function_piece<X,Y>>* it = get_enumerator();

    bool has_piece = false;
    function_piece<X,Y> previous_piece;
    bool has_previous_piece = false;

    while (it->move_next()) {
        const function_piece<X,Y>& current_piece = it->get_current();

        if (!current_piece.get_interval().intersects(inter)) {
            continue;
        }

        if (!has_piece) {
            has_piece = true;

            if (inter.get_left() < current_piece.get_left()) {
                delete it;
                return false;
            }

            if (inter.get_left() == current_piece.get_left() &&
                inter.left_included() &&
                !current_piece.left_included()) {
                delete it;
                return false;
            }
        }

        if (has_previous_piece) {
            if (previous_piece.get_right() < current_piece.get_left()) {
                delete it;
                return false;
            }

            if (previous_piece.get_right() > current_piece.get_left()) {
                delete it;
                return false;
            }

            if (!previous_piece.right_included() && !current_piece.left_included()) {
                delete it;
                return false;
            }
        }

        previous_piece = current_piece;
        has_previous_piece = true;

        if (inter.get_right() < current_piece.get_right()) {
            delete it;
            return true;
        }

        if (inter.get_right() == current_piece.get_right()) {
            if (!inter.right_included() || current_piece.right_included()) {
                delete it;
                return true;
            }
        }
    }

    delete it;

    if (!has_piece) {
        return false;
    }

    if (previous_piece.get_right() < inter.get_right()) {
        return false;
    }

    if (previous_piece.get_right() == inter.get_right() &&
        inter.right_included() &&
        !previous_piece.right_included()) {
        return false;
    }

    return true;
}

template <typename X, typename Y>
bool piecewise_function<X,Y>::is_continuous_on(const interval<X>& inter) const {
    if (!is_defined_on(inter)) {
        return false;
    }

    IEnumerator<function_piece<X,Y>>* it = get_enumerator();
    function_piece<X,Y> previous_piece;
    bool has_previous_piece = false;
    bool has_piece = false;

    while (it->move_next()) {
        const function_piece<X,Y>& current_piece = it->get_current();

        if (!current_piece.get_interval().intersects(inter)) {
            continue;
        }

        has_piece = true;

        interval<X> local_interval = current_piece.get_interval().get_intersection(inter);
        if (!current_piece.get_function().is_continuous_on(local_interval)) {
            delete it;
            return false;
        }

        if (has_previous_piece && previous_piece.get_right() == current_piece.get_left()) {
            try {
                Y left_value = previous_piece.get_function().calculate(current_piece.get_left());
                Y right_value = current_piece.get_function().calculate(current_piece.get_left());

                if (left_value != right_value) {
                    delete it;
                    return false;
                }
            }
            catch (...) {
                delete it;
                return false;
            }
        }

        previous_piece = current_piece;
        has_previous_piece = true;

        if (inter.get_right() < current_piece.get_right()) {
            break;
        }

        if (inter.get_right() == current_piece.get_right()) {
            if (!inter.right_included() || current_piece.right_included()) {
                break;
            }
        }
    }

    delete it;
    return has_piece;
}

template <typename X, typename Y>
monotonicity_type piecewise_function<X,Y>::get_monotonicity_on(const interval<X>& inter) const {
    IEnumerator<function_piece<X,Y>>* it = get_enumerator();

    bool has_piece = false;
    bool increasing_possible = true;
    bool decreasing_possible = true;
    bool all_constant = true;
    bool has_not_monotonic_piece = false;

    function_piece<X,Y> previous_piece;
    bool has_previous_piece = false;

    while (it->move_next()) {
        const function_piece<X,Y>& current_piece = it->get_current();

        if (!current_piece.get_interval().intersects(inter)) {
            continue;
        }

        // проверка, попадает ли хотя бы на первую часть данный нам интервал
        if (!has_piece) {
            has_piece = true;

            if (inter.get_left() < current_piece.get_left()) {
                delete it;
                return monotonicity_type::undefined;
            }

            if (inter.get_left() == current_piece.get_left() &&
                inter.left_included() &&
                !current_piece.left_included()) {
                delete it;
                return monotonicity_type::undefined;
            }
        }

        // проверка отношения кусков между собой
        if (has_previous_piece) {

            // если есть между частями расстояние
            if (previous_piece.get_right() < current_piece.get_left()) {
                delete it;
                return monotonicity_type::undefined;
            }

            if (previous_piece.get_right() > current_piece.get_left()) {
                delete it;
                return monotonicity_type::undefined;
            }

            if (!previous_piece.right_included() && !current_piece.left_included()) {
                delete it;
                return monotonicity_type::undefined;
            }

            try {
                Y left_value = previous_piece.get_function().calculate(current_piece.get_left());
                Y right_value = current_piece.get_function().calculate(current_piece.get_left());

                // если значения функий на границах кусков не равны
                if (left_value < right_value) {
                    decreasing_possible = false;
                } else if (left_value > right_value) {
                    increasing_possible = false;
                }

                if (left_value != right_value) {
                    all_constant = false;
                }
            }
            catch (...) {
                delete it;
                return monotonicity_type::undefined;
            }
        }

        interval<X> local_interval = current_piece.get_interval().get_intersection(inter);
        monotonicity_type local_monotonicity = current_piece.get_function().get_monotonicity_on(local_interval);

        // проверка монотонности самого участка
        switch (local_monotonicity) {
            case monotonicity_type::increasing:
                decreasing_possible = false;
                all_constant = false;
                break;
            case monotonicity_type::decreasing:
                increasing_possible = false;
                all_constant = false;
                break;
            case monotonicity_type::constant:
                break;
            case monotonicity_type::not_monotonic:
                has_not_monotonic_piece = true;
                all_constant = false;
                break;
            case monotonicity_type::undefined:
                delete it;
                return monotonicity_type::undefined;
        }

        previous_piece = current_piece;
        has_previous_piece = true;

        // если это последний кусок, затрагивающий интервал
        if (inter.get_right() < current_piece.get_right()) {
            break;
        }

        // если справа неопределено
        if (inter.get_right() == current_piece.get_right()) {
            if (!inter.right_included() || current_piece.right_included()) {
                break;
            }
        }
    }

    delete it;

    // если не попадает ни на одну часть
    if (!has_piece) {
        return monotonicity_type::undefined;
    }

    // если правая часть интервала не находится ни на одном куске
    if (previous_piece.get_right() < inter.get_right()) {
        return monotonicity_type::undefined;
    }

    // если находится на правом куске, но сама функция не определена на той границе
    if (previous_piece.get_right() == inter.get_right() &&
        inter.right_included() &&
        !previous_piece.right_included()) {
        return monotonicity_type::undefined;
    }

    // если хотя бы один кусок и возрастает и убывает
    if (has_not_monotonic_piece) {
        return monotonicity_type::not_monotonic;
    }

    // если всегда константа
    if (all_constant) {
        return monotonicity_type::constant;
    }

    // если возрастала и не убывала
    if (increasing_possible && !decreasing_possible) {
        return monotonicity_type::increasing;
    }

    // если убывала и не возрастала
    if (decreasing_possible && !increasing_possible) {
        return monotonicity_type::decreasing;
    }

    // если и убывала и возрастала
    if (!increasing_possible && !decreasing_possible) {
        return monotonicity_type::not_monotonic;
    }
 
    throw std::logic_error("Unreachable monotonicity state");
}
