#include <print>

#include "include/basic_units.hpp"
#include "include/unit_dimension.hpp"
#include <vector>

using namespace units;

int main() {
    auto a = basic_unit<double, float, int>(1,basic_MP_dimension{MP::k,meter});
    auto b = basic_unit<double, float, int8_t>(2,basic_MP_dimension{MP::c,meter});
    auto e = a / b;
    std::print("{0},{1}", e.value, true);
    return 0;
}
