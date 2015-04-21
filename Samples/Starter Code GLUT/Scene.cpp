#include "Scene.h"

float Scene::PI = 3.141593f;
glm::vec3 Scene::lastLightPos = glm::vec3();

//purely for debugging/printing purposes
bool onDiagonal = false;

Scene::Scene()
{
	editedSinceLastRender = true;
}

void Scene::setConsts(int xMaxIn, int yMaxIn, glm::vec3 eyePosIn, glm::vec3 eyeDirIn, glm::vec3 eyeUpVecIn, float fovyIn) {
  screenMaxX = xMaxIn;
  screenMaxY = yMaxIn;
  eyePos = eyePosIn;
  eyeDir = eyeDirIn;
  eyeUpVec = eyeUpVecIn;
  fovy = fovyIn;

  planeRightVec = glm::cross(eyeDir, eyeUpVec);
  planeUpVec = glm::cross(glm::cross(eyeDir, eyeUpVec), eyeDir);
  normalizedViewDir = glm::normalize(eyeDir);
  pixelSize = abs(tan(fovy * 3.1416f / 180)) * 2.0f / screenMaxY;
}

void Scene::parseForRaytracer(char* filename) {
  materials.clear();
  lights.clear();

  std::string line;
  std::ifstream myfile(filename);

  std::string FILE;
  int RESO_X, RESO_Y;
  float EYEP_X, EYEP_Y, EYEP_Z;
  float VDIR_X, VDIR_Y, VDIR_Z;
  float UVEC_X, UVEC_Y, UVEC_Z;
  float FOVY;
  float LPOS_X, LPOS_Y, LPOS_Z;
  float LCOL_R, LCOL_G, LCOL_B;
  float ACOL_R, ACOL_G, ACOL_B;
  
  if (myfile.is_open()) {
    while (getline(myfile, line)) {
      std::istringstream lineStream(line);

      std::string fieldName;
      lineStream >> fieldName;

      if (fieldName == "FILE") {
        lineStream >> FILE;
	    } else if (fieldName == "RESO") {
        lineStream >> RESO_X >> RESO_Y;
	    } else if (fieldName == "EYEP") {
        lineStream >> EYEP_X >> EYEP_Y >> EYEP_Z;
	    } else if (fieldName == "VDIR") {
        lineStream >> VDIR_X >> VDIR_Y >> VDIR_Z;
	    } else if (fieldName == "UVEC") {
        lineStream >> UVEC_X >> UVEC_Y >> UVEC_Z;
	    } else if (fieldName == "FOVY") {
        lineStream >> FOVY;
	    } else if (fieldName == "LPOS") {
        lineStream >> LPOS_X >> LPOS_Y >> LPOS_Z;
	    } else if (fieldName == "LCOL") {
        lineStream >> LCOL_R >> LCOL_G >> LCOL_B;
	    } else if (fieldName == "ACOL") {
        lineStream >> ACOL_R >> ACOL_G >> ACOL_B;
		  } else if (fieldName == "light") {
        float lposx, lposy, lposz;
        float lcolr, lcolg, lcolb;
        lineStream >> lposx >> lposy >> lposz;
        lineStream >> lcolr >> lcolg >> lcolb;
        lights.push_back(Light(glm::vec4(lposx, lposy, lposz, 1.0f), glm::vec3(lcolr, lcolg, lcolb)));
		  } else {
        float diffColR, diffColG, diffColB;
        float specExp, reflParam, transparency, indexOfRefraction;
        float fresCoef1, fresCoef2;

        lineStream >> diffColR >> diffColG >> diffColB;
        lineStream >> specExp >> reflParam >> transparency >> indexOfRefraction;
        lineStream >> fresCoef1 >> fresCoef2;

        materials.push_back(Material(
			      glm::vec3(diffColR, diffColG, diffColB)
          , specExp, reflParam, transparency, indexOfRefraction
          , fresCoef1, fresCoef2
				));
		  } //final else (the material case) 
	  } //while getline
  } //if myfile is open
  setConsts(
      RESO_X
    , RESO_Y
    , glm::vec3(EYEP_X, EYEP_Y, EYEP_Z)
    , glm::vec3(VDIR_X, VDIR_Y, VDIR_Z)
    , glm::vec3(UVEC_X, UVEC_Y, UVEC_Z)
    , FOVY
  );

  outFilename = FILE;
  lights.push_back(Light(glm::vec4(LPOS_X, LPOS_Y, LPOS_Z, 1.0f), glm::vec3(LCOL_R, LCOL_G, LCOL_B)));
  ambientCol = glm::vec3(ACOL_R, ACOL_G, ACOL_B);

  myfile.close();
}

