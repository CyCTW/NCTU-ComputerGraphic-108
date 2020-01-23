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
void TextureInit();
bool showBoard = false;
int switchmodel = 0;
const int slice = 500;
const int stack = 400;
double day = 0;
const int X = 90;
const int Particlenum = 10000;
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

struct ParticleAttribute {
	GLfloat position[3] = {};
	GLfloat speed[3];
	GLfloat fade;
	GLfloat life;
	GLfloat color[3] = {};
	void setPosition(float x, float y, float z) {
		position[0] = x;
		position[1] = y;
		position[2] = z;
	};
	void setColor(float x, float y, float z) {
		color[0] = x;
		color[1] = y;
		color[2] = z;
	}
};
ParticleAttribute particles[Particlenum];

void initParticlePosition() {
	for (int i = 0; i < Particlenum; i++) {
		particles[i].position[0] = ((rand() % 10000)-5000) / 1000.0;
		particles[i].position[1] = ((rand() % 10000)-5000) / 1000.0;
		particles[i].position[2] = ((rand() % 10000)-5000) / 1000.0;

		float stepi = 2 * PI / 180;
		int randomi = rand() % 180;
		float stepj = 2 * PI / 180;
		int randomj = rand() % 180;
		particles[i].speed[0] = sin(randomj * stepj) * cos(randomi * stepi) * 100.0;
		particles[i].speed[1] = cos(randomj * stepj) * 100.0;
		particles[i].speed[2] = sin(randomj * stepj) * cos(randomi * stepi) * 100.0;

		particles[i].setColor( (rand()%100 / 100.0), (rand() % 100 / 100.0), (rand() % 100 / 100.0));
		//particles[i].setColor(1.0, 0.0, 0.0);
		/*
		float stepi = 2 * PI / 180;
		int randomi = rand() % 180;
		float stepj = 2 * PI / 180;
		int randomj = rand() % 180;
		particles[i].speed[0] = sin(randomj * stepj) * cos(randomi * stepi) * 100.0;
		particles[i].speed[1] = cos(randomj * stepj) * 100.0;
		particles[i].speed[2] = sin(randomj * stepj) * cos(randomi * stepi) * 100.0;
		if (i < 5000) {
			particles[i].speed[0] = sin(randomj * stepj) * cos(randomi * stepi) * 500.0;
		}
		particles[i].life = 2.0f;
		particles[i].fade = GLfloat(rand() % 100) / 1000.0f + 0.003f;
		*/
	}
}
//Collecting vertex datas
VertexAttribute *drawSphere();
VertexAttribute* drawSquare();

ParticleAttribute* drawRing();


GLuint vert, frag;
GLuint FBO, rboDepth;

int main(int argc, char** argv) {	
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("ComputerGraphicsDemo");

	glewInit();
	
	vert = createShader("Shaders/test.vert", "vertex");
	frag = createShader("Shaders/test.frag", "fragment");
	initParticlePosition();
	shaderInit();
	TextureInit();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
GLuint vao1, vbo1, vvbo, vboring, vbosq, vbosphere, vbotext;

void shaderInit() {
	program = createProgram(vert, frag);

	//Copy vertex data to the buffer object
	VertexAttribute *vertices;
	//draw sphere
	vertices = drawSphere();
	glGenBuffers(1, &vbosphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbosphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * verticeNumber, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//draw square to texture
	vertices = drawSquare();
	glGenBuffers(1, &vbotext);
	glBindBuffer(GL_ARRAY_BUFFER, vbotext);

	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * 4, vertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Generate VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	static const GLfloat g_vertex_buffer_data[] = {
		 -0.01f, -0.01f, 0.0f,
		  0.01f, -0.01f, 0.0f,
		 -0.01f,  0.01f, 0.0f,
		  0.01f,  0.01f, 0.0f,
	};
	glGenBuffers(1, &vbo1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//Generate two names of vbo
	glGenBuffers(1, &vbo);

	//Generate first buffer object
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * Particlenum, particles, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, life)));
	
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, color)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoords)));
	
	//Gen ring buffers
	glGenBuffers(1, &vboring);
	ParticleAttribute* points;
	points = drawRing();
	glBindBuffer(GL_ARRAY_BUFFER, vboring);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 722, points, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//Gen square buffer
	glGenBuffers(1, &vbosq);
	ParticleAttribute* sqpoints = new ParticleAttribute[10];
	sqpoints[0].setPosition(-1009.0, -1009.0, 1.5);
	sqpoints[1].setPosition(-1009.0, 1009.0, 1.5);
	sqpoints[2].setPosition(1009.0, -1009.0, 1.5);
	sqpoints[3].setPosition(1009.0, 1009.0, 1.5);
	glBindBuffer(GL_ARRAY_BUFFER, vbosq);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 4, sqpoints, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


}
double timee = 10.0f;
double turnleft = -0.001;
double turnright = 10.0;
bool switchdir = false;
bool _texture_ = false;

