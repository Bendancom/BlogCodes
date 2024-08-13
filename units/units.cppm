export module units;

#include <typeindex>
#include <concepts>

template<typename T>
concept rational = requires (T x, T y) {
	{x + y} -> std::convertible_to<T>;
	{x - y}  -> std::convertible_to<T>;
	{x* y} -> std::convertible_to<T>;
	{x / y} -> std::convertible_to<T>;
	{-x} -> std::same_as<T>;
	x = y;
	requires T(3) / T(2) == T(1.5);
	requires std::totally_ordered<T>;
};

export struct basic_dimension{
	float magnitude;
    char power;
    template<typename Self>
    constexpr std::type_index getId(this Self&& self){
        return typeid(self);
    }
    template<typename Self>
    constexpr operator std::type_index(this Self&& self){
        return typeid(self);
    }
    constexpr bool operator==(const basic_dimension&) const = default;
};

export
template<>
struct std::hash<basic_dimension>{
    static constexpr size_t operator()(const std::convertible_to<basic_dimension> auto& dimen){
       return dimen.getId().hash_code();
    }
};

export namespace dimensions{
    struct Length : public basic_dimension{};
    struct Mass : public basic_dimension{};
}
using namespace dimensions;

export namespace units{
    constexpr Length meter = {1,1};
    constexpr Length litre = {1e-1,3};
    constexpr Length inch = {2.54e-2,1};
    constexpr Mass gram = {1,1};
}

