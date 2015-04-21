#include "SceneNode.h"

SceneNode::SceneNode(glm::vec3 dispxyzIN, float rotyIN, glm::vec3 scalexyzIN, ShapeGen::ShapeType shapeTypeIn, int matIndIN, bool inheritsColorIN)
{
  dispxyz = dispxyzIN;
  roty = rotyIN;
  scalexyz = scalexyzIN;
  shapeType = shapeTypeIn;
  matInd = matIndIN;
  inheritsColor = inheritsColorIN;

  meshContents = Mesh();
}

SceneNode::SceneNode(glm::vec3 dispxyzIN, float rotyIN, glm::vec3 scalexyzIN, Mesh meshIn, int matIndIN, bool inheritsColorIN)
{
  dispxyz = dispxyzIN;
  roty = rotyIN;
  scalexyz = scalexyzIN;
  meshContents = meshIn;
  matInd = matIndIN;
  inheritsColor = inheritsColorIN;

  shapeType = ShapeGen::MESH;
}

//I guess that each transformation matrix is only applied to children?
void SceneNode::reCacheTrans(glm::mat4 parentTrans, float height, bool isInvis) {
  //move back to origin //not needed since model matrix is already at origin?
  //rotate around y axis roty degrees
  //scale by the amounts specified in scalexyz
  //move back to original location + dispxz + height of current object * scaley

  nextHeight = height + (isInvis ? 0 : scalexyz.y) / 2;
  cachedTrans
	  = parentTrans 
	  * glm::translate(dispxyz)
	  * glm::rotate(glm::mat4(), roty, glm::vec3(0, 1, 0))
    * glm::scale(scalexyz)
	  ;
  return;
}

SceneNode::~SceneNode(void)
{
}
