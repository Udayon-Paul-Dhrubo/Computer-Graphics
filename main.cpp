#include "Object.hpp"
#include "bitmap_image.hpp"

#include "Sphere.hpp"
#include "Triangle.hpp"
#include "Floor.hpp"

using namespace std;

// window parameters
int windowWidth, windowHeight;
double viewAngle;

extern int recursions;
int numberOfPixels;

double widthOfCell;



// camera parameters
double cameraHeight;
double cameraAngle;
Vector3D pos, u, r, l;

// perspective parameters
double nearDistance, farDistance, fovY, aspectRatio;


extern vector<Object *> objects;
extern vector<PointLight *> pointLights;
extern vector<SpotLight *> spotLights;


bool drawaxes;
void drawAxes();
void clear_Vectors();
void specialKeyListener(int key, int x, int y);
void keyboardListener(unsigned char key, int xx, int yy);

void transformPyramid_to_triangles(Vector3D bottomLeftCorner, double width, double height, Color color, double ambient, double diffuse, double specular, double reflection, int shine);
void transformCube_to_triangles(Vector3D bottomLeftCorner, double width, Color color, double ambient, double diffuse, double specular, double reflection, int shine);

void loadData(){
	ifstream scene("description.txt");

	double r, g, b,ambient, diffuse, specular, reflection;
    int shine;

	

	if(scene.is_open()){
		scene >> nearDistance >> farDistance >> fovY >> aspectRatio;

		
		scene >> recursions;
		scene >> numberOfPixels;
		

		Object *tempObject;

		
		// floor input
		double tileWidth;
		scene >> tileWidth;

		cout << "tileWidth : " << tileWidth << endl;

		tempObject  = new Floor(pos, tileWidth); 
		scene >> ambient >> diffuse >> reflection;

		cout << "ambient : " << ambient << " && diffuse : " << diffuse << " && reflection : " << reflection << endl;

		tempObject->setCoefficients(ambient, diffuse, 0, reflection);
		tempObject->setShine(0);

		objects.push_back(tempObject);

		// objects input
		int numberOfObjects;
		scene >> numberOfObjects;

		//object type
		int objectType; // 0 for sphere, 1 for pyramid, 2 for cube

		for( int i = 0; i < numberOfObjects;  i++ ) {
			string name;
            scene >> name;

			Vector3D bottomLeftCorner, center;
			double width, height, radius;

			// parameters input
			if( name == "sphere"){
				// sphere parameters: center and radius

                scene >> center.x >> center.y >> center.z;
                scene >> radius;

				objectType = 0;
			}
			else if( name == "pyramid"){
				// pyramid parameters : bottomLeftCorner, width, height
				
				scene >> bottomLeftCorner.x >> bottomLeftCorner.y >> bottomLeftCorner.z;
				scene >> width >> height;

				objectType = 1;
			}
			else if( name == "cube"){
				// cube parameters : bottomLeftCorner, width 

				scene >> bottomLeftCorner.x >> bottomLeftCorner.y >> bottomLeftCorner.z;
				scene >> width;

				objectType = 2;
			}
			else{
				cout << "Error: Object name not found" << endl;
			}
						
			// color input
			scene >> r >> g >> b;

			//coEfficients input
			scene >> ambient >> diffuse >> specular >> reflection;
			scene >> shine;

			// create object
			if( objectType == 0 ){ //sphere
				tempObject = new Sphere(center, radius);

				// set color and coEfficients
				tempObject->setColor(r, g, b);
				tempObject->setCoefficients(ambient, diffuse, specular, reflection);
				tempObject->setShine(shine);

				// add object to objects vector
				objects.push_back(tempObject);

			}
			else if( objectType == 1 ){ //pyramid
				transformPyramid_to_triangles(bottomLeftCorner, width, height, Color(r, g, b), ambient, diffuse, specular, reflection, shine);
			}
			else if( objectType == 2){ //cube
				transformCube_to_triangles(bottomLeftCorner, width, Color(r, g, b), ambient, diffuse, specular, reflection, shine);
			}	
			
		}


		// point lights input
		int numberOfPointLights;
		scene >> numberOfPointLights;

		PointLight *tempPointLight;
		

		for ( int i = 0; i < numberOfPointLights; i++ ){
			//point light parameters: lightSource, falloff

			Vector3D lightSource;
			double falloff;
			scene >> lightSource.x >> lightSource.y >> lightSource.z;

			scene >> falloff;

			tempPointLight = new PointLight(lightSource, falloff);

			pointLights.push_back(tempPointLight);
		}


		// spot lights input
		int numberOfSpotLights;
		scene >> numberOfSpotLights;

		SpotLight *tempSpotLight;

		for ( int i = 0; i < numberOfSpotLights; i ++ ){
			// spot light parameters: lightSource, falloff, lightDirection, cutoffAngle, 

			Vector3D position, direction;
            double cutoffAngle, falloff;

			scene >> position.x >> position.y >> position.z;
			scene >> falloff;
			scene >> direction.x >> direction.y >> direction.z;
			scene >> cutoffAngle;

			tempPointLight = new PointLight(position, falloff);

			tempSpotLight = new SpotLight(*tempPointLight, direction, cutoffAngle);

			spotLights.push_back(tempSpotLight);
		}
		scene.close();		
	}
	else{
		cout << "Error opening file" << endl;
	}
}


