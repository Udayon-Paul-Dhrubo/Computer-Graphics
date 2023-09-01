#pragma once

#include "Vector3D.hpp"

class Ray{

public:
    Vector3D start;
    Vector3D dir;

    Ray()
    {

    }
    Ray(Vector3D start, Vector3D dir)
    {
        this->start = start;
        this->dir = dir;
        this->normalize();
    }

    void normalize()
    {
        this->dir.normalize();
    }   
    friend ostream& operator<<(ostream& os, const Ray& r);

};