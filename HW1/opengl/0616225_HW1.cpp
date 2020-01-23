//Your Student ID is 0616225. (Modify it)
#include "glut.h"
#include <iostream>
#include <windows.h>

#include <cmath>
using namespace std;
const float PI = acos(-1);

int width = 400, height = 400;

double day = 0;
int X = 360;//degree
GLfloat Y = 1;//radius
static int pause = 0;
static bool switch_Earth = 0;
void display();
class Point {
	public:
		Point(){};
		Point(double a, double b, double c): x(a), y(b), z(c) {};
	public:
		double x;
		double y;
		double z;
};
Point getPoint(double u, double v, char type) {
	int radius = 0;
	switch (type) {
		case 'S':
			radius = 7 * Y;break;
		case 'E':
			radius = 2 * Y;break;
		case 'M':
			radius = Y; break;
	}
	double x = radius * sin( PI * v) * cos( 2 * PI * u);
	double y = radius * sin( PI * v) * sin( 2 * PI * u);
	double z = radius * cos(PI * v);
	return Point(x, y, z);
}
double getSlice(char type) {
	switch (type) {
		case 'S':
		case 'M':
			return 240.0; break;
		case 'E':
			if (switch_Earth)
				return 4.0;
			else
				return 360;
			break;
	}
	return 0;

}
double getStack(char type) {
	switch (type) {
	case 'S':
	case 'M':
		return 60.0; break;
	case 'E':
		if (switch_Earth)
			return 2.0;
		else
			return 180.0;
		break;
	}
	return 0;
}

void drawSphere(char type){
	
	double uStepsNum = getSlice(type);
	double vStepsNum = getStack(type);

	double ustep = 1 / uStepsNum;
	double vstep = 1 / vStepsNum;
	double u = 0,v = 0;
	glEnable(GL_NORMALIZE);

    for(int i = 0;i<uStepsNum;i++)
    {
        glBegin(GL_TRIANGLES);
        Point a = getPoint(0,0, type);
        Point b = getPoint(u,vstep, type);
        Point c = getPoint(u+ustep,vstep, type);

		glNormal3f(a.x, a.y, a.z);
		glVertex3d(a.x, a.y, a.z);
		glNormal3f(b.x, b.y, b.z);
		glVertex3d(b.x, b.y, b.z);
		glNormal3f(c.z, c.y, c.z);
		glVertex3d(c.x, c.y, c.z);
        u += ustep;
        glEnd();
    }
    
    u = 0, v = vstep;
    for(int i=1;i<vStepsNum-1;i++)
    {
        for(int j=0;j<uStepsNum;j++)
        {
            glBegin(GL_QUADS);

            Point a = getPoint(u,v, type);
            Point b = getPoint(u + ustep,v, type);
            Point c = getPoint(u + ustep,v + vstep, type);
            Point d = getPoint(u,v + vstep, type);

			glNormal3f(a.x, a.y, a.z);
			glVertex3d(a.x,a.y,a.z);
			glNormal3f(b.x, b.y, b.z);
			glVertex3d(b.x,b.y,b.z);
			glNormal3f(c.x, c.y, c.z);
			glVertex3d(c.x,c.y,c.z);
			glNormal3f(d.x, d.y, d.z);

			glVertex3d(d.x,d.y,d.z);
            
			u += ustep;
            glEnd();
        }
		u = 0;
        v += vstep;
    }

    u = 0;
    for(int i=0;i<uStepsNum;i++)
    {
        glBegin(GL_TRIANGLES);
        Point a = getPoint(0,1, type);
        Point b = getPoint(u,1-vstep, type);
        Point c = getPoint(u+ustep,1-vstep, type);

		glNormal3f(a.x, a.y, a.z);

        glVertex3d(a.x,a.y,a.z);
		glNormal3f(b.x, b.y, b.z);
        glVertex3d(b.x,b.y,b.z);
		glNormal3f(c.z, c.y, c.z);
        glVertex3d(c.x,c.y,c.z);
		u += ustep;
        glEnd();
    }
}
void drawcircle(float r) {
	glBegin(GL_LINE_LOOP);
	const int seg = 300;
	for (int i = 0; i < seg; i++) {
		float theta = 2 * PI * double(i) / double(seg);
		float x = r * cosf(theta);
		float y = r * sinf(theta);
		glVertex3f(x, 0.0, y);
	}
	glEnd();
}
void lighting()
{
	// enable lighting
	glEnable(GL_LIGHTING);
	//Add directed light
	GLfloat diffuseColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat ambientColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat position[] = { 0.0f, 10.0f, 0.0f, 1.0f };

	glEnable(GL_LIGHT0);								//open light0
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);	    //set diffuse color of light0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);	    //set ambient color of light0
	glLightfv(GL_LIGHT0, GL_POSITION, position);		//set position of light0
}

