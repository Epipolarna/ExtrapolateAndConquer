/*
 * Author: Mattias Tiger 2013
 */
#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include "FlowControl.hpp"

/*
 *  Sequence
 *
 *      Using as function argument:
 *
 *          template<size_t N>
 *          void printIndexRange_() {
 *              std::cout << N << "\n";
 *          }
 *
 *          template<size_t N1, size_t N2, size_t... N_rest>
 *          void printIndexRange_() {
 *              std::cout << N1 << " - ";
 *              printIndexRange_<N2, N_rest...>();
 *          }
 *
 *          template<size_t... sequence>
 *          void printIndexRange(meta::Sequence<sequence...>) {
 *                printIndexRange_<sequence...>();
 *          }
 *
 *          //Example usage
 *          printIndexRange(Range<0,10>::type{});   // Observe the {} brackets at the end!
 *
 *
 *      Using as template parameter:
 *
 *          template<typename S1, typename S2>
 *          struct AnExample;
 *
 *          template<size_t... sequence1, size_t... sequence2>
 *          struct AnExample<Sequence<sequence1>, Sequence<sequence2>>
 *          {
 *              // Use 'sequence1' and 'sequence2'
 *          };
 *
 *          //Example usage
 *          AnExample<Range<0,5>::type, Range<6,10>::type>
 *
 *
 *
 *  More efficient, but less clear, algorithm to generate indices: http://stackoverflow.com/questions/18942322/effective-way-to-select-last-parameter-of-variadic-template
 */

namespace meta
{
    /*!
     *  \brief  A sequence is collection of integer numbers.
     */
    template<size_t... sequence>
    struct Sequence {};


    /*!
     *  \brief  A Range is a Sequence that spans the integers between [MIN MAX].
     */
    template<size_t MIN, size_t N, size_t... sequence>
    struct Range_ : public Range_<MIN, N-1, N-1, sequence...>
    {
        typedef typename Range_<MIN, N-1, N-1, sequence...>::type type;
    };

    template<size_t MIN, size_t... sequence>
    struct Range_<MIN, MIN, sequence...> : public Sequence<sequence...>
    {
        typedef Sequence<sequence...> type;
    };

    template<size_t MIN, size_t MAX>
    struct Range : public Range_<MIN, MAX, MAX> {};


    /*!
     *  \brief  IndexRange is a Range that spans the integers between [0 LENGTH]
     */
    template<size_t LENGTH>
    struct IndexRange : public Range<0, LENGTH> {};


}


#endif