Scene::Scene(char* filename) {
	parseForRaytracer("..\\raytracer_config_sample.txt");

  std::ifstream myfile (filename);

  addNode1(glm::vec3(), 0, glm::vec3(1, 1, 1), 0, ShapeGen::INVIS_SHAPE, -1); 

  if (myfile.is_open()) { 
    int xSize, zSize;
    int numItems;

    myfile >> xSize >> zSize;
    myfile >> numItems; 

    addNode1(glm::vec3(), 0, glm::vec3(xSize * 2, 0.01, zSize * 2), 0, ShapeGen::CUBE, 0); 
    
    std::vector<std::vector<int>> grid = std::vector<std::vector<int>>();
    for (int i = 0; i < xSize; i++) {
      for (int j = 0; j < zSize; j++) {
        grid.push_back(std::vector<int>());
        grid[i * zSize + j].push_back(addNode1(
          glm::vec3(2.0f * (float) i, 1, 2.0f * (float) j)
          , 0
          , glm::vec3(1, 1, 1)
          , 0
          , ShapeGen::INVIS_SHAPE
          , 0
          )
		  );
	    }
	  }
    std::vector<int> tables = std::vector<int>(); 
    std::vector<int> chairs = std::vector<int>(); 
    std::vector<int> cabinets = std::vector<int>(); 
    std::vector<int> lamps = std::vector<int>(); 

    for (int i = 0; i < numItems; i++) {
      std::string furnitureType;
	    myfile >> furnitureType;
		  if (furnitureType == "mesh") {
        std::string meshFilename;
        //std::string textureFilename;
        int matInd;
        int xIndex, zIndex;
        float rotation;
        float xScale, yScale, zScale;
        
        myfile >> meshFilename;
        //myfile >> textureFilename;
        myfile >> matInd; //TODO: get rid of this maybe
        myfile >> xIndex >> zIndex;
        myfile >> rotation;
        myfile >> xScale >> yScale >> zScale;


        const int gridIndex = xIndex * zSize + zIndex;

        float yDisp = 0;
        for (int i = 0; i < (int) grid[gridIndex].size(); i++) {
          yDisp += nodes[grid[gridIndex][i]].scalexyz.y / 2;
        }
   
        //parentIndex = 1 to account for the floor, maybe?
          int parentIndex = 1; 
          if (grid[gridIndex].size() > 0) {
            parentIndex = grid[gridIndex][grid[gridIndex].size() - 1];
        }
       
        Mesh m = MeshGen::fromFile(meshFilename);
        int newNodeIndex = addNode1(
          glm::vec3(xIndex, yDisp, zIndex)
          , rotation
          , glm::vec3(xScale, yScale, zScale)
          , m
          , parentIndex
          , matInd - 1
          );
      } else {
        int matInd;
        int xIndex, zIndex;
        float rotation;
        float xScale, yScale, zScale;

        myfile >> matInd;
        myfile >> xIndex >> zIndex;
        myfile >> rotation;
        myfile >> xScale >> yScale >> zScale;

        const int gridIndex = xIndex * zSize + zIndex;

        float yDisp = 0;
        for (int i = 0; i < (int) grid[gridIndex].size(); i++) {
        yDisp += nodes[grid[gridIndex][i]].scalexyz.y / 2;
        }
        //yDisp += yScale;
       
        int parentIndex = 1; 
        if (grid[gridIndex].size() > 0) {
        parentIndex = grid[gridIndex][grid[gridIndex].size() - 1];
        }
        if (furnitureType == "table") {
        int newNodeIndex = addNode1(
          glm::vec3(0, yDisp, 0)
        , rotation
        , glm::vec3(xScale, yScale, zScale)
        , matInd
        , ShapeGen::INVIS_SHAPE
        , parentIndex
        );
        tables.push_back(newNodeIndex);
        grid[gridIndex].push_back(newNodeIndex);
        } else if (furnitureType == "chair") {
        int newNodeIndex = addNode1(
          glm::vec3(0, yDisp, 0)
        , rotation
        , glm::vec3(xScale, yScale, zScale)
        , matInd
        , ShapeGen::INVIS_SHAPE
        , parentIndex
        );
        chairs.push_back(newNodeIndex);
        grid[gridIndex].push_back(newNodeIndex);
        } else if (furnitureType == "cabinet") {
        int newNodeIndex = addNode1(
          //glm::vec3(2.0f * (float) xIndex * xScale, yDisp, 2.0f * (float) zIndex * zScale)
          glm::vec3(0, yDisp, 0)
        , rotation
        , glm::vec3(xScale, yScale, zScale)
        , matInd
        , ShapeGen::INVIS_SHAPE
        , parentIndex
        );
        cabinets.push_back(newNodeIndex);
        grid[gridIndex].push_back(newNodeIndex);
        } else if (furnitureType == "lamp") {
        int newNodeIndex = addNode1(
          //glm::vec3(2.0f * (float) xIndex * xScale, yDisp, 2.0f * (float) zIndex * zScale)
          glm::vec3(0, yDisp, 0)
        , rotation
        , glm::vec3(xScale, yScale, zScale)
        , matInd
        , ShapeGen::INVIS_SHAPE
        , parentIndex
        );
        lamps.push_back(newNodeIndex);
        grid[gridIndex].push_back(newNodeIndex);
        }
      }
    } //shape loop
    myfile.close();

    if (tables.size() > 0) {
      int tableArchetype = addTable(
          glm::vec3(0, 0, 0)
        , 0.0f
        , glm::vec3(1, 1, 1)
        , &tables[0]
        , tables.size()
        );
    }
    if (chairs.size() > 0) {
      int chairArchetype = addChair(
          glm::vec3(0, 0, 0)
        , 0.0f
        , glm::vec3(1, 1, 1)
        , &chairs[0]
        , chairs.size()
        );
	  }
	  if (cabinets.size() > 0) {
      int cabinetArchetype = addCabinet(
          glm::vec3(0, 0, 0)
        , 0.0f
        , glm::vec3(1, 1, 1)
        , &cabinets[0]
        , cabinets.size()
        );
    }
	  if (lamps.size() > 0) {
      int lampsArchetype = addLamp(
          glm::vec3(0, 0, 0)
        , 0.0f
        , glm::vec3(1, 1, 1)
        , &lamps[0]
        , lamps.size()
        );
    }
  } else { //if myfile is open
    std::cout << "unable to open file";
    exit(1);
  }
}

