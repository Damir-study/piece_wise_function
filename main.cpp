#include <iostream>
#include <limits>
#include <string>

#include "piecewise_function.h"
#include "projectile_task.h"
#include "functions/power_function.h"
#include "functions/exponential_function.h"
#include "functions/logarithm_function.h"
#include "functions/sine_function.h"
#include "functions/cosine_function.h"
#include "sequence/mutable_array_sequence.h"
#include "sequence/immutable_array_sequence.h"
#include "sequence/mutable_list_sequence.h"
#include "sequence/immutable_list_sequence.h"

int get_int() {
    int val;
    while (!(std::cin >> val)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Enter an integer: ";
    }
    return val;
}

double get_double() {
    double val;
    while (!(std::cin >> val)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Enter a number: ";
    }
    return val;
}

bool get_bool_choice(const std::string& prompt) {
    std::cout << prompt << " (1 = yes, 0 = no): ";
    while (true) {
        int value = get_int();
        if (value == 0) {
            return false;
        }

        if (value == 1) {
            return true;
        }

        std::cout << "Enter 1 or 0: ";
    }
}

interval<double> get_interval_from_user(bool force_left_included = false) {
    std::cout << "Enter left bound: ";
    double left = get_double();

    std::cout << "Enter right bound: ";
    double right = get_double();

    bool include_left = force_left_included ? true : get_bool_choice("Include left bound?");
    bool include_right = get_bool_choice("Include right bound?");

    return interval<double>(left, right, include_left, include_right);
}

std::string monotonicity_to_string(monotonicity_type type) {
    switch (type) {
        case monotonicity_type::increasing:
            return "Increasing";
        case monotonicity_type::decreasing:
            return "Decreasing";
        case monotonicity_type::constant:
            return "Constant";
        case monotonicity_type::not_monotonic:
            return "Not monotonic";
        case monotonicity_type::undefined:
            return "Undefined";
    }

    return "Unknown";
}

void print_interval(const interval<double>& inter) {
    std::cout << (inter.left_included() ? "[" : "(")
              << inter.get_left() << ", " << inter.get_right()
              << (inter.right_included() ? "]" : ")");
}

std::string get_function_description(const function<double, double>& func) {
    const auto* power = dynamic_cast<const power_function<double, double>*>(&func);
    if (power != nullptr) {
        return "x^" + std::to_string(power->get_exponent());
    }

    const auto* exponential = dynamic_cast<const exponential_function<double, double>*>(&func);
    if (exponential != nullptr) {
        return std::to_string(exponential->get_base()) + "^x";
    }

    const auto* logarithm = dynamic_cast<const logarithm_function<double, double>*>(&func);
    if (logarithm != nullptr) {
        return "log_" + std::to_string(logarithm->get_base()) + "(x)";
    }

    if (dynamic_cast<const sine_function<double, double>*>(&func) != nullptr) {
        return "sin(x)";
    }

    if (dynamic_cast<const cosine_function<double, double>*>(&func) != nullptr) {
        return "cos(x)";
    }

    return "unknown function";
}

void print_piecewise(const piecewise_function<double, double>& func) {
    std::cout << "Piecewise function contains " << func.get_piece_count() << " piece(s)\n";

    if (func.get_piece_count() == 0) {
        std::cout << "[ empty ]\n";
        return;
    }

    for (int i = 0; i < func.get_piece_count(); ++i) {
        function_piece<double, double> piece = func.get_piece(i);
        function<double, double>* piece_function = func.get_function(i);
        std::cout << "Piece " << i << ": ";
        print_interval(piece.get_interval());
        std::cout << " -> " << get_function_description(*piece_function);
        std::cout << '\n';
        delete piece_function;
    }
}

sequence<function_piece<double, double>>* create_piece_storage(int struct_choice, int mut_choice) {
    if (struct_choice == 1 && mut_choice == 1) {
        return new mutable_array_sequence<function_piece<double, double>>();
    }

    if (struct_choice == 1 && mut_choice == 2) {
        return new immutable_array_sequence<function_piece<double, double>>();
    }

    if (struct_choice == 2 && mut_choice == 1) {
        return new mutable_list_sequence<function_piece<double, double>>();
    }

    if (struct_choice == 2 && mut_choice == 2) {
        return new immutable_list_sequence<function_piece<double, double>>();
    }

    throw std::invalid_argument("Invalid storage configuration");
}

