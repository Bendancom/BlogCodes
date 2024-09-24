#ifndef UNIT_DIMENSIONS
#define UNIT_DIMENSIONS

#include "basic_dimension.hpp"

namespace dimensions{
    struct Length : dimension { };
    struct Mass : dimension {};
    namespace name {
        constexpr std::string Length = "L";
        constexpr std::string Mass = "M";
    }
}
namespace units{
    using namespace dimensions;

    constexpr Length meter = {1,1,"m",name::Length};
    constexpr Length litre = {1e-1f16,3,"L",name::Length};
    constexpr Length inch = {2.54e-2f16,1,"in",name::Length};

    constexpr Mass gram = {1,1,"g",name::Mass};
}
#endif
