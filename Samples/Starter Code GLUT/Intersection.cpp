#include "Intersection.h"

Intersection::Intersection(ShapeGen::ShapeType st, Mesh m, Ray r, glm::mat4 const& tI, glm::mat4 const& t, int matIndIN) {
  matInd = matIndIN;
  distance = -1.0f;

  switch (st) {
    case ShapeGen::MESH: 
      //distance = rcyiInner(glm::vec3(r.start), glm::vec3(r.dir), glm::mat4(1.0f));//tI);
      glm::vec4 p1, p2, p3;
      for (int i = 0; i < m.indices.size(); i += 3) {
        p1 = m.positions[m.indices[i]]; 
        p2 = m.positions[m.indices[i + 1]];
        p3 = m.positions[m.indices[i + 2]];
        const float possibleMinDist = rpiInner(glm::vec3(r.start), glm::vec3(r.dir), glm::vec3(p1), glm::vec3(p2), glm::vec3(p3), tI);
        if ((possibleMinDist > -0.5f && distance < -0.5f) || (possibleMinDist < distance && possibleMinDist > -0.5f)) {
          distance = possibleMinDist;
          tan1 = p2 - p1;
          tan2 = p3 - p1;
        }
      }
    break;
  }
}

Intersection::Intersection(float testDistance) {
  distance = testDistance;
}

glm::vec4 Intersection::closestNormal(glm::vec4 rayDir) {
  glm::vec4 possibleNormal = glm::vec4(glm::cross(glm::vec3(tan1), glm::vec3(tan2)), 0.0f);

  if (glm::dot(possibleNormal, rayDir) < 0) {
    return glm::normalize(-possibleNormal);
  } else {
    return glm::normalize(possibleNormal);
  }
}

bool Intersection::hitSomething() {
  return distance > -0.5f;
}


Intersection::~Intersection(void)
{
}
