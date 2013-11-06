#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <string>

template<typename... RequiredComponents>
class Component
{
public:
    virtual const std::string getName() = 0;
    typedef std::tuple<RequiredComponents...> REQUIRED_COMPONENTS;
    int entityOwnerID;
};

#endif // COMPONENT_HPP
