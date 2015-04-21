#include "ShapeGen.h"

ShapeGen::ShapeGen(void)
{
  
}

float ShapeGen::PI = 3.141593f;

GLuint ShapeGen::vertBufIDs[] = {-1};
GLuint ShapeGen::colBufIDs[] = {-1};
GLuint ShapeGen::normBufIDs[] = {-1};
GLuint ShapeGen::indBufIDs[] = {-1};

GLuint ShapeGen::shaderVertID = -1;
GLuint ShapeGen::shaderFragID = -1;
GLuint ShapeGen::shaderProgID = -1;

GLuint ShapeGen::transMatID = -1;
GLuint ShapeGen::camMatID = -1;
GLuint ShapeGen::lightPosID = -1;
GLuint ShapeGen::matColorID = -1;
GLuint ShapeGen::eyeDirID = -1;
GLuint ShapeGen::eyePosID = -1;
GLuint ShapeGen::lightColID = -1;

void ShapeGen::setupShapes() { 
 
	glGenBuffers(NON_EMPTY_SHAPE_TYPE_COUNT, vertBufIDs);
	glGenBuffers(NON_EMPTY_SHAPE_TYPE_COUNT, colBufIDs);
	glGenBuffers(NON_EMPTY_SHAPE_TYPE_COUNT, normBufIDs);
	glGenBuffers(NON_EMPTY_SHAPE_TYPE_COUNT, indBufIDs);
	
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	glDepthFunc(GL_LEQUAL); 

	const char* fragFilename = "../diffuseFrag.frag";
	const char* vertFilename = "../diffuseVert.vert";
    /*
	const char* fragFilename = "C:\diffuseFrag.frag";
	const char* vertFilename = "C:\diffuseVert.vert";
    */
	shaderVertID = glCreateShader(GL_VERTEX_SHADER);
	shaderFragID = glCreateShader(GL_FRAGMENT_SHADER);
	shaderProgID = glCreateProgram();

	const char* vertShaderSource = readFile(vertFilename); 
	glShaderSource(shaderVertID, 1, &vertShaderSource, 0);
	glCompileShader(shaderVertID);

	const char* fragShaderSource = readFile(fragFilename);
	glShaderSource(shaderFragID, 1, &fragShaderSource, 0);
	glCompileShader(shaderFragID);
  
	ShapeGen::shaderErrors();
 
	//set the attribute locations for our shaders
	glBindAttribLocation(shaderProgID, posAttID, "vs_position");
	glBindAttribLocation(shaderProgID, normAttID, "vs_normal");
	glBindAttribLocation(shaderProgID, colAttID, "vs_color");

	//finish shader setup
	glAttachShader(shaderProgID, shaderVertID);
	glAttachShader(shaderProgID, shaderFragID);
	glLinkProgram(shaderProgID);

	ShapeGen::linkErrors();

	lightPosID = glGetUniformLocation(shaderProgID, "lightPos");
	transMatID = glGetUniformLocation(shaderProgID, "transMat");
	camMatID = glGetUniformLocation(shaderProgID, "camMat");
	matColorID = glGetUniformLocation(shaderProgID, "matColor");
	eyeDirID = glGetUniformLocation(shaderProgID, "eyeDir");
	eyePosID = glGetUniformLocation(shaderProgID, "eyePos");
	lightColID = glGetUniformLocation(shaderProgID, "lightCol");

	glUseProgram(shaderProgID);

	setupCUBE();
}

