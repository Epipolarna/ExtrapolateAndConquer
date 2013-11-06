#ifndef VECTORS_HPP
#define VECTORS_HPP

#include <vector>

#include "MetaLibrary.hpp"

template<typename... Types>
class Vectors : public MultiContainer<std::vector, Types...> {};

#endif
