// GameObject.h
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Transform.h"
#include <string>

struct GameObject {
    unsigned int id;
    std::string name;
    Transform transform;

    static unsigned int nextID; // Static counter for unique IDs

    // Default constructor: assigns a unique ID and a default name.
    GameObject(const std::string& name = "GameObject")
        : id(nextID++), name(name) {}

    // Constructor to specify an ID (e.g., for deserialization)
    // This also ensures nextID is always ahead of any manually assigned ID.
    GameObject(unsigned int specific_id, const std::string& name = "GameObject")
        : id(specific_id), name(name) {
        if (specific_id >= nextID) {
            nextID = specific_id + 1;
        }
    }
    // You might want a constructor that takes an ID and a name if you need to initialize both explicitly during creation
    // For instance, the one I used in main.cpp : sceneGameObjects.emplace_back(0, "Triangle Alpha");
    // Needs to be handled or use a different way to initialize in main. For simplicity, I'll remove specific ID for emplace_back now.
    // So, let's simplify the emplace_back in main for now, or add another constructor.
};

#endif // GAMEOBJECT_H