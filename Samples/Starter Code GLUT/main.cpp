#include "glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../freeglut/include/GL/glut.h"
#include <fstream>
#include <iostream>
#include <time.h>
#include <vector>

#include "ShapeGen.h"
#include "Scene.h"
#include "tests.h"

using namespace glm;

//vertex arrays needed for drawing
unsigned int vbo;
unsigned int cbo;
unsigned int nbo;
unsigned int ibo;

//attributes
unsigned int positionLocation = 0;
unsigned int colorLocation = 1;
unsigned int normalLocation = 2;

//uniforms
unsigned int u_modelMatrixLocation;
unsigned int u_projMatrixLocation;
	
//needed to compile and link and use the shaders
unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int shaderProgram;

//Animation/transformation stuff
clock_t old;
float rotation = 0.0f;
float lightY = 10.0f;
glm::vec3 lightCol = glm::vec3(1.0f, 1.0f, 1.0f);

//helper function to read shader source and put it in a char array
//thanks to Swiftless
char* textFileRead(const char*);

//some other helper functions from CIS 565
void printLinkInfoLog(int);
void printShaderInfoLog(int);

//standard glut-based program functions
void init(void);
void resize(int, int);
void display(void);
void keypress(unsigned char, int, int);
void cleanup(void);

void createRedSquare(mat4);
void createBlueSquare(mat4);

GLint lightPosID;

float cameraTheta = 0.0f;
float cameraY = 5.0f;
float cameraX = 5.0f;
float cameraZ = 5.0f;
float targetY = 5.0f;
float cameraZoom = 12.0f;
int heightGlobal;
int widthGlobal;

Scene* scenester;

int main(int argc, char** argv) {
  //RunTests();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Starter Code");

	glewInit();

  ShapeGen::setupShapes();
  Scene::setTransMat(mat4());

  /*
  int baseNode = scenester.addNode1(vec3(0, 0, 0), 0.0f, vec3(1.0f, 1.0f, 1.0f), vec3(), ShapeGen::INVIS_SHAPE, -1);
  int baseNode1 = scenester.addNode1(vec3(1.0, 0, 0), 45.0f, vec3(1.0f, 0.5f, 1.0f), vec3(1.0), ShapeGen::CUBE, baseNode);
  int baseNode2 = scenester.addNode1(vec3(1.0, 0, 0), 0.0f, vec3(1.0f, 2.0f, 1.0f), vec3(1.0), ShapeGen::CUBE, baseNode1);
  int baseNode3 = scenester.addNode1(vec3(1.0, 0, 0), 45.0f, vec3(1.0f, 2.0f, 1.0f), vec3(1.0), ShapeGen::CUBE, baseNode2);

  int nodem4[4] = {baseNode1, baseNode2, baseNode3};
  scenester.addTable(vec3(0, 0, 2.0f), 0, vec3(2.0f, 1.0f, 1.0f), vec3(0.5, 0.5, 0.0), nodem4, 3);
  */

  //scenester = new Scene(argv[1]);
  scenester = new Scene("..\\HW5_scene.txt");

  resize(640, 480);
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keypress);
	glutIdleFunc(display);

	glutMainLoop();
	return 0;
}

void cleanup() {
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &cbo);
	glDeleteBuffers(1, &nbo);
	glDeleteBuffers(1, &ibo);

	//Tear down the shader program in reverse of building it
	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(shaderProgram);
}
 