void drawScene(){

	for(Object *object: objects){
		object->draw();
	}

	for(PointLight *pointLight: pointLights){
		pointLight->draw();
	}

	for(SpotLight *spotLight: spotLights){
		spotLight->draw();
	}
}



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// control viewing (or camera)
    gluLookAt(pos.x, pos.y, pos.z,                   // where is the eye/camerea
              pos.x + l.x, pos.y + l.y, pos.z + l.z, // where is the center ( look = center - eye )
              u.x, u.y, u.z);                        // what is the up direction


	// draw the scene
	drawAxes();
	drawScene();

	// again select MODEL-VIEW
	// glMatrixMode(GL_MODELVIEW);
	glutSwapBuffers();
}

void init(){
	cameraHeight=40.0;
	cameraAngle=1.32;

	pos = Vector3D(0, -160, 60);
	u = Vector3D(0.0, 0.0, 1.0);
	r = Vector3D(1, 0, 0.0);
	l = Vector3D(0, 1, 0.0);

	drawaxes = true;
	windowWidth = windowHeight = 900;
	viewAngle = 80.0;

	loadData();

	//clear the screen
	glClearColor(0,0,0,0);

	glMatrixMode(GL_PROJECTION);
	gluPerspective(fovY, aspectRatio, nearDistance, farDistance);
}

int main(int argc, char **argv){
    atexit(clear_Vectors);
	
	glutInit(&argc,argv);
	glutInitWindowSize(900, 900);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	
	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);


	glutMainLoop();		//The main loop of OpenGL

    return 0;
}

// ------------------------------ Capture ------------------------------//
vector<vector<Color>> frame_buffer = vector<vector<Color>>(numberOfPixels, vector<Color>(numberOfPixels, Color(0, 0, 0)));
int captureCount = 0;

void capture()
{
	bitmap_image *image;
	image = new bitmap_image(numberOfPixels, numberOfPixels);

	double planeDistance = ((double)windowHeight / 2.0) / tan(viewAngle / 2.0 * pi / 180.0);
	Vector3D topLeft = pos + l.multiply(planeDistance) - r.multiply((double)windowWidth / 2.0) + u.multiply((double)windowHeight / 2.0);

	double du = (double)windowWidth / (double)numberOfPixels;
	double dv = (double)windowHeight / (double)numberOfPixels;

	// choose middle of the grid cell
	topLeft = topLeft + r.multiply(du / 2.0) - u.multiply(dv / 2.0);

	cout << "Output_" << captureCount << ".bmp" << endl;
	// capture objects for each pixel
	for (int j = 0; j < numberOfPixels; j++)
	{
		for (int i = 0; i < numberOfPixels; i++)
		{
			Vector3D currentPixel;
			currentPixel = topLeft + r.multiply(du * j) - u.multiply(dv * i);
			Ray ray(pos, currentPixel - pos);
			ray.normalize();

			double tmin = inf;
			Color color(0, 0, 0);
			int idx = 0, objnum = -1;

			for (Object *obj : objects)
			{
				double t = obj->intersect(ray, color, 0);

				if (t >= 0.0 && (tmin - t) > eps)
				{
					tmin = t;
					objnum = idx;
				}
				idx++;
			}
			if (abs(tmin - inf) > eps)
			{
				objects[objnum]->intersect(ray, color, 1);
				image->set_pixel(j, i, 255 * color.r, 255 * color.g, 255 * color.b);
			}
		}
	}
	image->save_image("Output_" + to_string(captureCount) + ".bmp");
	delete image;

	cout << "------------------------------------------" << endl;
}

