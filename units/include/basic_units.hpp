#ifndef UNITS
#define UNITS
#include "math.hpp"
#include "basic_metric_prefix_dimension.hpp"
#include <set>
#include <expected>
#include <print>

template<rational value_t,rational magnitude_t,integral power_t>
class basic_unit {
public:
    template<rational M,integral P>
    struct MP_dimension_less {
        constexpr bool operator()(const basic_MP_dimension<M, P>& ldimen,const basic_MP_dimension<M, P>& rdimen) const {
            return ldimen.dimen.dimension_name < rdimen.dimen.dimension_name;
        }
    };
    using dimension_t = basic_MP_dimension<magnitude_t, power_t>;
    using MP_Less_t = MP_dimension_less<magnitude_t, power_t>;
private:
    value_t value;
    std::set<dimension_t,MP_Less_t> dimens;

    template<rational V,rational M,integral P>
    requires requires {
        requires std::convertible_to<V, value_t>;
        requires std::convertible_to<M, magnitude_t>;
        requires std::convertible_to<P, power_t>;
    }
    constexpr value_t MagnitudeCalculateFrom(const basic_unit<V, M, P>& u) const {
        value_t all_magnitude = 1;
        for(auto i : dimens) {
            value_t magni = 1;
            auto dimen = basic_MP_dimension<M, P>::convert(i);
            if (auto iter = u.getDimensions().find(dimen); iter != u.getDimensions().end()){
                magni *= pow<value_t>(iter->dimen.magnitude / i.dimen.magnitude, iter->dimen.power);
                magni *= pow<value_t>(MP_array[(char)iter->metric_prefix] / MP_array[(char)i.metric_prefix],iter->dimen.power);
            }
            all_magnitude *= magni;
        }
        return all_magnitude;
    }
public:
    constexpr basic_unit() {}

    template<rational V,rational M,integral P>
    requires requires {
        requires std::convertible_to<V, value_t>;
        requires std::convertible_to<M, magnitude_t>;
        requires std::convertible_to<P, power_t>;
    }
    constexpr basic_unit(const basic_unit<V, M, P>& unit) : value(unit.getValue()) {
        for(auto i : unit.getDimensions())
            dimens.insert(basic_MP_dimension<magnitude_t, power_t>::convert(i));        
    }
    
    constexpr basic_unit(const value_t& value) : value(value) {}

    template<rational M,integral P>
    requires requires {
        requires std::convertible_to<M,magnitude_t>;
        requires std::convertible_to<P,power_t>;
    }
    constexpr basic_unit(const value_t& value,const basic_MP_dimension<M, P>& dimen) : value(value) {
        dimens.insert(basic_MP_dimension<magnitude_t, power_t>::convert(dimen));
    }

    template<std::ranges::input_range Rng>
    requires requires {
        requires std::same_as<decltype(std::ranges::range_value_t<Rng>::metric_prefix), MP>;
        requires std::convertible_to<decltype(std::ranges::range_value_t<Rng>::dimen.magnitude),magnitude_t>;
        requires std::convertible_to<decltype(std::ranges::range_value_t<Rng>::dimen.power),power_t>;
        requires std::same_as<decltype(std::ranges::range_value_t<Rng>::dimen.short_name), std::string_view>;
        requires std::same_as<decltype(std::ranges::range_value_t<Rng>::dimen.dimension_name), std::string_view>;
    }
    constexpr basic_unit(const value_t& value, Rng&& range) : value(value) {
        for(auto i : range) {
            auto dimen = basic_MP_dimension<magnitude_t, power_t>::convert(i);
            if (std::ranges::iterator_t<decltype(dimens)> iter = dimens.find(dimen);iter == dimens.end())
                dimens.insert(dimen);
        }
    }

    constexpr const value_t& getValue() const {
        return value;
    }
    constexpr const std::set<dimension_t, MP_Less_t>& getDimensions() const {
        return dimens;
    }