function<double, double>* create_function_from_user() {
    std::cout << "\nSelect function type:\n"
              << "1) Power: x^n\n"
              << "2) Exponential: a^x\n"
              << "3) Logarithm: log_a(x)\n"
              << "4) Sine: sin(x)\n"
              << "5) Cosine: cos(x)\n"
              << "Choice: ";

    int choice = get_int();

    switch (choice) {
        case 1: {
            std::cout << "Enter non-negative integer exponent n: ";
            int exponent = get_int();
            return new power_function<double, double>(exponent);
        }
        case 2: {
            std::cout << "Enter base a: ";
            double base = get_double();
            return new exponential_function<double, double>(base);
        }
        case 3: {
            std::cout << "Enter base a: ";
            double base = get_double();
            return new logarithm_function<double, double>(base);
        }
        case 4:
            return new sine_function<double, double>();
        case 5:
            return new cosine_function<double, double>();
        default:
            throw std::invalid_argument("Unknown function type");
    }
}

sequence<double>* create_speed_sequence_from_user() {
    std::cout << "How many allowed speeds do you want to enter? ";
    int count = get_int();

    if (count < 0) {
        throw std::invalid_argument("Speed count must not be negative");
    }

    sequence<double>* speeds = new mutable_array_sequence<double>();

    try {
        for (int i = 0; i < count; ++i) {
            std::cout << "Enter speed " << i << ": ";
            speeds->append(get_double());
        }
    }
    catch (...) {
        delete speeds;
        throw;
    }

    return speeds;
}

