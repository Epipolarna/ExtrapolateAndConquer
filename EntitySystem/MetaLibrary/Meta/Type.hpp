/*
 * Author: Mattias Tiger 2013
 */
#ifndef TYPE_HPP
#define TYPE_HPP


/*
 *
 */

/*!
 *  \brief  TypeName contain a types name (declared using DECLARE_TYPE_NAME(Type).
 */
/*
template <typename T>
struct TypeName {
    static const char * execute() { return "Type not declared!"; }
};
#define DECLARE_TYPE_NAME(Type) template <> struct TypeName<Type> { static const char * execute() { return #Type; } };
*/

template<typename T>
struct TypeName {
    static constexpr const char * value = "Type not declared!";
};

#define DECLARE_TYPE_NAME(Type) template <> struct TypeName<Type> { static constexpr const char * value = #Type; };

DECLARE_TYPE_NAME(void)
DECLARE_TYPE_NAME(int)
DECLARE_TYPE_NAME(bool)
DECLARE_TYPE_NAME(float)
DECLARE_TYPE_NAME(double)
DECLARE_TYPE_NAME(short)
DECLARE_TYPE_NAME(long)
DECLARE_TYPE_NAME(char)
DECLARE_TYPE_NAME(std::tuple<>)

namespace meta
{

}


#endif
