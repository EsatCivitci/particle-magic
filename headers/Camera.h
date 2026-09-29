#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp> 
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
    Camera (glm::vec3 newFront,
            glm::vec3 newUp,
            glm::vec3 newPos);

    glm::mat4 getViewMatrix() const;
    glm::vec3 getCamPosition() const;
    glm::vec3 getFrontVector() const;

    void setCamPos(glm::vec3 pos);

private:
    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 front;
    glm::vec3 right;
    float t = 0.5f; 

};

#endif 