void ShapeGen::setupCUBE() {
  //CUBE!
	{
    //Make a vbo containing the following vertices 
    //000 100 010 001 110 101 011 111
    glBindBuffer(GL_ARRAY_BUFFER, vertBufIDs[0]);
    float vertices0[8 * 4] = { 
      1.0f, 1.0f, 1.0f, 1.0f, //111 -> I
      -1.0f, 1.0f, 1.0f, 1.0f, //011 -> II
      -1.0f, -1.0f, 1.0f, 1.0f, //001 -> III
      1.0f, -1.0f, 1.0f, 1.0f, //101 -> IV
      1.0f, 1.0f, -1.0f, 1.0f, //110 -> V
      -1.0f, 1.0f, -1.0f, 1.0f, //010 -> VI
      -1.0f, -1.0f, -1.0f, 1.0f, //000 -> VII
      1.0f, -1.0f, -1.0f, 1.0f  //100 -> VIII
    }; 
    float vertices1[8 * 3 * 4];
    for (int i = 0; i < 8; i++) {
      //along x, along y, along z
      for (int j = 0; j < 3; j++) {
        //x, y, z, w
        for (int k = 0; k < 4; k++) {
          vertices1[i * 3 * 4 + j * 4 + k] = vertices0[i * 4 + k];
		    }
	    }
	  }
    glBufferData(GL_ARRAY_BUFFER, 4 * 8 * 3 * sizeof(float), vertices1, GL_STATIC_DRAW);
	}

	{
    glBindBuffer(GL_ARRAY_BUFFER, colBufIDs[0]);
    float colors1[8 * 3 * 3];
    for (int i = 0; i < 8 * 3 * 3; i++) {
      colors1[i] = 1.0f;
	  }
    /*
    for (int i = 0; i < 8; i++) {
      //along x, along y, along z
      for (int j = 0; j < 3; j++) {
        //x, y, z
        for (int k = 0; k < 3; k++) {
          //This should make it so that faces with normals along the x axis
          // be red, y aligned ones will be green, and z aligned ones will be blue.
          colors1[i * 3 * 3 + j * 3 + k] = ((float) (j == k)) * 0.7 + 0.2;
		    }
	    }
	  }*/
    glBufferData(GL_ARRAY_BUFFER, 8 * 3 * 3 * sizeof(float), colors1, GL_STATIC_DRAW);
	}

	{
    float normals0[24] = {
       1.0f,  1.0f,  1.0f, 
      -1.0f,  1.0f,  1.0f, 
      -1.0f, -1.0f,  1.0f, 
       1.0f, -1.0f,  1.0f, 
       1.0f,  1.0f, -1.0f, 
      -1.0f,  1.0f, -1.0f, 
      -1.0f, -1.0f, -1.0f, 
       1.0f, -1.0f, -1.0f
    };

    //TODO: make the normals line up with 
    glBindBuffer(GL_ARRAY_BUFFER, normBufIDs[0]);
    float normals1[8 * 3 * 4];
    for (int i = 0; i < 8; i++) {
      //directions
      for (int j = 0; j < 3; j++) {
        //dimensions
        for (int k = 0; k < 4; k++) {
          const int index = i * 3 * 4 + j * 4 + k;
          if (j == k) { 
            normals1[index] = normals0[i * 3 + j];
		      } else {
            normals1[index] = 0;
		      }
		    }
	    }
	  }
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), normals1, GL_STATIC_DRAW);
	}

  //Now we're entering IBO mode.
	glEnableVertexAttribArray(posAttID);
	glEnableVertexAttribArray(normAttID);
	glEnableVertexAttribArray(colAttID);
  
  glBindBuffer(GL_ARRAY_BUFFER, vertBufIDs[0]);
  glVertexAttribPointer(posAttID, 4, GL_FLOAT, 0, 0, static_cast<GLvoid*>(0));

  glBindBuffer(GL_ARRAY_BUFFER, colBufIDs[0]);
  glVertexAttribPointer(colAttID, 3, GL_FLOAT, 0, 0, static_cast<GLvoid*>(0));

  glBindBuffer(GL_ARRAY_BUFFER, normBufIDs[0]);
  glVertexAttribPointer(normAttID, 4, GL_FLOAT, 0, 0, static_cast<GLvoid*>(0));

  {
    const int indCount = 3 * 2 * 6;
    //x, then y, then z
      //positive, then negative
        //and finally the particular vertex
    GLint octants[indCount] = {
      1, 5, 4, 8, 5, 4, //x
      2, 6, 3, 7, 6, 3,

      2, 4, 1, 2, 4, 3, //y
      6, 8, 7, 6, 8, 5,

      3, 8, 7, 3, 8, 4, //z
      2, 5, 1, 2, 5, 6
    };
    GLint indices[indCount];
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 2; j++) {
        for (int k = 0; k < 6; k++) {
          const int octantsIndex = i * 2 * 6 + j * 6 + k;
          indices[octantsIndex] = cubeVert(octants[octantsIndex], i);
		    }
	    }
	  }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBufIDs[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indCount * sizeof(GLint), indices, GL_STATIC_DRAW); 
  }

}

