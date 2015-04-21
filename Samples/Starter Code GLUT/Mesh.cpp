#include "Mesh.h"

Mesh::Mesh() {
	isEmptyMesh = true;
}

Mesh::Mesh(std::vector<glm::vec4> poss, std::vector<glm::vec3> cols, std::vector<glm::vec4> norms, std::vector<int> inds)
{
	isEmptyMesh = false;

  positions = poss;
  colors = cols;
  normals = norms;
  indices = inds;
}

Mesh::Mesh(std::vector<glm::vec4> poss, glm::vec3 color, std::vector<int> inds0) {
    isEmptyMesh = false;
  //the last two or three indices are ignored if the total number of indices isn't a multiple of three
  std::vector<int> inds1;
  for (int i = 0; i < inds0.size(); i += 3) {
    const glm::vec4 p1 = poss[inds0[i]];
    const glm::vec4 p2 = poss[inds0[i + 1]];
    const glm::vec4 p3 = poss[inds0[i + 2]];

    positions.push_back(p1);
    positions.push_back(p2);
    positions.push_back(p3);

    const glm::vec4 normal = glm::vec4(glm::cross(
          glm::vec3(p2) - glm::vec3(p1)
        , glm::vec3(p3) - glm::vec3(p1)
		)
		, 0.0f);
    for (int j = 0; j < 3; j++) {
      normals.push_back(normal);
	  }

    for (int j = 0; j < 3; j++) {
      inds1.push_back(i + j);
	  }
  }

  for (int i = 0; i < inds0.size(); i++) {
    colors.push_back(color);
  }

  indices = inds1;
}

void Mesh::setupMesh() {

  float* positions0 = new float[positions.size() * 4];
  float* colors0 = new float[colors.size() * 3]; 
  float* normals0 = new float[normals.size() * 4]; 
  for (int i = 0; i < positions.size(); i++) {
    positions0[4 * i + 0] = positions[i].x;
    positions0[4 * i + 1] = positions[i].y;
    positions0[4 * i + 2] = positions[i].z;
    positions0[4 * i + 3] = positions[i].w; 
    colors0[3 * i + 0] = colors[i].x;
    colors0[3 * i + 1] = colors[i].y;
    colors0[3 * i + 2] = colors[i].z;
    normals0[4 * i + 0] = normals[i].x;
    normals0[4 * i + 1] = normals[i].y;
    normals0[4 * i + 2] = normals[i].z;
    normals0[4 * i + 3] = normals[i].w;
  }
  glBindBuffer(GL_ARRAY_BUFFER, ShapeGen::vertBufIDs[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * positions.size() * sizeof(float), positions0, GL_STATIC_DRAW);
  delete(positions0); 
	glBindBuffer(GL_ARRAY_BUFFER, ShapeGen::colBufIDs[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * positions.size() * sizeof(float), colors0, GL_STATIC_DRAW);
  delete(colors0);
	glBindBuffer(GL_ARRAY_BUFFER, ShapeGen::normBufIDs[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * positions.size() * sizeof(float), normals0, GL_STATIC_DRAW);
  delete(normals0);
 
	glEnableVertexAttribArray(ShapeGen::posAttID);
	glEnableVertexAttribArray(ShapeGen::colAttID);
	glEnableVertexAttribArray(ShapeGen::normAttID);

	glBindBuffer(GL_ARRAY_BUFFER, ShapeGen::vertBufIDs[0]);
	glVertexAttribPointer(ShapeGen::posAttID, 4, GL_FLOAT, 0, 0, static_cast<GLvoid*>(0));

	glBindBuffer(GL_ARRAY_BUFFER, ShapeGen::colBufIDs[0]);
	glVertexAttribPointer(ShapeGen::colAttID, 3, GL_FLOAT, 0, 0, static_cast<GLvoid*>(0));

	glBindBuffer(GL_ARRAY_BUFFER, ShapeGen::normBufIDs[0]);
	glVertexAttribPointer(ShapeGen::normAttID, 4, GL_FLOAT, 0, 0, static_cast<GLvoid*>(0));


  GLint* indices0 = new GLint[indices.size()]; 
  for (int i = 0; i < indices.size(); i++) {
    indices0[i] = indices[i];
  }
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ShapeGen::indBufIDs[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLint), indices0, GL_STATIC_DRAW);
  delete(indices0);
}

void Mesh::drawMeshTriangles() {
	//glUniform3f(ShapeGen::lightPosID, lightPos.x, lightPos.y, lightPos.z);
	//glUniformMatrix4fv(ShapeGen::transMatID, 1, GL_FALSE, &trans[0][0]);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

Mesh::~Mesh()
{
    /*
  if (positions == NULL) {
    return;
  }
	if (!isEmptyMesh) {
		delete(positions);
		delete(colors);
		delete(normals);
		delete(indices);
	}
    */
}
