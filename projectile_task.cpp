#include "projectile_task.h"

#include <cmath>
#include <stdexcept>

projectile_range_function::projectile_range_function(double initial_speed)
    : initial_speed(initial_speed), gravity(9.81) {
    if (initial_speed <= 0.0) {
        throw std::invalid_argument("Initial speed must be positive");
    }
}

double projectile_range_function::calculate(const double& angle) const {
    return (initial_speed * initial_speed / gravity) * std::sin(2.0 * angle);
}

function<double, double>* projectile_range_function::clone() const {
    return new projectile_range_function(*this);
}

bool projectile_range_function::is_continuous_on(const interval<double>&) const {
    return true;
}

monotonicity_type projectile_range_function::get_monotonicity_on(const interval<double>& inter) const {
    const double critical_angle = projectile_pi / 4.0;

    if (inter.get_left() == inter.get_right()) {
        return monotonicity_type::constant;
    }

    if (inter.get_right() <= critical_angle) {
        return monotonicity_type::increasing;
    }

    if (critical_angle <= inter.get_left()) {
        return monotonicity_type::decreasing;
    }

    return monotonicity_type::not_monotonic;
}

piecewise_function<double, double> build_projectile_range_piecewise_function(double initial_speed) {
    piecewise_function<double, double> result;
    projectile_range_function range_function(initial_speed);

    result.override_on(interval<double>(0.0, projectile_pi / 4.0, true, true), range_function);
    result.override_on(interval<double>(projectile_pi / 4.0, projectile_pi / 2.0, true, true), range_function);

    return result;
}

double analytic_low_angle(double initial_speed, double target_x) {
    const double gravity = 9.81;
    double ratio = gravity * target_x / (initial_speed * initial_speed);

    if (ratio < -1.0 || ratio > 1.0) {
        throw std::invalid_argument("Target is unreachable for the chosen speed");
    }

    return 0.5 * std::asin(ratio);
}

double analytic_high_angle(double initial_speed, double target_x) {
    return projectile_pi / 2.0 - analytic_low_angle(initial_speed, target_x);
}

double find_angle_by_bisection(const piecewise_function<double, double>& range_function,
                               double target_x,
                               const interval<double>& angle_interval,
                               double epsilon) {
    if (epsilon <= 0.0) {
        throw std::invalid_argument("Epsilon must be positive");
    }

    monotonicity_type monotonicity = range_function.get_monotonicity_on(angle_interval);
    if (monotonicity != monotonicity_type::increasing &&
        monotonicity != monotonicity_type::decreasing &&
        monotonicity != monotonicity_type::constant) {
        throw std::invalid_argument("Bisection requires a monotonic interval");
    }

    double left = angle_interval.get_left();
    double right = angle_interval.get_right();
    double left_value = range_function.calculate(left);
    double right_value = range_function.calculate(right);

    if (monotonicity == monotonicity_type::constant) {
        if (std::abs(left_value - target_x) > epsilon) {
            throw std::invalid_argument("Target value is not on the chosen interval");
        }
        return left;
    }

    double min_value = left_value < right_value ? left_value : right_value;
    double max_value = left_value < right_value ? right_value : left_value;
    if (target_x < min_value || max_value < target_x) {
        throw std::invalid_argument("Target value is outside the interval range");
    }

    while (right - left > epsilon) {
        double middle = (left + right) / 2.0;
        double middle_value = range_function.calculate(middle);

        if (monotonicity == monotonicity_type::increasing) {
            if (middle_value < target_x) {
                left = middle;
            } else {
                right = middle;
            }
        } else {
            if (middle_value < target_x) {
                right = middle;
            } else {
                left = middle;
            }
        }
    }

    return (left + right) / 2.0;
}

projectile_solution solve_projectile_task(const sequence<double>* allowed_speeds,
                                          double target_left,
                                          double target_right,
                                          double epsilon) {
    if (allowed_speeds == nullptr) {
        throw std::invalid_argument("Allowed speeds sequence must not be null");
    }

    if (target_right < target_left) {
        throw std::invalid_argument("Target interval is invalid");
    }

    if (target_left < 0.0) {
        throw std::invalid_argument("Target interval must lie on the positive x-axis");
    }

    IEnumerator<double>* it = allowed_speeds->get_enumerator();

    try {
        while (it->move_next()) {
            double current_speed = it->get_current();
            piecewise_function<double, double> range_function =
                build_projectile_range_piecewise_function(current_speed);

            double min_range = range_function.calculate(0.0);
            double max_range = range_function.calculate(projectile_pi / 4.0);
            double reachable_left = target_left < min_range ? min_range : target_left;
            double reachable_right = max_range < target_right ? max_range : target_right;

            if (reachable_right < reachable_left) {
                continue;
            }

            double chosen_target = reachable_left;
            double angle = find_angle_by_bisection(
                range_function,
                chosen_target,
                interval<double>(0.0, projectile_pi / 4.0, true, true),
                epsilon
            );

            delete it;
            return {true, current_speed, angle, chosen_target};
        }
    } catch (...) {
        delete it;
        throw;
    }

    delete it;
    return {false, 0.0, 0.0, 0.0};
}
