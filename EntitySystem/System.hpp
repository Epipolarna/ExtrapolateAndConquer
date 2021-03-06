#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "EntitySystem.hpp"

template<typename PrimaryComponent, typename... Components>
class System
{
    static_assert(meta::Tuple_findType<std::tuple<Components...>, PrimaryComponent>::value != -1, "The primary component of the system is not in the components list!");
public:
    System() { es = 0; }

    // External methods
    void initialize(EntityManager<Components...> & entityManager) { es = &entityManager; }
    virtual void processStep(PrimaryComponent & c) = 0;
    virtual void batch();

protected:
    // Internal methods
    Entity<Components...> & getEntity(PrimaryComponent & c);
    std::vector<PrimaryComponent> & getComponents();

private:
    EntityManager<Components...> * es;

};

/*!
 * \brief   Calls processStep for every component of PrimaryComponent type.
 * TODO: Parallize!
 */
template<typename PrimaryComponent, typename... Components>
void System<PrimaryComponent, Components...>::batch() {
    std::vector<PrimaryComponent> & components = es->template getComponents<PrimaryComponent>();
    for(int n = 0; n < components.size(); n++) {
        processStep(components[n]);
    }
}

template<typename PrimaryComponent, typename... Components>
Entity<Components...> & System<PrimaryComponent, Components...>::getEntity(PrimaryComponent & c) {
    return es->getEntity(c.entityOwnerID);
}

template<typename PrimaryComponent, typename... Components>
std::vector<PrimaryComponent> & System<PrimaryComponent, Components...>::getComponents() {
    return es->template getComponents<PrimaryComponent>();
}



#endif // SYSTEM_HPP
