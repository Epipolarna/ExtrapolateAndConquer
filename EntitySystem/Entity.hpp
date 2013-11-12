#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <map>

#include "Component.hpp"
#include "Vectors.hpp"

/* Prototypes
 ***********************************/
template<typename... Components>
class EntityManager;

/* Entity
 ***********************************/
template<typename... Components>
class Entity
{
    friend class EntityManager<Components...>;
public:
    Entity()                 { id = -1; es = 0; }
    Entity(long id) : id(id) {}
    const long getID()       { return id; }
    template<typename Component> bool has();
    template<typename Component> void add();
    template<typename Component> Component & get();
    template<typename Component> void remove();

private:
    long id;
    EntityManager<Components...> * es;
    std::tuple<Components*...> componentIndexes;

    template<typename Component> void assign(long index);
    template<typename Component> long getIndex();
    constexpr bool hasComponent(long index) {  return index > 0; }
};


/*!
 *  \brief  std::vector<Component> & getComponents<Component>()
 */
template<typename... Components>
template<typename Component>
bool Entity<Components...>::has() {
    Component * index = std::get<meta::Tuple_findType<std::tuple<Components...>, Component>::value>(componentIndexes);
    return hasComponent((long)index);
}

/*!
 *  \brief  void add<Component>()
 */
template<typename... Components>
template<typename Component>
void Entity<Components...>::add() {
    if(!has<Component>())
        es-> template addComponent<Component>(*this);
}

/*!
 *  \brief Component & get<Component>()
 */
template<typename... Components>
template<typename Component>
Component & Entity<Components...>::get() {
    Component * index = std::get<meta::Tuple_findType<std::tuple<Components...>, Component>::value>(componentIndexes);
    std::cerr << std::to_string((long)(long*)this)+": Component with index "+std::to_string(((long)index)-1);
    return es->template getComponents<Component>()[((long)index)-1];
}

/*!
 *  \brief void remove<Component>()
 */
template<typename... Components>
template<typename Component>
void Entity<Components...>::remove() {
    if(has<Component>())
        es->template removeComponent<Component>(*this);
}

/*!
*   \brief  PRIVATE
*/
template<typename... Components>
template<typename Component>
void Entity<Components...>::assign(long index) {
     std::get<meta::Tuple_findType<std::tuple<Components...>, Component>::value>(componentIndexes) = (Component*)(index+1);
}

template<typename... Components>
template<typename Component>
long Entity<Components...>::getIndex() {
    return meta::Tuple_findType<std::tuple<Components...>, Component>::value;
}

#endif // ENTITY_HPP
