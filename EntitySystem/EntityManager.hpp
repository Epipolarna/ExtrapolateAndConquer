#ifndef EntityManager_HPP
#define EntityManager_HPP

#include <map>

#include "Vectors.hpp"
#include "Lists.hpp"
#include "Entity.hpp"
#include "Component.hpp"

/* EntityManager
 ***********************************/
template<typename... Components>
class EntityManager
{
public:
    EntityManager() { nextID = 1; }
    template<typename Component> std::vector<Component> & getComponents();
    Entity<Components...> & createEntity();
    Entity<Components...> & getEntity(long id);
    template<typename Component> void hasComponent(Entity<Components...> & entity);
    template<typename Component> void addComponent(Entity<Components...> & entity);
    template<typename Component> void removeComponent(Entity<Components...> & entity);
private:
    Vectors<Components...> components;
    Lists<Components*...> freeLists;
    std::map<long, Entity<Components...>> entities;
    long nextID;

    long getUniqueID() { return nextID++; }
};

/*!
 *  \brief  std::vector<Component> & getComponents<Component>()
 */
template<typename... Components>
template<typename Component>
std::vector<Component> & EntityManager<Components...>::getComponents() {
    static_assert(meta::Tuple_findType<std::tuple<Components...>, Component>::value != -1, "Type not part of Components...");

    return components.template getContainer<Component>();
}

/*!
*  \brief  Entity & createEntity()
*/
template<typename... Components>
Entity<Components...> & EntityManager<Components...>::createEntity() {
    long id = getUniqueID();
    entities[id].id = id;
    entities[id].es = this;
    return entities[id];
}

/*!
*  \brief  Entity & getEntity(long id)
*/
template<typename... Components>
Entity<Components...> & EntityManager<Components...>::getEntity(long id) {
    return entities[id];
}

/*!
*  \brief  bool hasComponent<Component>(Entity & entity)
*/
template<typename... Components>
template<typename Component>
void EntityManager<Components...>::hasComponent(Entity<Components...> & entity) {
    static_assert(meta::Tuple_findType<std::tuple<Components...>, Component>::value != -1, "Type not part of Components...");

    return entity.template has<Component>();
}

/* Remove any component (Element) which require a certaint component (Component)
 ******************************/
template<typename Component, typename... Components>
struct REMOVE_COMPONENT {
    static void execute(Entity<Components...> & e) {
        if(e.template has<Component>()) {
            Component c;
            std::cout << "Entity (" << e.getID() << ") warning: ";
            std::cout << "A component has been removed which is required by \"" << c.getName() << "\", it is now auto removed.\n";
        }
        e. template remove<Component>();
    }
};
template<typename Component, typename... Components>
struct REMOVE_COMPONENT_FALSE {
    static void execute(Entity<Components...> & e) {}
};
template<typename Element, typename Component, typename... Components>
struct REMOVE_IF_REQUIRES : public
        meta::IF< meta::Condition<meta::Tuple_containType<typename Element::REQUIRED_COMPONENTS, Component>::value>,
                  REMOVE_COMPONENT<Element, Components...>,
                  REMOVE_COMPONENT_FALSE<Element, Components...>
                > {};

/* Add a component (since it is required)
 ******************************/
template<typename Element, typename Component, typename... Components>
struct ADD_COMPONENT {
    static void execute(Entity<Components...> & e) {
        if(!e.template has<Element>()) {
            e.template add<Element>();
            Component c;
            Element el;
            std::cout << "Entity (" << e.getID() << ") warning: ";
            std::cout << "Adding \"" << c.getName() << "\" require \"" << el.getName() << "\" which is missing, it is now auto added.\n";
        }
    }
};



/*!
*  \brief  void addComponent<Component>(Entity & entity)
*/
template<typename... Components>
template<typename Component>
void EntityManager<Components...>::addComponent(Entity<Components...> & entity) {
    static_assert(meta::Tuple_findType<std::tuple<Components...>, Component>::value != -1, "Type not part of Components...");

    // Add Component to entity
    if(!freeLists.template getContainer<Component*>().empty()) {
        long index = (long)freeLists.template getContainer<Component*>().back();
        getComponents<Component>()[index].entityOwnerID = entity.getID();
        entity.template assign<Component>(index);
        freeLists.template getContainer<Component*>().pop_back();
    } else {
        getComponents<Component>().push_back(Component());
        getComponents<Component>().back().entityOwnerID = entity.getID();
        long index = getComponents<Component>().size()-1;
        entity.template assign<Component>(index);
    }

    // If additional Components are required, add these aswell (recursively)
    meta::FOR_EACH< typename Component::REQUIRED_COMPONENTS,    // List to iterate over
                    ADD_COMPONENT,                              // Template to apply on each item
                    std::tuple<Component, Components...>,       // Additional template parameters to above template
                    std::tuple<Entity<Components...>&>          // Argument types to the above templatet's function "execute"
                  >::execute(entity);
}

/*!
*  \brief  void removeComponent<Component>(Entity & entity)
*/
template<typename... Components>
template<typename Component>
void EntityManager<Components...>::removeComponent(Entity<Components...> & entity) {
    static_assert(meta::Tuple_findType<std::tuple<Components...>, Component>::value != -1, "Type not part of Components...");

    // Remove Component from entity
    /*
     * TODO: add components to free-list and only swap out components if not "reallocated" soon enough.
     */
    if(getComponents<Component>().size() > 1) {
        long index = entity.template getIndex<Component>();
        long lastIndex = getComponents<Component>().size()-1;
        std::swap(getComponents<Component>()[index], getComponents<Component>()[lastIndex]);
    }
    getComponents<Component>().pop_back();
    entity.template assign<Component>(-1);

    // If any other Component require this component, remove it aswell.
    meta::FOR_EACH< std::tuple<Components...>,              // List to iterate over
                    REMOVE_IF_REQUIRES,                     // Template to apply on each item
                    std::tuple<Component, Components...>,   // Additional template parameters to above template
                    std::tuple<Entity<Components...>&>      // Argument types to the templet's function "execute"
                  >::execute(entity);
}


#endif // EntityManager_HPP