bool stopdir = false;

bool burst = false;
bool _fade_ = false;
bool _defade_ = false;
double back = 5.0;
double shake = 0.0;
double transparent = 0.0;
double ring_dis = 2.0;
double nearing = 4.0f;
double ee = 1.0;

bool _swing_ = false;
bool _absorb_ = false;
bool _burst_ = false;
bool _turnright_ = false;
bool _back_ = false;
bool shakee = false;
bool bburst = false;

void absorb();
void fading();


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

	//control initially swing head
	if (turnleft < -5.0) {
		//switchdir = true;
		turnleft = -5.0;
	}
	else if (turnleft > 5.0) {
		//switchdir = false;
		turnleft = 5.0;
	}
	else if (turnleft >= 0 && turnleft <= 0.01 && !switchdir) {
		turnleft = 0.0;
		stopdir = true;
	}

	if (switchdir && !stopdir)
		turnleft += 0.01;
	else if (!stopdir) 
		turnleft -= 0.01;

	/*float mov = 0.0;
	if (burst)
		mov = 1.0;*/
	if (stopdir && !burst)
		absorb();

	if (_defade_) {
		if (_turnright_) {

			if (turnright <= 0.0)
				turnright = 0.0;
			else
				turnright -= 0.01;
		}
		if (!_back_) {
			gluLookAt(0.0f, 0.0, 5.0f,// eye
				0.0f, turnright, 0.0f,// center
				1.0f, 0.0, 0.0f);// up
		}
		else {
			back += 0.1;			
			gluLookAt(0.0f, 0.0, back,// eye
				0.0f, turnright, 0.0f,// center
				1.0f, 0.0, 0.0f);// up
		}
	}
	else if (stopdir && !burst) {
		nearing += 0.01;
		gluLookAt(0.0f, 0.0, nearing,// eye
			0.0f, turnleft, 0.0f,// center
			1.0f, 0.0, 0.0f);// up
	}
	else if (burst && !_fade_ ) {
		static const GLfloat g_vertex_buffer_data[] = {
		 -0.05f, -0.05f, 0.0f,
		  0.05f, -0.05f, 0.0f,
		 -0.05f,  0.05f, 0.0f,
		  0.05f,  0.05f, 0.0f,
		};
		glBindBuffer(GL_ARRAY_BUFFER, vbo1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
		
		if (shakee) 
			shake = 0.3;
		else 
			shake = -0.3;
		shakee = !shakee;
		gluLookAt(0.5f, 0.0, nearing,// eye
			0.0f, shake, 0.0f,// center
			1.0f, 0.0, 0.0f);// up
		//nearing -= 0.03;
		if (nearing <= 2.5)
			nearing = 2.5;
		else
			nearing -= 0.03;
	}
	else if (_fade_) {
		if (shakee)
			shake = 0.3;
		else
			shake = -0.3;
		shakee = !shakee;
		gluLookAt(0.0f, 0.0, 3.0,// eye
			0.0f, shake, 0.0f,// center
			1.0f, 0.0, 0.0f);// up
	}
	else {
		gluLookAt(1.0f, 0.0, nearing,// eye
			0.0f, turnleft, 0.0f,// center
			1.0f, 0.0, 0.0f);// up
	}
	//timee -= 0.001;
	GLUquadric* quad = gluNewQuadric();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	//glColor4f(1.0, 0.0, 0.0, 1.0);
	//gluSphere(quad, 1, 100, 50);
	//set light position


	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)512 / (GLfloat)512, 1, 500);

	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);

	glMatrixMode(GL_MODELVIEW);

	// glRotatef(day, 0.0f, 1.0f, 0.0f);

	GLint switchcolorLoc = glGetUniformLocation(program, "switchcolor");
	GLint tranLoc = glGetUniformLocation(program, "transparent");
	GLint textureLoc = glGetUniformLocation(program, "_texture_");
	GLint fadeLoc = glGetUniformLocation(program, "_fade_");
	GLint spcolorLoc = glGetUniformLocation(program, "_spcolor_");
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

	glUniform1i(switchcolorLoc, burst);
	glUniform1f(tranLoc, transparent);
	glUniform1i(textureLoc, false);
	glUniform1i(fadeLoc, false);
	glUniform1i(spcolorLoc, false);
	//Active, bind and set up the value of uniform variable in fragment shader

	GLuint texLoc = glGetUniformLocation(program, "ourTexture");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texLoc, 0);


	//input the modelview matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	//input the rotation matrix into vertex shader
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);

	//draw particles

	if (transparent < 1.0 && turnright >= ee) {
		if (!burst) {
			glBindVertexArray(vao);

			glDrawArrays(GL_POINTS, 0, 20000);
			
			glBindVertexArray(0);
		}
		else {
			glBindVertexArray(vao);
			glVertexAttribDivisor(0, 1);
			glVertexAttribDivisor(1, 1);
			glVertexAttribDivisor(2, 0);

			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, Particlenum);
			glBindVertexArray(0);

		}
	}
	//draw spheres
	if (!_defade_ ){
		glBindBuffer(GL_ARRAY_BUFFER, vbosphere);
		glUniform1i(spcolorLoc, true);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
		

		glDrawArrays(GL_TRIANGLE_STRIP, 0, verticeNumber);
		glUniform1i(spcolorLoc, false);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//Bind texture
	if (_defade_) {
		
		glUniform1i(textureLoc, true);

		glBindBuffer(GL_ARRAY_BUFFER, vbotext);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoords)));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glUniform1i(textureLoc, false);

	}

	//draw rings
	if (burst) {
		ParticleAttribute* points;
		points = drawRing();
		glBindBuffer(GL_ARRAY_BUFFER, vboring);
		glUniform1i(spcolorLoc, true);

		glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 722, points, GL_STATIC_DRAW);

		ring_dis += 0.1;
		glEnableVertexAttribArray(0);
	
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 722);
		glUniform1i(spcolorLoc, false);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		fading();
	}
	//draw square to simulate fading
	if (_fade_) {
		glUniform1i(fadeLoc, true);

		glBindBuffer(GL_ARRAY_BUFFER, vbosq);
		transparent += 0.005;
		if (transparent >= 1) {
			transparent = 1.0;
		}
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glUniform1i(fadeLoc, false);
	}
	else if (_defade_) {
		glUniform1i(fadeLoc, true);

		glBindBuffer(GL_ARRAY_BUFFER, vbosq);
		transparent -= 0.001;
		if (transparent <= 0.0) {
			transparent = 0.0;
		}
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glUniform1i(fadeLoc, false);
	}
	//Unbind the texture
	glBindTexture(GL_TEXTURE_2D, NULL);

	glUseProgram(0);
	glPopMatrix();
	glutSwapBuffers();
}
void TextureInit() {
	glEnable(GL_TEXTURE_2D);

	//Load the texture
	const char* pFilename = "galaxy1.jpg";
	//const char* pFilename = "earth_texture_map.jpg";

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	FreeImage_Unload(p32BitsImage);
	FreeImage_Unload(pIimage);

}

