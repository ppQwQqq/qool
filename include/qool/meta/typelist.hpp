/**
 * @file qool/meta/typelist.hpp
 * @brief typelist utils
 */
#pragma once

#include <cstdint>
#include <type_traits>
#include <utility>

namespace qool::meta {

namespace tlist {
/*
 ****************************************
 *
 *      typelist
 *  Construct type as list(container)
 *  Aims to operate types in compile time for more flexibility
 *  Type can be operated like any variables
 *
 ****************************************
 */

/**
 * @brief typelist constructor
 *
 * tparam Ts types for typelist constructing
 */
template <typename... Ts> struct typelist {
  using type = typelist<Ts...>;
};

/**
 * @brief specified for typelist constructor base case also empty typelist
 *
 * @tparam empty
 */
template <> struct typelist<> {
  using type = typelist<>;
};

/*
 ******************************************
 *
 * typelist properties
 *   - typelist_size_v(typelist_size)
 *   - typelist_at_t(typelist_at_vv)
 *   - typelist_front_t(typelist_front)
 *   - typelist_back_t(typelist_back)
 *
 ******************************************
 */

/**************************typelist_size************************************/
/**
 * @brief get typelist size(count of type)
 *
 * @tparam List typelist container
 */
template <typename List> struct typelist_size;

/**
 * @brief specified for typelist to get its size
 *
 * @tparam Ts types in typelist
 */
template <typename... Ts> struct typelist_size<typelist<Ts...>> {
  static constexpr std::size_t value = sizeof...(Ts);
};

template <typename List>
inline constexpr std::size_t typelist_size_v = typelist_size<List>::value;

/**************************typelist_at************************************/
/**
 * @brief get type with index N in typelist
 *
 * @tparam N target type index in typelist
 * @tparam List target typelist
 */
template <std::size_t N, typename List> struct typelist_at;

/**
 * @brief get type with index N in typelist(traverse from First to Last)
 *
 * @tparam N size of current typelist
 * @tparam First find target type from First
 * @tparam Rest rest types to find
 */
template <std::size_t N, typename First, typename... Rest>
struct typelist_at<N, typelist<First, Rest...>> {
  using type = typename typelist_at<N - 1, typelist<Rest...>>::type;
};

/**
 * @brief specified for typelist with single type
 *
 * @tparam First single type
 * @tparam Rest single type
 */
template <typename First, typename... Rest>
struct typelist_at<0, typelist<First, Rest...>> {
  using type = First;
};

/**
 * @brief specified for empty typelist(typelist<>)
 *
 * @tparam N index of target type
 */
template <std::size_t N> struct typelist_at<N, typelist<>> {
  using type = void;
};

/**
 * @brief find type with index N in typelist with range check
 *
 * @tparam N size of current typelist
 * @tparam List target typelist
 */
template <std::size_t N, typename List> struct typelist_at_vv {
  static_assert(N < typelist_size_v<List>, "Index out of range");
  using type = typename typelist_at<N, List>::type;
};

template <std::size_t N, typename List>
using typelist_at_t = typename typelist_at_vv<N, List>::type;

/**************************typelist_front************************************/
/**
 * @brief get front type in typelist
 *
 * @tparam List target typelist
 */
template <typename List> struct typelist_front;

/**
 * @brief specified for getting the front type in typelist
 *
 * @tparam First first type in typelist
 * @tparam Rest rest type in typelist
 */
template <typename First, typename... Rest>
struct typelist_front<typelist<First, Rest...>> {
  using type = First;
};

template <typename List>
using typelist_front_t = typename typelist_front<List>::type;

/**************************typelist_back************************************/
/**
 * @brief get the back type in typelist
 *
 * @tparam List target typelist
 */
template <typename List> struct typelist_back;

/**
 * @brief specified for typelist to get back type
 *
 * @tparam First traverse from first type
 * @tparam Rest rest types to traverse
 */
template <typename First, typename... Rest>
struct typelist_back<typelist<First, Rest...>> {
  using type = typelist_back<typelist<Rest...>>::type;
};

/**
 * @brief specified for single type in typelist
 *
 * @tparam Last single type
 */
template <typename Last> struct typelist_back<typelist<Last>> {
  using type = Last;
};

/**
 * @brief specified for empty typelist
 *
 * @tparam empty typelist
 */
template <> struct typelist_back<typelist<>> {
  using type = void;
};

template <typename List>
using typelist_back_t = typename typelist_back<List>::type;

/*
 ******************************************
 *
 * typelist iterator
 *   - typelist_iterator
 *   - typelist_begin_t(typelist_begin)
 *   - typelist_end_t(typelist_end)
 *   - typelist_deref_t(typelist_deref)
 *   - typelist_next_t(typelist_next)
 *   - typelist_prev_t(typelist_prev)
 *   - typelist_iter_equal_v(typelist_iter_equal)
 *
 ******************************************
 */

/**************************typelist_iterator************************************/
struct input_iterator_tag {};
struct forward_iterator_tag : input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : bidirectional_iterator_tag {};

template <typename List, std::size_t Index> struct typelist_iterator {
  using iterator_category = random_access_iterator_tag;
  using type = /* If type is out of typelist bound, then it will be void */
      /*** TODO: Use lazy calculation to replace std::conditional_t***/
      std::conditional_t < Index<typelist_size_v<List>,
                                 typename typelist_at<Index, List>::type, void>;
  using value_type = type;
  using difference_type = typename std::ptrdiff_t;
  static constexpr std::size_t index = Index;
  using list_type = List;

  using next = typelist_iterator<List, Index + 1>;
  using prev = typelist_iterator<List, Index - 1>;

  template <std::size_t N> using advance = typelist_iterator<List, Index + N>;
  template <std::size_t N> using retreat = typelist_iterator<List, Index - N>;
  template <typename Other>
  static constexpr std::size_t dist = Other::index - Index;
};

/*****************************typelist_begin**************************************/
template <typename List> struct typelist_begin {
  using type = typelist_iterator<List, 0>;
};

template <typename List> using typelist_begin_t = typelist_begin<List>::type;

/******************************typelist_end***************************************/
template <typename List> struct typelist_end {
  using type = typelist_iterator<List, typelist_size_v<List>>;
};

template <typename List> using typelist_end_t = typelist_end<List>::type;

/*****************************typelist_deref**************************************/
template <typename Iter> struct typelist_deref;

template <typename List, std::size_t Index>
struct typelist_deref<typelist_iterator<List, Index>> {
  using type = typename typelist_at<Index, List>::type;
};

template <typename Iter> using typelist_deref_t = typelist_deref<Iter>::type;

/*****************************typelist_next***************************************/
template <typename Iter> struct typelist_next;

template <typename List, std::size_t Index>
struct typelist_next<typelist_iterator<List, Index>> {
  using type = typelist_iterator<List, Index + 1>;
};

template <typename Iter> using typelist_next_t = typelist_next<Iter>::type;

/*****************************typelist_prev***************************************/
template <typename Iter> struct typelist_prev;

template <typename List, std::size_t Index>
struct typelist_prev<typelist_iterator<List, Index>> {
  using type = typelist_iterator<List, Index - 1>;
};

template <typename Iter> using typelist_prev_t = typelist_prev<Iter>::type;

/**************************typelist_iter_equal************************************/
template <typename IterL, typename IterR> struct typelist_iter_equal {
  static constexpr bool value =
      std::is_same_v<typename IterL::list_type, typename IterR::list_type> &&
      (IterL::index == IterR::index);
};

template <typename IterL, typename IterR>
constexpr inline bool typelist_iter_equal_v =
    typelist_iter_equal<IterL, IterR>::value;

/*
 ******************************************
 *
 * typelist operator
 *   - typelist_cat_t(typelist_cat)
 *   - typelist_take_t(typelist_take)
 *   - typelist_drop_t(typelist_drop)
 *   - typelist_slice_t(typelist_slice)
 *
 ******************************************
 */

/**************************typelist_concat************************************/
/**
 * @brief concat two typelists
 *
 * @tparam Lists lists to concat
 */
template <typename... Lists> struct typelist_cat;

/**
 * @brief specified concat for two typelists
 *
 * @tparam TsL left typelists
 * @tparam TsR right typelists
 */
template <typename... TsL, typename... TsR>
struct typelist_cat<typelist<TsL...>, typelist<TsR...>> {
  using type = typelist<TsL..., TsR...>;
};

/**
 * @brief specified for single typelist
 *
 * @tparam List single typelist
 */
template <typename List> struct typelist_cat<List> {
  using type = List;
};

/**
 * @brief specified for types more than 3
 *
 * @tparam L1 first typelist
 * @tparam L2 second typelist
 * @tparam L3 third typelist
 * @tparam Rest rest typelists
 */
template <typename L1, typename L2, typename L3, typename... Rest>
struct typelist_cat<L1, L2, L3, Rest...> {
  using type = typename typelist_cat<typename typelist_cat<L1, L2>::type, L3,
                                     Rest...>::type;
};

/**
 * @brief specified for empty typelists
 *
 * @tparam empty typelist
 */
template <> struct typelist_cat<> {
  using type = typelist<>;
};

template <typename... Lists>
using typelist_cat_t = typename typelist_cat<Lists...>::type;

/**************************typelist_take************************************/
/**
 * @brief implementation of taking first N types in typelist
 *
 * @tparam N count of taking types
 * @tparam List target typelist
 * @tparam Acc accumulated taking types
 */
template <std::size_t N, typename List, typename Acc = typelist<>>
struct typelist_take_impl;

/**
 * @brief specified for empty typelist (N == 0)
 *
 * @tparam N count of taking types
 * @tparam List target typelist
 * @tparam AccTs accumulated taking types
 */
template <std::size_t N, typename List, typename... AccTs>
  requires(N == 0)
struct typelist_take_impl<N, List, typelist<AccTs...>> {
  using type = typelist<AccTs...>;
};

/**
 * @brief specified for non-empty typelist (N > 0)
 *
 * @tparam N count of taking types
 * @tparam First first type in typelist
 * @tparam Rest rest types in typelist
 * @tparam AccTs accumulated taking types
 */
template <std::size_t N, typename First, typename... Rest, typename... AccTs>
  requires(N > 0)
struct typelist_take_impl<N, typelist<First, Rest...>, typelist<AccTs...>> {
  using type = typename typelist_take_impl<N - 1, typelist<Rest...>,
                                           typelist<AccTs..., First>>::type;
};

/**
 * @brief specified for empty typelist (N > 0)
 *
 * @tparam N count of taking types
 * @tparam AccTs accumulated taking types
 */
template <std::size_t N, typename... AccTs>
  requires(N > 0)
struct typelist_take_impl<N, typelist<>, typelist<AccTs...>> {
  using type = typelist<AccTs...>;
};

/**
 * @brief wrapper of typelist_take_impl
 *
 * @tparam N count of taking types
 * @tparam List target typelist
 */
template <std::size_t N, typename List> struct typelist_take {
  using type = typename typelist_take_impl<N, List, typelist<>>::type;
};

template <std::size_t N, typename List>
using typelist_take_t = typename typelist_take<N, List>::type;

/**************************typelist_drop************************************/
/**
 * @brief implementation of drop first N types
 *
 * @tparam N count of dropping types
 * @tparam List target typelist
 */
template <std::size_t N, typename List> struct typelist_drop_impl;

/**
 * @brief specified for empty list (N == 0)
 *
 * @tparam N count of dropping types
 * @tparam List target list
 */
template <std::size_t N, typename List>
  requires(N == 0)
struct typelist_drop_impl<N, List> {
  using type = List;
};

/**
 * @brief specified for empty list (N > 0)
 *
 * @tparam N count of dropping types
 * @tparam List target list
 */
template <std::size_t N, typename First, typename... Rest>
  requires(N > 0)
struct typelist_drop_impl<N, typelist<First, Rest...>> {
  using type = typename typelist_drop_impl<N - 1, typelist<Rest...>>::type;
};

/**
 * @brief specified for empty list (N > 0)
 *
 * @tparam N count of dropping types
 */
template <std::size_t N>
  requires(N > 0)
struct typelist_drop_impl<N, typelist<>> {
  using type = typelist<>;
};

/**
 * @brief wrapper of typelist_drop_impl
 *
 * @tparam N count of dropping types
 * @tparam List target typelist
 */
template <std::size_t N, typename List> struct typelist_drop {
  using type = typename typelist_drop_impl<N, List>::type;
};

template <std::size_t N, typename List>
using typelist_drop_t = typelist_drop<N, List>::type;

/**************************typelist_slice************************************/
/**
 * @brief slice a typelist in range [Start, End)
 *
 * @tparam Start left side of slice range
 * @tparam End right side of slice range
 * @tparam List target list
 */
template <std::size_t Start, std::size_t End, typename List>
struct typelist_slice {
  static_assert(Start <= End, "Start must be <= End");
  using type = typelist_take_t<End - Start, typelist_drop_t<Start, List>>;
};

template <std::size_t Start, std::size_t End, typename List>
using typelist_slice_t = typename typelist_slice<Start, End, List>::type;

/*
 ******************************************
 *
 * typelist algos
 *   - typelist_reverse_t(typelist_reverse)
 *   - typelist_contains_v(typelist_contains)
 *   - typelist_index_v(typelist_index)
 *   - typelist_find(typelist_contains_v, typelist_index_v)
 *   - typelist_unique_t(tyeplist_unique_t)
 *   - typelist_filter_t(typelist_filter)
 *   - typelist_transform_t(typelist_transform)
 *   - typelist_for_each
 *   - typelist_fold_t(typelist_fold)
 *
 ******************************************
 */

/**************************typelist_reverse************************************/
/**
 * @brief reverse typelist
 *
 * @tparam List target typelist
 */
template <typename List> struct typelist_reverse;

/**
 * @brief specified for empty typelist
 */
template <> struct typelist_reverse<typelist<>> {
  using type = typelist<>;
};

/**
 * @brief specified for typelist<First, Rest...>
 *
 * @tparam First first type in typelist
 * @tparam Rest rest types in typelist
 */
template <typename First, typename... Rest>
struct typelist_reverse<typelist<First, Rest...>> {
  using type =
      typelist_cat_t<typename typelist_reverse<typelist<Rest...>>::type,
                     typelist<First>>;
};

template <typename List>
using typelist_reverse_t = typename typelist_reverse<List>::type;

/**************************typelist_contains************************************/
/**
 * @brief whether target type in typelist
 *
 * @tparam T target type
 * @tparam List target typelist
 */
template <typename T, typename List> struct typelist_contains;

/**
 * @brief specified for empty typelist
 *
 * @tparam T any type
 */
template <typename T> struct typelist_contains<T, typelist<>> {
  static constexpr bool value = false;
};

/**
 * @brief specified for typelist_contains<T, typelist<First, Rest...>>
 *
 * @tparam T target type
 * @tparam First first type in current typelist
 * @tparam Rest rest types in current typelist
 */
template <typename T, typename First, typename... Rest>
struct typelist_contains<T, typelist<First, Rest...>> {
  static constexpr bool value = std::is_same_v<T, First> ||
                                typelist_contains<T, typelist<Rest...>>::value;
};

template <typename T, typename List>
inline constexpr bool typelist_contains_v = typelist_contains<T, List>::value;

/**
 * @brief general semantic alias for typelist_contains_v
 */
template <typename T, typename List>
concept is_one_of = typelist_contains_v<T, List>;

/**************************typelist_index************************************/
/**
 * @brief find index of the first target type in typelist
 *
 * @tparam T target type
 * @tparam List target typelist
 */
template <typename T, typename List> struct typelist_index;

/**
 * @brief specified for the case when index is 0
 *
 * @tparam T target type
 * @tparam Ts target typelist
 */
template <typename T, typename... Ts>
struct typelist_index<T, typelist<T, Ts...>> {
  static constexpr std::size_t value = 0;
};

/**
 * @brief specified for typelist_index<T, typelist<First, Rest...>>
 *
 * @tparam T target type
 * @tparam First first type in current typelist
 * @tparam Rest rest types in current typelist
 */
template <typename T, typename First, typename... Rest>
struct typelist_index<T, typelist<First, Rest...>> {
  static constexpr std::size_t value =
      1 + typelist_index<T, typelist<Rest...>>::value;
};

template <typename T, typename List>
inline constexpr std::size_t typelist_index_v = typelist_index<T, List>::value;

/***typelist_find***/
template <typename T, typename... Lists> struct typelist_find;

template <typename T, typename FirstList, typename... RestLists>
struct typelist_find<T, FirstList, RestLists...> {
  static constexpr bool found_in_first = typelist_contains_v<T, FirstList>;

