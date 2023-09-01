#pragma once

#include "Vector3D.hpp"
#include "Color.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"

#include "Ray.hpp"

#include<GL/glut.h>


#define pi acos(0.0) * 2.0
#define sq2 1.0 / sqrt(2)
#define inf 1e18
#define AMB 0
#define DIFF 1
#define SPEC 2
#define REC_REFLECTION 3
#define eps 1e-8




class Object;

vector<Object *> objects;
vector<PointLight *> pointLights;
vector<SpotLight *> spotLights; 

int recursions;

class Object {
public: 
    Vector3D referencePoint;
    double height, width, length;
    Color color;
    double coEfficients[4]; // ambient, diffuse, specular, reflection coefficients
    int shine; // exponent term of specular component

    Object()
    {

    }
    Object(Vector3D referencePoint)
    {
        this->referencePoint = referencePoint;
    }
    Object(Vector3D referencePoint, double length, double width, double height)
    {
        this->referencePoint = referencePoint;
        this->height = height;
        this->width = width;
        this->length = length;
    }
    void setColor(double r, double g, double b)
    {
        this->color = Color(r, g, b);
    }

    void setShine(int shine)
    {
        this->shine = shine;
    }
    void setCoefficients(double ambient, double diffuse, double specular, double reflection)
    {
        this->coEfficients[0] = ambient;
        this->coEfficients[1] = diffuse;
        this->coEfficients[2] = specular;
        this->coEfficients[3] = reflection;
    }

    virtual void draw()
    {
        cout << "base draw function" << endl;
    }

    virtual Color getColorAtPoint(Vector3D &point)
    {
        cout << "base getColorAtPoint function" << endl;
        return this->color;
    }


    // T = nearest intersection point from ray.start
    virtual double getT(Ray &ray)
    {
        cout << "base getT" << endl;
        return -1.0;
    }

    virtual Vector3D getNormal(Ray &ray, Vector3D &intersectionPoint)
    {
        cout << "base getNormal function" << endl;
        return Vector3D(0, 0, 0);
    }

    double intersect(Ray &ray, Color &color, int level);

    void getColorByPhongsModel(double tCurrent, Ray &ray, Color &color, Vector3D &intersectionPoint,
                                Color &intersectionColor, Vector3D &normal);

};

double Object::intersect(Ray &ray, Color &color, int level=0){
    ray.normalize();

    // find the nearest intersection point
    double t = getT(ray);

    if(level == 0 || t < 0) return t;

     // do lighting and reflection
    Vector3D intersectionPoint = ray.start + ray.dir.multiply(t);
    Color intersectionColor = this->getColorAtPoint(intersectionPoint);

    // calculate normal
    Vector3D normal = this->getNormal(ray, intersectionPoint);

    getColorByPhongsModel(t, ray, color, intersectionPoint, intersectionColor, normal);

    // recursive reflection
    if(level >= recursions) return t;

    Ray reflected = Ray(intersectionPoint, normal.multiply(2.0 * normal.dot(-ray.dir)) + ray.dir);
    // slightly forward for avoiding self intersection
    reflected.start = reflected.start + reflected.dir.multiply(0.00001);
    reflected.normalize();

    double tmin = inf;
    int objnum = -1, idx = 0;
    Color reflectedColor;
    for(Object *obj: objects)
    {
        double tt = obj->intersect(reflected, reflectedColor, 0);
        if(tt >= 0.0 && (tmin-tt) > eps)
        {
            tmin = tt;
            objnum = idx;
            
        }
        idx++;
    }

    if(abs(tmin - inf) > eps)
    {
        objects[objnum]->intersect(reflected, reflectedColor, level + 1);
        color = color + reflectedColor * this->coEfficients[REC_REFLECTION];  
        color.fixRange();          
    }
    
    return t;

}

void Object::getColorByPhongsModel(double tCurrent, Ray &ray, Color &finalColor, Vector3D &intersectionPoint,
                                Color &intersectionColor, Vector3D &normal)
{

    // do lighting and reflection
    finalColor = (intersectionColor * (this->coEfficients[AMB]));
    finalColor.fixRange();
    
    double lambertValue = 0.0, phongValue = 0.0;

    for(PointLight *pl: pointLights)
    {
        // direction vector from light source -> intersection point
        Ray rayL = Ray(pl->lightPos, intersectionPoint - pl->lightPos);
        rayL.normalize();
        

        double tmin = inf; // nearest intersection point found so far
        for(Object *o: objects) 
        {
            double tnow = o->intersect(rayL, color, 0);
            if(tnow > 0.0 && (tmin - tnow)  > eps) // t > 0.0 => intersects with object 
                                                                                    // t < tmin => nearer object
            {
                tmin = tnow; 
            }
        }

        if((tCurrent - tmin) > eps) // if the intersection point is not the nearest
        {
            continue; // no contribution from this light source
        }

        // calculate the lighting by phong model
        Vector3D recurssiveIntersectionPoint = rayL.start + rayL.dir.multiply(tmin);
        Vector3D toSource = pl->lightPos - recurssiveIntersectionPoint;
        toSource.normalize();

        double distance = pl->lightPos.distance(recurssiveIntersectionPoint);
        double scallingFactor = exp(-pl->falloff * (distance * distance));

        Vector3D normal = this->getNormal(rayL, recurssiveIntersectionPoint);
        normal.normalize();

         lambertValue += scallingFactor * max(normal.dot(toSource), 0.0);
        Vector3D reflected = ray.dir - normal.multiply(2.0 * normal.dot(ray.dir));
        reflected.normalize();

        phongValue += scallingFactor * pow(max(reflected.dot(toSource), 0.0), this->shine);

    }

    for(SpotLight *spl: spotLights)
    {

        Vector3D light = intersectionPoint - spl->pointLight.lightPos;
        light.normalize();

        double angle = light.dot(spl->lightDirection) / (light.length() * spl->lightDirection.length());
        angle = acos(angle) * 180.0 / pi;

        if(angle > spl->cutoffAngle)
            continue;

        Ray rayL = Ray(spl->pointLight.lightPos, intersectionPoint - spl->pointLight.lightPos);
        double distance = spl->pointLight.lightPos.distance(intersectionPoint);

        
        // check if the ray intersects with any object
        double tmin = inf;
        for(Object *o: objects)
        {
            double tnow = o->intersect(rayL, color, 0);
            if(tnow > 0.0 && (tmin - tnow)  > eps)
            {
                tmin = tnow;
            }
        }

        if((tCurrent - tmin) > eps)
        {
            continue;
        }

         // calculate the lighting by phong model
        Vector3D recurssiveIntersectionPoint = rayL.start + rayL.dir.multiply(tmin);
        Vector3D toSource =spl->pointLight.lightPos - recurssiveIntersectionPoint;
        toSource.normalize();

        distance = spl->pointLight.lightPos.distance(recurssiveIntersectionPoint);
        double fallOff = spl->pointLight.falloff;
        double scallingFactor = exp(- fallOff * (distance * distance));

        Vector3D normal = this->getNormal(rayL, recurssiveIntersectionPoint);
        normal.normalize();

         lambertValue += scallingFactor * max(normal.dot(toSource), 0.0);
        Vector3D reflected = ray.dir - normal.multiply(2.0 * normal.dot(ray.dir));
        reflected.normalize();

        phongValue += scallingFactor * pow(max(reflected.dot(toSource), 0.0), this->shine);

    }

    finalColor = finalColor + intersectionColor * (this->coEfficients[DIFF] * lambertValue + this->coEfficients[SPEC] * phongValue);
    finalColor.fixRange();

    return ;


}





