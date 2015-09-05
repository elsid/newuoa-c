#include <cassert>
#include <algorithm>
#include <cmath>
#include <iostream>

#include <newuoa.h>

namespace {

void print_array(const std::size_t size, const double *values) {
    std::cout << '[' << size << ']';
    std::for_each(values, values + size, [&] (double value) {
        std::cout << ' ' << value;
    });
}

template <class F>
NewuoaClosure make_closure(F &function) {
    struct Wrap {
        static double call(void *data, long n, const double *values) {
            return reinterpret_cast<F *>(data)->operator()(n, values);
        }
    };
    return NewuoaClosure {&function, &Wrap::call};
}

} // namespace

int main(int argc, char **argv) {
    const long variables_count = 2;
//    const int number_of_interpolation_conditions = variables_count + 2;
    const long number_of_interpolation_conditions = (variables_count + 1)*(variables_count + 2)/2;
    double variables_values[] = {0.0, -sqrt(5.0)};
    const double initial_trust_region_radius = 1e-3;
    const double final_trust_region_radius = 1e3;
    const long max_function_calls_count = 100;
    const size_t working_space_size = NEWUOA_WORKING_SPACE_SIZE(variables_count,
                                                                number_of_interpolation_conditions);
    double working_space[working_space_size];

    std::cout << "initial: ";
    print_array(variables_count, variables_values);
    std::cout << std::endl;

    std::size_t function_calls_count = 0;

    auto function = [&] (long n, const double *x) {
        assert(n == 2);
        ++function_calls_count;
        return -4*x[0]*x[1] + 5*x[0]*x[0] + 8*x[1]*x[1] + 16*sqrt(5.0)*x[0] + 8*sqrt(5.0)*x[1] - 44.0;
    };
    auto closure = make_closure(function);

    const double result = newuoa_closure(
            &closure,
            variables_count,
            number_of_interpolation_conditions,
            variables_values,
            initial_trust_region_radius,
            final_trust_region_radius,
            max_function_calls_count,
            working_space);

    std::cout << "final: ";
    print_array(variables_count, variables_values);
    std::cout << std::endl;

    std::cout << "function_calls_count: " << function_calls_count << std::endl;

    std::cout << "result: " << result << std::endl;

    return 0;
}
