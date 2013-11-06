#ifndef MULTI_CONTAINER_HPP
#define MULTI_CONTAINER_HPP

#include <algorithm>
#include <type_traits>

#include "VariadicUtilities.hpp"

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
        return std::get<get_index<T,Types...>::INDEX>(containers);
    }

    /*!
     * \Brief   Get container index by type (getIndex<Type>).
     */
    template<typename T>
    constexpr const int getIndex()
    {
        return get_index<T, Types...>::INDEX;
    }

    /*!
     * \Brief   Get the number of containers.
     */
    constexpr int size()
    {
        return get_size<Types...>::SIZE;
    }

};


#endif
