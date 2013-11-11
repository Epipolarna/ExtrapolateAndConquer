/*
 * Author: Mattias Tiger 2013
 */
#ifndef CONDITION_HPP
#define CONDITION_HPP

#include "FlowControl.hpp"

/*
 *  True    (type)
 *  False   (type)
 *  NOT<Condition> -> Negation of Condition
 *
 */

namespace meta
{
    struct True {
        typedef True type;
        static void value() { std::cout << "True\n"; }
    };
    struct False {
        typedef False type;
        static void value() { std::cout << "False\n"; }
    };


    /*!
     * \brief   Condition constructor (bool to Condition)
     */
    template<bool Value> struct Condition {};
    template<>           struct Condition<true>  : public True {};
    template<>           struct Condition<false> : public False {};


    /*!
     * \brief   NOT<Condition> -> Negation of Condition
     */
    template<typename Condition>
    struct NOT {
        typedef IF<Condition, False, True> type;
    };

    /*!
     * \brief   OR<Condition1, Condition2> ->
     */
    template<typename Condition1, typename Condition2>
    struct OR : IF<Condition1, True, Condition2> {};

    /*!
     * \brief   AND<Condition1, Condition2> ->
     */
    template<typename Condition1, typename Condition2>
    struct AND : IF<Condition1, IF<Condition2, True, False>, False> {};

    /*!
     * \brief   XOR<Condition1, Condition2> ->
     */
    template<typename Condition1, typename Condition2>
    struct XOR : IF<Condition1, IF<Condition2, True, False>, IF<Condition2, False, True> > {};


}


#endif
