#pragma once
// GameObject.h
// A very simple representation of an object in the game world.

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Transform.h"
#include <string> // For a name, optional

struct GameObject {
    unsigned int id; // A unique ID, could be generated
    std::string name;
    Transform transform; // Each GameObject has a transform

    // In a true ECS, this GameObject would be an ID, and components
    // would be stored separately, associated by this ID.
    // For simplicity now, Transform is a direct member.

    GameObject(unsigned int id, const std::string& name = "GameObject")
        : id(id), name(name) {}
};

#endif // GAMEOBJECT_H
