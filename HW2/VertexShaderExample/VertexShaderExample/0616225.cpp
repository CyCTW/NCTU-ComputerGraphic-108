#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
#include "shader.h"
#include <iostream>
#include <string>
#include <cmath>

#define PI 3.14159265358

using namespace std;

int windowSize[2] = { 600, 600 };
void display();
void idle();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);

GLuint program;
GLuint vao;
GLuint vbo[2];
GLuint texture;

//Initialize the shaders 
void shaderInit();
void textureInit();

const int slice = 360;
const int stack = 180;
double day = 0;
const int X = 90;
//Number of vertices that will be sent to shader
int verticeNumber = slice * (stack + 1) * 2;


//Storing vertex datas that will be sent to shader
class VertexAttribute {
public:
	GLfloat position[3];
	GLfloat texcoord[2];
	void setPosition(float x, float y, float z) {
		position[0] = x;
		position[1] = y;
		position[2] = z;
	};
	void setTexcoord(float x, float y) {
		texcoord[0] = x;
		texcoord[1] = y;
	};
};

//Collecting vertex datas
VertexAttribute *drawSphere();

int main(int argc, char** argv) {	
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("ComputerGraphicsDemo");

	glewInit();
	shaderInit();
	textureInit();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

void shaderInit() {
	GLuint vert = createShader("Shaders/example.vert", "vertex");
	GLuint frag = createShader("Shaders/example.frag", "fragment");
	program = createProgram(vert, frag);

	//Copy vertex data to the buffer object
	VertexAttribute *vertices;
	vertices = drawSphere();

	//Generate VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//Generate two names of vbo
	glGenBuffers(2, vbo);

	//Generate first buffer object
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * verticeNumber, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Generate second buffer object
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * verticeNumber, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	
	glBindVertexArray(0);
}

void textureInit() {
	glEnable(GL_TEXTURE_2D);
	
	//Load the texture
	const char *pFilename = "earth_texture_map.jpg";
	FIBITMAP* pIimage = FreeImage_Load(FreeImage_GetFileType(pFilename, 0), pFilename, JPEG_DEFAULT);
	
	FIBITMAP* p32BitsImage = FreeImage_ConvertTo32Bits(pIimage);
	int iWidth = FreeImage_GetWidth(pIimage);
	int iHeight = FreeImage_GetHeight(pIimage);
	
	//Generate texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage) );
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	FreeImage_Unload(p32BitsImage);
	FreeImage_Unload(pIimage);

}

void display() {   
	// Clear the color buffer and the depth buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f,// eye
		0.0f, 0.0f, 0.0f,// center
		0.0f, 1.0f, 0.0f);// up

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)512 / (GLfloat)512, 1, 500);

	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);

	glMatrixMode(GL_MODELVIEW);
	glRotatef(day, 0.0f, 1.0f, 0.0f);

	GLfloat pmtx[16];
	GLfloat mmtx[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
	//Get the location of uniform variable in vertex shader
 	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");
	
	//Get the location of uniform variable in fragment shader
	GLuint texLoc = glGetUniformLocation(program, "ourTexture");
	
	//Install the program object as part of current rendering state
	glUseProgram(program);

	//Active, bind and set up the value of uniform variable in fragment shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texLoc, 0);

	//input the modelview matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	//input the rotation matrix into vertex shader
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);
	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, verticeNumber);
	glBindVertexArray(0);
	//Unbind the texture
	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(0);
	glPopMatrix();
	glutSwapBuffers();
}

VertexAttribute *drawSphere() {
	double x, y, z;
	double a, b;
	double slice_step = 2 * PI / slice, stack_step = PI / stack;

	VertexAttribute *vertices;
	vertices = new VertexAttribute[ verticeNumber ];
	int idx = 0;
	for (int i = 0; i < slice; i++) {
		
		for (int j = 0; j < stack + 1; j++) {
			a = 1 - (i / double(slice));
			b = 1 - (j / double(stack));

			x = sin(j * stack_step) * cos(i*slice_step);
			y = cos(j * stack_step);
			z = sin(j * stack_step) * sin(i*slice_step);
			
			vertices[idx].setPosition(x, y, z);
			vertices[idx++].setTexcoord(a, b);
			x = sin(j * stack_step) * cos((i + 1)*slice_step);
			y = cos(j * stack_step);
			z = sin(j * stack_step) * sin((i + 1)*slice_step);
			
			vertices[idx].setPosition(x, y, z);
			vertices[idx++].setTexcoord(a, b);

		}	
	}
	return vertices;
}
void reshape(GLsizei w, GLsizei h) {
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y) {

}

void idle() {
	day += (X / 365.0);
	if (day >= 360) day = 0;
	glutPostRedisplay();
}