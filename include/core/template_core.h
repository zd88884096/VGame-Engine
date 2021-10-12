#pragma once
#include <type_traits>
#include <string>
#include <cstring>
#include "template_concept.h"


namespace vmath_template_core
{

/*
 *  Template utility
 */
    template <typename... _Tps>
    struct typelist {};

    using vmath_concept::EmptyType;
/*
 *  Template utility for getting arithmetic type
 *  after binary operator
 */
    template<typename _Tp1, typename _Tp2>
    requires std::is_arithmetic_v<_Tp1> &&
             std::is_arithmetic_v<_Tp2>
    struct UpcastTypeHelper
    {
        typedef decltype(static_cast<_Tp1>(0) + static_cast<_Tp2>(0)) upcast_type;
    };

    template<typename _Tp1, typename _Tp2>
    using upcastType = typename UpcastTypeHelper<_Tp1, _Tp2>::upcast_type;

/*
 *  Template Utility
 *  Usage: Structs should derive from this base class
 */
    struct ReturnTypeHelperBase 
    {
        typedef EmptyType return_type;
    };

    template<typename _Tp>
    requires std::is_base_of_v<ReturnTypeHelperBase, _Tp>
    using returnType = typename _Tp::return_type;

/*
 *  size detailementation
 */
    namespace detail{
        template <typename TypeList>
        struct typelistSizeHelper;

        template <template <typename...> class TypeList, typename... _Tps>
        struct typelistSizeHelper<TypeList<_Tps...>>{
            static constexpr auto value = std::integral_constant<std::size_t, sizeof...(_Tps)>::value;
        };
    }

    template <typename TypeList>
    constexpr auto typelist_size = detail::typelistSizeHelper<TypeList>::value;


/*
 * front detailementation
 */
    namespace detail{
	template <typename TypeList>
	struct front_type;

	template <template <typename...> class TypeList>
	struct front_type<TypeList<>>{
	    using type = EmptyType;
	};

	template <template <typename...> class TypeList, typename _Tp1, typename... _Tps>
	struct front_type<TypeList<_Tp1, _Tps...>>{
    	    using type = _Tp1;
	};
    }

    template <typename TypeList>
    using front = typename detail::front_type<TypeList>::type;

/*
 * back detailementation
 */
    namespace detail{
	template <typename TypeList>
	struct back_type;

	template <template <typename...> class TypeList>
	struct back_type<TypeList<>>{
	    using type = EmptyType;
	};

	template <template <typename...> class TypeList, typename _Tp1>
	struct back_type<TypeList<_Tp1>>{
	    using type = _Tp1;
	};

	template <template <typename...> class TypeList, typename _Tp1, typename... _Tps>
	struct back_type<TypeList<_Tp1, _Tps...>>{
	    using type = typename back_type<TypeList<_Tps...>>::type;
	};
    }

    template <typename TypeList>
    using back = typename detail::back_type<TypeList>::type;

/*
 * push_front detailementation
 */
    namespace detail{
	template <typename TypeList, typename _Tp1>
	struct push_front_type;

	template <template <typename...> class TypeList, typename _Tp1, typename... _Tps>
	struct push_front_type<TypeList<_Tps...>, _Tp1>{
	    using type = TypeList<_Tp1, _Tps...>;
	};
    }

    template <typename TypeList, typename _Tp1>
    using push_front = typename detail::push_front_type<TypeList, _Tp1>::type;

/*
 * push_back detailementation
 */
    namespace detail{
        template <typename TypeList, typename _Tp1>
        struct push_back_type;

        template <template <typename...> class TypeList, typename _Tp1, typename... _Tps>
        struct push_back_type<TypeList<_Tps...>, _Tp1>{
            using type = TypeList<_Tps..., _Tp1>;
        };
    }

    template <typename TypeList, typename _Tp1>
    using push_back = typename detail::push_back_type<TypeList, _Tp1>::type;


/*
 * pop_front detailementation
 */
    namespace detail{
	template <typename TypeList>
	struct pop_front_type;

	template <template <typename...> class TypeList>
	struct pop_front_type<TypeList<>>{
	    using type = EmptyType;
	};

