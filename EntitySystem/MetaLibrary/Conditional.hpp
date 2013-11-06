#ifndef CONDITIONAL_HPP
#define CONDITIONAL_HPP

/*
 *  IF<Condition, Then, Else> -> Then or Else
 *
 *  True    (type)
 *  False   (type)
 *  NOT<Condition> -> Negation of Condition
 *
 *  NOPE (no operation, has an empty ::execute())
 *
 */

namespace meta
{
    struct True {
        typedef True metaValue;
        static void value() { std::cout << "True\n"; }
    };
    struct False {
        typedef False metaValue;
        static void value() { std::cout << "False\n"; }
    };


    struct Nope {
        static void execute() { std::cout << "NOPE"; }
    };

    /*!
     * \brief   IF<Condition,Then,Else> -> Then or Else depending on Condition
     */
    template<typename Condition, typename Then, typename Else>
    struct IF_ {};

    template<typename Then, typename Else>
    struct IF_<True, Then, Else> : public Then {};

    template<typename Then, typename Else>
    struct IF_<False, Then, Else> : public Else {};

    template<typename Condition, typename Then, typename Else = Nope>
    struct IF : public IF_<typename Condition::metaValue, Then, Else> {};


    /*!
     * \brief   NOT<Condition> -> Negation of Condition
     */
    template<typename Condition>
    struct NOT {
        typedef IF<Condition, False, True> metaValue;
    };

}


#endif
