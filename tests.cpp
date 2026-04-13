#include <gtest/gtest.h>
#include <cmath>
#include <stdexcept>

#include "interval.h"
#include "function_piece.h"
#include "piecewise_function.h"
#include "function.h"
#include "projectile_task.h"

#include "functions/power_function.h"
#include "functions/exponential_function.h"
#include "functions/logarithm_function.h"
#include "functions/sine_function.h"
#include "functions/cosine_function.h"

#include "sequence/mutable_array_sequence.h"
#include "sequence/mutable_list_sequence.h"
#include "sequence/immutable_array_sequence.h"
#include "sequence/immutable_list_sequence.h"

TEST(interval_test, invalid_bounds_throw) {
    EXPECT_THROW(interval<double>(5.0, 1.0), std::invalid_argument);

    interval<double> inter(1.0, 3.0);
    EXPECT_THROW(inter.set_left(4.0), std::invalid_argument);
    EXPECT_THROW(inter.set_right(0.0), std::invalid_argument);
}

TEST(interval_test, intersection_respects_open_and_closed_bounds) {
    interval<double> left(0.0, 1.0, true, false);
    interval<double> right(1.0, 2.0, true, true);
    interval<double> overlap_a(0.0, 2.0, true, true);
    interval<double> overlap_b(1.0, 3.0, true, false);

    EXPECT_FALSE(left.intersects(right));
    EXPECT_TRUE(overlap_a.intersects(overlap_b));

    interval<double> inter = overlap_a.get_intersection(overlap_b);
    EXPECT_DOUBLE_EQ(inter.get_left(), 1.0);
    EXPECT_DOUBLE_EQ(inter.get_right(), 2.0);
    EXPECT_TRUE(inter.left_included());
    EXPECT_TRUE(inter.right_included());
}

TEST(power_function_test, basic_behavior) {
    power_function<double, double> f(2);

    EXPECT_DOUBLE_EQ(f.calculate(3.0), 9.0);
    EXPECT_TRUE(f.is_continuous_on(interval<double>(-10.0, 10.0)));
    EXPECT_EQ(f.get_monotonicity_on(interval<double>(-5.0, -1.0)), monotonicity_type::decreasing);
    EXPECT_EQ(f.get_monotonicity_on(interval<double>(1.0, 5.0)), monotonicity_type::increasing);
    EXPECT_EQ(f.get_monotonicity_on(interval<double>(-1.0, 1.0)), monotonicity_type::not_monotonic);
}

TEST(exponential_and_logarithm_test, domains_and_monotonicity) {
    exponential_function<double, double> exp_f(2.0);
    logarithm_function<double, double> log_f(2.0);

    EXPECT_NEAR(exp_f.calculate(3.0), 8.0, 1e-12);
    EXPECT_EQ(exp_f.get_monotonicity_on(interval<double>(-3.0, 2.0)), monotonicity_type::increasing);

    EXPECT_NEAR(log_f.calculate(8.0), 3.0, 1e-12);
    EXPECT_THROW(log_f.calculate(0.0), std::invalid_argument);
    EXPECT_FALSE(log_f.is_continuous_on(interval<double>(0.0, 1.0, true, true)));
    EXPECT_EQ(log_f.get_monotonicity_on(interval<double>(0.5, 8.0)), monotonicity_type::increasing);
    EXPECT_EQ(log_f.get_monotonicity_on(interval<double>(-1.0, 8.0)), monotonicity_type::undefined);
}

TEST(exponential_and_logarithm_test, special_cases) {
    exponential_function<double, double> constant_exp(1.0);
    logarithm_function<double, double> decreasing_log(0.5);

    EXPECT_DOUBLE_EQ(constant_exp.calculate(10.0), 1.0);
    EXPECT_EQ(constant_exp.get_monotonicity_on(interval<double>(-2.0, 3.0)),
              monotonicity_type::constant);

    EXPECT_EQ(decreasing_log.get_monotonicity_on(interval<double>(0.25, 4.0)),
              monotonicity_type::decreasing);
}

TEST(trigonometric_function_test, monotonicity) {
    sine_function<double, double> sin_f;
    cosine_function<double, double> cos_f;
    constexpr double pi = 3.14159265358979323846;

    EXPECT_NEAR(sin_f.calculate(pi / 2.0), 1.0, 1e-12);
    EXPECT_NEAR(cos_f.calculate(0.0), 1.0, 1e-12);

    EXPECT_EQ(sin_f.get_monotonicity_on(interval<double>(-pi / 2.0, pi / 2.0)),
              monotonicity_type::increasing);
    EXPECT_EQ(sin_f.get_monotonicity_on(interval<double>(0.0, pi)),
              monotonicity_type::not_monotonic);

    EXPECT_EQ(cos_f.get_monotonicity_on(interval<double>(0.0, pi)),
              monotonicity_type::decreasing);
    EXPECT_EQ(cos_f.get_monotonicity_on(interval<double>(0.0, 2.0 * pi)),
              monotonicity_type::not_monotonic);
}