// ---------------- functions ---------------- //

void transformPyramid_to_triangles(Vector3D bottomLeftCorner, double width, double height, Color color, double ambient, double diffuse, double specular, double reflection, int shine){

	Triangle *tempTriangle;

	//front 
	Vector3D point1 = bottomLeftCorner;
	Vector3D point2 = bottomLeftCorner + Vector3D(width, 0, 0);
	Vector3D point3 = bottomLeftCorner + Vector3D(width / 2, width / 2, height);
	
	tempTriangle = new Triangle(point1, point2, point3);
	tempTriangle->setColor(color.r, color.g, color.b);
	tempTriangle->setCoefficients(ambient, diffuse, specular, reflection);
	tempTriangle->setShine(shine);

	objects.push_back(tempTriangle);

	//left
	point1 = bottomLeftCorner;
	point2 = bottomLeftCorner + Vector3D(0, width, 0);
	point3 = bottomLeftCorner + Vector3D(width / 2, width / 2, height);
	
	tempTriangle = new Triangle(point1, point2, point3);
	tempTriangle->setColor(color.r, color.g, color.b);
	tempTriangle->setCoefficients(ambient, diffuse, specular, reflection);
	tempTriangle->setShine(shine);

	objects.push_back(tempTriangle);

	//right
	point1 = bottomLeftCorner + Vector3D(width, 0, 0);
	point2 = bottomLeftCorner + Vector3D(width, width, 0);
	point3 = bottomLeftCorner + Vector3D(width / 2, width / 2, height);

	tempTriangle = new Triangle(point1, point2, point3);
	tempTriangle->setColor(color.r, color.g, color.b);
	tempTriangle->setCoefficients(ambient, diffuse, specular, reflection);
	tempTriangle->setShine(shine);

	objects.push_back(tempTriangle);

	//back
	point1 = bottomLeftCorner + Vector3D(0, width, 0);
	point2 = bottomLeftCorner + Vector3D(width, width, 0);
	point3 = bottomLeftCorner + Vector3D(width / 2, width / 2, height);

	tempTriangle = new Triangle(point1, point2, point3);
	tempTriangle->setColor(color.r, color.g, color.b);
	tempTriangle->setCoefficients(ambient, diffuse, specular, reflection);
	tempTriangle->setShine(shine);

	objects.push_back(tempTriangle);

	//base1
	point1 = bottomLeftCorner;
	point2 = bottomLeftCorner + Vector3D(width, 0, 0);
	point3 = bottomLeftCorner + Vector3D(width, width, 0);

	tempTriangle = new Triangle(point1, point2, point3);
	tempTriangle->setColor(color.r, color.g, color.b);
	tempTriangle->setCoefficients(ambient, diffuse, specular, reflection);
	tempTriangle->setShine(shine);

	objects.push_back(tempTriangle);

	//base2
	point1 = bottomLeftCorner;
	point2 = bottomLeftCorner + Vector3D(0, width, 0);
	point3 = bottomLeftCorner + Vector3D(width, width, 0);

	tempTriangle = new Triangle(point1, point2, point3);
	tempTriangle->setColor(color.r, color.g, color.b);
	tempTriangle->setCoefficients(ambient, diffuse, specular, reflection);
	tempTriangle->setShine(shine);
	
	objects.push_back(tempTriangle);
}


