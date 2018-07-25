#ifndef EMBMESSENGER_TEMPLATES_HPP
#define EMBMESSENGER_TEMPLATES_HPP

#include <cstdint>
#include <cstddef>

namespace emb
{
    template <size_t S> struct var_uint;
    template <> struct var_uint<1> { using type = uint8_t; };
    template <> struct var_uint<2> { using type = uint16_t; };
    template <> struct var_uint<4> { using type = uint32_t; };
    template <> struct var_uint<8> { using type = uint64_t; };

    /**
    * @brief Gives a `uint` the same size of `T`
    */
    template <typename T> using var_uint_t = typename var_uint<sizeof(T)>::type;
}

#endif