int Scene::addNode(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, int matInd, ShapeGen::ShapeType shapeType, int parentIDs[], int parentCount) {
	editedSinceLastRender = true;

  int returnLaterIndex = nodes.size();

  nodes.push_back(SceneNode(dispxyz, roty, scaleXYZ, shapeType, matInd, false));

  for (int i = 0; i < parentCount; i++) {
    if (parentIDs[i] >= 0) {
      nodes[parentIDs[i]].childIDs.push_back(returnLaterIndex);
    }
  }

  return returnLaterIndex;
}

int Scene::addNode(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, ShapeGen::ShapeType shapeType, int parentIDs[], int parentCount) {
	editedSinceLastRender = true;

  int returnLaterIndex = nodes.size();

  //While debugging, check whether or not this addNode method ends up getting used.
  nodes.push_back(SceneNode(dispxyz, roty, scaleXYZ, shapeType, 0, true));

  for (int i = 0; i < parentCount; i++) {
    if (parentIDs[i] >= 0) {
      nodes[parentIDs[i]].childIDs.push_back(returnLaterIndex);
    }
  }

  return returnLaterIndex;
}

int Scene::addNode1(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, int matInd, ShapeGen::ShapeType shapeType, int parentID) {
	editedSinceLastRender = true;

  nodes.push_back(SceneNode(dispxyz, roty, scaleXYZ, shapeType, matInd, false));

  if (parentID >= 0) {
    nodes[parentID].childIDs.push_back(nodes.size() - 1);
  }

  return nodes.size() - 1;
}

