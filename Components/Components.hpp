#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

// List all components <TODO: replace with std::tuple>
#define Components Name

/*
 * Component prototype list
 ************************************************************/
class Name;


/*
 * Component list
 ************************************************************/
class Name : public Component<> {
    const std::string getName() override { return "Name"; }

    std::string name;
};

#endif
