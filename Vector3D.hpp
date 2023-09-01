#pragma once

#include<bits/stdc++.h>
using namespace std;


class Vector3D {

public:
    double x, y, z;
    Vector3D()
    {
        this->x = 0.0;
        this->y = 0.0;
        this->z = 0.0;
    }
    Vector3D(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    Vector3D operator +(Vector3D const &v)
    {
        return Vector3D(this->x+v.x, this->y+v.y, this->z+v.z);
    }
    Vector3D operator -(Vector3D const &v)
    {
        return Vector3D(this->x-v.x, this->y-v.y, this->z-v.z);
    }
    Vector3D operator -()
    {
        return Vector3D(-this->x, -this->y, -this->z);
    }
    Vector3D multiply(double a)
    {
        return Vector3D(a*this->x, a*this->y, a*this->z);
    }
    double dot(Vector3D const &v)
    {
        return (this->x * v.x + this->y * v.y + this->z * v.z);
    }
    Vector3D cross(Vector3D const &v)
    {
        return Vector3D(this->y * v.z - this->z * v.y, this->z * v.x - this->x * v.z, this->x * v.y - this->y * v.x);
    }
    double square()
    {
        return (this->x * this->x + this->y * this->y + this->z * this->z);
    }
    void normalize()
    {
        double n = sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
        this->x /= n;
        this->y /= n;
        this->z /= n;
    }
   double distance(Vector3D const &v){
    return sqrt((this->x-v.x)*(this->x-v.x)+(this->y-v.y)*(this->y-v.y)+(this->z-v.z)*(this->z-v.z));
   }

   double length(){
    return sqrt(this->x*this->x+this->y*this->y+this->z*this->z);
   }

};