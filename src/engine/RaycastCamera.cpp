#include <engine/Raycaster.h>

RaycastCamera::RaycastCamera(float FOV) {

  dir.x = 0;
  dir.y = -1;

  // PLANE_DIST defined in Raycast.h
  const float PLANE_WIDTH = (PLANE_DIST)*std::tan((FOV * 0.01745329) / 2);

  plane.x = PLANE_WIDTH;
  plane.y = 0;

  // TODO: can rotate here
}

void RaycastCamera::rotate(float degrees) {
  plane.rotate(degrees);
  dir.rotate(degrees);
}
