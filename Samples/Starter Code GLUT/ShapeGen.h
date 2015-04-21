#pragma once
#include "glm/glm.hpp"
#include "glew.h"
#include <iostream>

class ShapeGen
{
  const static int NON_EMPTY_SHAPE_TYPE_COUNT = 2;

  const static int DREADED_CONE_CIRCLE_RESOLUTION = 8;

	static GLuint shaderVertID;
	static GLuint shaderFragID;
  static GLuint shaderProgID;

  static void shaderErrors();
  static void printShaderInfoLog(int shader);
  static void linkErrors();


public:
  static GLuint transMatID;
  static GLuint camMatID;
  static GLuint lightPosID;
  static GLuint matColorID;
  static GLuint eyeDirID;
  static GLuint eyePosID;
  static GLuint lightColID;

  const static GLuint posAttID = 0;
  const static GLuint normAttID = 1;
  const static GLuint colAttID = 2;

  //Why doesn't C++ allow const static floats?  Oh well.  It's defined
  //in the body file.
  static float PI;

  static GLuint vertBufIDs[NON_EMPTY_SHAPE_TYPE_COUNT];
  static GLuint colBufIDs[NON_EMPTY_SHAPE_TYPE_COUNT];
  static GLuint normBufIDs[NON_EMPTY_SHAPE_TYPE_COUNT];
  static GLuint indBufIDs[NON_EMPTY_SHAPE_TYPE_COUNT];





  enum ShapeType {
    INVIS_SHAPE = -1,
	  MESH = 0,
    CUBE = 1,
    DREADED_CONE = 2
  };
 
  //Any given generated model might have a bounding box which is higher than
  //y = 1.0.  In any case, though, 1.0 is the height of the model's stacking'
  //plane.  For instance, the seat of a chair will be at 1.0, while the back
  //might go up to 2.0, or something.


  //A bunch of GL stuff happens here.
  static void setupShapes();

  //This is only in a separate method to make things easier to read.  Besides
  //that, setupCUBE is really just a part of setupShapes.
  static void setupCUBE();

  static int cubeVert(int octant, int direction);

  static void setupDreadedCone();

  static char* readFile(const char* fileName);
 
	ShapeGen(void);
	~ShapeGen(void);
};