    template<rational V,rational M,integral P>
    requires requires {
        requires std::common_with<value_t, V>;
        requires std::common_with<magnitude_t, M>;
        requires std::common_with<power_t, P>;
    }
    constexpr std::expected<
        basic_unit<
            std::common_type_t<value_t,V>,
            std::common_type_t<magnitude_t,M>,
            std::common_type_t<power_t,P>
        >,
        std::exception
    > plus(const basic_unit<V,M,P>& unit) const noexcept {
        if (dimens.size() == unit.getDimensions().size()) {
            auto i = dimens.begin();
            auto j = unit.getDimensions().begin();
            for (;i != dimens.end() && j != unit.getDimensions().end();) {
                if (!i->type_equal(*j))
                    return std::unexpected(std::exception());
                i++;
                j++;
            }
            return basic_unit<
                std::common_type_t<value_t,V>,
                std::common_type_t<magnitude_t,M>,
                std::common_type_t<power_t,P>
            >(value + MagnitudeCalculateFrom(unit) * unit.getValue(),dimens);
        }
        else return std::unexpected(std::exception());
    }

    template<rational V,rational M,integral P>
    requires requires {
        requires std::common_with<value_t, V>;
        requires std::common_with<magnitude_t, M>;
        requires std::common_with<power_t, P>;
    }
    constexpr std::expected<
        basic_unit<
            std::common_type_t<value_t,V>,
            std::common_type_t<magnitude_t,M>,
            std::common_type_t<power_t,P>
        >,
        std::exception
    > minus(const basic_unit<V, M, P>& unit) const noexcept {
        if (dimens.size() == unit.getDimensions().size()){
            auto i = dimens.begin();
            auto j = unit.getDimensions().begin();
            for (;i != dimens.end() && j != unit.getDimensions().end();){
                if (!i->type_equal(*j))
                    return std::unexpected(std::exception());
                i++;
                j++;
            }
            return basic_unit<
                std::common_type_t<value_t,V>,
                std::common_type_t<magnitude_t,M>,
                std::common_type_t<power_t,P>
            >(value - MagnitudeCalculateFrom(unit)* unit.getValue() ,dimens);
        }
        else return std::unexpected(std::exception());
    }

    template<rational V,rational M,integral P>
    requires requires {
        requires std::common_with<value_t, V>;
        requires std::common_with<magnitude_t, M>;
        requires std::common_with<power_t, P>;
    }
    constexpr basic_unit<
        std::common_type_t<value_t,V>,
        std::common_type_t<magnitude_t,M>,
        std::common_type_t<power_t,P>
    > multiply(const basic_unit<V, M, P>& unit) const noexcept {
        std::set<
            basic_MP_dimension<
                std::common_type_t<magnitude_t,M>,
                std::common_type_t<power_t,P>
            >,
            MP_dimension_less<
                std::common_type_t<magnitude_t,M>,
                std::common_type_t<power_t,P>
            >
        > tmp_dimens;
        for (auto i : dimens){
            auto dimen = basic_MP_dimension<M, P>::convert(i);
            basic_dimension<
                std::common_type_t<magnitude_t,M>,
                std::common_type_t<power_t,P>
            > tmp_dimen {
                i.dimen.magnitude,
                i.dimen.power,
                i.dimen.short_name,
                i.dimen.dimension_name,
            };
            if (auto iter = unit.getDimensions().find(dimen); iter != unit.getDimensions().end())
                tmp_dimen.power += iter->dimen.power;
            tmp_dimens.insert(
                basic_MP_dimension<
                    std::common_type_t<magnitude_t,M>,
                    std::common_type_t<power_t,P>
                > {
                    i.metric_prefix,
                    tmp_dimen
                }
            );
        }
        return basic_unit<
            std::common_type_t<value_t,V>,
            std::common_type_t<magnitude_t,M>,
            std::common_type_t<power_t,P>
        >(value * MagnitudeCalculateFrom(unit) * unit.getValue(),tmp_dimens);
    }