//where 0 = x, 1 = y, 2 = z, for the direction argument
int ShapeGen::cubeVert(int octant, int direction) {
  return (octant - 1) * 3 + direction;
}

void ShapeGen::setupDreadedCone() {
  const int vertCount = DREADED_CONE_CIRCLE_RESOLUTION + 2;
	{
    glBindBuffer(GL_ARRAY_BUFFER, vertBufIDs[1]);
    float vertices[vertCount * 4] = {
      0.0f, 0.0f, 0.0f, 1.0f,
      0.0f, 1.0f, 0.0f, 1.0f
    }; 
    for (int i = 2; i < vertCount; i++) {
      vertices[i*4] = cos(2 * PI * i / DREADED_CONE_CIRCLE_RESOLUTION); //x
      vertices[i*4 + 1] = 0.0f; //y
      vertices[i*4 + 2] = sin(2 * PI * i / DREADED_CONE_CIRCLE_RESOLUTION); //z
      vertices[i*4 + 3] = 1.0f;//1.0 because it's a point and not a vector
    }
    glBufferData(GL_ARRAY_BUFFER, vertCount * 4 * sizeof(float), vertices, GL_STATIC_DRAW);
  } 

	{
    glBindBuffer(GL_ARRAY_BUFFER, colBufIDs[1]);
	  float colors[vertCount * 3] = {
      0.9f, 0.9f, 1.0f,
      1.0f, 0.9f, 0.8f
	  };
    for (int i = 2; i < vertCount; i++) {
      colors[i*3] = 1.0f;
      colors[i*3 + 1] = 1.0f;
      colors[i*3 + 2] = 1.0f;
	  }
    glBufferData(GL_ARRAY_BUFFER, vertCount * 3 * sizeof(float), colors, GL_STATIC_DRAW);
	}

	{
    glBindBuffer(GL_ARRAY_BUFFER, colBufIDs[1]);
    float normals[vertCount * 4] = {
      0.0f, -1.0f, 0.0f, 0.0f,
      0.0f,  1.0f, 0.0f, 0.0f
	  };
    for (int i = 2; i < vertCount; i++) {
      normals[i*4] = cos(2 * PI * i / DREADED_CONE_CIRCLE_RESOLUTION); //x
      normals[i*4 + 1] = 0.0f; //y
      normals[i*4 + 2] = sin(2 * PI * i / DREADED_CONE_CIRCLE_RESOLUTION); //z
      normals[i*4 + 3] = 0.0f;//0.0 because it's a vector
    }
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), normals, GL_STATIC_DRAW);
	}

}



void ShapeGen::shaderErrors() {
	GLint compiled;
	glGetShaderiv(shaderVertID, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		printShaderInfoLog(shaderVertID);
	} 
	glGetShaderiv(shaderFragID, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		printShaderInfoLog(shaderFragID);
	} 
}
 
void ShapeGen::printShaderInfoLog(int shader) {
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

void ShapeGen::linkErrors() {
	//check for linking success
	GLint linked;
	glGetProgramiv(shaderProgID,GL_LINK_STATUS, &linked);
	if (!linked) 
	{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    glGetProgramiv(shaderProgID, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0)
    {
      infoLog = new GLchar[infoLogLen];
      // error check for fail to allocate memory omitted
      glGetProgramInfoLog(shaderProgID,infoLogLen, &charsWritten, infoLog);
      std::cout << "InfoLog:" << std::endl << infoLog << std::endl;
      delete [] infoLog;
    }
	}
}


//from swiftless.com
char* ShapeGen::readFile(const char* fileName) {
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

 
ShapeGen::~ShapeGen(void)
{
}