	template <template <typename...> class TypeList, typename _Tp1, typename... _Tps>
	struct pop_front_type<TypeList<_Tp1, _Tps...>>{
	    using type = TypeList<_Tps...>;
	};
    }
    
    template <typename TypeList>
    using pop_front = typename detail::pop_front_type<TypeList>::type;

/*
 * pop_back detailementation
 */
    namespace detail{
        template <typename TypeList>
        struct pop_back_type;

        template <template <typename...> class TypeList>
        struct pop_back_type<TypeList<>>{
            using type = EmptyType;
        };

        template <template <typename...> class TypeList, typename _Tp1>
        struct pop_back_type<TypeList<_Tp1>>{
            using type = TypeList<>;
        };
	
        template <template <typename...> class TypeList, typename _Tp1, typename _Tp2, typename... _Tps>
        struct pop_back_type<TypeList<_Tp1, _Tp2, _Tps...>>{
            using recursive = typename pop_back_type<TypeList<_Tp2, _Tps...>>::type;
	        using type = typename push_front_type<recursive, _Tp1>::type;
        };
    }

    template <typename TypeList>
    using pop_back = typename detail::pop_back_type<TypeList>::type;
    

/*
 * at detailementation (traverse by _index)
 */
    namespace detail{
	template<typename TypeList, size_t _index>
	struct typelistAtHelper;
	
	template<template <typename...> class TypeList, size_t _index>
	struct typelistAtHelper<TypeList<>, _index>{
	    using type = EmptyType;
	};

	template<template <typename...> class TypeList, typename _Tp1, typename... _Tps>
	struct typelistAtHelper<TypeList<_Tp1, _Tps...>, static_cast<int>(0)>{
	    using type = _Tp1;
	};
	
	template<template <typename...> class TypeList, size_t _index, typename _Tp1, typename... _Tps>
        struct typelistAtHelper<TypeList<_Tp1, _Tps...>, _index>{
            using type = typename typelistAtHelper<TypeList<_Tps...>, _index-1>::type;
        };
    }

    template <typename TypeList, size_t _index>
    using typelist_at_t = typename detail::typelistAtHelper<TypeList, _index>::type;

/*
 * resize detailementation (shrink only)
 */
    namespace detail{
	template <typename TypeList, int sz>
	struct resize_type;

	template <template <typename...> class TypeList, int sz>
	struct resize_type<TypeList<>, sz>{
	    using type = TypeList<>;
	};

	template <template <typename...> class TypeList, typename _Tp1, typename... _Tps>
	struct resize_type<TypeList<_Tp1, _Tps...>, static_cast<int>(0)>{
	    using type = TypeList<>;
	};

	template <template <typename...> class TypeList, int sz, typename _Tp1, typename... _Tps>
	struct resize_type<TypeList<_Tp1, _Tps...>, sz>{
	    using recursive = typename resize_type<TypeList<_Tps...>, sz-1>::type;
	    using type = typename push_front_type<recursive, _Tp1>::type;
	};
    }

    template <typename TypeList, int sz>
    using resize = typename detail::resize_type<TypeList, sz>::type;



/*
 * empty detailementation
 */
    namespace detail{
	template <typename TypeList>
	struct empty_type;

	template <template <typename...> class TypeList>
	struct empty_type<TypeList<>>{
	    static const bool m_Value = true;
	};

	template <template <typename...> class TypeList, typename _Tp1, typename... _Tps>
	struct empty_type<TypeList<_Tp1, _Tps...>>{
	    static const bool m_Value = false;
	};
    }
    
    template <typename TypeList>
    static const bool empty = detail::empty_type<TypeList>::m_Value;



/*
 * assign detailementation
 */
    namespace detail{
        template <typename TypeList, size_t _index, typename _Tp1>
        struct assign_type;

	template <template <typename...> class TypeList, size_t _index, typename _Tp1>
	struct assign_type<TypeList<>, _index, _Tp1>{
	    using type = TypeList<>;
	};

