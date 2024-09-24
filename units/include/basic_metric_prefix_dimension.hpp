#ifndef METRIC_PREFIX_DIMENSION
#define METRIC_PREFIX_DIMENSION
#include "metric_prefix.hpp"
#include "basic_dimension.hpp"

template<rational magnitude_t,integral power_t>
struct basic_MP_dimension{
    MP metric_prefix;
    basic_dimension<magnitude_t, power_t> dimen;

    template<rational R,integral I>
    requires requires {
        requires std::convertible_to<R,magnitude_t>;
        requires std::convertible_to<I,power_t>;
    }
    static constexpr basic_MP_dimension<magnitude_t, power_t> convert(const basic_MP_dimension<R, I>& MP_dimen) {
        return {
            MP_dimen.metric_prefix,
            basic_dimension<magnitude_t, power_t>::convert(MP_dimen.dimen),
        };
    }

    template<rational R,integral I>
    requires requires {
        requires std::common_with<magnitude_t,R>;
        requires std::common_with<power_t,I>;
    }
    constexpr bool operator==(const basic_MP_dimension<R,I>& MP_dimen) const {
        return dimen == MP_dimen && metric_prefix == MP_dimen.metric_prefix;
    }
    template<rational R,integral I>
    requires requires {
        requires std::common_with<magnitude_t,R>;
        requires std::common_with<power_t,I>;
    }
    constexpr bool type_equal(const basic_MP_dimension<R, I>& MP_dimen) const {
        return dimen.type_equal(MP_dimen);
    }
};
#endif
