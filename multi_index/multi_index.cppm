export module multi_index;
import <vector>;
import <ranges>;
import <concepts>;
import <tuple>;

template <template <typename, typename, typename... Args> typename T>
struct map_wrapper {};

template <typename T, typename K, typename V>
struct map_wrapper_expand;
template <
    template <template <typename, typename, typename... Args> typename args> typename T,
    template <typename, typename, typename... Args> typename Other,
    typename K,
    typename V>
struct map_wrapper_expand<T<Other>, K, V>
{
    using type = Other<K, V>;
};

export template <template <typename, typename, typename... Args> typename... Others>
struct maps_wrapper {};

template <typename T>
struct maps_wrapper_expand;
template <template <template <typename, typename, typename... Args> typename... args> typename T, template <typename, typename, typename... Args> typename... Others>
struct maps_wrapper_expand<T<Others...>>
{
    using type = std::tuple<map_wrapper<Others>...>;
};

template <typename T>
using map_key = std::conditional_t<sizeof(T) < sizeof(size_t), T, std::reference_wrapper<T>>;
template <template <typename T, typename... Args> typename container, typename... Args>
using map_value = std::ranges::iterator_t<container<std::tuple<Args...>>>;

template <typename T, typename K, typename V, typename Index>
struct maps_tuple_before;
template <typename T, typename K, typename V, size_t... N>
struct maps_tuple_before<T, K, V, std::index_sequence<N...>>
{
    using type = std::tuple<typename map_wrapper_expand<
        std::tuple_element_t<N, typename maps_wrapper_expand<T>::type>,
        std::tuple_element_t<N, K>,
        V>::type...>;
};

template <
    template <typename T, typename... container_args> typename container,
    typename associative_containers,
    typename... args>
struct maps_tuple
{
    using type = maps_tuple_before<
        associative_containers,
        std::tuple<map_key<args>...>,
        map_value<container, args...>,
        std::make_index_sequence<sizeof...(args)>>::type;
};

template <
    template <typename T, typename... container_args> typename container,
    typename associative_containers,
    typename... args>
using maps_tuple_t = maps_tuple<container, associative_containers, args...>::type;

export template <
    template <typename T, typename... container_args> typename container,
    typename associative_containers,
    typename... args>
    requires requires(
        maps_tuple_t<container, associative_containers, args...> maps,
        map_key<args>... keys,
        map_value<container, args...> value) {
        std::ranges::common_range<container<std::tuple<args...>>>;
        std::ranges::sized_range<container<std::tuple<args...>>>;
        sizeof...(args) == std::tuple_size_v<decltype(maps)>;

        // common_range
        [&maps]<size_t... N>(std::index_sequence<N...>)
        {
            return (... && std::ranges::common_range<std::tuple_element_t<N, decltype(maps)>>);
        }(std::make_index_sequence<sizeof...(args)>{});

        // find
        [&maps]<size_t... N>(std::index_sequence<N...>, auto &&...keys) -> bool
        {
            return (... && std::same_as<
                               decltype(std::ranges::begin(std::get<N>(maps))),
                               decltype(std::get<N>(maps).find(keys))>);
        }(std::make_index_sequence<sizeof...(args)>{}, keys...);

        // count
        [&maps]<size_t... N>(std::index_sequence<N...>, auto &&...keys)
        {
            return (... && std::same_as<decltype(std::ranges::begin(std::get<N>(maps))),
                                        decltype(std::get<N>(maps).count(keys))>);
        }(std::make_index_sequence<sizeof...(args)>{}, keys...);

        // erase
        [&maps]<size_t... N>(std::index_sequence<N...>)
        {
            return (... && std::same_as<
                               decltype(std::ranges::begin(std::get<N>(maps))),
                               decltype(std::get<N>(maps).erase(std::ranges::begin(std::get<N>(maps))))>);
        }(std::make_index_sequence<sizeof...(args)>{});

        // insert
        [&maps, &value]<size_t... N>(std::index_sequence<N...>, auto &&...keys)
        {
            return (... && std::same_as<
                               decltype(std::ranges::begin(std::get<N>(maps))),
                               decltype(std::get<N>(maps).insert({keys, value}))>);
        }(std::make_index_sequence<sizeof...(args)>{}, keys...);

        // can decompress by function std::get<0>
        [&maps]<size_t... N>(std::index_sequence<N...>, auto &&...keys)
        {
            return (... && std::same_as<
                               decltype(keys),
                               decltype(std::get<0>(*std::ranges::begin(std::get<N>(maps))))>);
        }(std::make_index_sequence<sizeof...(args)>{}, keys...);

        // can decompress by function std::get<1>
        [&maps, &value]<size_t... N>(std::index_sequence<N...>)
        {
            return (... && std::same_as<
                               decltype(value),
                               decltype(std::get<1>(*std::ranges::begin(std::get<N>(maps))))>);
        }(std::make_index_sequence<sizeof...(args)>{});
    }
