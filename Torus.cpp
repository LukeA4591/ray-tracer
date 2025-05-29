#include "Torus.h"
#include <cmath>

static constexpr float EPSILON   = 1e-4f;
static constexpr float MAX_DIST  = 100.0f;
static constexpr int   MAX_STEPS = 100;
static constexpr float SURF_DIST = 1e-3f;

float Torus::distanceEstimator(const glm::vec3& p) const {
    glm::vec3 q = p - center;
    glm::vec2  v(glm::length(glm::vec2(q.x, q.y)) - Rmaj, q.z);
    return glm::length(v) - Rmin;
}

float Torus::intersect(glm::vec3 p0, glm::vec3 dir) {
    float t = 0.0f;
    for (int i = 0; i < MAX_STEPS && t < MAX_DIST; ++i) {
        glm::vec3 pos = p0 + dir * t;
        float d = distanceEstimator(pos);
        if (d < SURF_DIST)
            return t;
        t += d;
    }
    return -1.0f;
}

glm::vec3 Torus::normal(glm::vec3 p) {
    glm::vec3 P = p - center;
    float u = P.x*P.x + P.y*P.y + P.z*P.z + Rmaj*Rmaj - Rmin*Rmin;

    glm::vec3 n;
    n.x = 4.0f * u * P.x - 8.0f * Rmaj*Rmaj * P.x;
    n.y = 4.0f * u * P.y - 8.0f * Rmaj*Rmaj * P.y;
    n.z = 4.0f * u * P.z;

    return glm::normalize(n);
}
