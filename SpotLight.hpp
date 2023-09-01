#pragma once

#include "Vector3D.hpp"
#include "Color.hpp"
#include<GL/glut.h>

#include "PointLight.hpp"
#include "Ray.hpp"

#define pi acos(0.0) * 2.0
#define sq2 1.0 / sqrt(2)

class SpotLight{

public:
    PointLight pointLight;
    Vector3D lightDirection;
    double cutoffAngle;
    Color color;

    SpotLight(){ }
    
    SpotLight(PointLight pointLight, Vector3D lightDirection, double cutoffAngle)
    {
        this->pointLight = pointLight;
        this->color = Color(1, 1, 1); //green
        this->lightDirection = lightDirection;
        this->cutoffAngle = cutoffAngle;
    }

    void drawCone(double tipX, double tipY, double tipZ, double dirX,double dirY,double dirZ) {   
    
        glTranslatef(tipX, tipY, tipZ);
        int numSlices = 100;
        int coneHeight = 8;
        
        float length = sqrt(dirX * dirX + dirY * dirY + dirZ * dirZ);
        
        // Normalize the direction vector
        dirX /= length;
        dirY /= length;
        dirZ /= length;
        
        // Calculate the angle between the cone axis and the vertical axis
        float angle = acos(dirZ);
        float axisRotation = atan2(dirY, dirX) * 180.0f / M_PI;
        
        // Apply rotation to align the cone with the specified direction
        glRotatef(axisRotation, 0.0f, 0.0f, 1.0f);
        glRotatef(angle * 180.0f / M_PI, 0.0f, 1.0f, 0.0f);
        
        // Draw the cone's surface using triangle strips
        glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= numSlices; ++i) {
            float theta = i * (2.0f * M_PI / numSlices);
            
            float x = 0.5f * length * cos(theta);
            float y = 0.5f * length * sin(theta);
            float z = -coneHeight;
            
            glVertex3f(x, y, z);
            glVertex3f(0.0f, 0.0f, coneHeight);
        }
        glEnd();

}

    void draw()
    {
         //draw cone
        glPushMatrix();
        {          
            glColor3f(this->color.r, this->color.g, this->color.b);           
            drawCone(this->pointLight.lightPos.x, this->pointLight.lightPos.y, this->pointLight.lightPos.z, this->lightDirection.x, this->lightDirection.y, this->lightDirection.z);
        }
        glPopMatrix();
    
    }
    
    bool exceedsCutoff(Ray &ray)
    {
        double dot = this->lightDirection.dot(ray.dir);
        dot /= sqrt(this->lightDirection.square() * ray.dir.square()); 
        double theta = acos(abs(dot));

        return (theta > (this->cutoffAngle * pi / 180.0));
    }
    
};