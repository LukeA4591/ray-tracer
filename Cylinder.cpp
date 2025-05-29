#include "Cylinder.h"
#include <cmath>

static const float EPSILON = 1e-4f;

float Cylinder::intersect(glm::vec3 p0, glm::vec3 dir) {
    glm::vec3 ro = p0 - center;
    float halfH = height * 0.5f;

    float A = dir.x*dir.x + dir.z*dir.z;
    float B = 2.0f * (ro.x*dir.x + ro.z*dir.z);
    float C = ro.x*ro.x + ro.z*ro.z - radius*radius;
    float disc = B*B - 4.0f*A*C;
    float tSide = -1.0f;

    if (disc > 0.0f) {
        float sq = std::sqrt(disc);
        float t0 = (-B - sq) / (2.0f*A);
        float t1 = (-B + sq) / (2.0f*A);

        if (t0 > EPSILON) tSide = t0;
        else if (t1 > EPSILON) tSide = t1;

        if (tSide > 0.0f) {
            float yHit = ro.y + dir.y * tSide;
            if (yHit < -halfH || yHit > halfH) 
                tSide = -1.0f;
        }
    }

    float tCap = -1.0f;
    if (std::fabs(dir.y) > EPSILON) {

        float t2 = (-halfH - ro.y) / dir.y;
        glm::vec3 p2 = ro + dir * t2;
        if (t2 > EPSILON && (p2.x*p2.x + p2.z*p2.z) <= radius*radius)
            tCap = t2;

        float t3 = ( halfH - ro.y) / dir.y;
        glm::vec3 p3 = ro + dir * t3;
        if (t3 > EPSILON && (p3.x*p3.x + p3.z*p3.z) <= radius*radius) {
            if (tCap < 0.0f || t3 < tCap) tCap = t3;
        }
    }

    if (tSide > 0.0f && tCap > 0.0f) return std::min(tSide, tCap);
    else if (tSide > 0.0f)          return tSide;
    else                            return tCap;
}

glm::vec3 Cylinder::normal(glm::vec3 p) {
    glm::vec3 lp = p - center;
    float halfH = height * 0.5f;
    const float tol = 1e-3f;

    if (std::fabs(lp.y - halfH) < tol)  return glm::vec3(0, +1, 0);
    if (std::fabs(lp.y + halfH) < tol)  return glm::vec3(0, -1, 0);

    glm::vec3 n(lp.x, 0, lp.z);
    return glm::normalize(n);
}
