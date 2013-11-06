#ifndef ENTITY_MANAGER_HPP
#define ENTITY_MANAGER_HPP

#include <map>

#include "Vectors.hpp"
#include "Lists.hpp"
#include "Entity.hpp"
#include "Component.hpp"

template<typename... Components>
class EntityManager
{
public:
    EntityManager() {}

    template<typename Component> std::vector<Component> & getComponents();

    Entity<Components...> & createEntity();

    Entity<Components...> & getEntity(int id);

    template<typename Component> void hasComponent(Entity<Components...> & entity);

    template<typename Component> void addComponent(Entity<Components...> & entity);

    template<typename Component> void removeComponent(Entity<Components...> & entity);

private:
    Vectors<Components...> components;
    Lists<Components*...> freeLists;
    std::map<int, Entity<Components...> > entities;

    // Add all required components (specified by the types in the tuple)
    // Interface: add_components<tuple>::ADD(EntityManager, entity);
    //-------------------------->>
    // Recursion
    template<int N, int N_MAX, typename tuple, typename tupleType>
    struct add_components__ {
        static inline void ADD(EntityManager<Components...> & es, Entity<Components...> & e) {
            typedef typename std::tuple_element<N,tuple>::type TYPE_N;
            if(!e.template has<tupleType>()) {
                e.template add<tupleType>();
                std::cerr << "When adding a new component, Entity("+std::to_string(e.getID())+") didn't have its required component \""+e.template get<tupleType>().getName()+"\". It has now been added!";
            }
            add_components__<N+1, N_MAX, tuple, TYPE_N>::ADD(es, e);
        }
    };
    // Terminate when N == N_MAX
    template<int N_MAX, typename tuple, typename tupleType>
    struct add_components__<N_MAX, N_MAX, tuple, tupleType> {
        static inline void ADD(EntityManager<Components...> & es, Entity<Components...> & e) {}
    };
    // Requirements exist, start adding components!
    template<int N_MAX, typename tuple>
    struct add_components_ {
        typedef typename std::tuple_element<0,tuple>::type TYPE_0;
        static inline void ADD(EntityManager<Components...> & es, Entity<Components...> & e) {
            add_components__<0, N_MAX, tuple, TYPE_0>::ADD(es,e);
        }
    };
    // Exit if no requirements exist
    template<typename tuple>
    struct add_components_<0, tuple> {
        static inline void ADD(EntityManager<Components...> & es, Entity<Components...> & e) {}
    };
    // Interface
    template<typename tuple>
    struct add_components {
        static const int size = std::tuple_size<tuple>::value;
        static inline void ADD(EntityManager<Components...> & es, Entity<Components...> & e) {
            add_components_<size, tuple>::ADD(es,e);
        }

    };
    //<<--------------------------
};

template<typename... Components>
template<typename Component>
std::vector<Component> & EntityManager<Components...>::getComponents() {
    return components.template getContainer<Component>();
}

template<typename... Components>
Entity<Components...> & EntityManager<Components...>::createEntity() {
    int id = (int) new int;
    entities[id].id = id;
    entities[id].es = this;
    return entities[id];
}

template<typename... Components>
Entity<Components...> & EntityManager<Components...>::getEntity(int id) {
    return entities[id];
}

template<typename... Components>
template<typename Component>
void EntityManager<Components...>::hasComponent(Entity<Components...> & entity) {
    return entity.template has<Component>();
}



template<typename... Components>
template<typename Component>
void EntityManager<Components...>::addComponent(Entity<Components...> & entity) {

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
    add_components<typename Component::REQUIRED_COMPONENTS>::ADD(*this, entity);

    //getComponents<Component>().reserve(100);    // Should probably reserve tons of components somewhere...
}

// ->

namespace meta
{

    struct test {
        static void execute() {
            std::cout << "tesing!\n";
        }
    };


    template<int N, int N_MAX, typename Tuple, template<typename...> class Template, typename TemplateArgs>
    struct ForEach_ {
        static void next() {
            typedef typename std::tuple_element<N,Tuple>::type TypeN;
            MetaFunction<Template, typename merge_tuples<std::tuple<TypeN>, TemplateArgs>::type>::execute();
            ForEach_<N+1, N_MAX, Tuple, Template, TemplateArgs>::next();
        }
    };
    template<int N_MAX, typename Tuple, template<typename...> class Template, typename TemplateArgs>
    struct ForEach_<N_MAX, N_MAX, Tuple, Template, TemplateArgs> {
        static void next() {}
    };
    template<typename Tuple, template<typename...> class Template, typename TemplateArgs>
    struct ForEach {
        static void execute() {
            ForEach_<0, std::tuple_size<Tuple>::value, Tuple, Template, TemplateArgs>::next();
        }
    };
}

template<typename Type, typename Tuple, int N>
struct Contains_ : public meta::True {};

template<typename Type, typename Tuple>
struct Contains_<Type, Tuple, -1> : public meta::False {};
template<typename Type, typename Tuple>
struct Contains_<Type, Tuple, 0> : public meta::True {};
template<typename Type, typename Tuple>
struct Contains_<Type, Tuple, 1> : public meta::True {};

template<typename Type, typename Tuple, int N>
struct Contains_handleEmptyTuple : public Contains_<Type, Tuple, MetaFunction<get_index, typename merge_tuples<std::tuple<Type>, Tuple >::type>::INDEX> {};
template<typename Type, typename Tuple>
struct Contains_handleEmptyTuple<Type, Tuple, 0> : public meta::False {};

template<typename Type, typename Tuple>
struct Contains__ : public Contains_<Type, Tuple, MetaFunction<get_index, typename merge_tuples<std::tuple<Type>, Tuple >::type>::INDEX> {};

template<typename Type, typename Tuple>
struct Contains : public Contains_handleEmptyTuple<Type, Tuple, std::tuple_size<Tuple>::value> {};
/*
struct Contains : public meta::IF<meta::NOT<isTupleEmpty<Tuple> >,
        Contains__<Type, Tuple>,
        meta::False> {};
*/

template<typename AComponent, typename PrimaryComponent>
struct RemoveIfRequired {
    static void execute() {
        meta::IF<Contains<PrimaryComponent, typename AComponent::REQUIRED_COMPONENTS>, meta::test>::execute();
    }
};

// <-

template<typename... Components>
template<typename Component>
void EntityManager<Components...>::removeComponent(Entity<Components...> & entity) {
    freeLists.template getContainer<Component*>().push_back((Component*)entity.template getIndex<Component>());
    //getComponents<Component>()[index].entityOwnerID = entity.getID();
    entity.template assign<Component>(-1);

    //ForEach<std::tuple<Components...>, RemoveIfRequired, std::tuple<Component> >::execute();
}

#endif // EntityManager_HPP