void keypress(unsigned char key, int x, int y) {
	switch(key) {
	case 'q':
		cleanup();
		exit(0);
		break;
	case '[': 
    cameraTheta += 10.0f/180;
    break;
	case ']': 
    cameraTheta -= 10.0f/180;
    break;
	case 'w':
    cameraX += 0.19f;
    break;
	case 's':
    cameraX -= 0.19f;
    break;
	case 'a':
    cameraZ += 0.19f;
    break;
	case 'd':
    cameraZ -= 0.19f;
    break;
	case ',':
    rotation += 19.0f / (2.0f * 3.141f);
    break;
	case '.':
    rotation -= 19.0f / (2.0f * 3.141f);
    break;
	case '=':
    cameraZoom -= 0.19f;
    break;
	case '-':
    cameraZoom += 0.19f;
    break;
	case 'r': 
    lightCol.x = glm::clamp(lightCol.x + 0.019f, 0.0f, 1.0f);
    break;
	case 'f': 
    lightCol.x = glm::clamp(lightCol.x - 0.019f, 0.0f, 1.0f);
    break;
	case 't': 
    lightCol.y = glm::clamp(lightCol.y + 0.019f, 0.0f, 1.0f);
    break;
	case 'g': 
    lightCol.y = glm::clamp(lightCol.y - 0.019f, 0.0f, 1.0f);
    break;
	case 'y': 
    lightCol.z = glm::clamp(lightCol.z + 0.019f, 0.0f, 1.0f);
    break;
	case 'h': 
    lightCol.z = glm::clamp(lightCol.z - 0.019f, 0.0f, 1.0f);
    break;
	case 'u':
    lightY += 0.19f;
    break;
	case 'j':
    lightY -= 0.19f;
    break;
	case 'i':
    cameraY += 0.19f;
    break;
	case 'k':
    cameraY -= 0.19f;
    break;
	case 'o':
    targetY += 0.19f;
    break;
	case 'l':
    targetY -= 0.19f;
    break;
	case 'p': 
	  scenester->parseForRaytracer("..\\raytracer_config_sample.txt");
    scenester->renderFile(); 
    std::cout << "done!";
    break;
	}

  glm::vec3 eyePos = glm::vec3(cameraZoom * cos(cameraTheta) + cameraX, cameraY, cameraZoom * sin(cameraTheta) + cameraZ);
  glm::vec3 eyeTarget = glm::vec3(cameraX, targetY, cameraZ);
  glm::vec3 eyeVec = eyeTarget - eyePos;

	glm::mat4 projection = glm::perspective(60.0f, static_cast<float>(widthGlobal) / static_cast<float>(heightGlobal), 0.1f, 40.0f);
	//glm::mat4 camera = glm::lookAt(glm::vec3(20, 10, 20), glm::vec3(0, 10, 0), glm::vec3(0, 1, 0));
	glm::mat4 camera = glm::lookAt(eyePos, eyeTarget, glm::vec3(0, 1, 0));
	//glm::mat4 camera = glm::lookAt(glm::vec3(5.0, 0.5, 0.0), glm::vec3(0.0, 0.5, 0.0), glm::vec3(0, 1, 0));
	projection = projection * camera;

	//set the projection matrix here, it only needs to be changed if the screen is resized otherwise it can stay the same
	glUniformMatrix4fv(u_projMatrixLocation, 1, GL_FALSE, &projection[0][0]);
  glUniform3f(ShapeGen::eyeDirID, eyeVec.x - eyePos.x, eyeVec.y - eyePos.y, eyeVec.z - eyePos.z);
  glUniform3f(ShapeGen::eyePosID, eyePos.x, eyePos.y, eyePos.z);
  glUniform3f(ShapeGen::lightColID, lightCol.x, lightCol.y, lightCol.z);

	glutPostRedisplay();
}

void display() {
	//Always and only do this at the start of a frame, it wipes the slate clean
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	clock_t newTime = clock();

	//part of the animation
	//rotation += 10 * (static_cast<float>(newTime - old) / static_cast<float>(CLOCKS_PER_SEC));

	//create an identity matrix for the modelview matrix
	glm::mat4 modelView = glm::mat4(1.0) * glm::scale(vec3(1.0f, 1.0f, 1.0f));

  glm::vec3 lightPos = glm::vec3(15.0f * cos(rotation / 30.0f), lightY, 15.0f * sin(rotation / 30.0f));

    /*
	//Draw the two components of our scene separately, for your scenegraphs it will help
	//your sanity to do separate draws for each type of primitive geometry, otherwise your VBOs will
	//get very, very complicated fast
	createRedSquare(modelView);	//PASS MODELVIEW BY COPY to get the nice scoping for hierarchical (i'm sure i spelled that wrong) transformations
								//like Norm mentioned in his Scene Graph lecture
	createBlueSquare(modelView);
    */
 
	//glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
  scenester->render(modelView, lightPos);

  //ShapeGen::renderShapeGen(ShapeGen::CUBE, modelView);

	glutSwapBuffers();
	old = newTime;
}

