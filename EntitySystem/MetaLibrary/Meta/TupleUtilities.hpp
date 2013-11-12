/*
 * Author: Mattias Tiger 2013
 */
#ifndef TUPLE_UTILITIES
#define TUPLE_UTILITIES

#include <ostream>
#include <tuple>
#include "Sequence.hpp"
#include "FlowControl.hpp"
#include "Type.hpp"

/*
 * Get
 *  Tuple_getType<Tuple,Index>::type        := type att Index in Tuple
 *
 * Add
 *  Tuple_add<Tuple,Index,Type>::type       := A copy of Tuple in which Type is palced at Index and the previous types at Index and after have their indexes incremented by one.
 *  Tuple_addBack<Tuple,Type>::type         := A copy of Tuple in which Type is added at the back
 *
 * Remove
 *  Tuple_removeType<Tuple,Type>::type      := A copy of Tuple in which the first occurance of type Type has been removed
 *  Tuple_removeAllType<Tuple,Type>::type   := A copy of Tuple in which all occurances of type Type has been removed
 *  Tuple_remove<Tuple,Index>::type         := A copy of Tuple in which the type at Index has been removed
 *
 * Boolean
 *  Tuple_containType<Tuple,Type>::value    := true|false, if Tuple contain type Type
 *  Tuple_isEmpty<Tuple>::value             := true|false, if Tuple is empty
 *
 * Informatic
 *  Tuple_length<Tuple>::value                  := length of tuple T
 *  Tuple_findType<Tuple,Type>::value           := int, index of type Type in tuple T, -1 if it doesn't exist
 *  Tuple_occuranceAmount<Tuple, Type>::value   := int, number of occurances of Type in Tuple, -1 if it doesn't exist
 *  Tuple_occurances<Tuple, Type>::type         := Indices of the occurances of Type'a in Tuple
 *
 * Manage tuples
 *  Tuple_unique<Tuple>::type               := A copy of Tuple in which no type occures more than once (the first occurance is saved)
 *  Tuple_merge<Tuple1,Tuple2,...>::type    := A tuple consisting of the types of first Tuple1, then Tuple2, then ....
 *  Tuple_subset<Tuple, Indices>::type      := A tuple consisting of the types indexed in the Indices list (all within range)
 *
 * Set operations
 *  Tuple_union<Tuple1,Tuple2>::type            := A tuple consisting of the union of the types in T1 and T2
 *  Tuple_intersect<Tuple1,Tuple2>::type        := A tuple consisting of the intersection of types in T1 and T2
 *  Tuple_exclude<Tuple1,Tuple2>::type          := A tuple consisting of the types in T1 excluding any types in T2
 *  Tuple_cartesianProduct<Tuple1,Tuple2>::type := A tuple of tuples (pairs) consisting of all pairwise combinations of the types in T1 and T2
 *
 */


namespace meta
{

////////
///


/*!
 *  \brief  PrintTupleTypes<Tuple>::stream is an ostream containing the names of the types in Tuple and can for example be sent to cout.
 */
template<typename First, typename... Rest>
struct PrintTupleTypes_ {
    inline static std::ostream &stream(std::ostream &os) {
        os << TypeName<First>::value;
        os << PrintTupleTypes_<Rest...>::stream;
        return os;
    }
};
template<typename First>
struct PrintTupleTypes_<First> {
     inline static std::ostream &stream(std::ostream &os) {
         os << TypeName<First>::value;
         return os;
     }
};
// If the tuple contain a tuple and then other types/tuples
template<typename TypeFirst1, typename... TypeFirst, typename Rest1, typename... Rest>
struct PrintTupleTypes_<std::tuple<TypeFirst1, TypeFirst...>, Rest1, Rest...> {
    inline static std::ostream &stream(std::ostream &os) {
        os << "<";
        os << PrintTupleTypes_<TypeFirst1, TypeFirst...>::stream;
        os << ">, ";
        os << PrintTupleTypes_<Rest1, Rest...>::stream;
        return os;
    }
};
// If the tuple contain a tuple
template<typename Types1, typename... Types >
struct PrintTupleTypes_<std::tuple<Types1, Types...>> {
     inline static std::ostream &stream(std::ostream &os) {
         os << "<";
         os << PrintTupleTypes_<Types1, Types...>::stream;
         os << ">";
         return os;
     }
 };
template<typename... Types>
struct PrintTupleTypes;
template<typename Types1, typename... Types>
struct PrintTupleTypes<std::tuple<Types1, Types...>> {
    inline static std::ostream &stream(std::ostream &os) {
        os << "<";
        os << PrintTupleTypes_<Types1, Types...>::stream;
        os << ">";
        return os;
    }
};
// Empty tuple
template<>
struct PrintTupleTypes<std::tuple<>> {
    inline static std::ostream &stream(std::ostream &os) {
        os << "<>";
        return os;
    }
};

////////






/*!
 *  \brief  Merges two or several tuples, the new merged tuple is found in Tuple_merge<Tuples...>::type
 */
template<typename... Types>
struct Tuple_merge;

// If more than 2 tuples
template<typename...Types1,typename...Types2,typename...Tuples>
struct Tuple_merge<std::tuple<Types1...>,std::tuple<Types2...>, Tuples...>
{
  typedef typename Tuple_merge<std::tuple<Types1...,Types2...>, Tuples...>::type type;
};

// If 2 tuples
template<typename...Types1,typename...Types2>
struct Tuple_merge<std::tuple<Types1...>,std::tuple<Types2...>>
{
  typedef std::tuple<Types1..., Types2...> type;
};


////////

