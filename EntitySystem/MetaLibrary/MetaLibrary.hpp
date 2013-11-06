#ifndef META_LIBRARY
#define META_LIBRARY

/*!
 * \brief   Provides meta functionality in a clear and neat way.
 */

#include "MetaLibrary/VariadicUtilities.hpp"
#include "MetaLibrary/VariadicIndexing.hpp"
#include "MetaLibrary/VariadicTypedef.hpp"
#include "MetaLibrary/MultiContainer.hpp"

#include "MetaLibrary/Conditional.hpp"
#include "MetaLibrary/TupleUtilities.hpp"


////////////////////

// Meta programming

// type 1) Pure meta. Handles only types. Meta functions that declare a type from other types.

// type 2) Meta decisions on which non-meta functions to be called.

// type 3) Templated functions / types. e.g. std::vector

////////////////////

/* meta:
 *  // All statements result in templates that are the given type asked for.
 *  IF<Condition, Then, Else>: <meta::True|meta::False, Template, Template> -> Type
 *  ForEach<Tuple, MetaFunction, Parameters>: <Tuple, Template, Tuple> -> Type
 *
 */

/* metaValue:
 *  // All statements result in templates with a ::execute(...) static method.
 *  IF<Condition, Then, Else>: <meta::True|meta::False, Template, Template> -> Template (with static method ::execute())
 *  ForEach<Tuple, MetaFunction, Parameters, ArgumentTypes>: <Tuple, Template, Tuple, Tuple> -> Template (with static method ::execute(ArgumentTypes))
 *
 */


////////////////////


/*!
 *  \brief  Check if a std::tuple<> have zero parameters.
 */
template<int size>
struct isTupleEmpty_ {
    static const bool value = false;
};
template<>
struct isTupleEmpty_<0> {
    static const bool value = true;
};
template<typename tuple>
struct isTupleEmpty {
    static const bool value = isTupleEmpty_<std::tuple_size<tuple>::value>::value;
};

/*!
 *  \brief      If control structures.
 *  \details    * IF<condition, Then>
 *              * IF<condition, Then, Else>
 */

template <bool condition, typename Then, typename Else>
struct IF {
    typedef Then RETURN;
};
template <typename Then, typename Else>
struct IF<false, Then, Else> {
    typedef Else RETURN;
};
template <bool condition, typename Then>
struct IF<condition, Then, void> {
    typedef Then RETURN;
};
template <typename Then>
struct IF<false, Then, void>{
    typedef void RETURN;
};

struct NOPE {};


///////////////////////



////

/*
 * MetaFunction takes another template and arguments as a tuple that are applied on the template.
 *
 *      MetaFunction<Template, std::tuple<Parameters...> >::type  :=  Template<Parameters...>
 */

template<int N, int N_MAX, template<typename... A> class MFunc, typename ArgTuple, int... Indices>
struct MetaFunction_ : public MetaFunction_<N+1, N_MAX, MFunc, ArgTuple, Indices..., N>{
    // typedef typename MetaFunction_<N+1, N_MAX, MFunc, ArgTuple, Indices..., N>::type type;
};
template<int N_MAX, template<typename... A> class MFunc, typename ArgTuple, int... Indices>
struct MetaFunction_<N_MAX, N_MAX, MFunc, ArgTuple, Indices...> : public MFunc<typename std::tuple_element<Indices, ArgTuple>::type...> {
    // typedef MFunc<typename std::tuple_element<Indices, ArgTuple>::type...> type;
};

template<template<typename... A> class MFunc, typename ArgTuple>
struct MetaFunction : public MetaFunction_<0, std::tuple_size<ArgTuple>::value, MFunc, ArgTuple> {
    //typedef typename MetaFunction_<0, std::tuple_size<ArgTuple>::value, MFunc, ArgTuple>::type type;
};

///////////

// Merge tuples
template<typename... Types>
struct merge_tuples{
  typedef std::tuple<Types...> type;
};

template<>
struct merge_tuples<> {
  typedef std::tuple< > type;
};

template<typename Type>
struct merge_tuples<Type> {
  typedef std::tuple<Type> type;
};

template<typename...Types>
struct merge_tuples<std::tuple<Types...>> {
  typedef std::tuple<Types...> type;
};

template<typename...Types1,typename...Types2>
struct merge_tuples<std::tuple<Types1...>,std::tuple<Types2...>> {
  typedef std::tuple<Types1...,Types2...> type;
};

template<typename Type1,typename Type2>
struct merge_tuples<Type1,Type2> {
  typedef std::tuple<Type1,Type2> type;
};

template<typename Type,typename... Types>
struct merge_tuples<Type,std::tuple<Types...>> {
  typedef std::tuple<Type,Types...> type;

};

template<typename... Types,typename Type>
struct merge_tuples<std::tuple<Types...>,Type> {
  typedef std::tuple<Types...,Type> type;
};

template<typename...Types,typename Type,typename... Rest>
struct merge_tuples<std::tuple<Types...>,Type,Rest...> {
  typedef typename merge_tuples<Rest...>::type temp;
  typedef typename merge_tuples<std::tuple<Types...,Type>,temp>::type type;
};

template<typename Type,typename... Types,typename... Rest>
struct merge_tuples<Type,std::tuple<Types...>,Rest...> {
  typedef typename merge_tuples<Rest...>::type temp;
  typedef typename merge_tuples<std::tuple<Type,Types...>,temp>::type type;
};


///////////////////////

#include <string>
#include <vector>

template<typename Component>
struct CoutComponentName {
    static void execute() {
        static Component c;
        std::cout << c.getName() << "\n";
    }
};
template<typename Component>
struct PushComponentName {
    static void execute(std::vector<std::string> & strings) {
        static Component c;
        strings.push_back(c.getName());
    }
};

// For each
//---------------------

namespace semi_meta
{
template<int N, int N_MAX, typename Tuple, template<typename... A> class MFunctor, typename... Args>
struct ForEach_ {
    static void next(Args &... arg) {
        MFunctor<typename std::tuple_element<N,Tuple>::type>::execute(arg...);
        ForEach_<N+1, N_MAX, Tuple, MFunctor, Args...>::next(arg...);
    }
};
template<int N_MAX, typename Tuple, template<typename... A> class MFunctor, typename... Args>
struct ForEach_<N_MAX, N_MAX, Tuple, MFunctor, Args...> {
    static void next(Args &... arg) {}
};

template<typename Tuple, template<typename... A> class MFunctor, typename... Args>
struct ForEach {
    static void execute(Args &... arg) {
        ForEach_<0, std::tuple_size<Tuple>::value, Tuple, MFunctor, Args...>::next(arg...);
    }
};
}

////
/*
template<int N, int N_MAX, typename Tuple, template<typename... A> class Template>
struct ForEach_ {

};
template<int N_MAX, typename Tuple, template<typename... A> class Template>
struct ForEach_<N_MAX, N_MAX, Tuple, Template> {

};
template<typename Tuple, template<typename... A> class Template>
struct ForEach : public ForEach_<0, std::tuple_size<Tuple>::value, Tuple, Template> {};
*/
#endif
