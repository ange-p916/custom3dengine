// GameObject.h
// A very simple representation of an object in the game world.
// It has an ID, a name, and a Transform component.

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Transform.h" // Assuming Transform.h is in the same MyFirstEngine/ subfolder
#include <string>      // For std::string (used for the GameObject's name)

struct GameObject {
    unsigned int id;    // A unique identifier for the game object (could be generated)
    std::string name;   // A human-readable name for the game object (optional)
    Transform transform; // The transform component, defining position, rotation, and scale

    // Constructor
    // Initializes the GameObject with an ID and an optional name.
    // The transform member will be default-initialized by its own constructor.
    GameObject(unsigned int id, const std::string& name = "GameObject")
        : id(id), name(name) {}

    // In a more advanced Entity Component System (ECS):
    // - This GameObject struct might just be an 'Entity' (an ID).
    // - Components like Transform, MeshRenderer, etc., would be stored in separate
    //   data structures (e.g., arrays or maps) and associated with the Entity ID.
    // - Systems would operate on entities that have a specific set of components.
    // For our current simplicity, Transform is a direct member.
};

#endif // GAMEOBJECT_H
