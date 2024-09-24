#ifndef UNITS_BASCI_DIMENSION
#define UNITS_BASCI_DIMENSION

#include "math.hpp"
#include <string>
#include <compare>

template<rational magnitude_t,integral power_t>
struct basic_dimension{
    magnitude_t magnitude;
    power_t power;
    std::string_view short_name;
    std::string_view dimension_name;

    template<template<rational R,integral I> typename dimension_t,rational R,integral I>
    requires requires {
        requires std::derived_from<dimension_t<R,I>,basic_dimension<R,I>>; 
        requires std::common_with<magnitude_t,R>;
        requires std::common_with<power_t,I>;
    }
    constexpr bool operator==(const dimension_t<R,I>& dimen) const {
        return  magnitude == dimen.magnitude &&
                power == dimen.power &&
                short_name == dimen.short_name &&
                dimension_name == dimen.dimension_name;
    }
    template<template<rational R,integral I> typename dimension_t,rational R,integral I>
    requires requires {
        requires std::derived_from<dimension_t<R,I>,basic_dimension<R,I>>; 
        requires std::common_with<magnitude_t,R>;
        requires std::common_with<power_t,I>;
    }
    constexpr bool type_equal(const dimension_t<R, I>& dimen) const {
        return power == dimen.power && dimension_name == dimen.dimension_name;
    }

    template<template<rational R,integral I> typename dimension_t,rational R,integral I>
    requires requires {
        requires std::derived_from<dimension_t<R,I>,basic_dimension<R,I>>; 
        requires std::convertible_to<R,magnitude_t>;
        requires std::convertible_to<I,power_t>;
    }
    static constexpr basic_dimension<magnitude_t, power_t> convert (const dimension_t<R,I>& dimen) {
        return {
            dimen.magnitude,
            dimen.power,
            dimen.short_name,
            dimen.dimension_name
        };
    }
};

using dimension = basic_dimension<float,int8_t>;
#endif
