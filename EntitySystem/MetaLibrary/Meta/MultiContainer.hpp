#ifndef MULTI_CONTAINER_HPP
#define MULTI_CONTAINER_HPP

#include <algorithm>
#include <type_traits>
#include "TupleUtilities.hpp"

/*!
 * \Brief   Creates a class with a Container of each type of Types, accessible by that type.
 */
template<template<class T, class = std::allocator<T> > class Container, typename... Types>
class MultiContainer
{
private:
    std::tuple<Container<Types>...> containers;

public:
    /*!
     * \Brief   Get container by type (getContainer<Type>()).
     */
    template<typename T>
    Container<T> & getContainer()
    {
        return std::get<meta::Tuple_findType<std::tuple<Types...>, T>::value>(containers);
    }

    /*!
     * \Brief   Get container index by type (getIndex<Type>).
     */
    template<typename T>
    constexpr const int getIndex()
    {
        return meta::Tuple_findType<std::tuple<Types...>, T>::value;
    }

    /*!
     * \Brief   Get the number of containers.
     */
    constexpr int size()
    {
        return meta::Tuple_length<std::tuple<Types...>>::value;
    }

};


#endif