    template<rational V,rational M,integral P>
    requires requires {
        requires std::common_with<value_t, V>;
        requires std::common_with<magnitude_t, M>;
        requires std::common_with<power_t, P>;
    }
    constexpr std::expected<
        basic_unit<
            std::common_type_t<value_t,V>,
            std::common_type_t<magnitude_t,M>,
            std::common_type_t<power_t,P>
        >,
        std::exception
    > devide(const basic_unit<V, M, P>& unit) const noexcept {
        if (unit.getValue() == 0)
            return std::unexpected(std::exception());
        std::set<
            basic_MP_dimension<
                std::common_type_t<magnitude_t,M>,
                std::common_type_t<power_t,P>
            >,
            MP_dimension_less<
                std::common_type_t<magnitude_t,M>,
                std::common_type_t<power_t,P>
            >
        > tmp_dimens;
        for (auto i : dimens){
            auto dimen = basic_MP_dimension<M, P>::convert(i);
            basic_dimension<
                std::common_type_t<magnitude_t,M>,
                std::common_type_t<power_t,P>
            > tmp_dimen {
                i.dimen.magnitude,
                i.dimen.power,
                i.dimen.short_name,
                i.dimen.dimension_name,
            };
            if (auto iter = unit.getDimensions().find(dimen); iter != unit.getDimensions().end())
                tmp_dimen.power -= iter->dimen.power;
            tmp_dimens.insert(
                basic_MP_dimension<
                    std::common_type_t<magnitude_t,M>,
                    std::common_type_t<power_t,P>
                > {
                    i.metric_prefix,
                    tmp_dimen
                }
            );
        }
        return basic_unit<
            std::common_type_t<value_t,V>,
            std::common_type_t<magnitude_t,M>,
            std::common_type_t<power_t,P>
        >(value / (MagnitudeCalculateFrom(unit) * unit.getValue()),tmp_dimens);
    }

    template<rational V,rational M,integral P>
    requires requires {
        requires std::common_with<value_t, V>;
        requires std::common_with<magnitude_t, M>;
        requires std::common_with<power_t, P>;
    }
    constexpr basic_unit<std::common_type_t<value_t,V>, std::common_type_t<magnitude_t,M>, std::common_type_t<power_t,P>> operator+(const basic_unit<V,M,P>& unit) const {
        auto u = plus(unit);
        if (u.has_value())
            return u.value();
        else throw u.error();
    }

    template<rational V,rational M,integral P>
    requires requires {
        requires std::common_with<value_t, V>;
        requires std::common_with<magnitude_t, M>;
        requires std::common_with<power_t, P>;
    }
    constexpr basic_unit<std::common_type_t<value_t,V>, std::common_type_t<magnitude_t,M>, std::common_type_t<power_t,P>> operator-(const basic_unit<V,M,P>& unit) const {
        auto u = minus(unit);
        if (u.has_value())
            return u.value();
        else throw u.error();
    }

    template<rational V,rational M,integral P>
    requires requires {
        requires std::common_with<value_t, V>;
        requires std::common_with<magnitude_t, M>;
        requires std::common_with<power_t, P>;
    }
    constexpr basic_unit<std::common_type_t<value_t,V>, std::common_type_t<magnitude_t,M>, std::common_type_t<power_t,P>> operator*(const basic_unit<V,M,P>& unit) const noexcept {
        return multiply(unit);
    }

    template<rational V,rational M,integral P>
    requires requires {
        requires std::common_with<value_t, V>;
        requires std::common_with<magnitude_t, M>;
        requires std::common_with<power_t, P>;
    }
    constexpr basic_unit<std::common_type_t<value_t,V>, std::common_type_t<magnitude_t,M>, std::common_type_t<power_t,P>> operator/(const basic_unit<V,M,P>& unit) const {
        auto u = devide(unit);
        if (u.has_value())
            return u.value();
        else throw u.error();
    }
};
#endif
