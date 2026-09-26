/**
 * @file qool/meta/refl.hpp
 * @brief reflection utils
 */
#pragma once

#include "qool/meta/typelist.hpp"
#include <map>
#include <meta>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

namespace qool::meta::refl {

/**
 * @brief value category
 *   Boolean:       bool
 *   SignedInt:     int
 *   UnsignedInt:   std::size_t
 *   FloatingPoint: double
 *   String:        std::string
 *   Array:         std::vector
 *   Optional:      std::optional
 *   Object:        T
 *   Dictionary:    std::map, std::unordered_map
 *   Variant:       std::variant
 *   Enumeration:   enum
 *   Unsupported:   * not supported type *
 */
namespace vc {

struct Boolean {
  static constexpr std::string_view name = "boolean";
};

struct SignedInt {
  static constexpr std::string_view name = "signed_integer";
};

struct UnsignedInt {
  static constexpr std::string_view name = "unsigned_integer";
};

struct FloatingPoint {
  static constexpr std::string_view name = "floating_point";
};

struct String {
  static constexpr std::string_view name = "string";
};

struct Array {
  static constexpr std::string_view name = "array";
};

struct Optional {
  static constexpr std::string_view name = "optional";
};

struct Object {
  static constexpr std::string_view name = "object";
};

struct Dictionary {
  static constexpr std::string_view name = "dictionary";
};

struct Variant {
  static constexpr std::string_view name = "variant";
};

struct Enumeration {
  static constexpr std::string_view name = "enumeration";
};

struct Unsupported {
  static constexpr std::string_view name = "unsupported";
};

/**
 * @brief value category list
 */
using VCList =
    typelist<Boolean, SignedInt, UnsignedInt, FloatingPoint, String, Array,
             Optional, Object, Dictionary, Variant, Enumeration, Unsupported>;

/**
 * @brief scalar category list
 */
using SCList =
    typelist<Boolean, SignedInt, UnsignedInt, FloatingPoint, Enumeration>;

template <typename T>
concept Tag = is_one_of<std::remove_cvref_t<T>, VCList>;

template <typename T>
concept Scalar = is_one_of<std::remove_cvref_t<T>, SCList>;

template <typename T>
concept AllSerializable =
    Tag<T> && !std::same_as<std::remove_cvref_t<T>, Unsupported>;
} // namespace vc

/**
 * @brief supported annotations
 */
namespace ann {

/**
 * @brief none (annotations)
 */
struct none_t {};

inline constexpr none_t none{};

/**
 * @brief rename
 */
struct name_t {
  const char *value;
};

consteval name_t name(std::string_view value) {
  return {std::define_static_string(value)};
}

/**
 * @brief ignore
 */
struct ignore_t {};

inline constexpr ignore_t ignore{};

/**
 * @brief description
 */
struct desc_t {
  const char *value;
};

consteval desc_t desc(std::string_view value) {
  return {std::define_static_string(value)};
}

/**
 * @brief short name, like [help] -> [h]
 */
struct short_name_t {
  char value;
};

consteval short_name_t short_name(char c) { return {c}; }

/**
 * @brief omit empty, skipping null/empty struct/class
 */
struct omit_empty {};

/**
 * @brief throw if missing
 */
struct required_t {};

inline constexpr required_t required{};

/**
 * @brief inline nested struct fields into parent
 */
struct flatten_t {};

/**
 * @brief convert nested struct or enums to string
 */
struct as_string_t {};

/**
 * @brief Date/time formats, precision, etc.
 */
struct format_t {};

/**
 * @brief for deserialization fallback
 */
struct default_value_t {};

} // namespace ann

namespace internal {

/**
 * @brief std::meta::info elems array
 */
template <std::size_t N> struct minfo_array {
  std::meta::info elems[N];

  consteval const auto begin() const -> const std::meta::info * {
    return elems;
  }
  consteval const auto end() const -> const std::meta::info * {
    return elems + N;
  }
};

template <> struct minfo_array<0> {
  consteval const auto begin() const -> const std::meta::info * {
    return nullptr;
  }
  consteval const auto end() const -> const std::meta::info * {
    return nullptr;
  }
};

/**
 * @brief annotations array, getting annotations of member
 */
template <std::meta::info Member> consteval auto anns_array() {
  constexpr std::size_t size = std::meta::annotations_of(Member).size();
  minfo_array<size> result{};

  if constexpr (size > 0) {
    auto annotations = std::meta::annotations_of(Member);
    for (std::size_t i = 0; i < size; ++i) {
      result.elems[i] = annotations[i];
    }
  }
  return result;
}

/**
 * @brief fields(members) array of T, struct/class
 */
template <typename T> consteval auto fields_array() {
  constexpr auto ctx = std::meta::access_context::current();
  constexpr std::size_t size =
      std::meta::nonstatic_data_members_of(^^T, ctx).size();
  minfo_array<size> result;

  if constexpr (size > 0) {
    auto members = std::meta::nonstatic_data_members_of(^^T, ctx);
    for (std::size_t i = 0; i < size; ++i) {
      result.elems[i] = members[i];
    }
  }
  return result;
}

/**
 * @brief enumerators array of E, enum
 */
template <typename E> consteval auto enums_array() {
  using U = std::remove_cvref_t<E>;
  constexpr std::size_t size = std::meta::enumerators_of(^^U).size();
  minfo_array<size> result{};

  if constexpr (size > 0) {
    auto enumerators = std::meta::enumerators_of(^^U);
    for (std::size_t i = 0; i < size; ++i) {
      result.elems[i] = enumerators[i];
    }
  }
  return result;
}

/**
 * @brief get the string representation of a template type name
 *
 * @tparam T type name
 * @return type name in string
 */
template <typename T> consteval auto type_name_of() {

  std::string_view name =
      std::meta::display_string_of(^^std::remove_cvref_t<T>);

  /**
   * the implementation of std::meta::display_string_of
   *   depends on AST
   * thus, different compiler needs the different preprocess
   */
#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTERL_COMPILER)

  if (name.starts_with("class ")) {
    name.remove_prefix(6);
  } else if (name.starts_with("struct ")) {
    name.remove_prefix(7);
  } else if (name.starts_with("union ")) {
    name.remove_prefix(6);
  } else if (name.starts_with("enum ")) {
    name.remove_prefix(5);
  }

  while (!name.empty() &&
         !((name.back() >= 'a' && name.back() <= 'z') ||
           (name.back() >= 'A' && name.back() <= 'Z') ||
           (name.back() >= '0' && name.back() <= '9') || name.back() == '_')) {
    name.remove_suffix(1);
  }

  return name;

#elif defined(__clang__)

  if (name.starts_with("class ")) {
    name.remove_prefix(6);
  } else if (name.starts_with("struct ")) {
    name.remove_prefix(7);
  } else if (name.starts_with("union ")) {
    name.remove_prefix(6);
  } else if (name.starts_with("enum ")) {
    name.remove_prefix(5);
  }

  return name;

#else

  return name;

#endif
}

/**
 * @brief template arguments array, getting annotations of member
 */
template <typename T> consteval auto template_args_info_array() {
  constexpr std::size_t size = std::meta::template_arguments_of(^^T).size();
  minfo_array<size> result{};

  if constexpr (size > 0) {
    auto arguments = std::meta::template_arguments_of(^^T);
    for (std::size_t i = 0; i < size; ++i) {
      result.elems[i] = arguments[i];
    }
  }
  return result;
}
} // namespace internal

/**
 * @brief excluded type for reflection
 *   like std::string, std::vector, std::map, std::pair, etc.
 *   these are built-in types, thus, they are too complex to reflect because of
 *   its members and internal implementation
 */
namespace exclude {

/** std::string **/
template <typename T>
inline constexpr bool is_string_v =
    std::is_same_v<std::remove_cvref_t<T>, std::string>;

/** std::vector **/
template <typename T> struct is_vector : std::false_type {};

template <typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc>> : std::true_type {};

template <typename T>
inline constexpr bool is_vector_v = is_vector<std::remove_cvref_t<T>>::value;

/** std::optional **/
template <typename T> struct is_optional : std::false_type {};

template <typename T> struct is_optional<std::optional<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_optional_v =
    is_optional<std::remove_cvref_t<T>>::value;

/** std::map, std::unordered_map **/
template <typename T> struct is_map : std::false_type {};

template <typename K, typename V, typename Compare, typename Alloc>
struct is_map<std::map<K, V, Compare, Alloc>> : std::true_type {};

template <typename K, typename V, typename Compare, typename Alloc>
struct is_map<std::unordered_map<K, V, Compare, Alloc>> : std::true_type {};

template <typename T>
inline constexpr bool is_map_v = is_map<std::remove_cvref_t<T>>::value;

/** std::variant **/
template <typename T> struct is_variant : std::false_type {};

template <typename... Ts>
struct is_variant<std::variant<Ts...>> : std::true_type {};

template <typename T>
inline constexpr bool is_variant_v = is_variant<std::remove_cvref_t<T>>::value;

} // namespace exclude

template <typename T>
inline constexpr bool is_reflectable_v =
    std::is_class_v<std::remove_cvref_t<T>> && !exclude::is_string_v<T> &&
    !exclude::is_vector_v<T> && !exclude::is_optional_v<T> &&
    !exclude::is_map_v<T> && !exclude::is_variant_v<T>;

/**
 * @brief value category for T
 */
template <typename T> consteval auto vc4() {
  using U = std::remove_cvref_t<T>;

  if constexpr (std::is_same_v<U, bool>) {
    return vc::Boolean{};
  } else if constexpr (std::is_integral_v<U> && std::is_signed_v<U>) {
    return vc::SignedInt{};
  } else if constexpr (std::is_integral_v<U> && std::is_unsigned_v<U>) {
    return vc::UnsignedInt{};
  } else if constexpr (std::is_floating_point_v<U>) {
    return vc::FloatingPoint{};
  } else if constexpr (std::is_enum_v<U>) {
    return vc::Enumeration{};
  }
  /** excluded **/
  else if constexpr (exclude::is_string_v<U>) {
    return vc::String{};
  } else if constexpr (exclude::is_vector_v<U>) {
    return vc::Array{};
  } else if constexpr (exclude::is_optional_v<U>) {
    return vc::Optional{};
  } else if constexpr (exclude::is_map_v<U>) {
    return vc::Dictionary{};
  } else if constexpr (exclude::is_variant_v<U>) {
    return vc::Variant{};
  } else if constexpr (is_reflectable_v<U>) {
    return vc::Object{};
  } else {
    return vc::Unsupported{};
  }
}

template <typename T> using category_of_t = decltype(vc4<T>());

template <std::meta::info Member> consteval auto member_category() {
  using MemberType = typename[:std::meta::type_of(Member):];
  return vc4<MemberType>();
}

template <std::meta::info Member>
using member_category_t = decltype(member_category<Member>());

template <vc::Tag Category> consteval std::string_view category_name() {
  return std::remove_cvref_t<Category>::name;
}

namespace ann {

/**
 * @brief check if ignored (with ignore annotation)
 *
 * @return ignored or not ignored
 */
template <std::meta::info Member> consteval bool is_ignored() {
  static constexpr auto annotations = internal::anns_array<Member>();

  template for (constexpr auto annotation : annotations) {
    using Ann = std::remove_cvref_t<typename[:std::meta::type_of(annotation):]>;
    if constexpr (std::is_same_v<Ann, ignore_t>) {
      return true;
    }
  }
  return false;
}

/**
 * @brief get name_t field of annotations
 *
 * @return value of name field
 */
template <std::meta::info Member> consteval std::string_view field_name() {
  static constexpr auto annotations = internal::anns_array<Member>();

  template for (constexpr auto annotation : annotations) {
    using Ann = std::remove_cvref_t<typename[:std::meta::type_of(annotation):]>;
    if constexpr (std::is_same_v<Ann, name_t>) {
      return std::meta::extract<Ann>(annotation).value;
    }
  }
  return std::meta::identifier_of(Member);
}

} // namespace ann

/**
 * @brief helper function to print schema of struct/class
 */
template <typename T> void print_schema(std::ostream &out) {
  static constexpr auto fields = internal::fields_array<T>();

  out << "schema(" << std::meta::display_string_of(^^T) << ")\n";

  template for (constexpr auto member : fields) {
    out << "  " << std::meta::identifier_of(member) << " -> key \""
        << ann::field_name<member>() << "\", category "
        << category_name<member_category_t<member>>();

    if constexpr (ann::is_ignored<member>()) {
      out << ", ignored";
    }
    out << '\n';
  }
}

namespace enumeration {

enum class StringCase : std::uint8_t {
  Default = 0,
  Lower = 1,
  Upper = 2,
};

/**
 * @brief enumerator entry
 */
template <typename E>
  requires(std::is_enum_v<std::remove_cvref_t<E>>)
struct EnumEntry {
  std::remove_cvref_t<E> value;
  std::string_view name;
};

template <std::meta::info E, StringCase Case>
inline constexpr auto transformed_name = []() {
  constexpr std::string_view raw = std::meta::identifier_of(E);
  std::array<char, raw.size() + 1> buf{};
  for (std::size_t j = 0; j < raw.size(); ++j) {
    char c = raw[j];
    if constexpr (Case == StringCase::Lower) {
      if (c >= 'A' && c <= 'Z') {
        c = c - 'A' + 'a';
      }
    } else if constexpr (Case == StringCase::Upper) {
      if (c >= 'a' && c <= 'z') {
        c = c - 'a' + 'A';
      }
    }
    buf[j] = c;
  }
  buf[raw.size()] = '\0';
  return buf;
}();

template <typename E, StringCase Case = StringCase::Default>
  requires(std::is_enum_v<std::remove_cvref_t<E>>)
consteval auto entries() {
  using U = std::remove_cvref_t<E>;

  static constexpr auto enums = internal::enums_array<U>();
  constexpr std::size_t size = std::meta::enumerators_of(^^U).size();

  std::array<EnumEntry<U>, size> arr{};

  std::size_t i = 0;
  template for (constexpr auto e : enums) {
    arr[i].value = [:e:];

    if constexpr (Case == StringCase::Default) {
      arr[i].name = std::meta::identifier_of(e);
    } else {
      constexpr std::string_view sv(transformed_name<e, Case>.data(),
                                    transformed_name<e, Case>.size() - 1);
      arr[i].name = sv;
    }
    ++i;
  }

  return arr;
}

template <StringCase Case = StringCase::Default, typename E>
  requires std::is_enum_v<std::remove_cvref_t<E>>
constexpr std::string_view to_string(E value) {
  using U = std::remove_cvref_t<E>;
  static constexpr auto arr = entries<U, Case>();

  for (const auto &entry : arr) {
    if (entry.value == value) {
      return entry.name;
    }
  }
  return "<unknown>";
}

template <typename E, StringCase Case = StringCase::Default>
  requires std::is_enum_v<std::remove_cvref_t<E>>
constexpr std::optional<std::remove_cvref_t<E>>
from_string(std::string_view name) {
  using U = std::remove_cvref_t<E>;
  static constexpr auto arr = entries<U, Case>();

  for (const auto &entry : arr) {
    if (entry.name == name) {
      return entry.value;
    }
  }
  return std::nullopt;
}
} // namespace enumeration

struct DefaultTag {};

/**
 * @brief type tag to wrap non-type template parameters (like enum)
 */
template <auto V, typename Tag = DefaultTag>
struct ValueT : std::integral_constant<decltype(V), V> {
  using tag_type = Tag;
};

namespace tpl {

/**
 * @brief get template params name
 *
 * @tparam T materialized template
 * @return template params name
 */
template <typename T> consteval auto template_arg_names_of() {
  constexpr auto args_info = internal::template_args_info_array<T>();
  constexpr std::size_t size = std::size(args_info.elems);

  std::array<std::string_view, size> names;

  if (size > 0) {
    for (std::size_t i = 0; i < size; ++i) {
      names[i] = std::meta::display_string_of(args_info.elems[i]);
    }
  }

  return names;
}

/**
 * @brief get enumeration name in typelist
 *
 * @tparam TypeList type list
 * @tparam TargetTag tag to find and stringfy
 * @param fallback fallback name
 * @return matched tag with enumeration name
 */
template <typename TypeList, typename TargetTag>
consteval std::string_view
get_tagged_enum_name(std::string_view fallback = "undefined") {
  return [fallback]<typename... Ts>(typelist<Ts...>) consteval {
    std::string_view name = fallback;
    ([&]<typename T>() -> bool {
      if constexpr (requires {
                      typename T::tag_type;
                      T::value;
                    }) {
        if constexpr (std::is_same_v<typename T::tag_type, TargetTag>) {
          if constexpr (std::is_enum_v<
                            std::remove_cvref_t<decltype(T::value)>>) {
            name = enumeration::to_string(T::value);
          }
          return true;
        }
      }
      return false;
    }.template operator()<Ts>() ||
                              ...);
    return name;
  }(TypeList{});
}

} // namespace tpl

namespace ns {

enum class SliceDir : std::uint8_t {
  Default, /** no slice direction, return original
             reflected namespace directly **/
  Inner,   /** from inside to outside **/
  Outer    /** from outsize to inside **/
};

/**
 * @brief slice the namespace string
 *
 * @param ns namespace string
 * @param Dir slice direction
 * @param layers slice layers
 */
consteval std::string_view
slice(std::string_view ns, SliceDir Dir = SliceDir::Default, int layers = 0) {
  if (Dir == SliceDir::Default || layers <= 0) {
    return ns;
  }

  int colons = 0;
  for (std::size_t i = 0; i + 1 < ns.size(); ++i) {
    if (ns[i] == ':' && ns[i + 1] == ':') {
      colons++;
    }
  }
  int total_layers = colons + 1;

  if (layers >= total_layers) {
    return ns;
  }

  if (Dir == SliceDir::Inner) {
    int skip = total_layers - layers;
    std::size_t pos = 0;
    for (int i = 0; i < skip; ++i) {
      pos = ns.find("::", pos) + 2;
    }
    return ns.substr(pos);
  } else if (Dir == SliceDir::Outer) {
    std::size_t pos = 0;
    for (int i = 0; i < layers; ++i) {
      pos = ns.find("::", pos);
      if (i < layers - 1) {
        pos += 2;
      }
    }
    return ns.substr(0, pos);
  }
}

/**
 * @brief get the context namespace string
 *
 *  \* This macro should be put in the expected namespace where you want
 */
#define AUTO_NS()                                                              \
  []() consteval {                                                             \
    struct _NsTag {};                                                          \
    constexpr std::string_view name = std::meta::display_string_of(^^_NsTag);  \
    constexpr std::size_t end_pos = name.rfind("::_NsTag");                    \
    if constexpr (end_pos != std::string_view::npos) {                         \
      constexpr std::string_view ns_1 = name.substr(0, end_pos);               \
      constexpr std::size_t space_pos = ns_1.rfind(' ');                       \
      constexpr std::string_view ns_2 = (space_pos != std::string_view::npos)  \
                                            ? ns_1.substr(space_pos + 1)       \
                                            : ns_1;                            \
      constexpr std::size_t lambda_pos = ns_2.find("::<lambda");               \
      if constexpr (lambda_pos != std::string_view::npos) {                    \
        return ns_2.substr(0, lambda_pos);                                     \
      }                                                                        \
      return ns_2;                                                             \
    }                                                                          \
    return name;                                                               \
  }()

} // namespace ns

} // namespace qool::meta::refl
