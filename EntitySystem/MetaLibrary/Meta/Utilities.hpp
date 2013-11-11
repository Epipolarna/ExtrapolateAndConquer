/*
 * Author: Mattias Tiger 2013
 */
#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <tuple>

#include "FlowControl.hpp"

/*
 *  */

namespace meta
{

// IMPROVE THIS! (using indices? using loops?: Using tuple<types...> specialization)
/*!
 * \brief       Apply takes a template and parameters as a tuple that are then applied on the template.
 *
 *  \details    Apply<Template, std::tuple<Parameters...> >::type  :=  Template<Parameters...>
 */
/*
template<int N, int N_MAX, template<typename... A> class Template, typename ArgTuple, int... Indices>
struct MetaCall_ : public MetaCall_<N+1, N_MAX, Template, ArgTuple, Indices..., N>{
    // typedef typename MetaCall_<N+1, N_MAX, Template, ArgTuple, Indices..., N>::type type;
};
template<int N_MAX, template<typename... A> class Template, typename ArgTuple, int... Indices>
struct MetaCall_<N_MAX, N_MAX, Template, ArgTuple, Indices...> : public Template<typename std::tuple_element<Indices, ArgTuple>::type...> {
    // typedef Template<typename std::tuple_element<Indices, ArgTuple>::type...> type;
};

template<template<typename... A> class Template, typename ArgTuple>
struct MetaCall : public MetaCall_<0, std::tuple_size<ArgTuple>::value, Template, ArgTuple> {
    //typedef typename MetaCall_<0, std::tuple_size<ArgTuple>::value, Template, ArgTuple>::type type;
};
*/

template<template<typename... T> class Template, typename... Types>
struct Apply_;

template<template<typename... T> class Template, typename... Types>
struct Apply_<Template, std::tuple<Types...>> {
    typedef typename Template<Types...>::type type;
};

template<template<typename... T> class Template, typename ArgTuple>
struct Apply {
    typedef typename Apply_<Template, ArgTuple>::type type;
};

}


#endif
