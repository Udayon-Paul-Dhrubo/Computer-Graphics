#pragma once

#include "Object.hpp"

class Sphere : public Object {
public: 
    Sphere(Vector3D center, double radius)
    {
        this->referencePoint = center;
        this->length = radius;
        this->width = radius;
        this->height = radius;
    }

    Color getColorAtPoint(Vector3D &point)
    {
        return this->color;
    }

    double getT(Ray &ray){  
        //cout << "sphere getT" << endl;

        Vector3D r0 = ray.start - this->referencePoint; // direction vector from center to ray start
        ray.normalize();

        // minus to change direction ray start to center
        double tp = -r0.dot(ray.dir); // projection of r0 on ray direction vector

        // behind the camera - no intersection
        if(tp < 0.0) {
            //cout << "behind the camera" << endl;
            return -1.0;
        }
       
       // d = distance from center  to ray
        double d2 = r0.square() - tp * tp;
        
        // if  distance > radius => no intersection
        if((d2 - this->length * this->length) > eps)
        {
            //cout << "ray distance is greater than radius" << endl;
            return -1.0;
        }

        // t` = sqrt(r^2 - d^2)
        double tprime = sqrt(this->length * this->length - d2); 
        
        // possible t values
        double t1 = tp + tprime;
        double t2 = tp - tprime;

        double t = -1.0;
        
        if((r0.square() - this->length * this->length) > eps) // if origin is outside the sphere
        {
            if(t2 >= 0.0)
            {
                t = t2;
            }
        }
        else // if origin is inside the sphere
        {
            if(t1 >= 0.0) 
            {
                t = t1;
            }  
        }

        return t;
    }

    Vector3D getNormal(Ray &ray, Vector3D &intersectionPoint)
    {
       // cout << "sphere getNormal" << endl;

        // normal = intersectionPoint - center
        Vector3D normal = intersectionPoint - this->referencePoint;
        normal.normalize();

    
        if((ray.start - this->referencePoint).square() - this->length * this->length > eps) // if origin is outside the sphere
        {
            normal = normal.multiply(-1.0);
        }
        return normal; 

    }

     void draw()
    {
        glPushMatrix();
        {            
            double radius = this->length;

            Vector3D points[100][100];
            int i,j;
            double h,r;
            int stacks = 20, slices=24;
            // gdsfgsdfg
            for(i=0;i<=stacks;i++)
            {
                h=radius*sin(((double)i/(double)stacks)*(pi/2));
                r=radius*cos(((double)i/(double)stacks)*(pi/2));
                for(j=0;j<=slices;j++)
                {
                    points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
                    points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
                    points[i][j].z=h;
                }
            }
            //draw quads using generated points
            glTranslatef(this->referencePoint.x, this->referencePoint.y, this->referencePoint.z);
            glColor3f(this->color.r, this->color.g, this->color.b);

            for(i=0;i<stacks;i++)
            {
                for(j=0;j<slices;j++)
                {
                    
                    glBegin(GL_QUADS);{
                        //upper hemisphere
                        
                        glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                        glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                        glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                        glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                        //lower hemisphere
                        glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
                        glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
                        glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
                        glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
                    }
                    glEnd();
                }
            }

        }
        glPopMatrix();
    }

};  