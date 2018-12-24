#ifndef DEVICE_TEMPLATES_HPP
#define DEVICE_TEMPLATES_HPP

template <typename T>
struct remove_reference
{
    typedef T type;
};

template <typename T>
struct remove_reference<T&>
{
    typedef T type;
};

template <typename T>
struct remove_reference<T&&>
{
    typedef T type;
};

struct true_type
{
};
struct false_type
{
};

template <typename T>
using void_t = void;

template <typename T>
T declval();

template <typename F, typename... Args>
using result_of_t = decltype(declval<F>()(declval<Args>()...));

template <typename T, typename F, typename = bool>
struct is_validator
{
    using type = false_type;
};

template <typename T, typename F>
struct is_validator<
    T, F, decltype(static_cast<bool>(declval<result_of_t<F, T&>>()))>
{
    using type = true_type;
};

template <typename T, typename F>
using is_validator_t = typename is_validator<T, F>::type;

#endif // DEVICE_TEMPLATES_HPP