int Scene::addNode1(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, ShapeGen::ShapeType shapeType, int parentID) {
	editedSinceLastRender = true;

  nodes.push_back(SceneNode(dispxyz, roty, scaleXYZ, shapeType, 0, true));

  if (parentID >= 0) {
    nodes[parentID].childIDs.push_back(nodes.size() - 1);
  }

  return nodes.size() - 1;
}

int Scene::addNode1(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, Mesh meshShape, int parentID, int matID) {
	editedSinceLastRender = true;

	nodes.push_back(SceneNode(dispxyz, roty, scaleXYZ, meshShape, matID, false));

	if (parentID >= 0) {
		nodes[parentID].childIDs.push_back(nodes.size() - 1);
	}

	return nodes.size() - 1;
}

int Scene::addTable(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, int parentOffsets[], int offsetCount) { 
  int centralTableNode = addNode(dispxyz, roty, scaleXYZ, ShapeGen::INVIS_SHAPE, parentOffsets, offsetCount);
  addNode1(glm::vec3(1.0f, 0.0, 1.0f), 0.0f, glm::vec3(0.2f, 1.0f, 0.2f), ShapeGen::CUBE, centralTableNode);
  addNode1(glm::vec3(-1.0f, 0.0, 1.0f), 0.0f, glm::vec3(0.2f, 1.0f, 0.2f), ShapeGen::CUBE, centralTableNode);
  addNode1(glm::vec3(1.0f, 0.0, -1.0f), 0.0f, glm::vec3(0.2f, 1.0f, 0.2f), ShapeGen::CUBE, centralTableNode);
  addNode1(glm::vec3(-1.0f, 0.0, -1.0f), 0.0f, glm::vec3(0.2f, 1.0f, 0.2f), ShapeGen::CUBE, centralTableNode);
  addNode1(glm::vec3(0.0f, 1.0, 0.0f), 0.0f, glm::vec3(1.0f, 0.2f, 1.0f), ShapeGen::CUBE, centralTableNode);
  return centralTableNode;
}

int Scene::addChair(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, int parentOffsets[], int offsetCount) {
  int centralChairNode = addNode(dispxyz, roty, scaleXYZ, ShapeGen::INVIS_SHAPE, parentOffsets, offsetCount);
  addNode1(glm::vec3(1.0f, 0.0f, 1.0f), 0.0f, glm::vec3(0.2f, 1.0f, 0.2f), ShapeGen::CUBE, centralChairNode);
  addNode1(glm::vec3(-1.0f, 0.0f, 1.0f), 0.0f, glm::vec3(0.2f, 1.0f, 0.2f), ShapeGen::CUBE, centralChairNode);
  addNode1(glm::vec3(1.0f, 0.0f, -1.0f), 0.0f, glm::vec3(0.2f, 1.0f, 0.2f), ShapeGen::CUBE, centralChairNode);
  addNode1(glm::vec3(-1.0f, 0.0f, -1.0f), 0.0f, glm::vec3(0.2f, 1.0f, 0.2f), ShapeGen::CUBE, centralChairNode);
  addNode1(glm::vec3(0.0f, 0.8f, 0.0f), 0.0f, glm::vec3(1.0f, 0.2f, 1.0f), ShapeGen::CUBE, centralChairNode);
  addNode1(glm::vec3(1.0f, 2.0f, 1.0f), 0.0f, glm::vec3(0.2f, 1.0f, 0.2f), ShapeGen::CUBE, centralChairNode);
  addNode1(glm::vec3(1.0f, 2.0f, -1.0f), 0.0f, glm::vec3(0.2f, 1.0f, 0.2f), ShapeGen::CUBE, centralChairNode);
  return centralChairNode;
}

