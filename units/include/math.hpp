#ifndef UNITS_MATH
#define UNITS_MATH

#include <concepts>

template<typename T>
concept rational = requires (T x, T y) {
	{x + y} -> std::convertible_to<T>;
	{x - y}  -> std::convertible_to<T>;
	{x * y} -> std::convertible_to<T>;
	{x / y} -> std::convertible_to<T>;
	{x += y} -> std::convertible_to<T>;
	{x -= y}  -> std::convertible_to<T>;
	{x *= y} -> std::convertible_to<T>;
	{x /= y} -> std::convertible_to<T>;
	{-x} -> std::convertible_to<T>;
	x = y;
	requires T(3) / T(2) == T(1.5);
	requires std::totally_ordered<T>;
};

template<typename T>
concept integral = requires (T x, T y) {
	{x + y} -> std::convertible_to<T>;
	{x - y}  -> std::convertible_to<T>;
	{x * y} -> std::convertible_to<T>;
	{x / y} -> std::convertible_to<T>;
	{x += y} -> std::convertible_to<T>;
	{x -= y}  -> std::convertible_to<T>;
	{x *= y} -> std::convertible_to<T>;
	{x /= y} -> std::convertible_to<T>;
	{-x} -> std::convertible_to<T>;
	x = y;
	requires T(3) / T(2) == T(1) || T(3) / T(2) == T(2);
	requires std::totally_ordered<T>;
};

template<rational T>
constexpr T pow(rational auto x,integral auto p){
    if (p > 0){
        T temp = 1;
        for (size_t i=0;i<p;i++)
                temp = temp * x;
        return temp;
    }
    else if (p == 0){
        if (x != 0)
            return 1;
        else
            throw std::exception();
    }
    else {
        T temp = 1;
        for (size_t i=0;i<-p;i++)
            temp = temp / x;
        return temp;
    }
}
#endif
