#include "MeshGen.h"


MeshGen::MeshGen()
{
}

Mesh MeshGen::extrusion(double height, glm::vec3 color, std::vector<glm::vec2> pointsIn) {
	std::vector<glm::vec4> poss; 
	std::vector<glm::vec3> colors;
	std::vector<glm::vec4> normals;
	std::vector<int> pointInds;

	for (int i = 0; i < pointsIn.size(); i++) {
		const glm::vec2 pFront = pointsIn[i];
		const glm::vec4 frontHigh = glm::vec4(pFront.x, height, pFront.y, 1);
		const glm::vec4 frontLow = glm::vec4(pFront.x, 0, pFront.y, 1);

		const glm::vec2 pBack = pointsIn[(i + 1) % pointsIn.size()];
		const glm::vec4 backHigh = glm::vec4(pBack.x, height, pBack.y, 1);
		const glm::vec4 backLow = glm::vec4(pBack.x, 0, pBack.y, 1);

    //this makes a square
    poss.push_back(frontHigh); poss.push_back(frontLow);
		poss.push_back(backHigh); poss.push_back(backLow); 

    for (int i = 0; i < 4; i++) {
      colors.push_back(glm::vec3(color)); 
      //colors.push_back(color);
	  }

		const glm::vec3 normal = glm::cross(
				  glm::vec3(backLow) - glm::vec3(frontLow)
				, glm::vec3(frontHigh)- glm::vec3(frontLow)
			);
    for (int i = 0; i < 4; i++) {
      normals.push_back(glm::vec4(normal, 0));
	  }
	}

	for (int i = 0; i < pointsIn.size(); i++) {
    const int frontHighInd = 4 * i;
    const int frontLowInd  = 4 * i + 1;
    const int backHighInd  = 4 * i + 2;
    const int backLowInd   = 4 * i + 3;

    //one triangle
		pointInds.push_back(frontLowInd);
		pointInds.push_back(backHighInd);
		pointInds.push_back(frontHighInd);

    //another triangle
    pointInds.push_back(frontLowInd);
    pointInds.push_back(backLowInd);
    pointInds.push_back(backHighInd);
	}


  //endcap time
  if (isConvex(pointsIn)) {
    const int centerPointIndTop = poss.size();
    const int centerPointIndBottom = poss.size() + 1;
    
    glm::vec2 center = glm::vec2(0.0, 0.0);
    for (int i = 0; i < pointsIn.size(); i++) {
      center = center + pointsIn[i];
	  }
    center = center * (1 / (float) pointsIn.size());

    poss.push_back(glm::vec4(center.x, height, center.y, 1.0f));
    colors.push_back(color);
    normals.push_back(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)); //pointing straight up

    poss.push_back(glm::vec4(center.x, 0.0f, center.y, 1.0f));
    colors.push_back(color);
    normals.push_back(glm::vec4(0.0f, -1.0f, 0.0f, 0.0f)); //pointing straight down

    for (int i = 0; i < pointsIn.size(); i++) {
      //const glm::vec2 pFront = pointsIn[i];
      //const glm::vec2 pBack = pointsIn[(i + 1) % pointsIn.size()];
      poss.push_back(glm::vec4(pointsIn[i].x, height, pointsIn[i].y, 1.0f));
      poss.push_back(glm::vec4(pointsIn[i].x, 0.0f, pointsIn[i].y, 1.0f));
      colors.push_back(glm::vec3(color));
      colors.push_back(glm::vec3(color));
      normals.push_back(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
      normals.push_back(glm::vec4(0.0f, -1.0f, 0.0f, 0.0f));
	  }

    //firstEdgeEndcapPointIndex 
    const int fEEPI = centerPointIndBottom + 1;
    for (int i = 0; i < pointsIn.size(); i++) {
      const int currentIndex = 2 * i + fEEPI;
      const int nextIndex = 2 * ((i + 1) % pointsIn.size()) + fEEPI;

      pointInds.push_back(centerPointIndTop);
      pointInds.push_back(currentIndex);
      pointInds.push_back(nextIndex);

      pointInds.push_back(centerPointIndBottom);
      pointInds.push_back(currentIndex + 1);
      pointInds.push_back(nextIndex + 1);
	  }
  }
 
	return Mesh(poss, colors, normals, pointInds);
}

