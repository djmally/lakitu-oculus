#include "stubs.h"

using namespace glm;

double Test_RaySphereIntersect(vec3 const& P0, vec3 const& V0, mat4 const& T) {
  return (double) rsiInner(P0, V0, inverse(T));
}

//v0 must be normalized for correct distance measurements
float rsiInner(vec3 const& p0, vec3 const& v0, mat4 const& tI) { 
  const vec4 p0T = tI * vec4(p0, 1.0f);
  const vec4 v0T = tI * vec4(normalize(v0), 0.0f);

  QuadFormu qf = QuadFormu(1
	  , 2 * dot(v0T, p0T)
    , p0T.x * p0T.x + p0T.y * p0T.y + p0T.z * p0T.z - 1
	  );

  if (qf.discriminant < 0) {
      return -1;
  } else {
      const float pa = qf.plusAnswer();
      const float ma = qf.minusAnswer();
	          if (pa < 0 && ma < 0)    { return -1; } 
		  else  if (pa >= 0 && ma >= 0)  { return QuadFormu::minF(pa, ma); } 
		  else                           { return QuadFormu::maxF(pa, ma); }
  }
}

double Test_RayPolyIntersect(vec3 const& P0, vec3 const& V0, vec3 const& p1, vec3 const& p2, vec3 const& p3, mat4 const& T) {
  return rpiInner( P0, V0 , p1, p2, p3 , inverse(T));
}

float rpiInner(vec3 const& p0, vec3 const& v0
			   , vec3 const& tp0, vec3 const& tp1, vec3 const& tp2
			   , mat4 const& tI) {
  const vec4 pT = tI * vec4(p0, 1.0f);
  const vec4 vT = tI * vec4(normalize(v0), 0.0f);

  const vec3 ts1 = tp1 - tp0;
  const vec3 ts2 = tp2 - tp0;

  //h
  const vec3 tNorm = cross(vec3(vT), ts2);

  const float dotNormSide = dot(tNorm, ts1);

  if (dotNormSide > -0.00001 && dotNormSide < 0.00001) {
    return -1;
  }

  //f
  const float dNSInv = 1 / dotNormSide;

  //s
  const vec3 fromACorner = vec3(pT) - tp0;

  const float u = dNSInv * dot(fromACorner, tNorm);

  if (u < 0.0 || u > 1.0) {
    return -1;
  }

  const vec3 q = cross(fromACorner, ts1);

  const float v = dNSInv * dot(vec3(vT), q);

  if (v < 0.0 || u + v > 1.0) {
    return -1;
  }

  const float t = dNSInv * dot(ts2, q);

  if (t > 0) {
    return t;
  } else {
    return -1;
  }
}

double Test_RayCubeIntersect(vec3 const& P0, vec3 const& V0, mat4 const& T) { 
  return (double) rciInner(P0, V0, inverse(T));
}

float rciInner(vec3 const& p0, vec3 const& v0, mat4 const& tI) {
  const vec4 p0T = tI * vec4(p0, 1.0f);
  const vec4 v0T = tI * vec4(normalize(v0), 0.0f);

  const float bmin = -0.5f , bmax = 0.5f;
  const float
      t0x = (bmin - p0T.x)/v0T.x, t1x = (bmax - p0T.x)/v0T.x,
      t0y = (bmin - p0T.y)/v0T.y, t1y = (bmax - p0T.y)/v0T.y,
      t0z = (bmin - p0T.z)/v0T.z, t1z = (bmax - p0T.z)/v0T.z;

  if (t0x > t1y || t0y > t1x) {
    return -1;
  }

  const float tmin2 = QuadFormu::maxF(t0x, t0y);
  const float tmax2 = QuadFormu::minF(t1x, t1y);

  if (tmin2 > t1z || t0z > tmax2) {
    return -1;
  }

  const float tmin3 = QuadFormu::maxF(tmin2, t0z);
  const float tmax3 = QuadFormu::minF(tmax2, t1z);

  float answer = QuadFormu::minF(tmin3, tmax3);
  std::cout << answer << " ";
  return answer;
}

double Test_RayCylinderIntersect(vec3 const& P0, vec3 const& V0, mat4 const& T) {
  double returnLater = (double) rcyiInner(P0, V0, inverse(T));
  std::cout << returnLater;
  return returnLater;
}

float rcyiInner(vec3 const& p0, vec3 const& v0, mat4 const& tI) {
  const float CYL_BOTT = -0.5;
  const float CYL_TOP = 0.5;

  const vec4 p0T = tI * vec4(p0, 1.0f);
  const vec4 v0T = tI * vec4(normalize(v0), 0.0f);
 
  QuadFormu qf = QuadFormu(1
	  , 2 * dot(vec2(v0T.x, v0T.z), vec2(p0T.x, p0T.z))
    , p0T.x * p0T.x + p0T.z * p0T.z - 0.25
	  );

  if (qf.discriminant < 0) {
    return -1;
  }

  const float tp = qf.plusAnswer();
  const float tm = qf.minusAnswer();

  const float yp = p0.y + v0.y * tp;
  const float ym = p0.y + v0.y * tm;

  if (tp < 0 && tm < 0) { 
    return -1; 
  } 

  float closestCircleDist;
  if (tm >= 0 && isConstrainedInclu(ym, CYL_BOTT, CYL_TOP)) {
	  closestCircleDist = tm;
  } else if (tp >= 0 && isConstrainedInclu(yp, CYL_BOTT, CYL_TOP)) {
	  closestCircleDist = tp;
  } else {
    closestCircleDist = 999999999;
  }

  const float t0y = (CYL_BOTT - p0T.y)/v0T.y;
	const float t1y = (CYL_TOP - p0T.y)/v0T.y;

  const float r0 = length(vec2(p0.x + v0.x * t0y, p0.z + v0.z * t0y));
  const float r1 = length(vec2(p0.x + v0.x * t1y, p0.z + v0.z * t1y));

  if (v0T.y != 0) {
    float closestEndcapDist;
    if (t0y < 0 && t1y < 0) { 
      return -1;
    } if (t0y >= 0 && r0 <= 1) {
      if (t1y >= 0 && r1 <= 1) {
        closestEndcapDist = QuadFormu::minF(t0y, t1y);
      } else {
        closestEndcapDist = t0y;
      }
    } else if (t1y >= 0 && r1 <= 1) {
      closestEndcapDist = t0y;
    } else {
      closestEndcapDist = 99999999;
      //return -1;
    }
    return QuadFormu::minF(closestCircleDist, closestEndcapDist);
  } else {
    return closestCircleDist;
  }
}

bool isConstrainedInclu(float f, float min, float max) {
  return f >= min && f <= max;
}