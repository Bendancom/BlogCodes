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
    
    basic_unit() {}
    basic_unit(const basic_unit<value_t, magnitude_t, power_t>&) = default;
    
    basic_unit(const value_t& value) : value(value) {}

    template<rational M,integral P>
    requires requires {
        requires std::convertible_to<M,magnitude_t>;
        requires std::convertible_to<P,power_t>;
    }
    basic_unit(const value_t& value,const basic_MP_dimension<M, P>& dimen) : value(value) {
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

    template<rational V,rational M,integral P>
    requires requires {
        requires std::common_with<value_t, V>;
        requires std::common_with<magnitude_t, M>;
        requires std::common_with<power_t, P>;
    }
    constexpr std::expected<basic_unit<std::common_type_t<value_t,V>, std::common_type_t<magnitude_t,M>, std::common_type_t<power_t,P>>,std::exception> plus(const basic_unit<V,M,P>& unit) const {
        if (dimens.size() == unit.dimens.size()) {
            auto i = dimens.begin();
            auto j = unit.dimens.begin();
            for (;i != dimens.end() && j != unit.dimens.end();) {
                if (!i->type_equal(*j))
                    return std::unexpected(std::exception());
                i++;
                j++;
            }
            return basic_unit<std::common_type_t<value_t,V>, std::common_type_t<magnitude_t,M>, std::common_type_t<power_t,P>>(unit.value + value,dimens);
        }
        else return std::unexpected(std::exception());
    }

    template<rational V,rational M,integral P>
    requires requires {
        requires std::common_with<value_t, V>;
        requires std::common_with<magnitude_t, M>;
        requires std::common_with<power_t, P>;
    }
    constexpr std::expected<basic_unit<std::common_type_t<value_t,V>, std::common_type_t<magnitude_t,M>,std::common_type_t<power_t,P>>,std::exception> minus(const basic_unit<V, M, P>& unit) const {
        if (dimens.size() == unit.dimens.size()){
            auto i = dimens.begin();
            auto j = unit.dimens.begin();
            for (;i != dimens.end() && j != unit.dimens.end();){
                if (!i->type_equal(*j))
                    return std::unexpected(std::exception());
                i++;
                j++;
            }
            return basic_unit<std::common_type_t<value_t,V>, std::common_type_t<magnitude_t,M>, std::common_type_t<power_t,P>>(value - unit.value,dimens);
        }
        else return std::unexpected(std::exception());
    }

    template<rational V,rational M,integral P>
    requires requires {
        requires std::common_with<value_t, V>;
        requires std::common_with<magnitude_t, M>;
        requires std::common_with<power_t, P>;
    }
    constexpr basic_unit<std::common_type_t<value_t,V>, std::common_type_t<magnitude_t,M>,std::common_type_t<power_t,P>> multiply(const basic_unit<V, M, P>& unit) const {
        std::set<basic_MP_dimension<std::common_type_t<magnitude_t,M>, std::common_type_t<power_t,P>>,MP_dimension_less<std::common_type_t<magnitude_t,M>, std::common_type_t<power_t,P>>> tmp_dimens;
        for (auto i : dimens){
            auto dimen = basic_MP_dimension<M, P>::convert(i);
            if (auto iter = unit.dimens.find(dimen); iter != unit.dimens.end()){
                 tmp_dimens.insert(basic_MP_dimension<std::common_type_t<magnitude_t,M>, std::common_type_t<power_t,P>>{i.magnitude,iter->power + i.power,i.short_name,i.dimension_name});
            }
            else tmp_dimens.insert(i);
        }
        return basic_unit<std::common_type_t<value_t,V>, std::common_type_t<magnitude_t,M>, std::common_type_t<power_t,P>>(value * unit.value,tmp_dimens);
    }

    template<rational V,rational M,integral P>
    requires requires {
        requires std::common_with<value_t, V>;
        requires std::common_with<magnitude_t, M>;
        requires std::common_with<power_t, P>;
    }
    constexpr std::expected<basic_unit<std::common_type_t<value_t,V>, std::common_type_t<magnitude_t,M>,std::common_type_t<power_t,P>>,std::exception> devide(const basic_unit<V, M, P>& unit) const {
        if (unit.value == 0)
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
            if (auto iter = unit.dimens.find(dimen); iter != unit.dimens.end()){
                tmp_dimens.insert(basic_MP_dimension<std::common_type_t<magnitude_t,M>, std::common_type_t<power_t,P>>{
                    i.metric_prefix,
                    {
                        i.dimen.magnitude,
                        iter->dimen.power - i.dimen.power,
                        i.dimen.short_name,
                        i.dimen.dimension_name
                    }
                });
            }
            else tmp_dimens.insert(i);
        }
        return basic_unit<std::common_type_t<value_t,V>, std::common_type_t<magnitude_t,M>, std::common_type_t<power_t,P>>(value / unit.value,tmp_dimens);
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
public:
    value_t value;
    std::set<basic_MP_dimension<magnitude_t, power_t>,MP_dimension_less<magnitude_t,power_t>> dimens;
};
#endif