    /*!
     *  \brief Tuple_length<Tuple>::value is the length of the tuple, the size in the number of types.
     */
    template<typename Tuple>
    struct Tuple_length {
        static const int value = std::tuple_size<Tuple>::value;
    };

    /*!
     *
     */
    template<typename Tuple, int Index>
    struct Tuple_getType {
        typedef typename std::tuple_element<Index, Tuple>::type type;
    };

    /*!
     *
     */
    template<typename Tuple, int Index>
    typename Tuple_getType<Tuple,Index>::type & Tuple_getValue(Tuple & tuple) {
        return std::get<Index>(tuple);
    }

    /*!
     *  \brief  Tuple_findType<Tuple,Type>::value is the index of the first occurance of Type (or -1 if Tuple does not contain any such type)
     */
    template<typename Tuple, typename Type, int Index = 0>
    struct Tuple_findType {
        static const int value = IF<Condition<std::is_same<typename Tuple_getType<Tuple, Index>::type, Type>::value>,
                                      Int<Index>,                               // Found
                                     IF<Condition<Index+1 >= Tuple_length<Tuple>::value>,
                                          Int<-1>,                              // Not found, end of Tuple
                                         Tuple_findType<Tuple, Type, Index+1>   // Recursion: try next index
                                       >
                                    >::value;
    };
    template<typename Type>
    struct Tuple_findType<std::tuple<>, Type> {
        static const int value = -1;
    };

    /*!
     *
     */
    template<typename Tuple, typename Type>
    struct Tuple_addBack {
        typedef typename Tuple_merge<Tuple,std::tuple<Type> >::type type;
    };

    /*!
     *  \brief  Tuple_add<Tuple, int Index, Type>::type is a tuple with Type at Index and the types in Tuple beginning at Index and counting have their indices incremented by 1.
     */
    template<typename Tuple, typename S>
    struct Tuple_add__;

    template<typename Tuple, size_t... sequence>
    struct Tuple_add__<Tuple, Sequence<sequence...>> {
        typedef std::tuple<typename std::tuple_element<sequence, Tuple>::type...> type;
    };

    template<typename Tuple, int Index, typename Type>
    struct Tuple_add_ {
        typedef typename Range<0, Index-1>::type preSequence;
        typedef typename Range<Index, std::tuple_size<Tuple>::value-1>::type postSequence;
        typedef typename Tuple_add__<Tuple, postSequence>::type preTuple;
        typedef typename Tuple_add__<Tuple, postSequence>::type postTuple;

        typedef typename Tuple_merge<preTuple, std::tuple<Type>, postTuple>::type type;
    };

    // Base
    template<typename Tuple, int Index, typename Type>
    struct Tuple_add;
    // Add at the end or in between two types.
    template<typename... types, int Index, typename Type>
    struct Tuple_add<std::tuple<types...>, Index, Type> {
        static_assert(Index <= int(sizeof...(types)), "Tuple_add<Tuple, index, Type> got a index that was out of bounds (to large)");
        static_assert(Index >= 0,                     "Tuple_add<Tuple, index, Type> got a index that was out of bounds (negative)");
        typedef typename IF<Condition<Index == sizeof...(types)>,
                             Tuple_addBack<std::tuple<types...>, Type>,
                            Tuple_add_<std::tuple<types...>, Index, Type>
                           >::type type;
    };
    // Add at the beginning
    template<typename... types, typename Type>
    struct Tuple_add<std::tuple<types...>, 0, Type> {
        typedef typename Tuple_merge<std::tuple<Type>, std::tuple<types...>>::type type;
    };


    /*!
     *
     */
    template<typename Tuple, typename Type>
    struct Tuple_removeType {
        typedef int type;
    };


    /*!
     *
     */
    template<typename Tuple, typename Type>
    struct Tuple_removeAllType {
        typedef int type;
    };


    /*!
     *
     */
    template<typename Tuple, int Index>
    struct Tuple_remove {
        typedef int type;
    };


    /*!
     *  \brief  Tuple_containType::value is a bool.
     */
    template<typename Tuple, typename Type>
    struct Tuple_containType {
        static const bool value = Tuple_findType<Tuple, Type>::value != -1;
    };

    /*!
     *  \brief Tuple_isEmpty::value is a bool
     */
    template<typename Tuple>
    struct Tuple_isEmpty {
        static const bool value = std::tuple_size<Tuple>::value == 0;
    };


    /*!
     * \brief   Tuple contains the entire Tuple_... interface (for easy access)
     */
    template<typename... Variadic>
    struct Tuple {
        typedef std::tuple<Variadic...> type;

        // Get
        template<int index>
        struct getType : public Tuple_getType<type, index> {};

        // Add
        template<int index, typename Type>
        struct add : public Tuple_add<type, index, Type> {};

        // Remove

        // Boolean
        static const bool isEmpty = Tuple_isEmpty<type>::value;

        // Informatic
        static const unsigned int length = Tuple_length<type>::value;

        // Manage tuples

        // Set operations

    };
}

#endif
