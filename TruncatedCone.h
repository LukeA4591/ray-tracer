#ifndef H_TRUNCATED_CONE
#define H_TRUNCATED_CONE

#include <glm/glm.hpp>
#include "SceneObject.h"

class TruncatedCone : public SceneObject {
private:
    glm::vec3 center;
    float     r1;
    float     r2;
    float     height; 

public:
    TruncatedCone() {}
    TruncatedCone(glm::vec3 c, float baseR, float topR, float h)
      : center(c), r1(baseR), r2(topR), height(h) {}

    float intersect(glm::vec3 p0, glm::vec3 dir) override;

    glm::vec3 normal(glm::vec3 p) override;
};

#endif // H_TRUNCATED_CONE