void createRedSquare(mat4 modelView) {
	//set up a transformation matrix, in this case we're doing rotation only
	modelView = glm::rotate(modelView, rotation, glm::vec3(0, 0, 1));

	//these four points define where the quad would be BEFORE transformations
	//this is referred to as object-space and it's best to center geometry at the origin for easier transformations
	float* vertices = new float[16];
	vertices[0] = -4.0f; vertices[1] = 4.0f; vertices[2] = -1.0f; vertices[3] = 1.0f;
	vertices[4] = -4.0f; vertices[5] = -4.0f; vertices[6] = -1.0f; vertices[7] = 1.0f;
	vertices[8] = 4.0f; vertices[9] = -4.0f; vertices[10] = -1.0f; vertices[11] = 1.0f;
	vertices[12] = 4.0f; vertices[13] = 4.0f; vertices[14] = -1.0f; vertices[15] = 1.0f;

	//now we put the data into the Vertex Buffer Object for the graphics system to use
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), vertices, GL_STATIC_DRAW); //the square vertices don't need to change, ever,
																				 //while the program runs

	//once the data is loaded, we can delete the float arrays, the data is safely stored with OpenGL
	delete [] vertices;

	//again with colors
	float* colors = new float[12];
	
	colors[0] = 1; colors[1] = 0; colors[2] = 0;
	colors[3] = 1; colors[4] = 0; colors[5] = 0;
	colors[6] = 1; colors[7] = 0; colors[8] = 0;
	colors[9] = 1; colors[10] = 0; colors[11] = 0;

	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	//always make sure you are telling OpenGL the right size to make the buffer, color data doesn't have as much data!
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), colors, GL_STREAM_DRAW);	//the color is going to change every frame
																				//as it bounces between squares
	delete [] colors;

	//once more, this time with normals
	float* normals = new float[16];
	
	normals[0] = 0; normals[1] = 0; normals[2] = 1; normals[3] = 0;
	normals[4] = 0; normals[5] = 0; normals[6] = 1; normals[7] = 0;
	normals[8] = 0; normals[9] = 0; normals[10] = 1; normals[11] = 0;
	normals[12] = 0; normals[13] = 0; normals[14] = 1; normals[15] = 0;

	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), normals, GL_STATIC_DRAW); //the square normals don't need to change, ever,
																				 //while the program runs
	delete [] normals;

	//activate our three kinds of information
	glEnableVertexAttribArray(positionLocation);
	glEnableVertexAttribArray(colorLocation);
	glEnableVertexAttribArray(normalLocation);
	
	//we're using the vertex data first
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//define how the vertex pointer should work, in our case we're accessing floats 4 at a time with no special pattern
	glVertexAttribPointer(positionLocation, 4, GL_FLOAT, 0, 0, static_cast<char*>(0));
	
	//now use color data, remember we're not using 4 at a time anymore
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, 0, 0, static_cast<char*>(0));
	
	//one more time with the normals
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glVertexAttribPointer(normalLocation, 4, GL_FLOAT, 0, 0, static_cast<char*>(0));
	
	//the last thing we need to do is setup our indices
	unsigned short* indices = new unsigned short[6];

	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 3; indices[4] = 0; indices[5] = 2;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned short), indices, GL_STATIC_DRAW);

	delete [] indices;

	//set the modelview uniform
	glUniformMatrix4fv(u_modelMatrixLocation, 1, GL_FALSE, &modelView[0][0]);

	//draw the elements
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	
	//shut off the information since we're done drawing
	glDisableVertexAttribArray(positionLocation);
	glDisableVertexAttribArray(colorLocation);
	glDisableVertexAttribArray(normalLocation);
}