int Scene::addCabinet(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, int parentOffsets[], int offsetCount) { 
  int centralCabinetNode = addNode(dispxyz, roty, scaleXYZ, ShapeGen::INVIS_SHAPE, parentOffsets, offsetCount);
  addNode1(glm::vec3(1.0f, 1.0f, 0.0f), 0.0f, glm::vec3(0.1f, 2.0f, 1.0f), ShapeGen::CUBE, centralCabinetNode);
  addNode1(glm::vec3(0.0f, 1.0f, 1.0f), 0.0f, glm::vec3(1.0f, 2.0f, 0.1f), ShapeGen::CUBE, centralCabinetNode);
  addNode1(glm::vec3(0.0f, 1.0f, -1.0f), 0.0f, glm::vec3(1.0f, 2.0f, 0.1f), ShapeGen::CUBE, centralCabinetNode);
  addNode1(glm::vec3(0.0f, -1.0f, 0.0f), 0.0f, glm::vec3(1.0f, 0.2f, 1.0f), ShapeGen::CUBE, centralCabinetNode);
  addNode1(glm::vec3(0.0f, 0.9f, 0.0f), 0.0f, glm::vec3(1.0f, 0.1f, 1.0f), ShapeGen::CUBE, centralCabinetNode);
  addNode1(glm::vec3(0.0f, 3.0f, 0.0f), 0.0f, glm::vec3(1.0f, 0.1f, 1.0f), ShapeGen::CUBE, centralCabinetNode);
  addNode1(glm::vec3(-1.4f, 1.0f, 0.4f), 45.0f, glm::vec3(0.1f, 2.0f, 1.0f), ShapeGen::CUBE, centralCabinetNode);
  return centralCabinetNode;
}

int Scene::addLamp(glm::vec3 dispxyz, float roty, glm::vec3 scaleXYZ, int parentOffsets[], int offsetCount) { 
  int centralLampNode = addNode(dispxyz, roty, scaleXYZ, ShapeGen::INVIS_SHAPE, parentOffsets, offsetCount);
  addNode1(glm::vec3(0.0f, 1.0f, 0.0f), 19.0f, glm::vec3(0.1f, 1.0f, 0.1f), ShapeGen::CUBE, centralLampNode);
  addNode1(glm::vec3(0.0f, 1.8f, 0.0f), 0.0f, glm::vec3(0.2f, 0.2f, 0.2f), ShapeGen::CUBE, centralLampNode);
  addNode1(glm::vec3(0.0f, 1.8f, 0.0f), 0.0f, glm::vec3(0.2f, 0.2f, 0.2f), ShapeGen::CUBE, centralLampNode);
  addNode1(glm::vec3(0.5f, 2.0f, 0.0f), 0.0f, glm::vec3(0.1f, 0.4f, 0.5f), ShapeGen::CUBE, centralLampNode);
  addNode1(glm::vec3(-0.5f, 2.0f, 0.0f), 0.0f, glm::vec3(0.1f, 0.4f, 0.5f), ShapeGen::CUBE, centralLampNode);
  addNode1(glm::vec3(0.0f, 2.0f, 0.5f), 0.0f, glm::vec3(0.5f, 0.4f, 0.1f), ShapeGen::CUBE, centralLampNode);
  addNode1(glm::vec3(0.0f, 2.0f, -0.5f), 0.0f, glm::vec3(0.5f, 0.4f, 0.1f), ShapeGen::CUBE, centralLampNode);
  addNode1(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec3(0.4f, 0.3f, 0.4f), ShapeGen::CUBE, centralLampNode);
  return centralLampNode;
}
 