int main() {
    try {
        std::cout << "Select internal storage for piecewise function:\n"
                  << "1) Array Sequence\n"
                  << "2) Linked List Sequence\n"
                  << "Choice: ";
        int struct_choice = get_int();

        std::cout << "\nSelect internal sequence mutability:\n"
                  << "1) Mutable\n"
                  << "2) Immutable\n"
                  << "Choice: ";
        int mut_choice = get_int();

        piecewise_function<double, double> current(create_piece_storage(struct_choice, mut_choice));

        while (true) {
            std::cout << "\n========================================\n";
            print_piecewise(current);
            std::cout << "----------------------------------------\n";
            std::cout << "CATEGORIES:\n";
            std::cout << "1) Elementary (piece count, list pieces, get piece)\n";
            std::cout << "2) Transformations (override on interval)\n";
            std::cout << "3) Analysis (value, definedness, continuity, monotonicity)\n";
            std::cout << "4) Projectile task\n";
            std::cout << "0) Exit\n";
            std::cout << "Choose a category: ";

            int category = get_int();

            if (category == 0) {
                break;
            }

            try {
                switch (category) {
                    case 1: {
                        std::cout << "\n--- Elementary Operations ---\n";
                        std::cout << "1) Get piece count\n";
                        std::cout << "2) Show all pieces\n";
                        std::cout << "3) Get piece by index\n";
                        std::cout << "4) Get interval by index\n";
                        std::cout << "5) Get function by index\n";
                        std::cout << "0) Back to main menu\n";
                        std::cout << "Choose an action: ";

                        int action = get_int();

                        switch (action) {
                            case 1:
                                std::cout << "Result: piece count = " << current.get_piece_count() << "\n";
                                break;
                            case 2:
                                print_piecewise(current);
                                break;
                            case 3: {
                                std::cout << "Enter index: ";
                                int index = get_int();
                                function_piece<double, double> piece = current.get_piece(index);
                                std::cout << "Piece " << index << " interval: ";
                                print_interval(piece.get_interval());
                                std::cout << "\n";
                                break;
                            }
                            case 4: {
                                std::cout << "Enter index: ";
                                int index = get_int();
                                interval<double> inter = current.get_interval(index);
                                std::cout << "Interval " << index << ": ";
                                print_interval(inter);
                                std::cout << "\n";
                                break;
                            }
                            case 5: {
                                std::cout << "Enter index: ";
                                int index = get_int();
                                function<double, double>* func = current.get_function(index);
                                std::cout << "Function from piece " << index
                                          << ": " << get_function_description(*func) << "\n";
                                delete func;
                                break;
                            }
                            case 0:
                                break;
                            default:
                                std::cout << "Invalid action choice.\n";
                                break;
                        }
                        break;
                    }

                    case 2: {
                        std::cout << "\n--- Transformations ---\n";
                        std::cout << "1) Override function on interval\n";
                        std::cout << "0) Back to main menu\n";
                        std::cout << "Choose an action: ";

                        int action = get_int();
                        if (action == 0) {
                            break;
                        }

                        if (action != 1) {
                            std::cout << "Invalid action choice.\n";
                            break;
                        }

                        interval<double> inter = get_interval_from_user(true);
                        function<double, double>* func = create_function_from_user();
                        current.override_on(inter, *func);
                        delete func;
                        std::cout << "Success: interval was redefined.\n";
                        break;
                    }

                    case 3: {
                        std::cout << "\n--- Analysis ---\n";
                        std::cout << "1) Calculate value at point\n";
                        std::cout << "2) Check if defined on interval\n";
                        std::cout << "3) Check continuity on interval\n";
                        std::cout << "4) Check monotonicity on interval\n";
                        std::cout << "0) Back to main menu\n";
                        std::cout << "Choose an action: ";

                        int action = get_int();
                        if (action == 0) {
                            break;
                        }

                        switch (action) {
                            case 1: {
                                std::cout << "Enter x: ";
                                double x = get_double();
                                double value = current.calculate(x);
                                std::cout << "Result: f(" << x << ") = " << value << "\n";
                                break;
                            }
                            case 2: {
                                interval<double> inter = get_interval_from_user();
                                bool result = current.is_defined_on(inter);
                                std::cout << "Defined on ";
                                print_interval(inter);
                                std::cout << ": " << (result ? "yes" : "no") << "\n";
                                break;
                            }
                            case 3: {
                                interval<double> inter = get_interval_from_user();
                                bool result = current.is_continuous_on(inter);
                                std::cout << "Continuous on ";
                                print_interval(inter);
                                std::cout << ": " << (result ? "yes" : "no") << "\n";
                                break;
                            }
                            case 4: {
                                interval<double> inter = get_interval_from_user();
                                monotonicity_type result = current.get_monotonicity_on(inter);
                                std::cout << "Monotonicity on ";
                                print_interval(inter);
                                std::cout << ": " << monotonicity_to_string(result) << "\n";
                                break;
                            }
                            default:
                                std::cout << "Invalid action choice.\n";
                                break;
                        }
                        break;
                    }

                    case 4: {
                        std::cout << "\n--- Projectile Task ---\n";
                        std::cout << "1) Solve for allowed speeds and target interval\n";
                        std::cout << "2) Analytic angles for a chosen speed and target\n";
                        std::cout << "0) Back to main menu\n";
                        std::cout << "Choose an action: ";

                        int action = get_int();
                        if (action == 0) {
                            break;
                        }

                        switch (action) {
                            case 1: {
                                sequence<double>* speeds = create_speed_sequence_from_user();

                                try {
                                    std::cout << "Enter target interval left bound x1: ";
                                    double x1 = get_double();
                                    std::cout << "Enter target interval right bound x2: ";
                                    double x2 = get_double();

                                    projectile_solution solution = solve_projectile_task(speeds, x1, x2);

                                    if (!solution.found) {
                                        std::cout << "No solution was found for the given speeds.\n";
                                    } else {
                                        std::cout << "Solution found:\n";
                                        std::cout << "Initial speed v0 = " << solution.initial_speed << "\n";
                                        std::cout << "Launch angle alpha = " << solution.launch_angle << "\n";
                                        std::cout << "Hit point x = " << solution.hit_x << "\n";
                                        std::cout << "Analytic low angle = "
                                                  << analytic_low_angle(solution.initial_speed, solution.hit_x) << "\n";
                                        std::cout << "Analytic high angle = "
                                                  << analytic_high_angle(solution.initial_speed, solution.hit_x) << "\n";
                                    }
                                }
                                catch (...) {
                                    delete speeds;
                                    throw;
                                }

                                delete speeds;
                                break;
                            }
                            case 2: {
                                std::cout << "Enter initial speed v0: ";
                                double speed = get_double();
                                std::cout << "Enter target x: ";
                                double target_x = get_double();

                                std::cout << "Analytic low angle = "
                                          << analytic_low_angle(speed, target_x) << "\n";
                                std::cout << "Analytic high angle = "
                                          << analytic_high_angle(speed, target_x) << "\n";
                                break;
                            }
                            default:
                                std::cout << "Invalid action choice.\n";
                                break;
                        }
                        break;
                    }

                    default:
                        std::cout << "Invalid category. Try again.\n";
                        break;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "ERROR: " << e.what() << "\n";
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