ParticleAttribute* drawRing() {
	double x, y;
	ParticleAttribute *points = new ParticleAttribute[722];
	float inner = ring_dis;
	float outer = ring_dis+2;
	float j = 0;
	for (int i = 0; i <= 720; i+=2) {
		float angle = 2.0 * PI * (j / 360);
		j += 1;

		points[i].position[0] = 0.0;
		points[i].position[1] = inner * cos(angle);
		points[i].position[2] = inner * sin(angle);
		
		points[i + 1].position[0] = 0.0;
		points[i + 1].position[1] = outer * cos(angle);
		points[i + 1].position[2] = outer * sin(angle);
	}
	return points;
}
VertexAttribute* drawSquare() {
	VertexAttribute* points = new VertexAttribute[4];
	points[0].setPosition(-2.0, -2.0, 0.0);
	points[0].settexcoords(0.0, 0.0);
	points[1].setPosition(-2.0, 2.0, 0.0);
	points[1].settexcoords(0.0, 1.0);
	points[2].setPosition(2.0, -2.0, 0.0);
	points[2].settexcoords(1.0, 0.0);
	points[3].setPosition(2.0, 2.0, 0.0);
	points[3].settexcoords(1.0, 1.0);

	return points;
}
VertexAttribute* drawCircle() {
	double x, y;
	VertexAttribute* points = new VertexAttribute[362];
	double radius = 3.0;
	for (int i = 0; i <= 360; i++) {
		float angle = 2.0 * PI * (i / 360.0);
		x = radius * cos(angle);
		y = radius * sin(angle);

		points[i].setPosition( x, y, 0.0);
	}
	return points;
}
VertexAttribute *drawSphere() {
	double x, y, z;
	double a, b;
	double slice_step = 2 * PI / slice, stack_step = PI / stack;
	double radius = 1.0;

	VertexAttribute *vertices;
	vertices = new VertexAttribute[ verticeNumber ];
	int idx = 0;
	for (int i = 0; i < slice; i++) {
		for (int j = 0; j < stack + 1; j++) {
			a = 1 - (i / double(slice));
			b = 1 - (j / double(stack));

			x = sin(j * stack_step) * cos(i*slice_step) * radius;
			y = cos(j * stack_step) * radius;
			z = sin(j * stack_step) * sin(i*slice_step) * radius;
			
			vertices[idx].setPosition(x, y, z);
			vertices[idx].setNormalVec(x, y, z);
			vertices[idx++].settexcoords(a, b);
			x = sin(j * stack_step) * cos((i + 1)*slice_step) * radius;
			y = cos(j * stack_step) * radius;
			z = sin(j * stack_step) * sin((i + 1)*slice_step) * radius;
			
			vertices[idx].setPosition(x, y, z);
			vertices[idx].setNormalVec(x, y, z);
			vertices[idx++].settexcoords(a, b);

		}	
	}
	return vertices;
}


