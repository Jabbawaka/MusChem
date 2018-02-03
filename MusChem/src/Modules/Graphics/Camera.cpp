// Self include files
#include "Modules/Graphics/Camera.h"

// Project include files
#include "GLogger/GLogger.h"
#include "GLheaders.h"
#include "GUtils.h"

#include "Modules/Graphics/Window.h"

#include "Systems/BasicModules.h"

// System include files


Camera::Camera()
{
    _projViewMatrix = glm::ortho
       ((float) -CAMERA_SCREEN_WIDTH / 2.0f, (float) CAMERA_SCREEN_WIDTH / 2.0f,
        (float) -CAMERA_SCREEN_HEIGHT / 2.0f, (float) CAMERA_SCREEN_HEIGHT / 2.0f);

    _size_pix = glm::vec2
       ((float) CAMERA_SCREEN_WIDTH / 2.0f, (float) CAMERA_SCREEN_HEIGHT / 2.0f);

    _pos_pix = glm::vec2(0.0f);
}

void Camera::setSize(Window &window)
{
    _projViewMatrix = glm::ortho
       ((float) -window.getWidth() / 2.0f, (float) window.getWidth() / 2.0f,
        (float) -window.getHeight() / 2.0f, (float) window.getHeight() / 2.0f);

    _size_pix = glm::vec2
       ((float) window.getWidth()/ 2.0f,
        (float) window.getHeight() / 2.0f);
}

void Camera::update(glm::vec2 pos_pix)
{
    _pos_pix = pos_pix;
}

void Camera::zoomOut()
{
    _projViewMatrix = glm::ortho
       ((float) -2 * _size_pix.x, (float) 2 * _size_pix.x,
        (float) -2 * _size_pix.y, (float) 2 * _size_pix.y);
    _size_pix.x = 2 * _size_pix.x;
    _size_pix.y = 2 * _size_pix.y;
}

void Camera::zoomIn()
{
    _projViewMatrix = glm::ortho
       ((float) -0.5 * _size_pix.x, (float) 0.5 * _size_pix.x,
        (float) -0.5 * _size_pix.y, (float) 0.5 * _size_pix.y);
    _size_pix.x = 0.5 * _size_pix.x;
    _size_pix.y = 0.5 * _size_pix.y;
}
