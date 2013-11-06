#ifndef TUPLE_UTILITIES
#define TUPLE_UTILITIES

#include <tuple>

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
 *  Tuple_append<Tuple1,Tuple2>::type       := A tuple consisting of the types of first T1 then T2
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

    template<typename Tuple, int Index>
    struct Tuple_getType {
        typedef typename std::tuple_element<Index, Tuple>::type type;
    };

    template<typename... Variadic>
    struct Tuple {
        typedef std::tuple<Variadic...> type;

        static const int length = std::tuple_size<type>::value;

        template<int index>
        struct getType : public Tuple_getType<type, index> {};
    };

}

#endif