  using type =
      /*** TODO: Use lazy calculation to replace std::conditional_t***/
      std::conditional_t<found_in_first, FirstList,
                         typename typelist_find<T, RestLists...>::type>;

  static constexpr bool found =
      found_in_first || typelist_find<T, RestLists...>::found;
};

template <typename T, typename LastList> struct typelist_find<T, LastList> {
  static constexpr bool found = typelist_contains_v<T, LastList>;
  using type = LastList;
};

/***typelist_find_t***/
template <typename T, typename List> struct typelist_find_t {
  static constexpr bool found = typelist_contains_v<T, List>;
  static constexpr std::size_t index = typelist_index_v<T, List>;
};

/**************************typelist_unique************************************/
template <typename List> struct typelist_unique;

template <> struct typelist_unique<typelist<>> {
  using type = typelist<>;
};

template <typename First, typename... Rest>
struct typelist_unique<typelist<First, Rest...>> {
  /*** TODO: Use lazy calculation to replace std::conditional_t***/
  using type = std::conditional_t<
      typelist_contains_v<First, typelist<Rest...>>,
      typename typelist_unique<typelist<Rest...>>::type,
      typelist_cat_t<typelist<First>,
                     typename typelist_unique<typelist<Rest...>>::type>>;
};

template <typename List>
using typelist_unique_t = typename typelist_unique<List>::type;

/**************************typelist_filter************************************/
template <template <typename> typename Pred, typename List>
struct typelist_filter;

template <template <typename> typename Pred>
struct typelist_filter<Pred, typelist<>> {
  using type = typelist<>;
};

template <template <typename> typename Pred, typename First, typename... Rest>
struct typelist_filter<Pred, typelist<First, Rest...>> {
  /*** TODO: Use lazy calculation to replace std::conditional_t***/
  using type = std::conditional_t<
      Pred<First>::value,
      typelist_cat_t<typelist<First>,
                     typename typelist_filter<Pred, typelist<Rest...>>::type>,
      typename typelist_filter<Pred, typelist<Rest...>>::type>;
};

template <template <typename> typename Pred, typename List>
using typelist_filter_t = typename typelist_filter<Pred, List>::type;

/**************************typelist_transform************************************/
template <template <typename> typename Func, typename... Ts>
struct typelist_transform;

template <template <typename> typename Func, typename... Ts>
struct typelist_transform<Func, typelist<Ts...>> {
  using type = typelist<typename Func<Ts>::type...>;
};

template <template <typename> typename Func, typename List>
using typelist_transform_t = typename typelist_transform<Func, List>::type;

/**************************typelist_for_each************************************/
template <typename List, template <typename> typename Func>
struct typelist_for_each;

template <template <typename> typename Func, typename... Ts>
struct typelist_for_each<typelist<Ts...>, Func> {
  template <typename... Args> static constexpr void apply(Args &&...args) {
    (Func<Ts>::apply(std::forward<Args>(args)...), ...);
  }
};

/**************************typelist_fold************************************/
template <template <typename, typename> typename Op, typename Init,
          typename List>
struct typelist_fold;

template <template <typename, typename> typename Op, typename Init>
struct typelist_fold<Op, Init, typelist<>> {
  using type = Init;
};

template <template <typename, typename> typename Op, typename Init,
          typename First, typename... Rest>
struct typelist_fold<Op, Init, typelist<First, Rest...>> {
  using type = typename typelist_fold<Op, typename Op<Init, First>::type,
                                      typelist<Rest...>>::type;
};

template <template <typename, typename> typename Op, typename Init,
          typename List>
using typelist_fold_t = typename typelist_fold<Op, Init, List>::type;

/*
 ******************************************
 *
 * typelist editor
 *   - typelist_push_back_t(typelist_push_back)
 *   - typelist_push_front_t(typelist_push_front)
 *   - typelist_pop_front_t(typelist_pop_front)
 *   - typelist_pop_back_t(typelist_pop_back)
 *   - typelist_insert_t(typelist_insert)
 *   - typelist_erase_t(typelist_erase)
 *   - typelist_replace_t(typelist_replace)
 *
 ******************************************
 */

/**************************typelist_push_back************************************/

/**
 * @brief push back type(s) in typelist
 *
 * @tparam List target typelist
 * @tparam Ts pushed types
 */
template <typename List, typename... Ts> struct typelist_push_back;

/**
 * @brief push back type(s) in typelists
 *
 * @tparam ListTs target typelists
 * @tparam Ts pushed types
 */
template <typename... ListTs, typename... Ts>
struct typelist_push_back<typelist<ListTs...>, Ts...> {
  using type = typelist<ListTs..., Ts...>;
};

template <typename List, typename... Ts>
using typelist_push_back_t = typelist_push_back<List, Ts...>::type;

/**************************typelist_push_front************************************/

/**
 * @brief push front type(s) in typelist
 *
 * @tparam List target typelist
 * @tparam Ts pushed types
 */
template <typename List, typename... Ts> struct typelist_push_front;

/**
 * @brief push front type(s) in typelists
 *
 * @tparam ListTs target typelists
 * @tparam Ts pushed types
 */
template <typename... ListTs, typename... Ts>
struct typelist_push_front<typelist<ListTs...>, Ts...> {
  using type = typelist<Ts..., ListTs...>;
};

template <typename List, typename... Ts>
using typelist_push_front_t = typename typelist_push_front<List, Ts...>::type;

/**************************typelist_pop_front************************************/
/**
 * @brief pop front type(s) in typelists
 *
 * @tparam List target list
 */
template <typename List> struct typelist_pop_front;

/**
 * @brief specified for typelist to pop front type in typelists
 *
 * @tparam First front type to pop
 * @tparam Rest rest types in popped typelist
 */
template <typename First, typename... Rest>
struct typelist_pop_front<typelist<First, Rest...>> {
  using type = typelist<Rest...>;
};

/**
 * @brief specified for empty typelist
 */
template <> struct typelist_pop_front<typelist<>> {
  using type = typelist<>;
};

template <typename List>
using typelist_pop_front_t = typelist_pop_front<List>::type;

/***************************typelist_pop_back************************************/
template <typename List> struct typelist_pop_back;

template <typename List>
using typelist_pop_back_t =
    typelist_reverse_t<typelist_pop_front_t<typelist_reverse_t<List>>>;

/**************************typelist_insert_t************************************/
template <std::size_t N, typename List, typename... Ts> struct typelist_insert;

template <std::size_t N, typename... ListTs, typename... Ts>
struct typelist_insert<N, typelist<ListTs...>, Ts...> {
  using prefix = typelist_take_t<N, typelist<ListTs...>>;
  using suffix = typelist_drop_t<N, typelist<ListTs...>>;
  using type = typelist_cat_t<prefix, typelist<Ts...>, suffix>;
};

template <std::size_t N, typename List, typename... Ts>
using typelist_insert_t = typename typelist_insert<N, List, Ts...>::type;

/**************************typelist_erase_t************************************/
template <std::size_t N, typename List> struct typelist_erase;

template <std::size_t N, typename List>
using typelist_erase_t =
    typelist_cat_t<typelist_take_t<N, List>, typelist_drop_t<N + 1, List>>;

/**************************typelist_replace_t************************************/
template <std::size_t N, typename List, typename T> struct typelist_replace;

template <std::size_t N, typename List, typename T>
using typelist_replace_t = typelist_insert_t<N, typelist_erase_t<N, List>, T>;

/**************************typelist_product************************************/

/**
 * @brief cartesian product for a single type and a typelist
 *
 * @tparam T target type from the first list
 * @tparam List target typelist (second first)
 * @tparam Wrapper template class to wrap the combined types
 */
template <typename T, typename List,
          template <typename, typename> class Wrapper>
struct typelist_product_single;

template <typename T, typename... Us,
          template <typename, typename> class Wrapper>
struct typelist_product_single<T, typelist<Us...>, Wrapper> {
  using type = typelist<Wrapper<T, Us>...>;
};

/**
 * @brief cartesian product of two typelists
 *
 * @tparam List1 first typelist
 * @tparam List2 second typelist
 * @tparam Wrapper template class to wrap the combined types/typelist
 */
template <typename List1, typename List2,
          template <typename, typename> class Wrapper>
struct typelist_product;

template <typename... Ts, typename List2,
          template <typename, typename> class Wrapper>
struct typelist_product<typelist<Ts...>, List2, Wrapper> {
  using type = typelist_cat_t<
      typename typelist_product_single<Ts, List2, Wrapper>::type...>;
};

/**
 * @brief specified for empty first list
 */
template <typename List2, template <typename, typename> class Wrapper>
struct typelist_product<typelist<>, List2, Wrapper> {
  using type = typelist<>;
};

template <typename List1, typename List2,
          template <typename, typename> class Wrapper>
using typelist_product_t =
    typename typelist_product<List1, List2, Wrapper>::type;

} // namespace tlist

/***Export typelist***/
template <typename... Ts> using typelist = tlist::typelist<Ts...>;

/*****typelist properties*****/
template <typename List>
constexpr std::size_t typelist_size = tlist::typelist_size_v<List>;

template <std::size_t N, typename List>
using typelist_at = tlist::typelist_at_t<N, List>;

template <typename List>
using typelist_front = typename tlist::typelist_front<List>::type;

template <typename List>
using typelist_back = typename tlist::typelist_back<List>::type;

/*****typelist iterator*****/
template <typename List, std::size_t Index>
using typelist_iterator = tlist::typelist_iterator<List, Index>;

template <typename List> using typelist_begin = tlist::typelist_begin_t<List>;

template <typename List> using typelist_end = tlist::typelist_end_t<List>;

template <typename Iter> using typelist_deref = tlist::typelist_deref_t<Iter>;

template <typename Iter> using typelist_next = tlist::typelist_next_t<Iter>;

template <typename Iter> using typelist_prev = tlist::typelist_prev_t<Iter>;

template <typename IterL, typename IterR>
constexpr inline bool typelist_iter_equal =
    tlist::typelist_iter_equal_v<IterL, IterR>;

/*****typelist operator*****/
template <typename... Lists>
using typelist_cat = tlist::typelist_cat_t<Lists...>;

template <std::size_t N, typename List>
using typelist_take = tlist::typelist_take_t<N, List>;

template <std::size_t N, typename List>
using typelist_drop = tlist::typelist_drop_t<N, List>;

template <std::size_t Start, std::size_t End, typename List>
using typelist_slice = tlist::typelist_slice_t<Start, End, List>;

/*****typelist algos*****/
template <typename List>
using typelist_reverse = tlist::typelist_reverse_t<List>;

template <typename T, typename List>
inline constexpr bool typelist_contains = tlist::typelist_contains_v<T, List>;

template <typename T, typename List>
inline constexpr bool is_one_of = tlist::is_one_of<T, List>;

template <typename T, typename List>
inline constexpr std::size_t typelist_index = tlist::typelist_index_v<T, List>;

template <typename T, typename... Lists>
using typelist_find = tlist::typelist_find<T, Lists...>;

template <typename T, typename List>
using typelist_find_t = tlist::typelist_find_t<T, List>;

template <typename List> using typelist_unique = tlist::typelist_unique_t<List>;

template <template <typename> typename Pred, typename List>
using typelist_filter = tlist::typelist_filter_t<Pred, List>;

template <template <typename> typename Func, typename List>
using typelist_transform = tlist::typelist_transform_t<Func, List>;

template <typename List, template <typename> typename Func>
using typelist_for_each = tlist::typelist_for_each<List, Func>;

template <template <typename, typename> typename Op, typename Init,
          typename List>
using typelist_fold = tlist::typelist_fold_t<Op, Init, List>;

/*****typelist editor*****/
template <typename List, typename... Ts>
using typelist_push_back = tlist::typelist_push_back_t<List, Ts...>;

template <typename List, typename... Ts>
using typelist_push_front = tlist::typelist_push_front_t<List, Ts...>;

template <typename List>
using typelist_pop_front = tlist::typelist_pop_front_t<List>;

template <typename List>
using typelist_pop_back = tlist::typelist_pop_back_t<List>;

template <std::size_t N, typename List, typename... Ts>
using typelist_insert = tlist::typelist_insert_t<N, List, Ts...>;

template <std::size_t N, typename List>
using typelist_erase = tlist::typelist_erase_t<N, List>;

template <std::size_t N, typename List, typename T>
using typelist_replace = tlist::typelist_replace_t<N, List, T>;

/*****typelist product******/
template <typename List1, typename List2,
          template <typename, typename> class Wrapper>
using typelist_product = tlist::typelist_product_t<List1, List2, Wrapper>;

} // namespace qool::meta