void transformCube_to_triangles(Vector3D bottomLeftCorner, double width, Color color, double ambient, double diffuse, double specular, double reflection, int shine){
	Triangle *tempTriangle;

	//base 1
	Vector3D point1 = bottomLeftCorner;
	Vector3D point2 = bottomLeftCorner + Vector3D(width, 0, 0);
	Vector3D point3 = bottomLeftCorner + Vector3D(width, width, 0);

	tempTriangle = new Triangle(point1, point2, point3);
	tempTriangle->setColor(color.r, color.g, color.b);
	tempTriangle->setCoefficients(ambient, diffuse, specular, reflection);
	tempTriangle->setShine(shine);

	objects.push_back(tempTriangle);

	//base2
	point1 = bottomLeftCorner;
	point2 = bottomLeftCorner + Vector3D(width, width, 0);
	point3 = bottomLeftCorner + Vector3D(0, width, 0);

	tempTriangle = new Triangle(point1, point2, point3);
	tempTriangle->setColor(color.r, color.g, color.b);
	tempTriangle->setCoefficients(ambient, diffuse, specular, reflection);
	tempTriangle->setShine(shine);

	objects.push_back(tempTriangle);

	//left1
	point1 = bottomLeftCorner;
	point2 = bottomLeftCorner + Vector3D(width, 0, 0);
	point3 = bottomLeftCorner + Vector3D(width, 0, width);

	tempTriangle = new Triangle(point1, point2, point3);
	tempTriangle->setColor(color.r, color.g, color.b);
	tempTriangle->setCoefficients(ambient, diffuse, specular, reflection);
	tempTriangle->setShine(shine);

	objects.push_back(tempTriangle);

	//left2
	point1 = bottomLeftCorner;
	point2 = bottomLeftCorner + Vector3D(width, 0, width);
	point3 = bottomLeftCorner + Vector3D(0, 0, width);

	tempTriangle = new Triangle(point1, point2, point3);
	tempTriangle->setColor(color.r, color.g, color.b);
	tempTriangle->setCoefficients(ambient, diffuse, specular, reflection);
	tempTriangle->setShine(shine);

	objects.push_back(tempTriangle);

	//back1
	point1 = bottomLeftCorner;
	point2 = bottomLeftCorner + Vector3D(0, width, 0);
	point3 = bottomLeftCorner + Vector3D(0, width, width);

	tempTriangle = new Triangle(point1, point2, point3);
	tempTriangle->setColor(color.r, color.g, color.b);
	tempTriangle->setCoefficients(ambient, diffuse, specular, reflection);
	tempTriangle->setShine(shine);

	objects.push_back(tempTriangle);

	//back2
	point1 = bottomLeftCorner;
	point2 = bottomLeftCorner + Vector3D(0, width, width);
	point3 = bottomLeftCorner + Vector3D(0, 0, width);

	tempTriangle = new Triangle(point1, point2, point3);
	tempTriangle->setColor(color.r, color.g, color.b);
	tempTriangle->setCoefficients(ambient, diffuse, specular, reflection);
	tempTriangle->setShine(shine);

	objects.push_back(tempTriangle);

	//front1
	point1 = bottomLeftCorner + Vector3D(width, 0, 0);
	point2 = bottomLeftCorner + Vector3D(width, width, 0);
	point3 = bottomLeftCorner + Vector3D(width, width, width);

	tempTriangle = new Triangle(point1, point2, point3);
	tempTriangle->setColor(color.r, color.g, color.b);
	tempTriangle->setCoefficients(ambient, diffuse, specular, reflection);
	tempTriangle->setShine(shine);

	objects.push_back(tempTriangle);

	//front2
	point1 = bottomLeftCorner + Vector3D(width, 0, 0);
	point2 = bottomLeftCorner + Vector3D(width, width, width);
	point3 = bottomLeftCorner + Vector3D(width, 0, width);

	tempTriangle = new Triangle(point1, point2, point3);
	tempTriangle->setColor(color.r, color.g, color.b);
	tempTriangle->setCoefficients(ambient, diffuse, specular, reflection);
	tempTriangle->setShine(shine);

	objects.push_back(tempTriangle);

	//right1
	point1 = bottomLeftCorner + Vector3D(0, width, 0);
	point2 = bottomLeftCorner + Vector3D(width, width, 0);
	point3 = bottomLeftCorner + Vector3D(width, width, width);

	tempTriangle = new Triangle(point1, point2, point3);
	tempTriangle->setColor(color.r, color.g, color.b);
	tempTriangle->setCoefficients(ambient, diffuse, specular, reflection);
	tempTriangle->setShine(shine);

	objects.push_back(tempTriangle);

	//right2
	point1 = bottomLeftCorner + Vector3D(0, width, 0);
	point2 = bottomLeftCorner + Vector3D(width, width, width);
	point3 = bottomLeftCorner + Vector3D(0, width, width);

	tempTriangle = new Triangle(point1, point2, point3);
	tempTriangle->setColor(color.r, color.g, color.b);
	tempTriangle->setCoefficients(ambient, diffuse, specular, reflection);
	tempTriangle->setShine(shine);

	objects.push_back(tempTriangle);

	//top1
	point1 = bottomLeftCorner + Vector3D(0, 0, width);
	point2 = bottomLeftCorner + Vector3D(width, 0, width);
	point3 = bottomLeftCorner + Vector3D(width, width, width);

	tempTriangle = new Triangle(point1, point2, point3);
	tempTriangle->setColor(color.r, color.g, color.b);
	tempTriangle->setCoefficients(ambient, diffuse, specular, reflection);
	tempTriangle->setShine(shine);

	objects.push_back(tempTriangle);

	//top2
	point1 = bottomLeftCorner + Vector3D(0, 0, width);
	point2 = bottomLeftCorner + Vector3D(width, width, width);
	point3 = bottomLeftCorner + Vector3D(0, width, width);

	tempTriangle = new Triangle(point1, point2, point3);
	tempTriangle->setColor(color.r, color.g, color.b);
	tempTriangle->setCoefficients(ambient, diffuse, specular, reflection);
	tempTriangle->setShine(shine);

	objects.push_back(tempTriangle);
}