void Scene::render(glm::mat4 initialTrans, glm::vec3 lightPos) {
  renderR(0, initialTrans, 1);

  lastLightPos = lightPos;

  editedSinceLastRender = false;
}

void Scene::renderR(int i, glm::mat4 parentTrans, float parentHeight) {
  //if (editedSinceLastRender) {
	  nodes[i].reCacheTrans(parentTrans, parentHeight, nodes[i].shapeType == ShapeGen::INVIS_SHAPE);
  //}

	renderComponents(nodes[i].meshContents, nodes[i].shapeType, nodes[i].cachedTrans, lastLightPos, glm::vec3(materials[nodes[i].matInd].diffCol), nodes[i].inheritsColor);

  //float objectHeight = nodes[i].shapeType == ShapeGen::INVIS_SHAPE ? 0 : nodes[i].scalexyz.y * 2;
  //The recursion only terminates on nodes whose childIDs std::vector is empty.
	for(int j = 0; j < nodes[i].childIDs.size(); j++) {
		renderR(
		  	nodes[i].childIDs[j]
		  , nodes[i].cachedTrans
			, nodes[i].nextHeight
	  );
	}
}

void Scene::setTransMat(glm::mat4 m4) {   
	glUniformMatrix4fv(ShapeGen::transMatID, 1, GL_FALSE, &m4[0][0]);
}

void Scene::renderComponents(Mesh m, ShapeGen::ShapeType shapetype, glm::mat4 trans, glm::vec3 lightPos, glm::vec3 matColor, bool useParentColor) {
	glUniform3f(ShapeGen::lightPosID, lightPos.x, lightPos.y, lightPos.z);
  if (!useParentColor) {
    glUniform3f(ShapeGen::matColorID, matColor.x, matColor.y, matColor.z);
  }
  glUniform3f(ShapeGen::matColorID, 1.0f, 1.0f, 1.0f);

  switch (shapetype) {
  case ShapeGen::INVIS_SHAPE:
    return;
  }

  //--
  setTransMat(trans);
  switch(shapetype) {
  case ShapeGen::CUBE:
    ShapeGen::setupCUBE();
	  glDrawElements(GL_TRIANGLES, 3 * 2 * 6, GL_UNSIGNED_INT, 0);
    break;
  case ShapeGen::MESH: 
    m.setupMesh();
	  m.drawMeshTriangles();
    break;
  }
}

glm::vec3 Scene::colorAt(int x, int y) {
  const glm::vec3 center = eyePos + normalizedViewDir;
  const glm::vec3 displacX = planeRightVec * (float)(x - screenMaxX / 2) * pixelSize;
  const glm::vec3 displacY = planeUpVec * (float)(y - screenMaxY / 2) * pixelSize;
  const glm::vec3 pvNormalized = glm::normalize(center + displacX + displacY - eyePos); 
  const Ray pixelRay = Ray(glm::vec4(eyePos, 1.0f), glm::vec4(pvNormalized, 0.0f));

  //std::cout << "trying a pixel: " << x << ", " << y << " | ";
  return glm::vec3(rayColor(pixelRay, REC_DEPTH));
}

glm::vec4 Scene::rayColor(Ray r, int recDepth) {
  Intersection closest = findIntersection(r, 0, glm::mat4(1.0f), Intersection(-1.0f));

  if (closest.hitSomething()) {
    glm::vec4 colorTotal = glm::vec4(ambientCol, 0.0f);

    float closestReflParam = materials[closest.matInd].reflParam;
    glm::vec4 closestPoint = r.start + closest.distance * r.dir * 0.99;
    if (closestReflParam > 0.001 && recDepth > 0) {
      colorTotal += rayColor(Ray(closestPoint, glm::reflect(r.dir, closest.closestNormal(r.dir))), recDepth - 1) * closestReflParam;
	  }

	  for (int i = 0; i < lights.size(); i++) {
      colorTotal += shadePt(r, lights[i], closest);
	  }

    return colorTotal;
  } else {
    return glm::vec4(0, 0, 0, 1.0f);
  }
}

