#ifndef VARIADIC_TYPE_DEF
#define VARIADIC_TYPE_DEF

/*!
 *  \brief
 *  \details    NOT WORKING... WARNING!
 */

 /*
	 What we want: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3416.html
	 Example below is from: http://stackoverflow.com/questions/4691657/is-it-possible-to-store-a-template-parameter-pack-without-expanding-it
 */
 
 /*
#include <tuple>

template <typename... Args>
struct variadicTypedef
{
    // this single type represents a collection of types,
    // as the template arguments it took to define it
};

template <typename... Args>
struct convertInTuple
{
    // base case, nothing special,
    // just use the arguments directly
    // however they need to be used
    typedef std::tuple<Args...> type;
};

template <typename... Args>
struct convertInTuple<variadicTypedef<Args...>>
{
    // expand the variadic_typedef back into
    // its arguments, via specialization
    // (doesn't rely on functionality to be provided
    // by the variadic_typedef struct itself, generic)
    typedef typename convertInTuple<Args...>::type type;
};
*/


#endif