TEST(function_piece_test, calculate_and_copy) {
    power_function<double, double> power(3);
    function_piece<double, double> piece(interval<double>(0.0, 2.0), power);

    EXPECT_TRUE(piece.contains(1.5));
    EXPECT_FALSE(piece.contains(3.0));
    EXPECT_DOUBLE_EQ(piece.calculate(2.0), 8.0);
    EXPECT_THROW(piece.calculate(3.0), std::invalid_argument);

    function_piece<double, double> copied(piece);
    EXPECT_DOUBLE_EQ(copied.calculate(2.0), 8.0);
}

TEST(function_piece_test, empty_piece_throws) {
    function_piece<double, double> piece;
    EXPECT_THROW(piece.calculate(0.0), std::logic_error);
}

TEST(piecewise_function_test, empty_state) {
    piecewise_function<double, double> func;

    EXPECT_EQ(func.get_piece_count(), 0);
    EXPECT_FALSE(func.is_defined_on(interval<double>(0.0, 1.0)));
    EXPECT_FALSE(func.is_continuous_on(interval<double>(0.0, 1.0)));
    EXPECT_EQ(func.get_monotonicity_on(interval<double>(0.0, 1.0)), monotonicity_type::undefined);
    EXPECT_THROW(func.calculate(0.0), std::out_of_range);
}

TEST(piecewise_function_test, constructor_rejects_invalid_pieces) {
    auto* overlapping = new mutable_list_sequence<function_piece<double, double>>();
    power_function<double, double> f1(1);
    power_function<double, double> f2(2);

    overlapping->append(function_piece<double, double>(interval<double>(0.0, 2.0), f1));
    overlapping->append(function_piece<double, double>(interval<double>(1.0, 3.0), f2));

    EXPECT_THROW((piecewise_function<double, double>(overlapping)), std::invalid_argument);

    auto* test_seq = new mutable_list_sequence<function_piece<double, double>>();
    test_seq->append(function_piece<double, double>(interval<double>(0.0, 1.0, true, true), f1));
    test_seq->append(function_piece<double, double>(interval<double>(1.0, 2.0, true, true), f2));

    EXPECT_THROW((piecewise_function<double, double>(test_seq)), std::invalid_argument);
}

TEST(piecewise_function_test, override_single_piece_and_value) {
    piecewise_function<double, double> func;
    power_function<double, double> power(2);

    func.override_on(interval<double>(0.0, 2.0, true, true), power);

    EXPECT_EQ(func.get_piece_count(), 1);
    EXPECT_DOUBLE_EQ(func.calculate(2.0), 4.0);
    EXPECT_TRUE(func.is_defined_on(interval<double>(0.0, 2.0, true, true)));
    EXPECT_FALSE(func.is_defined_on(interval<double>(0.0, 3.0, true, true)));
}

TEST(piecewise_function_test, override_allows_gaps) {
    piecewise_function<double, double> func;
    power_function<double, double> f(1);

    func.override_on(interval<double>(0.0, 1.0, true, true), f);
    func.override_on(interval<double>(2.0, 3.0, true, true), f);

    EXPECT_EQ(func.get_piece_count(), 2);
    EXPECT_TRUE(func.is_defined_on(interval<double>(0.0, 1.0, true, true)));
    EXPECT_TRUE(func.is_defined_on(interval<double>(2.0, 3.0, true, true)));
    EXPECT_FALSE(func.is_defined_on(interval<double>(0.0, 3.0, true, true)));
    EXPECT_EQ(func.get_monotonicity_on(interval<double>(0.0, 3.0, true, true)),
              monotonicity_type::undefined);
}

TEST(piecewise_function_test, override_splits_existing_piece) {
    piecewise_function<double, double> func;
    power_function<double, double> base_part(1);
    power_function<double, double> middle_part(2);

    func.override_on(interval<double>(0.0, 4.0, true, true), base_part);
    func.override_on(interval<double>(1.0, 3.0, true, true), middle_part);

    EXPECT_EQ(func.get_piece_count(), 3);

    EXPECT_DOUBLE_EQ(func.calculate(0.5), 0.5);
    EXPECT_DOUBLE_EQ(func.calculate(2.0), 4.0);
    EXPECT_DOUBLE_EQ(func.calculate(3.5), 3.5);

    EXPECT_TRUE(func.is_defined_on(interval<double>(0.0, 4.0, true, true)));
}

TEST(piecewise_function_test, overrides_keep_right_piece_rule) {
    piecewise_function<double, double> func;
    power_function<double, double> left_part(1);
    power_function<double, double> right_part(1);

    func.override_on(interval<double>(0.0, 1.0, true, true), left_part);
    EXPECT_NO_THROW(func.override_on(interval<double>(1.0, 2.0, true, true), right_part));

    EXPECT_EQ(func.get_piece_count(), 2);
    EXPECT_TRUE(func.is_defined_on(interval<double>(0.0, 2.0, true, true)));
    EXPECT_DOUBLE_EQ(func.calculate(1.0), 1.0);
}

