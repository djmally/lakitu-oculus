#pragma once

#include "Mesh.h"
#include "glm/glm.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>


class MeshGen
{
public:
	MeshGen();

	static Mesh extrusion(double height, glm::vec3 color, std::vector<glm::vec2> points);

  static Mesh surfrev(int sliceCount, glm::vec3 color, std::vector<glm::vec2> points);

  static bool isConvex(std::vector<glm::vec2> points);

	static Mesh fromFile(std::string filename);
 
	~MeshGen();
};