	template <template <typename...> class TypeList, typename _Tp1, typename _Tp2, typename... _Tps>
	struct assign_type<TypeList<_Tp2, _Tps...>, static_cast<int>(0), _Tp1>{
	    using type = TypeList<_Tp1, _Tps...>;
	};
        template <template <typename...> class TypeList, size_t _index, typename _Tp1, typename _Tp2, typename... _Tps>
        struct assign_type<TypeList<_Tp2, _Tps...>, _index, _Tp1>{
		using recursive = typename assign_type<TypeList<_Tps...>, _index-1, _Tp1>::type;
	    using type = typename push_front_type<recursive, _Tp2>::type;
	};
    }

    template <typename TypeList, size_t _index, typename _Tp1>
    using assign = typename detail::assign_type<TypeList, _index, _Tp1>::type;



/*
 * insert detailementation
 */
    namespace detail{
        template <typename TypeList, size_t _index, typename _Tp1>
        struct insert_type;

        template <template <typename...> class TypeList, typename _Tp1>
        struct insert_type<TypeList<>, static_cast<int>(0), _Tp1>{
            using type = TypeList<_Tp1>;
        };

        template <template <typename...> class TypeList, size_t _index, typename _Tp1>
        struct insert_type<TypeList<>, _index, _Tp1>{
            using type = TypeList<>;
        };

        template <template <typename...> class TypeList, typename _Tp1, typename _Tp2, typename... _Tps>
        struct insert_type<TypeList<_Tp2, _Tps...>, static_cast<int>(0), _Tp1>{
            using type = TypeList<_Tp1, _Tp2, _Tps...>;
        };
        template <template <typename...> class TypeList, size_t _index, typename _Tp1, typename _Tp2, typename... _Tps>
        struct insert_type<TypeList<_Tp2, _Tps...>, _index, _Tp1>{
            using recursive = typename insert_type<TypeList<_Tps...>, _index-1, _Tp1>::type;
            using type = typename push_front_type<recursive, _Tp2>::type;
        };
    }

    template <typename TypeList, size_t _index, typename _Tp1>
    using insert = typename detail::insert_type<TypeList, _index, _Tp1>::type;


		  
/*
 * erase detailementation
 */
    namespace detail{
        template <typename TypeList, size_t _index>
        struct erase_type;

        template <template <typename...> class TypeList, size_t _index>
        struct erase_type<TypeList<>, _index>{
            using type = TypeList<>;
        };

        template <template <typename...> class TypeList, typename _Tp1, typename... _Tps>
        struct erase_type<TypeList<_Tp1, _Tps...>, static_cast<int>(0)>{
            using type = TypeList<_Tps...>;
        };
        template <template <typename...> class TypeList, size_t _index, typename _Tp1, typename... _Tps>
        struct erase_type<TypeList<_Tp1, _Tps...>, _index>{
            using recursive = typename erase_type<TypeList<_Tps...>, _index-1>::type;
            using type = typename push_front_type<recursive, _Tp1>::type;
        };
    }

    template <typename TypeList, size_t _index>
    using erase = typename detail::erase_type<TypeList, _index>::type;
    
    /*
    
        Implementation of Loop Unrolling
    
    */
    namespace detail {
		template<typename Callable_Tp, size_t ...N>
		VMATH_CONSTEXPR void forwardUnroll_Impl(Callable_Tp& _callable, std::integer_sequence<size_t, N...> _seq)
        {
            (_callable(std::integral_constant<size_t, N>{}), ...);
        }

	 }

	VMATH_CONSTEXPR size_t unrollStride_dynamic(size_t _length)
	{
		if (static_cast<size_t>(4) <= _length && 
					  _length <= static_cast<size_t>(8))
		{
			return _length;
		}
		else return static_cast<size_t>(8);
	}

	template<size_t Length>
    constexpr size_t unrollStride_fix = unrollStride_dynamic(Length);



    template<size_t Length, typename Callable_Tp>
    void fowardUnroll(Callable_Tp&& _callable)
    {
        detail::forwardUnroll_Impl(_callable, 
						std::make_integer_sequence<size_t, Length>());
    }

	template<typename T>
	struct ref_selector
	{
		typedef
			typename std::conditional< std::is_rvalue_reference_v<T>,
			std::decay_t<T>, const std::decay_t<T>&>::type type;
	};



	template<typename T>
	using ref_selector_t = typename ref_selector<T>::type;

};
