/*--------------------------------------------------------------
* COSC363  Ray Tracer
*
*  The SceneObject class
*  This is a generic type for storing objects in the scene
*  Sphere, Plane etc. must be defined as subclasses of SceneObject.
*  Being an abstract class, this class cannot be instantiated.
-----------------------------------------------------------------*/

#include "SceneObject.h"
#include <glm/glm.hpp>
#include <glm/gtx/vector_query.hpp> 
#include <cmath>

glm::vec3 SceneObject::getColor() {
	return color_;
}

glm::vec3 SceneObject::lighting(glm::vec3 lightPos,
                                glm::vec3 viewVec,
                                glm::vec3 hit)
{
    glm::vec3 N = normal(hit);
    glm::vec3 L = glm::normalize(lightPos - hit);

    float NdotL = glm::max(glm::dot(N, L), 0.0f);
    glm::vec3 diffuse = NdotL * color_;

    glm::vec3 specular(0.0f);
    if (spec_) {
        glm::vec3 R = glm::reflect(-L, N);
        float RdotV = glm::max(glm::dot(R, viewVec), 0.0f);
        float s = powf(RdotV, shin_);
        specular = glm::vec3(s);
    }

    return diffuse + specular;
}

float SceneObject::getReflectionCoeff() {
	return reflc_;
}

float SceneObject::getRefractionCoeff() {
	return refrc_;
}

float SceneObject::getTransparencyCoeff() {
	return tranc_;
}

float SceneObject::getRefractiveIndex() {
	return refri_;
}

float SceneObject::getShininess() {
	return shin_;
}

bool SceneObject::isReflective() {
	return refl_;
}

bool SceneObject::isRefractive() {
	return refr_;
}


bool SceneObject::isSpecular() {
	return spec_;
}


bool SceneObject::isTransparent() {
	return tran_;
}

void SceneObject::setColor(glm::vec3 col) {
	color_ = col;
}

void SceneObject::setReflectivity(bool flag) {
	refl_ = flag;
}

void SceneObject::setReflectivity(bool flag, float refl_coeff) {
	refl_ = flag;
	reflc_ = refl_coeff;
}

void SceneObject::setRefractivity(bool flag) {
	refr_ = flag;
}

void SceneObject::setRefractivity(bool flag, float refr_coeff, float refr_index) {
	refr_ = flag;
	refrc_ = refr_coeff;
	refri_ = refr_index;
}

void SceneObject::setShininess(float shininess) {
	shin_ = shininess;
}

void SceneObject::setSpecularity(bool flag) {
	spec_ = flag;
}

void SceneObject::setTransparency(bool flag) {
	tran_ = flag;
}

void SceneObject::setTransparency(bool flag, float tran_coeff) {
	tran_ = flag;
	tranc_ = tran_coeff;
}