glm::vec4 Scene::shadePt(Ray incident, Light l, Intersection i) {
    incident = Ray(incident.start, glm::normalize(incident.dir));
	  const glm::vec4 camRayIntersPoint = incident.start + i.distance * incident.dir * 0.999;
    const Ray lightRay = Ray(l.pos, camRayIntersPoint - l.pos);
    Intersection lightInters = findIntersection(lightRay, 0, glm::mat4(1.0f), Intersection(-1.0f));

	  if (lightInters.hitSomething() && lightInters.distance <= glm::distance(camRayIntersPoint, l.pos)) { 
      return glm::vec4(0, 0, 0, 0);
	  } else {
      return diffuse(incident, l, i) + 0.0f * specular(incident, l, i);
	  }
}
    
glm::vec4 Scene::diffuse( Ray incident , Light l , Intersection i ) {
  const glm::vec4 lightDir = normalize(incident.start + incident.dir * i.distance - l.pos);
  glm::vec4 out = dot(lightDir, normalize(i.closestNormal(lightDir))) * glm::vec4(l.col, 1.0f) * materials[i.matInd].diffCol;
  //std::cout << out;
  return out;
}

glm::vec4 Scene::specular( Ray incident , Light l , Intersection i ) {
    const glm::vec4 lightDir = normalize(incident.start + incident.dir * i.distance - l.pos);
	  return glm::vec4(l.col, 1.0f) * pow(glm::dot(i.closestNormal(lightDir), normalize(incident.dir + lightDir)), materials[i.matInd].specExp);
}

Intersection Scene::findIntersection(Ray camRay, int i, const glm::mat4 &parentTrans, Intersection shortestIntersection) { 
//    i = 11; //TODO: remove; testing only

    //WARNING: BLATANT MISUSE OF THE SceneNode.reCacheTrans MEMBER FUNCTION'S "parentHeight" PARAMETER
	  nodes[i].reCacheTrans(parentTrans, -1.0f, nodes[i].shapeType == ShapeGen::INVIS_SHAPE);
	  Intersection possiblyShortest = Intersection(
		    nodes[i].shapeType
			, nodes[i].meshContents
			, camRay
			, glm::inverse(nodes[i].cachedTrans)
			, nodes[i].cachedTrans
      , nodes[i].matInd
			);

		if ((possiblyShortest.hitSomething() && !shortestIntersection.hitSomething()) 
			  || (possiblyShortest.distance < shortestIntersection.distance && possiblyShortest.hitSomething())) {
      shortestIntersection = possiblyShortest;
		}
    
    for(int j = 0; j < nodes[i].childIDs.size(); j++) {
      int childID = nodes[i].childIDs[j];
      possiblyShortest = findIntersection(
          camRay
		    , childID
        , nodes[i].cachedTrans
        , shortestIntersection
      );
      if ((possiblyShortest.hitSomething() && !shortestIntersection.hitSomething()) 
          || (possiblyShortest.distance < shortestIntersection.distance && possiblyShortest.hitSomething())) {
        shortestIntersection = possiblyShortest;
      }
    }

    return shortestIntersection;
}

int intCol(float f) {
  int i = (int) (f * 255);
  return i > 255 ? 255 : (i < 0 ? 0 : i);
}

void Scene::renderFile() {
  BMP output; 
  output.SetSize(screenMaxX, screenMaxY);
  output.SetBitDepth(24);

  for (int i = 0; i < screenMaxX; i++) {
    for (int j = 0; j < screenMaxY; j++) {
      int jRev = screenMaxY - j - 1;
      const vec3 colorAtPos = colorAt(i, jRev);
      output(i, j)->Red = intCol(colorAtPos.x);
      output(i, j)->Green = intCol(colorAtPos.y);
      output(i, j)->Blue = intCol(colorAtPos.z);
	  }
	  std::cout << i;
  }

  const char *c = outFilename.c_str();
  output.WriteToFile(c);
}

Scene::~Scene(void)
{
}