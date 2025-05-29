// TruncatedCone.cpp
#include "TruncatedCone.h"
#include <initializer_list>
#include <cmath>

static const float EPS = 1e-4f;

float TruncatedCone::intersect(glm::vec3 p0, glm::vec3 dir) {
    glm::vec3 ro = p0 - center;
    float halfH = height * 0.5f;

    float dr = (r2 - r1) / height;

    float u = ro.x, v = ro.z, w = ro.y + halfH;
    float dx = dir.x, dz = dir.z, dy = dir.y;
    float R0 = r1 + dr * w;
    float D  = dr * dy;

    float A = dx*dx + dz*dz - D*D;
    float B = 2.0f * (u*dx + v*dz - R0*D);
    float C = u*u + v*v - R0*R0;

    float tSide = -1.0f;
    float disc = B*B - 4*A*C;
    if (disc > 0.0f) {
        float sq = std::sqrt(disc);
        float t0 = (-B - sq) / (2*A);
        float t1 = (-B + sq) / (2*A);
        // pick nearest positive
        for (float t : {t0, t1}) {
            if (t > EPS) {
                float yHit = ro.y + dy*t;
                if (yHit >= -halfH && yHit <= halfH) {
                    tSide = (tSide < 0.0f) ? t : std::min(tSide, t);
                }
            }
        }
    }

    float tCap = -1.0f;
    if (std::fabs(dy) > EPS) {

        float tb = (-halfH - ro.y) / dy;
        glm::vec3 pb = ro + dir * tb;
        if (tb > EPS && (pb.x*pb.x + pb.z*pb.z) <= r1*r1)
            tCap = tb;

        float tt = ( halfH - ro.y) / dy;
        glm::vec3 pt = ro + dir * tt;
        if (tt > EPS && (pt.x*pt.x + pt.z*pt.z) <= r2*r2)
            tCap = (tCap<0 || tt < tCap) ? tt : tCap;
    }

    // return closest positive
    if (tSide>0 && tCap>0) return std::min(tSide,tCap);
    else if (tSide>0)     return tSide;
    else                   return tCap;
}

glm::vec3 TruncatedCone::normal(glm::vec3 p) {
    // local coords
    glm::vec3 lp = p - center;
    float halfH = height * 0.5f;
    const float tol = 1e-3f;

    // cap normals
    if (std::fabs(lp.y - halfH) < tol)   return glm::vec3(0, +1, 0);
    if (std::fabs(lp.y + halfH) < tol)   return glm::vec3(0, -1, 0);

    // side normal = ∇F = (2x, -2(r1+dr*(y+halfH))*dr, 2z)
    float dr  = (r2 - r1) / height;
    float R0  = r1 + dr*(lp.y + halfH);
    glm::vec3 n(lp.x,
                -R0 * dr,
                 lp.z);
    return glm::normalize(n);
}
