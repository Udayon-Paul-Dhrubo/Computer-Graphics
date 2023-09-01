#pragma once

#include "Object.hpp"
#include "bitmap_image.hpp"

bool showTexture = false;

class Floor : public Object {
public: 
    double tileWidth;
    Vector3D cameraPos;

    bitmap_image *whiteImage, *blackImage;

    Color color2;

    Color getColorAtPicture(Vector3D &point, int i, int j, bool isWhite = true)
    {
        double x = point.x - this->referencePoint.x - i * tileWidth;
        double y = point.y - this->referencePoint.y - j * tileWidth;

        bitmap_image *image = isWhite ? whiteImage : blackImage;

        int texture_x = floor((image->width() * x )/ tileWidth);
        int texture_y = floor((image->height() * y )/ tileWidth);

        texture_x = abs(texture_x);
        texture_y = abs(texture_y);


        unsigned char r, g, b;
        image->get_pixel(texture_x, texture_y, r, g, b);

        return Color(r / 255.0 * 0.5,  g / 255.0 * 0.5, b / 255.0 * 0.5);
    }

    Floor(Vector3D &cameraPos, double tileWidth)
    {        
        this->tileWidth = tileWidth;
        this->referencePoint =  cameraPos;

        //open image files
        whiteImage = new bitmap_image("texture_w.bmp");      
        blackImage = new bitmap_image("texture_b.bmp");

        
        
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

            if(showTexture)  return getColorAtPicture(point, i, j);
            
            return this->color;
        }
        else{

            if(showTexture)  return getColorAtPicture(point, i, j, false);
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