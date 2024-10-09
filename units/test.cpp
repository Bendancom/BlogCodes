#include <print>

#include "include/basic_units.hpp"
#include "include/unit_dimension.hpp"
#include <vector>

using namespace units;

int main() {
    auto a = basic_unit<double, float, int>(1,basic_MP_dimension{MP::k,litre});
    auto b = basic_unit<double, float, int8_t>(2,basic_MP_dimension{MP::null,litre});
    auto e = b + a;
    std::print("{0}", e.getValue());
    return 0;
}
