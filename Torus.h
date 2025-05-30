#ifndef H_TORUS
#define H_TORUS

#include <glm/glm.hpp>
#include "SceneObject.h"

class Torus : public SceneObject {
private:
    glm::vec3 center;
    float     Rmaj; 
    float     Rmin;

    float distanceEstimator(const glm::vec3& p) const;

public:
    Torus() {}
    Torus(glm::vec3 c, float majorR, float minorR)
      : center(c), Rmaj(majorR), Rmin(minorR) {}

    float       intersect(glm::vec3 p0, glm::vec3 dir) override;
    glm::vec3   normal   (glm::vec3 p)       override;
};

#endif