void drawAxes()
{
	if(drawaxes)
	{
		glBegin(GL_LINES);{

			// x-axis in red
            glColor3f(1.0, 0.0, 0.0); // red
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			// y-axis in green
			glColor3f(0.0, 1.0, 0.0); // green
			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			// z-axis in blue
			glColor3f(0.0, 0.0, 1.0); // blue
			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}

void clear_Vectors()
{
    cout << "Objects cleared" << endl;
    for(int i = 0; i < objects.size(); i++)
    {
        delete[] objects[i];
    }
    for(int i = 0; i < pointLights.size(); i++)
    {
        delete[] pointLights[i];
    }
    for(int i = 0; i < spotLights.size(); i++)
    {
        delete[] spotLights[i];
    }
    
    objects.clear();
    pointLights.clear();
    spotLights.clear();
    return ;

}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:
            pos = pos - l;
			break;
		case GLUT_KEY_UP:
            pos = pos + l;
			break;

		case GLUT_KEY_RIGHT:
			pos = pos + r;
			break;
		case GLUT_KEY_LEFT:
            pos = pos - r;
			break;

		case GLUT_KEY_PAGE_UP:
            pos = pos + u;
			break;
		case GLUT_KEY_PAGE_DOWN:
            pos = pos - u;
            break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
            break;
		case GLUT_KEY_END:
            break;
		default:
			break;
	}
	glutPostRedisplay();
}

void keyboardListener(unsigned char key, int xx, int yy)
{
    double A = 1.0 * pi / 180.0;
    switch(key){
        case '0':
			captureCount++;
            capture();
			break;

		case '1':
            // u constant            
            l = l.multiply(cos(A)) + u.cross(l).multiply(sin(A));
            r = r.multiply(cos(A)) + u.cross(r).multiply(sin(A));
                       
            break;

        case '2':
            // u constant            
            l = l.multiply(cos(-A)) + u.cross(l).multiply(sin(-A));
            r = r.multiply(cos(-A)) + u.cross(r).multiply(sin(-A));

            break;

        case '3':
            // r constant            
            u = u.multiply(cos(A)) + r.cross(u).multiply(sin(A));
            l = l.multiply(cos(A)) + r.cross(l).multiply(sin(A));
            break;

        case '4':
            // r constant            
            u = u.multiply(cos(-A)) + r.cross(u).multiply(sin(-A));
            l = l.multiply(cos(-A)) + r.cross(l).multiply(sin(-A));
            break;


        case '5':
            // l constant            
            r = r.multiply(cos(A)) + l.cross(r).multiply(sin(A));
            u = u.multiply(cos(A)) + l.cross(u).multiply(sin(A));

            break;
        case '6':
            // l constant
            
            r = r.multiply(cos(-A)) + l.cross(r).multiply(sin(-A));
            u = u.multiply(cos(-A)) + l.cross(u).multiply(sin(-A));

            break;
		default:
			break;
	}

    glutPostRedisplay();
}