void reshape(GLsizei w, GLsizei h) {
	windowSize[0] = w;
	windowSize[1] = h;
}


void keyboard(unsigned char key, int x, int y) {
	//swing head
	if (key == 'f') {
		_fade_ = true;
	}
	//absorb
	else if (key == 'd') {
		_fade_ = false;
		_defade_ = true;
		transparent = 1.0;
	}
	//turn_right
	else if (key == 't') {
		_turnright_ = true;
	}
	else if (key == 's') {
		switchdir = !switchdir;
	}
	else if (key == 'b') {
		ee = -100.0;
		_back_ = true;
	}
	else if (key == 'v') {
		bburst = true;
	}
}
bool check() {
	for (int i = 0; i < Particlenum; i++) {
		if (pow(particles[i].position[0], 2) + pow(particles[i].position[1], 2) + pow(particles[i].position[2], 2)
	> 1) {
			return false;
		}
	}
	return true;
}
void initpos() {
	for (int i = 0; i < Particlenum; i++) {
		particles[i].position[0] = (rand() % 100 - 50) / 100.0;
		particles[i].position[1] = (rand() % 100 - 50) / 100.0;
		particles[i].position[2] = (rand() % 100 - 50) / 100.0;

		float stepi = 2 * PI / 180;
		int randomi = rand() % 180;
		float stepj = 2 * PI / 180;
		int randomj = rand() % 180;
		particles[i].speed[0] = sin(randomj * stepj) * cos(randomi * stepi) * 100.0;
		particles[i].speed[1] = cos(randomj * stepj) * 100.0;
		particles[i].speed[2] = sin(randomj * stepj) * cos(randomi * stepi) * 100.0;
	}
}
void absorb() {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * Particlenum, particles, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));
	for (int i = 0; i < Particlenum; i++) {
		
		if (pow(particles[i].position[0], 2) + pow(particles[i].position[1], 2) + pow(particles[i].position[2], 2)
			< 9) {
			particles[i].position[0] -= particles[i].position[0] / 100.0;
			particles[i].position[1] -= particles[i].position[1] / 100.0;
			particles[i].position[2] -= particles[i].position[2] / 100.0;
		}
		else {
			particles[i].position[0] -= particles[i].position[0] / 1000.0;
			particles[i].position[1] -= particles[i].position[1] / 1000.0;
			particles[i].position[2] -= particles[i].position[2] / 1000.0;
		}
		//if (particles[i])
	}
	burst = check();
	if (burst)
		initpos();
}

void fading(){
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * Particlenum, particles, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));
	for (int i = 0; i < Particlenum; i++) {
		particles[i].position[0] += particles[i].speed[0] / 10000.0;
		particles[i].position[1] += particles[i].speed[1] / 10000.0;
		particles[i].position[2] += particles[i].speed[2] / 10000.0;

		/*
		particles[i].life -= particles[i].fade;
		if (particles[i].life < 0.0) {
			particles[i].life = 1.0f;
			particles[i].fade = float(rand() % 100) / 1000.0f + 0.003f;
			particles[i].position[0] = 0.0;
			particles[i].position[1] = 0.0;
			particles[i].position[2] = 0.0;
		}
		*/
	}
}
int time = 0;

void idle() {
	//day += (X / 365.0);
	//if (day >= 360) day = 0;

	time += 1;
	double cnt = 10.0f;
	
	if (time > 20) {

		//fading();
		/*
		if (shakee) {
			shake = 0.1;
		}
		else {
			shake = -0.1;
		}
		shakee = !shakee;
		

		ParticleAttribute* points;
		points = drawRing();
		glBindBuffer(GL_ARRAY_BUFFER, vboring);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 722, points, GL_STATIC_DRAW);

		if (stopdir)
			absorb();
		else if (burst) {
			fading();
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * Particlenum, particles, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));

		//Generate second buffer object

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, life)));
		//glEnableVertexAttribArray(2);
		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoords)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		*/
		
		time = 0;
	}
	
	glutPostRedisplay();
}