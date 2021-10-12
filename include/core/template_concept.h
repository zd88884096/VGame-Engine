#pragma once
#include "../common/setup.h"
#include <type_traits>

namespace vmath_concept
{
    struct EmptyType
    {

    };
    template<typename T, T val1, T val2>
    concept isGreater = val1 > val2;

    template<typename T, T val1, T val2>
    concept isGreaterEqual = val1 >= val2;

    template<typename T, T val1, T val2>
    concept isLess = val1 < val2;

    template<typename T, T val1, T val2>
    concept isLessEqual = val1 <= val2;

    template<typename T, T val1, T val2>
    concept isEqual = val1 == val2;

    // Concept For Floating Point Types
    template<typename T>
    concept Fp_Type = std::is_floating_point_v<T> || std::is_same_v<T, EmptyType>;

    // Concept For Integer Types
    template<typename T>
    concept Int_Type = std::is_integral_v<T> || std::is_same_v<T, EmptyType>;


    struct valid_type
    {

    };

    template<typename T> 
    constexpr bool is_valid_type_v = std::is_same_v<T, valid_type>;
    template<typename T>
    concept Valid_Type = std::is_same_v<T, valid_type>;

    template<typename T>
    concept Vector_Type = Valid_Type<typename T::is_vector_type>;

    template<typename T, typename U>
    constexpr bool addable = requires(T x, U y)
    {
        x + y;
    };

}
