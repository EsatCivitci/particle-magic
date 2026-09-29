
#include "../headers/Camera.h"


Camera::Camera (glm::vec3 newFront,
    glm::vec3 newUp,
    glm::vec3 newPos)
    : front(newFront),
      up(newUp),
      position(newPos)  
{
    right = glm::cross(front, up);
}


void Camera::setCamPos(glm::vec3 pos) {
    position = pos;
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}

glm::vec3 Camera::getCamPosition() const
{
    return position;
}

glm::vec3 Camera::getFrontVector() const 
{
    return front;
}  


