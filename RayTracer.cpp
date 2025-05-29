/*==================================================================================
* COSC 363  Computer Graphics
* Department of Computer Science and Software Engineering, University of Canterbury.
*
* A basic ray tracer
* See Lab07.pdf   for details.
*===================================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "Cylinder.h"
#include "TruncatedCone.h"
#include "Torus.h"
#include "SceneObject.h"
#include "Ray.h"
#include "Plane.h"
#include "TextureBMP.h"
#include <GL/freeglut.h>
using namespace std;

const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const int SAMPLES_PER_PIXEL = 4;
const float XMIN = -10.0;
const float XMAX = 10.0;
const float YMIN = -10.0;
const float YMAX = 10.0;
bool enableAA = true;

vector<SceneObject*> sceneObjects;
TextureBMP texture;

//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step) {
	glm::vec3 backgroundCol(0);						//Background colour = (0,0,0)
	glm::vec3 lightPos(10, 15, -3);					//Light's position
	glm::vec3 color(0);
	SceneObject* obj;
	SceneObject* shadowObj;

	ray.closestPt(sceneObjects);					//Compare the ray with all objects in the scene
	if(ray.index == -1) return backgroundCol;		//no intersection
	obj = sceneObjects[ray.index];					//object on which the closest point of intersection is found

	if (ray.index == 6)
	{
		// size of each square
		int stripeWidth = 5;

		// compute which “cell” we’re in on X and Z
		int ix = static_cast<int>(floor(ray.hit.x / stripeWidth));
		int iz = static_cast<int>(floor(ray.hit.z / stripeWidth));

		if ( (ix + iz) % 2 == 0 ) {
			color = glm::vec3(0, 1, 0);        
		} else {
			color = glm::vec3(1, 1, 0.5);     
		}
		obj->setColor(color);
	}

	if (ray.index == 0)
	{
		// get the unit normal on the sphere
        glm::vec3 N = obj->normal(ray.hit);

        // compute spherical UVs
        float u = 0.5f + atan2(N.z, N.x) / (2.0f * M_PI);
        float v = 0.5f - asin (N.y)    / M_PI;

        // sample your BMP loader
        glm::vec3 texCol = texture.getColorAt(u, v);

        // push it into the material color
        obj->setColor(texCol);
	}

	color = obj->lighting(lightPos, -ray.dir, ray.hit);						//Object's colour
	glm::vec3 lightVec = lightPos - ray.hit;

	if (obj->isRefractive() && step < MAX_STEPS)
	{
		float kr      = obj->getRefractionCoeff();
		float eta     = obj->getRefractiveIndex();
		glm::vec3 normal   = obj->normal(ray.hit);

		float n1 = 1.0f, n2 = eta;
		float etaRatio = n1 / n2;

		glm::vec3 refrDir = glm::refract(ray.dir, normal, etaRatio);
		refrDir = glm::normalize(refrDir);

		Ray throughRay(ray.hit, refrDir);
		throughRay.closestPt(sceneObjects);

		glm::vec3 exitPt = throughRay.hit;
		SceneObject* exitObj = sceneObjects[throughRay.index];
		glm::vec3 Nexit = exitObj->normal(exitPt);
		if (glm::dot(refrDir, Nexit) > 0.0f) {
			Nexit = -Nexit;
		}
		
		float etaRatioExit = n2 / n1;
		glm::vec3 refrDirExit = glm::refract(refrDir, Nexit, etaRatioExit);
		refrDirExit = glm::normalize(refrDirExit);

		Ray exitRay(exitPt, refrDirExit);
		exitRay.closestPt(sceneObjects);
		glm::vec3 refrColor = trace(exitRay, step + 1);

		color = color + kr * refrColor;
	}


	if (obj->isTransparent() && step < MAX_STEPS)
	{
		float rho = obj->getTransparencyCoeff();
		Ray throughRay(ray.hit, ray.dir);
		throughRay.closestPt(sceneObjects);
		Ray exitRay(throughRay.hit, throughRay.dir);
		glm::vec3 transparentColor = trace(exitRay, step + 1);
		color = color + (rho * transparentColor);
	}

	Ray shadowRay(ray.hit, lightVec); 
	shadowRay.closestPt(sceneObjects);
	if (shadowRay.index > -1) {
		shadowObj = sceneObjects[shadowRay.index];
		if (shadowObj->isTransparent() || shadowObj->isRefractive()) {
			glm::vec3 ambient = 0.2f * shadowObj->getColor();
			glm::vec3 diffSpec = color - ambient;
			color = ambient + 0.8f*diffSpec;
		} else {
			color = 0.2f * obj->getColor();
		}
	}

	if (obj->isReflective() && step < MAX_STEPS)
	{
		float rho = obj->getReflectionCoeff();
		glm::vec3 normalVec = obj->normal(ray.hit);
		glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
		Ray reflectedRay(ray.hit, reflectedDir);
		glm::vec3 reflectedColor = trace(reflectedRay, step + 1);
		color = color + (rho * reflectedColor);		
	}

	return color;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
// at top of your .cpp, to toggle anti-aliasing:

void display() {
    float xp, yp;
    float cellX = (XMAX - XMIN) / NUMDIV;
    float cellY = (YMAX - YMIN) / NUMDIV;
    glm::vec3 eye(0., 0., 0.);

    const int SPP = SAMPLES_PER_PIXEL;  // e.g. 4
    const int n   = (int)std::sqrt(SPP); // 2 if SPP==4

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glBegin(GL_QUADS);

    for (int i = 0; i < NUMDIV; ++i) {
        xp = XMIN + i * cellX;
        for (int j = 0; j < NUMDIV; ++j) {
            yp = YMIN + j * cellY;

            glm::vec3 color(0.0f);

            if (enableAA) {
                glm::vec3 accum(0.0f);
                for (int sx = 0; sx < n; ++sx) {
                    for (int sy = 0; sy < n; ++sy) {
                        float u = (sx + 0.5f) / float(n);
                        float v = (sy + 0.5f) / float(n);

                        glm::vec3 dir(
                            xp + u * cellX,
                            yp + v * cellY,
                            -EDIST
                        );
                        accum += trace(Ray(eye, dir), 1);
                    }
                }
                color = accum / 4.0f;
            }
            else {
                glm::vec3 dir(
                    xp + 0.5f * cellX,
                    yp + 0.5f * cellY,
                    -EDIST
                );
                color = trace(Ray(eye, dir), 1);
            }

            glColor3f(color.r, color.g, color.b);
            glVertex2f(xp,          yp);
            glVertex2f(xp + cellX,  yp);
            glVertex2f(xp + cellX,  yp + cellY);
            glVertex2f(xp,          yp + cellY);
        }
    }

    glEnd();
    glFlush();
}


//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL 2D orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize() {
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

	glClearColor(0, 0, 0, 1);

	texture = TextureBMP("../Mars.bmp");


	Sphere *sphere1 = new Sphere(glm::vec3(-7.0, -3.0, -70.0), 3.0);
	sphere1->setColor(glm::vec3(0, 0, 1));
	sceneObjects.push_back(sphere1);

	Sphere *sphere2 = new Sphere(glm::vec3( 0.0, -3.0, -70.0), 3.0);
	sphere2->setColor(glm::vec3(0.3, 0.3, 0.3));
	sphere2->setReflectivity(true, 0.05);
	sphere2->setTransparency(true, 0.9);
	sceneObjects.push_back(sphere2);

	Sphere *sphere3 = new Sphere(glm::vec3( 7.0, -10.0, -70.0), 3.0);
	sphere3->setColor(glm::vec3(0.1, 0.1, 0.1));  
	sphere3->setReflectivity(true, 0.2);
	sphere3->setRefractivity(true, 0.9, 1.5);
	sceneObjects.push_back(sphere3);

	Cylinder *cylinder = new Cylinder(glm::vec3(-7.0, -10, -70.0), 2.0, 5.0);
	cylinder->setColor(glm::vec3(0.2, 0.2, 0.2));
	sceneObjects.push_back(cylinder);

	TruncatedCone *cone = new TruncatedCone(glm::vec3(0.0, -10, -70.0), 2.5, 1.0, 5);
	cone->setColor(glm::vec3(0.75, 0.3, 0.75));
	sceneObjects.push_back(cone);

	Torus *torus = new Torus(glm::vec3(7.0, -3.0, -70.0), 2.0, 1.0);
	torus->setColor(glm::vec3(1, 0, 0));
	sceneObjects.push_back(torus);

	Plane *floor = new Plane (glm::vec3(-20., -15, -40), glm::vec3(20., -15, -40), glm::vec3(20., -15, -200), glm::vec3(-20., -15, -200));
	floor->setColor(glm::vec3(0.8, 0.8, 0));
	floor->setSpecularity(false);
	sceneObjects.push_back(floor);

	Plane *lWall = new Plane (glm::vec3(-20., -15, -40), glm::vec3(-20., -15, -200), glm::vec3(-20., 15, -200), glm::vec3(-20., 15, -40)); 
	lWall->setColor(glm::vec3(1.0, 0, 0));
	lWall->setSpecularity(false);
	sceneObjects.push_back(lWall);

	Plane *rWall = new Plane (glm::vec3(20., -15, -40), glm::vec3(20., -15, -200), glm::vec3(20., 15, -200), glm::vec3(20., 15, -40)); 
	rWall->setColor(glm::vec3(0, 1.0, 1.0));
	rWall->setSpecularity(false);
	sceneObjects.push_back(rWall);

	Plane *bWall = new Plane (glm::vec3(-20., -15, -200), glm::vec3(20., -15, -200), glm::vec3(20., 15, -200), glm::vec3(-20., 15, -200)); 
	bWall->setSpecularity(false);
	bWall->setColor(glm::vec3(0.173, 0.357, 0.369));
	sceneObjects.push_back(bWall);

	Plane *roof = new Plane (glm::vec3(-20., 15, -40), glm::vec3(20., 15, -40), glm::vec3(20., 15, -200), glm::vec3(-20., 15, -200));
	roof->setColor(glm::vec3(1.0, 0, 1.0));
	roof->setSpecularity(false);
	sceneObjects.push_back(roof);

	Plane *mirror = new Plane (glm::vec3(-10., 1, -84), glm::vec3(10., 1, -84), glm::vec3(10., 10, -80), glm::vec3(-10., 10, -80)); 
	mirror->setSpecularity(false);
	mirror->setReflectivity(true, 0.8);
	mirror->setColor(glm::vec3(0.1, 0.1, 0.1));
	sceneObjects.push_back(mirror);
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(3200, 20);
	glutCreateWindow("Raytracing");

	glutDisplayFunc(display);
	initialize();

	glutMainLoop();
	return 0;
}