class multi_index
{
public:
    using data_list_t = std::tuple<args...>;
    using data_sheet_t = container<data_list_t>;
    using iterator = std::ranges::iterator_t<data_sheet_t>;
    using reverse_iterator = decltype(data_sheet_t{}.rbegin());
    using const_iterator = std::ranges::const_iterator_t<data_sheet_t>;
    using const_reverse_iterator = decltype(data_sheet_t{}.crbegin());
    using multi_index_t = maps_tuple<container, associative_containers, args...>::type;

private:
    data_sheet_t data_sheet;
    multi_index_t map;

    template <size_t... N>
    constexpr void make_indexs(iterator data, std::index_sequence<N...>)
    {
        (std::get<N>(map).insert({std::get<N>(*data), data}), ...);
    }
    template <size_t... N>
    constexpr void erase_indexs(const iterator &iter, std::index_sequence<N...>)
    {
        (std::get<N>(map).erase(std::get<N>(map).find(std::get<N>(*iter))), ...);
    }
    template <size_t... N>
    constexpr void clear_indexs(std::index_sequence<N...>)
        requires requires(multi_index_t th) { (std::get<N>(th).clear(), ...); }
    {
        (std::get<N>(map).clear, ...);
    }
    template <size_t... N>
    constexpr void swap_indexs(multi_index_t &other, std::index_sequence<N...>)
        requires requires(multi_index_t th, multi_index_t other) { (std::get<N>(th).swap(std::get<N>(other)), ...); }
    {
        (std::get<N>(map).swap(std::get<N>(other)), ...);
    }

public:
    constexpr multi_index() {}

    template <std::ranges::input_range Rng>
        requires requires { std::convertible_to<std::ranges::range_reference_t<Rng>, data_list_t>; }
    constexpr multi_index(Rng &&t) : data_sheet(std::ranges::to<data_sheet_t>(t))
    {
        for (iterator iter = data_sheet.begin(); iter != data_sheet.end(); iter++)
            make_indexs(iter, std::make_index_sequence<sizeof...(args)>{});
    }
    constexpr multi_index(const multi_index<container, associative_containers, args...> &d) : data_sheet(d.data_sheet), map(d.map) {}