Mesh MeshGen::surfrev(int sliceCount, glm::vec3 color, std::vector<glm::vec2> points) {
    //and by x I mean r and by y I still mean y (but in cylindrical coordinates)
    std::vector<glm::vec4> poss; 
    std::vector<int> inds;
    for (int i = 0; i < sliceCount; i++) {
      const float theta = ((float) i) * 2.0f * 3.141 / ((float) sliceCount);
      for (int j = 0; j < points.size(); j++) {
        poss.push_back(glm::vec4(
			      cos(theta) * points[j].x
          , points[j].y
          , sin(theta) * points[j].x
          , 1.0f
			  ));
	    }
	  }
    for (int i = 0; i < sliceCount; i++) {
      for (int j = 0; j < points.size(); j++) {
        //Note that it isn't really top and bottom.
		    int rightIndex = ((i + 1) % sliceCount);
        int bottomIndex = ((j + 1) % points.size());
 
        int topLeft = i * points.size() + j; 
		    int topRight = rightIndex * points.size() + j;
        int bottomLeft = i * points.size() + bottomIndex;
        int bottomRight = ((i + 1) % sliceCount) * points.size() + ((j + 1) % points.size());

        //triangles must be arranged counter-clockwise 
		    //when facing in the opposite direction of the normal vector
        inds.push_back(bottomLeft); inds.push_back(bottomRight); inds.push_back(topRight);
        inds.push_back(bottomLeft); inds.push_back(topRight); inds.push_back(topLeft);
	    }
	  }

    const glm::vec2 top = points[0];
    const glm::vec2 bottom = points[points.size() - 1];
    //if (top != bottom) {
    if (true) {
      const int topIndex = poss.size() - 2;
      const int bottomIndex = poss.size() - 1;
      const int firstTriangleIndex = poss.size();

      poss.push_back(glm::vec4(0.0f, top.y, 0.0f, 1.0));
      poss.push_back(glm::vec4(0.0f, bottom.y, 0.0f, 1.0)); 
      for (int i = 0; i < sliceCount; i++) {
        const float theta = ((float) i) * 2.0f * 3.141 / ((float) sliceCount);
        poss.push_back(glm::vec4(
			      cos(theta) * top.x
          , top.y
          , sin(theta) * top.x
          , 1.0f
			  ));
        poss.push_back(glm::vec4(
			      cos(theta) * bottom.x
          , bottom.y
          , sin(theta) * bottom.x
          , 1.0f
			  ));
	    }

      for (int i = 0; i < sliceCount - 1; i++) {
        //note that 'left' and 'right' are all from the perspective of the upper part of the view while
        //looking in the negative y direction
        //this means that the top triangles have to be RLC, and bottom ones must be LRC
        const int topRightIndex    = 2 * i + firstTriangleIndex;
        const int topLeftIndex     = 2 * ((i + 1) % sliceCount) + firstTriangleIndex;
        const int bottomRightIndex = 2 * i +                      1 + firstTriangleIndex;
        const int bottomLeftIndex  = 2 * ((i + 1) % sliceCount) + 1 + firstTriangleIndex;

        inds.push_back(topRightIndex); inds.push_back(topLeftIndex); inds.push_back(topIndex);
        inds.push_back(bottomLeftIndex); inds.push_back(bottomRightIndex); inds.push_back(bottomIndex);
	    }
	  }

    return Mesh(poss, color, inds);
}

bool MeshGen::isConvex(std::vector<glm::vec2> points) {
  if (points.size() < 3) {
      return false;
  }

  float zComp1 = glm::cross(
        glm::vec3(points[1], 0) - glm::vec3(points[0], 0)
      , glm::vec3(points[points.size() - 1], 0) - glm::vec3(points[0], 0)
	  ).z;

  for(int i = 1; i < points.size(); i++) {
      if (!(
		       zComp1 <= 0 
               &&
		       glm::cross(
               glm::vec3(points[(i + 1) % points.size()], 0) - glm::vec3(points[i], 0)
             , glm::vec3(points[i - 1], 0)                   - glm::vec3(points[i], 0)
	         ).z <= 0
			)) {
        return false;
	    }
  }

  return true;
}

Mesh MeshGen::fromFile(std::string filename0) {
  const char* filename1 = (filename0.c_str());
	std::ifstream myfile(filename1);
	//std::ifstream myfile("testsurfrev2.dat");

	std::string furnitureType;

	myfile >> furnitureType;

	if (myfile.is_open()) {
		if (furnitureType == "extrusion") {
			float height;
			int pointCount;

			myfile >> height;
			myfile >> pointCount;

			std::vector<glm::vec2> points;
			for (int i = 0; i < pointCount - 1; i++) {
				float x;
				float z;
				myfile >> x >> z;

				points.push_back(glm::vec2(x, z));
			} 
			return MeshGen::extrusion(height, glm::vec3(1.0f, 1.0f, 1.0f), points);
		} else if (furnitureType == "surfrev") {
      int sliceCount;
      int pointCount;
      myfile >> sliceCount;
      myfile >> pointCount;

      std::vector<glm::vec2> points;
      for (int i = 0; i < pointCount; i++) {
        float r;
        float y;
        myfile >> r >> y;
        points.push_back(glm::vec2(r, y));
	    }
      return MeshGen::surfrev(sliceCount, glm::vec3(1.0f, 1.0f, 1.0f), points);
		}
	}
	std::cout<<"failed to parse: "<<filename0;
}

MeshGen::~MeshGen()
{
}
