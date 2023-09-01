#pragma once

#include "Object.hpp"

class Floor : public Object {
public: 
    double tileWidth;
    Vector3D cameraPos;

    Color color2;

    Floor(Vector3D &cameraPos, double tileWidth)
    {        
        this->tileWidth = tileWidth;
        this->referencePoint =  cameraPos;
        
        this->color = Color(1.0, 1.0, 1.0); // white
        this->color2 = Color(0.0, 0.0, 0.0); // black
    }

    Color getColorAtPoint(Vector3D &point)
    {
       // x = refX + i * tileWidth; => i = (x - refX) / tileWidth
        int i =  floor((point.x - this->referencePoint.x) / tileWidth); 

        // y = refY + j * tileWidth; => j = (y - refY) / tileWidth
        int j = floor((point.y - this->referencePoint.y) / tileWidth);

        if((i + j) % 2 == 0){ // white tile
            return this->color;
        }
        else{
            return this->color2; // black tile
        }
    }

    double getT(Ray &ray)
    {
        //cout << "floor getT" << endl;

        Vector3D normal = Vector3D(0, 0, 1); // floor is in xy plane => so normal is z axis

        if(ray.dir.dot(normal) == 0.0) // ray is parallel to floor
        {
            return -1.0;
        }

        // t is distance from ray start to intersection point        

        // t = -(D + n.Ro) / n.Rd  
        // here D = 0 because floor is in xy plane 
        //and D is the distance from origin to floor
        double t = -ray.start.dot(normal) / ray.dir.dot(normal); 

        // P(t) = Ro + t * Rd 
        Vector3D intersectionPoint = ray.start + ray.dir.multiply(t);

        return t;
    }

    Vector3D getNormal(Ray &ray, Vector3D &intersectionPoint)
    {
        //cout << "floor getNormal" << endl;

        Vector3D normal = Vector3D(0, 0, 1); // floor is in xy plane => so normal is z axis

        if(ray.start.z < 0.0) // ray is coming from below the floor
        {
            normal.z = -1.0;
        }

        return normal;
    }
    
    void draw(){
        glPushMatrix();
        {
            
            int n = 50;
            double refX = floor(this->referencePoint.x / tileWidth);
            double refY = floor(this->referencePoint.y/ tileWidth);
            

            for(int i = -n; i <= n; i++){
                for(int j = -n; j <= n; j++){
                    if((i + j) % 2 == 0){
                        glColor3f(this->color.r, this->color.g, this->color.b);
                    }
                    else{
                        glColor3f(this->color2.r, this->color2.g, this->color2.b);
                    }

                    double x = refX + i * tileWidth;
                    double y = refY + j * tileWidth;
                    double z = 0;

                    glBegin(GL_QUADS);
                    {
                        glVertex3f(x, y, z);
                        glVertex3f(x + tileWidth, y, z);
                        glVertex3f(x + tileWidth, y + tileWidth, z);
                        glVertex3f(x, y + tileWidth, z);
                    }
                    glEnd();
                }
            }
        }
        glPopMatrix();
    }

    

};