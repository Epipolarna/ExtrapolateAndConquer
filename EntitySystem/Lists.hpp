#ifndef LISTS_HPP
#define LISTS_HPP

#include <list>

#include "MetaLibrary.hpp"

template<typename... Types>
class Lists : public MultiContainer<std::list, Types...> {};

#endif
