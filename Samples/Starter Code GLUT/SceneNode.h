#pragma once 

#include <vector>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "ShapeGen.h"
#include "Mesh.h"
#include "Ray.h"
#include "stubs.h"

class SceneNode
{


public:
  glm::vec3 dispxyz;
  float roty;
  glm::vec3 scalexyz;
  ShapeGen::ShapeType shapeType;
  int matInd;
  bool inheritsColor;

  float nextHeight;
  glm::mat4 cachedTrans;

  std::vector<int> childIDs;

  Mesh meshContents;

  SceneNode(glm::vec3 dispxyz, float roty, glm::vec3 scalexyz, Mesh meshIn, int matIndIN, bool inheritsColor);
  SceneNode(glm::vec3 dispxyz, float roty, glm::vec3 scalexyz, ShapeGen::ShapeType shapeTypeIn, int matIndIN, bool inheritsColor);
  
  void reCacheTrans(glm::mat4 parentTrans, float height, bool isInvis);

  ~SceneNode(void);
};

