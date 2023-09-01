#pragma once

#include "Vector3D.hpp"
#include "Color.hpp"
#include<GL/glut.h>

#define pi acos(0.0) * 2.0
#define sq2 1.0 / sqrt(2)


class PointLight {
    public:
    Vector3D lightPos;
    Color color;
    double falloff;

    PointLight(){}
  
    PointLight(Vector3D lightPos, double falloff){
        this->lightPos = lightPos;
        this->falloff = falloff;
        this->color = Color(1.0, 1.0, 1.0); //white
    }
    void setColor(double r, double g, double b)
    {
        this->color = Color(r, g, b);
    }

    void draw()
    {
        glPushMatrix();
        {

            glTranslatef(this->lightPos.x, this->lightPos.y, this->lightPos.z);
            double radius = 4;

            Vector3D points[100][100];
            int i,j;
            double h,r;
            int stacks = 20, slices=24;
            //generate points
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
                    }glEnd();
                }
            }

        }
        glPopMatrix();
    }

};