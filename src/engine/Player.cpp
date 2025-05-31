#include <SDL3/SDL.h>
#include <engine/Player.h>
#include <engine/Raycaster.h>
#include <iostream>

Player::Player() {
  pos.x = 1.5;
  pos.y = 1.5;
  dir.x = 0;
  dir.y = -1;
}

void Player::key_down(SDL_KeyboardEvent *event) {

  if (movement_state == MOVEMENT_STATES::STILL &&
      turning_state == TURNING_STATES::STILL) {
    if (event->key == SDLK_W) {
      movement_state = MOVEMENT_STATES::TRANSLATING;
      destination = pos + dir;
    }

    if (event->key == SDLK_D) {
      movement_state = MOVEMENT_STATES::TRANSLATING;
      destination = pos + dir.rotated(90);
    }

    if (event->key == SDLK_A) {
      movement_state = MOVEMENT_STATES::TRANSLATING;
      destination = pos + dir.rotated(-90);
    }
    if (event->key == SDLK_S) {
      movement_state = MOVEMENT_STATES::TRANSLATING;
      destination = pos + dir.rotated(180);
    }
  }

  if (turning_state == TURNING_STATES::STILL) {
    if (event->key == SDLK_E) {
      turning_state = TURNING_STATES::TURNING;
      desired_angle += 90;
      turn_dir = 1;
    }
    if (event->key == SDLK_Q) {
      turning_state = TURNING_STATES::TURNING;
      desired_angle -= 90;
      turn_dir = -1;
    }
    if (desired_angle < 0)
      desired_angle += 360;
    if (desired_angle >= 360)
      desired_angle -= 360;
  }
}

/** Gets distance in current direction to desired angle */
float dist_to_desired_angle(float angle, float desired_angle, int turn_dir) {
  float dist;
  if (turn_dir == 1) {
    dist = desired_angle - angle;
  } else if (turn_dir == -1) {
    dist = angle - desired_angle;
  }
  if (dist < 0)
    dist += 360;
  if (dist > 360)
    dist -= 360;

  // std::cout << dist << std::endl;
  return dist;
}

void Player::update(float dt) {
  if (movement_state == MOVEMENT_STATES::TRANSLATING) {
    Vec2 move_dir = destination - pos;
    // std::cout << "destination: " << destination.x << ", " << destination.y
    //           << std::endl;
    pos += move_dir.normalized() * move_speed * dt;
    if (move_dir.get_magnitude() < 0.01) {
      pos.x = destination.x;
      pos.y = destination.y;
      movement_state = MOVEMENT_STATES::STILL;
    }
    // std::cout << "x: " << pos.x << ", y: " << pos.y << std::endl;
  }

  if (turning_state == TURNING_STATES::TURNING) {
    float turn_amnt = turn_speed * turn_dir * dt;
    dir.rotate(turn_amnt);

    if (dist_to_desired_angle(angle, desired_angle, turn_dir) <
        std::abs(turn_amnt)) {
      angle = desired_angle;
      // Ensure cardinal TODO: derive from angle
      dir.x = std::round(dir.x);
      dir.y = std::round(dir.y);
      turning_state = TURNING_STATES::STILL;
    } else {
      angle += turn_amnt;
    }
  }
}

RaycastCamera Player::get_raycast_camera() {
  RaycastCamera cam(120);
  cam.pos.x = pos.x;
  cam.pos.y = pos.y;

  cam.rotate(angle);

  return cam;
}
