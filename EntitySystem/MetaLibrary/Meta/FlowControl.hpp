/*
 * Author: Mattias Tiger 2013
 */
#ifndef FLOW_CONTROL
#define FLOW_CONTROL

#include <tuple>
#include <iostream>

/*
 *  NOP::execute(...)
 *
 *  IF<Condition, Then, Else> -> Then or Else
 *
 *
 *
 */

namespace meta
{
    struct True;
    struct False;

    template<typename T>
    struct Type {
        typedef T type;
    };

    template <int N>
    struct Int {
        static const int value = N;
    };



    /*!
     * \brief   No Operator
     */
    struct NOP {
        static void execute() { std::cout << "NOPE"; }
        typedef NOP type;
    };

    /*!
     * \brief   IF<Condition,Then,Else> -> Then or Else depending on Condition
     */
    template<typename Condition, typename Then, typename Else>
    struct IF_ {};

/*
    template<typename Then, typename Else>
    struct IF_<True, Then, Else> {
        typedef typename Then::type type;
    };

    template<typename Then, typename Else>
    struct IF_<False, Then, Else> {
        typedef typename Else::type type;
    };

    template<typename Condition, typename Then, typename Else = Type<NOP> >
    struct IF : public IF_<typename Condition::type, Then, Else> {};
    */

    template<typename Then, typename Else>
    struct IF_<True, Then, Else> : public Then {};

    template<typename Then, typename Else>
    struct IF_<False, Then, Else> : public Else {};

    template<typename Condition, typename Then, typename Else = Type<NOP> >
    struct IF : public IF_<typename Condition::type, Then, Else> {};


    /*!
     * \brief   LOOP<Tuple, >
     */


    /*!
     * \brief   FOR_EACH<Tuple, Template, ParameterTuple, ArgumentTypeTuple>::execute(Arguments)
     */

    template<typename Tuple, template<typename... T> class Template, typename ParameterTuple, typename ArgumentTypeTuple>
    struct FOR_EACH_;

    template<typename First, typename...Rest, template<typename... T> class Template, typename... Parameters, typename... ArgumentTypes>
    struct FOR_EACH_<std::tuple<First, Rest...>,Template,std::tuple<Parameters...>,std::tuple<ArgumentTypes...>> {
        static void execute(ArgumentTypes... Args) {
            Template<First, Parameters...>::execute(Args...);
            FOR_EACH_<std::tuple<Rest...>, Template, std::tuple<Parameters...>, std::tuple<ArgumentTypes...>>::execute(Args...);
        }
    };
    template<template<typename... T> class Template, typename... Parameters, typename... ArgumentTypes>
    struct FOR_EACH_<std::tuple<>,Template,std::tuple<Parameters...>,std::tuple<ArgumentTypes...>> {
        static void execute(ArgumentTypes... Args) {}
    };

    template<typename Tuple, template<typename... T> class Template, typename ParameterTuple, typename ArgumentTypeTuple>
    struct FOR_EACH : public FOR_EACH_<Tuple, Template, ParameterTuple, ArgumentTypeTuple> {};

}


#endif
