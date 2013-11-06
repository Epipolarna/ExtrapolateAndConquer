#ifndef VARIADIC_UTILITIES_HPP
#define VARIADIC_UTILITIES_HPP

/*!
 *  \brief
 */
template <typename DataType, class... Args>
std::tuple<Args...> tupleAssign(DataType value)
{
    return std::make_tuple(Args(value)...);
}


/* Get the length of a variadic list
 */
template<typename ...Types>
struct get_size {
    enum {SIZE = sizeof...(Types)};
};

#endif