void timeproc(int id) {
	glutPostRedisplay();
	glutTimerFunc(33, timeproc, 1);

}
void idle() {
	if (!pause)	day += (X/365.0);

	if (day >= 360) day = 0;

	glutPostRedisplay();

}
void reshape(int w, int h) {
	width = w;
	height = h;	
}
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 'P':
			pause = !pause;
			break;
		case 'O':
			switch_Earth = !switch_Earth;
			break;
	}
}
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("HW1");
	lighting();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
void display() {
	//ModelView Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(0.0f, 30.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	//Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45, width / (GLfloat)height, 0.1, 100);
	
	//Viewport Matrix
	glViewport(0, 0, width, height);

	glMatrixMode(GL_MODELVIEW);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_DEPTH_BUFFER_BIT);


	GLfloat mat_ambient[] = { 0.174500, 0.011750, 0.011750, 0.550000 };
	GLfloat mat_diffuse[] = { 0.614240, 0.041360, 0.041360, 0.550000 };
	GLfloat mat_specular[] = { 0.727811, 0.626959, 0.626959, 0.550000 };
	GLfloat mat_shininess[] = { 76.800003 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	//drawcircle(18.0);
	
	drawSphere(('S'));
	
	//glutSolidSphere(5, 30, 30);
	GLfloat cir[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, cir);

	glRotatef(day , 0.0f, 1.0f, 0.0f); //revolution
	glTranslatef(18.0f, 0.0f, 0.0f);
	glRotatef(23.5, sinf(day / 360.0*(2*PI)), 0.0, -cosf(day / 360.0 * (2*PI)));//Obliquity
	glPushMatrix(); //push before earth rotate

	glRotatef(day * 360.0, 0.0f, 1.0f, 0.0f); //rotation

	GLfloat cir2[] = { 0.0, 0.0, 0.5, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, cir2);
	//drawcircle(3.0);

	GLfloat amat_ambient[] = { 0, 0.1, 0.9, 1.0};
	GLfloat amat_diffuse[] = { 0.075680, 0.614240, 0.075680, 0.550000 };
	GLfloat amat_specular[] = { 0.133000, 0.127811, 0.433000, 0.150000};
	GLfloat amat_shininess[] = { 10.800003 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, amat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, amat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, amat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, amat_shininess);
	
	drawSphere(('E'));
	
	//glPushMatrix();

	glRotatef(90.0, -1.0, 0.0, 0.0);//rotate since cylinder grow up along zaxis
	glTranslatef(0.0, 0.0, -4.0);

	GLfloat cmat_ambient[] = { 0.247250, 0.199500, 0.074500, 1.000000 };
	GLfloat cmat_diffuse[] = { 0.751640, 0.606480, 0.226480, 1.000000 };
	GLfloat cmat_specular[] = { 0.628281, 0.555802, 0.366065, 1.000000 };
	GLfloat cmat_shininess[] = { 51.200001 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, cmat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, cmat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, cmat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, cmat_shininess);

	GLUquadricObj *quad = gluNewQuadric();
	gluCylinder(quad, 0.5, 0.5, 8*Y, 30.0, 30.0);

	glPopMatrix();
	
	glRotatef(day * 360.0 / 28.0, 0.0f, 1.0f, 0.0f); //resolution
	glTranslatef(3.0f, 0.0f, 0.0f);
	glRotatef(day * 360.0 / 28.0, 0.0f, 1.0f, 0.0f); //rotation

	GLfloat mmat_ambient[] = { 0.231250, 0.231250, 0.231250, 1.000000 };
	GLfloat mmat_diffuse[] = { 0.277500, 0.277500, 0.277500, 1.000000 };
	GLfloat mmat_specular[] = { 0.773911, 0.773911, 0.773911, 1.000000 };
	GLfloat mmat_shininess[] = { 89.599998 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mmat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mmat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mmat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mmat_shininess);

	drawSphere('M');

	glutSwapBuffers();
}