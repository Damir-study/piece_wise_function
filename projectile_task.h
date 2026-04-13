#ifndef PROJECTILE_TASK_H
#define PROJECTILE_TASK_H

#include "function.h"
#include "interval.h"
#include "piecewise_function.h"
#include "sequence/sequence.h"

constexpr double projectile_pi = 3.14159265358979323846;

struct projectile_solution {
    bool found;
    double initial_speed;
    double launch_angle;
    double hit_x;
};

class projectile_range_function : public function<double, double> {
public:
    projectile_range_function(double initial_speed);

    double calculate(const double& angle) const override;
    function<double, double>* clone() const override;
    bool is_continuous_on(const interval<double>& inter) const override;
    monotonicity_type get_monotonicity_on(const interval<double>& inter) const override;

private:
    double initial_speed;
    const double gravity;
};

piecewise_function<double, double> build_projectile_range_piecewise_function(double initial_speed);

double analytic_low_angle(double initial_speed, double target_x);
double analytic_high_angle(double initial_speed, double target_x);

double find_angle_by_bisection(const piecewise_function<double, double>& range_function,
                               double target_x,
                               const interval<double>& angle_interval,
                               double epsilon = 1e-6);

projectile_solution solve_projectile_task(const sequence<double>* allowed_speeds,
                                          double target_left,
                                          double target_right,
                                          double epsilon = 1e-6);

#endif
