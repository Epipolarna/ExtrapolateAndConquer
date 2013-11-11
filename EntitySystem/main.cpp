#include <iostream>
#include "EntitySystem.hpp"

/* Component prototypes
 ****************************/
class Name;
class Position;

/* Components
 ****************************/
class Name : public Component<> {
public:
    const std::string getName() override { return "Name"; }
    std::string nameString;
};

class Position : public Component<Name> {
public:
    const std::string getName() override { return "Position"; }
    float x;
};

/* Component "lists"
 ****************************/
#define AllComponents Name,Position

/* Systems
 ****************************/
class SystemTest : public System<Name, AllComponents>
{
public:
    SystemTest(EntityManager<AllComponents> & es):System<Name, AllComponents>::System(es) {}
    void processStep(Name & name) {
        Entity<AllComponents> & e = getEntity(name);
        std::cerr << "name: " + e.get<Name>().nameString + "\n";
    }
};

int main()
{

    EntityManager<AllComponents> EntityManager;

    Entity<AllComponents> & e = EntityManager.createEntity();
    Entity<AllComponents> & e2 = EntityManager.createEntity();

    //e.add<Name>();
    //e.get<Name>().nameString = "John Doe";

    e.add<Position>();  // Require component Name! It is added by automagic.
    e.get<Name>().nameString = "John Doe";
    e.remove<Name>();

    std::cerr << "\n";
    //SystemTest test(EntityManager);
    //test.batch();

    return 0;
}

