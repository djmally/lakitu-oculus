#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "ShapeGen.h"
#include "MeshGen.h"
#include "SceneNode.h"
#include "Intersection.h"
#include "Ray.h"
#include "EasyBMP.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class Scene
{
  static const int REC_DEPTH = 5;

  std::vector<SceneNode> nodes;

  void renderR(int i, glm::mat4 transform, float parentHeight);

  bool editedSinceLastRender;

  static float PI;

  static glm::vec3 lastLightPos;

  static void renderComponents(Mesh mesh, ShapeGen::ShapeType shapeType, glm::mat4 trans, glm::vec3 lightPos, glm::vec3 lightColor, bool useParentColor);

  //MASSIVE TODO: make it so that Materials contain integers referring to elements in the materials array here, 
  //rather than containing materials themselves.


  //Things which I haven't decided how to initialize yet.
  std::vector<Material> materials;
 
  std::string outFilename;
  std::vector<Light> lights;
  glm::vec3 ambientCol;

  //The following are needed for setting other constants.
  int screenMaxX, screenMaxY;
  glm::vec3 eyePos, eyeDir, eyeUpVec;
  float fovy;
  //The following are calculated from the preceding settings.
  glm::vec3 planeRightVec, planeUpVec, normalizedViewDir;
  float pixelSize;

  void setConsts(
	    int xMaxIn
	  , int yMaxIn
	  , glm::vec3 eyePosIn
	  , glm::vec3 eyeDirIn
	  , glm::vec3 eyeUpVecIn
	  , float fovy);

public:
  void parseForRaytracer(char* filename);
	Scene();
  Scene(char* filename);

  int addNode(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, int matInd, ShapeGen::ShapeType shapeType, int parentNodes[], int parentCount);
  int addNode(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, ShapeGen::ShapeType shapeType, int parentNodes[], int parentCount);
  int addNode1(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, int matInd, ShapeGen::ShapeType shapeType, int parentNode);
  int addNode1(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, ShapeGen::ShapeType shapeType, int parentNode);
  //void addNodeRelative(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, ShapeGen::ShapeType shapeType, int parentOffsets[], int offsetCount);
  int addNode1(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, Mesh meshShape, int parentID, int matID);

  int addTable(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, int parentOffsets[], int offsetCount);
  int addChair(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, int parentOffsets[], int offsetCount);
  int addCabinet(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, int parentOffsets[], int offsetCount);
  int addLamp(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, int parentOffsets[], int offsetCount);

  int addCylinder(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, int matInd, int parentOffsets[], int offsetCount, int baseComponent);

  //this is the GL render function; nothing to do with the raytracer
  void render(glm::mat4 initialTrans, glm::vec3 lightPos);
  static void setTransMat(glm::mat4 m4);
 
  glm::vec3 colorAt(int x, int y);

  glm::vec4 rayColor(Ray r, int recDepth);

  glm::vec4 Scene::shadePt(Ray r, Light l, Intersection i);

  glm::vec4 diffuse(Ray r, Light l, Intersection i);

  glm::vec4 specular(Ray r, Light l, Intersection i);


  Intersection findIntersection(Ray camRay, int parentIndex, glm::mat4 const& parentTrans, Intersection shortestIntersection);

  void renderFile();

	~Scene(void);
};