void createBlueSquare(mat4 modelView) {
	//two transformations put into the modelview matrix
	modelView = glm::translate(modelView, glm::vec3(-2.0f, 2.0f, -0.1f));
	modelView = glm::rotate(modelView, -rotation, glm::vec3(0, 0, 1));

	//the only difference between a red square and a blue square is the color, so we can leave the other VBOs as they are
	float* colors = new float[12];
	
	colors[0] = 0; colors[1] = 0; colors[2] = 1;
	colors[3] = 0; colors[4] = 0; colors[5] = 1;
	colors[6] = 0; colors[7] = 0; colors[8] = 1;
	colors[9] = 0; colors[10] = 0; colors[11] = 1;

	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), colors, GL_STREAM_DRAW);
	
	delete [] colors;

	//activate our three kinds of information
	glEnableVertexAttribArray(positionLocation);
	glEnableVertexAttribArray(colorLocation);
	glEnableVertexAttribArray(normalLocation);
	
	//bind again
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(positionLocation, 4, GL_FLOAT, 0, 0, static_cast<char*>(0));
	
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, 0, 0, static_cast<char*>(0));
	
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glVertexAttribPointer(normalLocation, 4, GL_FLOAT, 0, 0, static_cast<char*>(0));

	//set the modelview matrix again since it changed
	glUniformMatrix4fv(u_modelMatrixLocation, 1, GL_FALSE, &modelView[0][0]);

	//draw again, even the indices from before are good
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	
	//shut off the information since we're done drawing
	glDisableVertexAttribArray(positionLocation);
	glDisableVertexAttribArray(colorLocation);
	glDisableVertexAttribArray(normalLocation);
}

void resize(int width, int height) {
  heightGlobal = height;
  widthGlobal = width;
	//set the viewport, more boilerplate
	glViewport(0, 0, width, height);

	//
	glm::mat4 projection = glm::perspective(60.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 40.0f);
	//glm::mat4 camera = glm::lookAt(glm::vec3(20, 10, 20), glm::vec3(0, 10, 0), glm::vec3(0, 1, 0));
	glm::mat4 camera = glm::lookAt(glm::vec3(20 * cos(cameraTheta), 10, 20 * sin(cameraTheta)), glm::vec3(0, 10, 0), glm::vec3(0, 1, 0));
	projection = projection * camera;

	//set the projection matrix here, it only needs to be changed if the screen is resized otherwise it can stay the same
	glUniformMatrix4fv(u_projMatrixLocation, 1, GL_FALSE, &projection[0][0]);
  glUniform3f(ShapeGen::lightColID, lightCol.x, lightCol.y, lightCol.z);

	glutPostRedisplay();
}

//from swiftless.com
char* textFileRead(const char* fileName) {
    char* text;
    
    if (fileName != NULL) {
        FILE *file = fopen(fileName, "rt");
        
        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);
            
            if (count > 0) {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';	//cap off the string with a terminal symbol, fixed by Cory
            }
            fclose(file);
        }
    }
    return text;
}

void printLinkInfoLog(int prog) 
{
	int infoLogLen = 0;
	int charsWritten = 0;
	GLchar *infoLog;

	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

	// should additionally check for OpenGL errors here

	if (infoLogLen > 0)
	{
		infoLog = new GLchar[infoLogLen];
		// error check for fail to allocate memory omitted
		glGetProgramInfoLog(prog,infoLogLen, &charsWritten, infoLog);
		std::cout << "InfoLog:" << std::endl << infoLog << std::endl;
		delete [] infoLog;
	}
}

void printShaderInfoLog(int shader)
{
	int infoLogLen = 0;
	int charsWritten = 0;
	GLchar *infoLog;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

	// should additionally check for OpenGL errors here

	if (infoLogLen > 0)
	{
		infoLog = new GLchar[infoLogLen];
		// error check for fail to allocate memory omitted
		glGetShaderInfoLog(shader,infoLogLen, &charsWritten, infoLog);
		std::cout << "InfoLog:" << std::endl << infoLog << std::endl;
		delete [] infoLog;
	}

	// should additionally check for OpenGL errors here
}