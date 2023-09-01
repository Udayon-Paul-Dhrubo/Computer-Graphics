#pragma once

#include "Object.hpp"

class Triangle : public Object {
public :
    Vector3D point1, point2, point3;

    Triangle() {}

    Triangle(Vector3D point1, Vector3D point2, Vector3D point3)
    {
        this->point1 = point1;
        this->point2 = point2;
        this->point3 = point3;
    }

    Color getColorAtPoint(Vector3D &point)
    {
        return this->color;
    }

    void draw() {
        glPushMatrix();
        {
            glColor3f(this->color.r, this->color.g, this->color.b);

            glBegin(GL_TRIANGLES);
            {
                glVertex3f(point1.x, point1.y, point1.z);
                glVertex3f(point2.x, point2.y, point2.z);
                glVertex3f(point3.x, point3.y, point3.z);
            }
            glEnd();
        }
        glPopMatrix();
    }

    double getT(Ray &ray){
        //cout << "triangle getT" << endl;
        /* 
                 | ax - bx      ax - cx       Rdx  |      | B |   =  | ax - startx |
                | ay - by     ay - cy       Rdy   |      | Y |   =  | ay - starty |
               | az - bz     az - cz       Rdz   |      | t  |    =  | az - startz |        
         */
        double A[3][3] = {
            {point1.x - point2.x, point1.x - point3.x, ray.dir.x},
            {point1.y - point2.y, point1.y - point3.y, ray.dir.y},
            {point1.z - point2.z, point1.z - point3.z, ray.dir.z}
        };

        double detA = determinant_of_a_matrix(A);

        if(abs(detA) < eps) // ray is parallel to the triangle
        {
            return -1.0;
        }

        double Ab[3][3] = {
            {point1.x - ray.start.x, point1.x - point3.x, ray.dir.x},
            {point1.y - ray.start.y, point1.y - point3.y, ray.dir.y},
            {point1.z - ray.start.z, point1.z - point3.z, ray.dir.z}
        };

        double detBeta = determinant_of_a_matrix(Ab);


        double beta = detBeta / detA;
        if(beta  <=  0.0 ) // intersection point is outside the triangle
        {
            return -1.0;
        }

        double Ay[3][3] = {
            {point1.x - point2.x, point1.x - ray.start.x, ray.dir.x},
            {point1.y - point2.y, point1.y - ray.start.y, ray.dir.y},
            {point1.z - point2.z, point1.z - ray.start.z, ray.dir.z}
        };

        double detGamma = determinant_of_a_matrix(Ay);

        double gamma = detGamma / detA;
        if(gamma <= 0.0 ) // intersection point is outside the triangle
        {
            return -1.0;
        }

        if((beta + gamma - 1.0) >= eps){
            return -1.0;
        }

        double At[3][3] = {
            {point1.x - point2.x, point1.x - point3.x, point1.x - ray.start.x},
            {point1.y - point2.y, point1.y - point3.y, point1.y - ray.start.y},
            {point1.z - point2.z, point1.z - point3.z, point1.z - ray.start.z}
        };

        double detT = determinant_of_a_matrix(At);     


        double t = detT / detA;

        if(t< 0)
            return -1;

        return t;      

    }

    double determinant_of_a_matrix( double mat[3][3])
    {
        double det = mat[0][0] * (mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1])
        - mat[0][1] * (mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0])
        + mat[0][2] * (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]);    

        return det;
    }

    Vector3D getNormal(Ray &ray, Vector3D &intersectionPoint)
    {
        //cout << "triangle getNormal" << endl;
        Vector3D normal = (this->point1 - this->point2).cross(this->point3 - this->point2);
        normal.normalize();

        if(ray.dir.dot(normal) > 0.0)
        {
            normal = normal.multiply(-1.0);
        }
        return normal;
    }

};