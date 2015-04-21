#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "glew.h"
#include "ShapeGen.h"

class Mesh
{ 
public:
	std::vector<glm::vec4> positions;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec4> normals;
	std::vector<int> indices;

	bool isEmptyMesh;

	Mesh();
	Mesh(std::vector<glm::vec4> poss, std::vector<glm::vec3> cols, std::vector<glm::vec4> norms, std::vector<int> inds);
  Mesh(std::vector<glm::vec4> poss, glm::vec3 color, std::vector<int> indices);

	void setupMesh();

	//void drawMeshTriangles(glm::mat4 trans, glm::vec3 lightPos);
	void drawMeshTriangles();

	~Mesh();
};