TEST(piecewise_function_test, right_boundary_may_be_covered_by_next_piece) {
    auto* storage = new mutable_list_sequence<function_piece<double, double>>();
    power_function<double, double> first(1);
    power_function<double, double> second(2);
    exponential_function<double, double> third(2.0);

    storage->append(function_piece<double, double>(interval<double>(-2.0, 1.0, true, false), first));
    storage->append(function_piece<double, double>(interval<double>(1.0, 7.0, true, false), second));
    storage->append(function_piece<double, double>(interval<double>(7.0, 9.0, true, false), third));

    piecewise_function<double, double> func(storage);

    EXPECT_TRUE(func.is_defined_on(interval<double>(1.0, 7.0, true, true)));
}

TEST(piecewise_function_test, continuity_detects_jump) {
    auto* storage = new mutable_list_sequence<function_piece<double, double>>();
    power_function<double, double> left_part(1);
    exponential_function<double, double> right_part(2.0);

    storage->append(function_piece<double, double>(interval<double>(0.0, 1.0, true, false), left_part));
    storage->append(function_piece<double, double>(interval<double>(1.0, 2.0, true, true), right_part));

    piecewise_function<double, double> func(storage);

    EXPECT_TRUE(func.is_defined_on(interval<double>(0.0, 2.0, true, true)));
    EXPECT_FALSE(func.is_continuous_on(interval<double>(0.0, 2.0, true, true)));
}

TEST(piecewise_function_test, monotonicity_across_pieces) {
    auto* storage = new mutable_list_sequence<function_piece<double, double>>();
    power_function<double, double> first(1);
    power_function<double, double> second(1);

    storage->append(function_piece<double, double>(interval<double>(0.0, 1.0, true, false), first));
    storage->append(function_piece<double, double>(interval<double>(1.0, 3.0, true, true), second));

    piecewise_function<double, double> func(storage);

    EXPECT_TRUE(func.is_continuous_on(interval<double>(0.0, 3.0, true, true)));
    EXPECT_EQ(func.get_monotonicity_on(interval<double>(0.0, 3.0, true, true)),
              monotonicity_type::increasing);
}

TEST(piecewise_function_test, calculate_throws_inside_gap) {
    piecewise_function<double, double> func;
    power_function<double, double> part(1);

    func.override_on(interval<double>(0.0, 1.0, true, true), part);
    func.override_on(interval<double>(3.0, 4.0, true, true), part);

    EXPECT_THROW(func.calculate(2.0), std::out_of_range);
}

TEST(piecewise_function_test, getters_return_correct_piece_data) {
    piecewise_function<double, double> func;
    power_function<double, double> first(1);
    power_function<double, double> second(2);

    func.override_on(interval<double>(0.0, 1.0, true, true), first);
    func.override_on(interval<double>(2.0, 3.0, true, true), second);

    interval<double> inter = func.get_interval(1);
    EXPECT_DOUBLE_EQ(inter.get_left(), 2.0);
    EXPECT_DOUBLE_EQ(inter.get_right(), 3.0);
    EXPECT_TRUE(inter.left_included());
    EXPECT_TRUE(inter.right_included());

    function<double, double>* extracted = func.get_function(1);
    ASSERT_NE(extracted, nullptr);
    EXPECT_DOUBLE_EQ(extracted->calculate(3.0), 9.0);
    delete extracted;
}

TEST(projectile_task_test, solve_finds_valid_speed_and_angle) {
    mutable_array_sequence<double> allowed_speeds;
    allowed_speeds.append(5.0);
    allowed_speeds.append(10.0);
    allowed_speeds.append(20.0);

    projectile_solution result = solve_projectile_task(&allowed_speeds, 8.0, 8.5);

    ASSERT_TRUE(result.found);
    EXPECT_DOUBLE_EQ(result.initial_speed, 10.0);
    EXPECT_LE(8.0, result.hit_x);
    EXPECT_LE(result.hit_x, 8.5);

    projectile_range_function range_function(result.initial_speed);
    EXPECT_NEAR(range_function.calculate(result.launch_angle), result.hit_x, 1e-5);
    EXPECT_NEAR(result.launch_angle,
                analytic_low_angle(result.initial_speed, result.hit_x),
                1e-4);
}

TEST(projectile_task_test, analytic_high_angle_matches_same_range) {
    double speed = 12.0;
    double target_x = 10.0;

    double low_angle = analytic_low_angle(speed, target_x);
    double high_angle = analytic_high_angle(speed, target_x);
    projectile_range_function range_function(speed);

    EXPECT_LT(low_angle, high_angle);
    EXPECT_NEAR(range_function.calculate(low_angle), target_x, 1e-9);
    EXPECT_NEAR(range_function.calculate(high_angle), target_x, 1e-9);
}

TEST(projectile_task_test, solve_returns_not_found_for_unreachable_target) {
    mutable_array_sequence<double> allowed_speeds;
    allowed_speeds.append(2.0);
    allowed_speeds.append(3.0);

    projectile_solution result = solve_projectile_task(&allowed_speeds, 20.0, 25.0);

    EXPECT_FALSE(result.found);
}
