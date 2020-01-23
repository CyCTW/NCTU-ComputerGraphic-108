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
GLuint vbo;
GLuint texture;

//Initialize the shaders 
void shaderInit();
void textureInit();
void TextureInit();
bool showBoard = false;
int switchmodel = 0;
const int slice = 20;
const int stack = 10;
double day = 0;
const int X = 90;
//Number of vertices that will be sent to shader
int verticeNumber = slice * (stack + 1) * 2;

float  Ks = 0, Kd = 0;
float shininess = 3.6;
//Storing vertex datas that will be sent to shader
class VertexAttribute {
public:
	GLfloat position[3];
	GLfloat normalvec[3];
	GLfloat texcoords[2];
	void setPosition(float x, float y, float z) {
		position[0] = x;
		position[1] = y;
		position[2] = z;
	};
	void setNormalVec(float x, float y, float z) {
		normalvec[0] = x;
		normalvec[1] = y;
		normalvec[2] = z;
	};
	void settexcoords(float x, float y) {
		texcoords[0] = x;
		texcoords[1] = y;
	};
};

//Collecting vertex datas
VertexAttribute *drawSphere();
void drawBorder();
GLuint vert, frag;
GLuint FBO, rboDepth;

int main(int argc, char** argv) {	
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("ComputerGraphicsDemo");

	glewInit();
	
	vert = createShader("Shaders/gouraud.vert", "vertex");
	frag = createShader("Shaders/gouraud.frag", "fragment");
	shaderInit();
	//TextureInit();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

void shaderInit() {
	program = createProgram(vert, frag);

	//Copy vertex data to the buffer object
	VertexAttribute *vertices;
	vertices = drawSphere();

	//Generate VAO
	
	/*glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);*/

	//Generate two names of vbo
	glGenBuffers(1, &vbo);

	//Generate first buffer object
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * verticeNumber, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));

	//Generate second buffer object
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normalvec)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoords)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//glBindVertexArray(0);
}
void TextureInit() {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowSize[0], windowSize[1], 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowSize[0], windowSize[1]);
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
	//glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 5.6f,// eye
		0.0f, 0.0f, 0.0f,// center
		0.0f, 1.0f, 0.0f);// up
	//set light position
	glTranslatef(1.1f, 1.0f, 1.3f);
	
	GLUquadric* quad = gluNewQuadric();
	glColor3f(0.4, 0.5, 0.0);
	gluSphere(quad, 0.05, 10, 5);

	glTranslatef(-1.1f, -1.0f, -1.3f);
	
	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)512 / (GLfloat)512, 1, 500);

	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);

	glMatrixMode(GL_MODELVIEW);
	if (showBoard)
		drawBorder();
	//glRotatef(day, 0.0f, 1.0f, 0.0f);

	GLfloat lightvec[3] = { 1.1, 1.0, 1.3 };

	GLint lightLoc = glGetUniformLocation(program, "LightPos");
	GLint KaLoc = glGetUniformLocation(program, "M.Ka");
	GLint KdLoc = glGetUniformLocation(program, "M.Kd");
	GLint KsLoc = glGetUniformLocation(program, "M.Ks");
	GLint specLoc = glGetUniformLocation(program, "M.Shininess");

	GLint LaLoc = glGetUniformLocation(program, "L.La");
	GLint LdLoc = glGetUniformLocation(program, "L.Ld");
	GLint LsLoc = glGetUniformLocation(program, "L.Ls");
	
	GLfloat pmtx[16];
	GLfloat mmtx[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
	//Get the location of uniform variable in vertex shader
 	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");

	//Get the location of uniform variable in fragment shader
	
	//Install the program object as part of current rendering state
	glUseProgram(program);

	

	glUniform3fv(lightLoc, 1, lightvec);
	glUniform1f(KaLoc, 0.5f);
	glUniform1f(KsLoc, Ks);
	glUniform1f(KdLoc, Kd);
	glUniform1f(specLoc, 3.6f);
	glUniform3f(LaLoc, 0.2f, 0.2f, 0.2f);
	glUniform3f(LdLoc, 0.5f, 0.5f, 0.5f);
	glUniform3f(LsLoc, 0.8f, 0.8f, 0.8f);
	//Active, bind and set up the value of uniform variable in fragment shader
	GLuint texLoc = glGetUniformLocation(program, "ourTexture");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texLoc, 0);
	//glActiveTexture(GL_TEXTURE0);
	
	//glBindTexture(GL_TEXTURE_2D, texture);
	//glUniform1i(texLoc, 0);
	
	//input the modelview matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	//input the rotation matrix into vertex shader
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);

	//glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, verticeNumber);
	//glBindVertexArray(0);
	//Unbind the texture
	glBindTexture(GL_TEXTURE_2D, NULL);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
			vertices[idx].setNormalVec(x, y, z);
			vertices[idx++].settexcoords(a, b);
			x = sin(j * stack_step) * cos((i + 1)*slice_step);
			y = cos(j * stack_step);
			z = sin(j * stack_step) * sin((i + 1)*slice_step);
			
			vertices[idx].setPosition(x, y, z);
			vertices[idx].setNormalVec(x, y, z);
			vertices[idx++].settexcoords(a, b);

		}	
	}
	return vertices;
}
void drawBorder() {
	glBegin(GL_LINE_LOOP);
	glColor3f(0.5, 0.5, 0.5);
	double x, y;
	for (int i = 0; i <= 3000; i++) {
		float theta = 2.0 * PI * (i / 3000.0);
		x = 1.03*cosf(theta) ;
		y = 1.03*sinf(theta) ;
		glVertex3f(x, y, 0);
		glVertex3f(x + 0.005, y + 0.005, 0);
		glVertex3f(x - 0.005, y - 0.005, 0);

	}

	glEnd();
}

void reshape(GLsizei w, GLsizei h) {
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y) {
	if (key == '1' && ((Ks - 0.1) >= -0.00001)) Ks -= 0.1;
	if (key == '2' && ((Ks + 0.1) <= 1.000001)) Ks += 0.1;
	if (key == '3' && ((Kd - 0.1) >= -0.00001)) Kd -= 0.1;
	if (key == '4' && ((Kd + 0.1) <= 1.000001)) Kd += 0.1;

	if (key == 'B') {
		switchmodel = (switchmodel + 1) % 3;
		if (switchmodel==0) {
			vert = createShader("Shaders/gouraud.vert", "vertex");
			frag = createShader("Shaders/gouraud.frag", "fragment");
			shaderInit();
		}
		else if (switchmodel==1){
			vert = createShader("Shaders/phong.vert", "vertex");
			frag = createShader("Shaders/phong.frag", "fragment");
			shaderInit();
		}
		else if (switchmodel == 2) {
			vert = createShader("Shaders/tone.vert", "vertex");
			frag = createShader("Shaders/tone.frag", "fragment");
			shaderInit();
		}
		
	}
	if (key == 'E') {
		showBoard = !showBoard;
	}
}

void idle() {
	day += (X / 365.0);
	if (day >= 360) day = 0;
	glutPostRedisplay();
}