#ifndef EntitySystem_HPP
#define EntitySystem_HPP

#include <sstream>
#define LOG(message) {std::stringstream ss; ss << message; std::cerr << std::string(__FUNCTION__)+": "+ss.str();}

#include "EntityManager.hpp"
#include "System.hpp"

#endif // EntitySystem_HPP