    template <size_t N>
    constexpr std::vector<iterator> find(const std::tuple_element_t<N, data_list_t> &data)
    {
        std::vector<iterator> tmp;
        auto iter = std::get<N>(map).find(data);
        if (iter != std::get<N>(map).end())
            for (size_t i = 0; i < std::get<N>(map).count(std::get<0>(*iter)); i++)
                tmp.push_back(std::get<1>(*++iter));
        return tmp;
    }
    constexpr iterator begin()
    {
        return std::ranges::begin(data_sheet);
    }
    constexpr const_iterator cbegin() const
    {
        return std::ranges::cbegin(data_sheet);
    }
    constexpr iterator end()
    {
        return std::ranges::end(data_sheet);
    }
    constexpr const_iterator cend() const
    {
        return std::ranges::cend(data_sheet);
    }
    constexpr reverse_iterator rbegin()
        requires requires(data_sheet_t data) { data.rbegin(); }
    {
        return data_sheet.rbegin();
    }
    constexpr const const_reverse_iterator crbegin() const
        requires requires(data_sheet_t data) { data.crbegin(); }
    {
        return data_sheet.crbegin();
    }
    constexpr reverse_iterator rend()
        requires requires(data_sheet_t data) { data.rend(); }
    {
        return data_sheet.rend();
    }
    constexpr const const_reverse_iterator crend() const
        requires requires(data_sheet_t data) { data.crend(); }
    {
        return data_sheet.crend();
    }
    constexpr iterator insert(const iterator &iter, const data_list_t &data)
        requires requires(data_sheet_t data, iterator iter, data_list_t list) { data.insert(iter, list); }
    {
        iterator tmp = data_sheet.insert(iter, data);
        make_indexs(tmp, std::make_index_sequence<sizeof...(args)>{});
        return tmp;
    }
    template <std::ranges::input_range Rng>
        requires requires { std::convertible_to<std::ranges::range_reference_t<Rng>, data_list_t>; }
    constexpr iterator insert_range(const iterator &iter, Rng &&data)
        requires requires(data_sheet_t data, iterator iter, Rng &&range) { data.insert_range(iter, range); }
    {
        const size_t size = data.size();
        iterator tmp = data_sheet.insert_range(iter, data);
        iterator i = tmp;
        for (size_t j = 0; j < size; j++)
        {
            make_indexs(i, std::make_index_sequence<sizeof...(args)>{});
            i++;
        }
        return tmp;
    }
    constexpr void push_back(const data_list_t &data)
        requires requires(data_sheet_t data, data_list_t list) { data.push_back(list); }
    {
        data_sheet.push_back(data);
        make_indexs(std::prev(std::ranges::end(data_sheet)), std::make_index_sequence<sizeof...(args)>{});
        return;
    }
    constexpr iterator erase(const iterator &iter)
        requires requires(data_sheet_t data, iterator iter) { data.erase(iter); }
    {
        erase_indexs(iter, std::make_index_sequence<sizeof...(args)>{});
        return data_sheet.erase(iter);
    }
    constexpr void pop_back()
        requires requires(data_sheet_t data) { data.pop_back(); }
    {
        erase_indexs(data_sheet.end()--, std::make_index_sequence<sizeof...(args)>{});
        data_sheet.pop_back();
    }
    template <std::ranges::input_range Rng>
        requires std::convertible_to<std::ranges::range_reference_t<Rng>, data_list_t>
    constexpr void append_range(Rng &&data)
        requires requires(data_sheet_t data, Rng &&range) { data.append_range(range); }
    {
        size_t size = data.size();
        data_sheet.append_range(data);
        size_t i = 0;
        for (iterator tmp = std::prev(data_sheet.end()); i < size; i++)
        {
            make_indexs(tmp, std::make_index_sequence<sizeof...(args)>{});
            tmp = std::prev(tmp);
        }
    }
    constexpr void resize(const size_t &size)
        requires requires(data_sheet_t data, size_t size) { data.resize(size); }
    {
        data_sheet.resize(size);
    }
    constexpr void swap(multi_index<container, associative_containers, args...> &other)
        requires requires(data_sheet_t data, data_sheet_t &other) { data.swap(other); }
    {
        data_sheet.swap(other.data_sheet);
        swap_indexs(other.map, std::make_index_sequence<sizeof...(args)>{});
    }
    constexpr size_t size() const
    {
        return data_sheet.size();
    }
    constexpr void clear()
        requires requires(data_sheet_t data, maps_tuple_t maps) { data.clear(); maps.clear(); }
    {
        data_sheet.clear();
        clear_indexs(std::make_index_sequence<sizeof...(args)>{});
    }
    constexpr bool empty() const
        requires requires(data_sheet_t data) { data.empty(); }
    {
        return data_sheet.empty();
    }
    constexpr void push_front(const data_list_t &data)
        requires requires(data_sheet_t data, data_list_t list) { data.push_front(list); }
    {
        data_sheet.push_front(data);
        make_indexs(data_sheet.begin(), std::make_index_sequence<sizeof...(args)>{});
    }
    constexpr void pop_front()
        requires requires(data_sheet_t data) { data.pop_front(); }
    {
        erase_indexs(data_sheet.begin(), std::make_index_sequence<sizeof...(args)>{});
        data_sheet.pop_front();
    }
    template <std::ranges::input_range Rng>
        requires requires { std::convertible_to<std::ranges::range_reference_t<Rng>, data_list_t>; }
    constexpr void prepend_range(Rng &&range)
        requires requires(data_sheet_t data, Rng &&range) { data.prepend_range(range); }
    {
        data_sheet.prepend_range(range);
        iterator iter = data_sheet.begin();
        for (size_t i = 0; i < range.size(); i++)
        {
            make_indexs(iter, std::make_index_sequence<sizeof...(args)>{});
            iter++;
        }
    }
};