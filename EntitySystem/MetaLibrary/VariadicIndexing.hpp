#ifndef VARIADIC_INDEXING
#define VARIADIC_INDEXING

#include <tuple>

/*!
 *  \brief   Get the index to a specific type in a variadic list (of the first found)
 *  \details Usage:
 *              get_index<float, int, float, double>::INDEX
 *
 *              Wanted:   ^^^^^
 *              Variadic:        ^^^^^^^^^^^^^^^^^^
 *
 *              Result: 1 (the float type is the second in the variadic list)
 */


/*!
 *  \brief  2) Recursion, traverse Types
 */
template<typename Wanted, int N, int N_MAX, typename First, typename... Types>
struct get_index_helper {
    enum {INDEX = get_index_helper<Wanted, N+1, N_MAX, Types...>::INDEX};
};

/*!
 *  \brief  3) Break when Wanted found in Types and "return" N.
 */
template<typename Wanted, int N, int N_MAX, typename... Types>
struct get_index_helper<Wanted, N, N_MAX, Wanted, Types...> {
    enum {INDEX = N};
};

// 4) not found
template<typename Wanted, int N_MAX, typename First, typename... Types>
struct get_index_helper<First, N_MAX, N_MAX, Wanted, Types...> {
    enum {INDEX = -1};
};

/*! \brief  1) Interface.
 *  \details       Wanted = Type which index we want.
 *                 Types = all types in order.
 */
template<typename Wanted, typename... Types>
struct get_index {
    enum {INDEX = get_index_helper<Wanted, 0, sizeof...(Types), Types...>::INDEX};
};

#endif
