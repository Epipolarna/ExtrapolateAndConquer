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
    Entity()                 { id = -1; es = 0; std::fill_n(componentIndexes, meta::Tuple_length<std::tuple<Components...>>::value, -1); }
    Entity(long id) : id(id) {}
    const long getID()       { return id; }
    template<typename Component> bool has();
    template<typename Component> void add();
    template<typename Component> Component & get();
    template<typename Component> void remove();

private:
    long id;
    EntityManager<Components...> * es;
    long componentIndexes[meta::Tuple_length<std::tuple<Components...>>::value];

    template<typename Component> void assign(long index);
    template<typename Component> long getIndex();
};


/*!
 *  \brief  std::vector<Component> & getComponents<Component>()
 */
template<typename... Components>
template<typename Component>
bool Entity<Components...>::has() {
    static_assert(meta::Tuple_findType<std::tuple<Components...>, Component>::value != -1, "Type not part of Components...");

    return componentIndexes[meta::Tuple_findType<std::tuple<Components...>, Component>::value] >= 0;
}

/*!
 *  \brief  void add<Component>()
 */
template<typename... Components>
template<typename Component>
void Entity<Components...>::add() {
    static_assert(meta::Tuple_findType<std::tuple<Components...>, Component>::value != -1, "Type not part of Components...");

    if(!has<Component>())
        es-> template addComponent<Component>(*this);
}

/*!
 *  \brief Component & get<Component>()
 */
template<typename... Components>
template<typename Component>
Component & Entity<Components...>::get() {
    static_assert(meta::Tuple_findType<std::tuple<Components...>, Component>::value != -1, "Type not part of Components...");

    long index = componentIndexes[meta::Tuple_findType<std::tuple<Components...>, Component>::value];
    return es->template getComponents<Component>()[index];
}

/*!
 *  \brief void remove<Component>()
 */
template<typename... Components>
template<typename Component>
void Entity<Components...>::remove() {
    static_assert(meta::Tuple_findType<std::tuple<Components...>, Component>::value != -1, "Type not part of Components...");

    if(has<Component>())
        es->template removeComponent<Component>(*this);
}

/*!
*   \brief  PRIVATE
*/
template<typename... Components>
template<typename Component>
void Entity<Components...>::assign(long index) {
    std::cerr << std::to_string(index)+": Component with index "+std::to_string(index)+" assigned"; // Debug
    componentIndexes[meta::Tuple_findType<std::tuple<Components...>, Component>::value] = index;
}

/*!
*   \brief  PRIVATE
*/
template<typename... Components>
template<typename Component>
long Entity<Components...>::getIndex() {
    return componentIndexes[meta::Tuple_findType<std::tuple<Components...>, Component>::value];
}

#endif // ENTITY_HPP
