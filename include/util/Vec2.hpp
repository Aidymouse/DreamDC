#pragma once

#include <cmath>
#include <iostream>

class Vec2 {

public:
  float x;
  float y;

  Vec2(float x_in, float y_in) {
    x = x_in;
    y = y_in;
  };

  Vec2() { Vec2(0, 0); };

  void operator=(Vec2 a) {
    x = a.x;
    y = a.y;
  }

  /** Addition and assignmnet += */
  Vec2 &operator+=(Vec2 rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
  };
  Vec2 &operator+=(float rhs) {
    x += rhs;
    y += rhs;
    return *this;
  };

  /** Addition + */
  friend Vec2 operator+(Vec2 lhs, const Vec2 &rhs) {
    return Vec2(lhs.x + rhs.x, lhs.y + rhs.y);
  };
  friend Vec2 operator+(Vec2 lhs, const float &rhs) {
    return Vec2(lhs.x + rhs, lhs.y + rhs);
  };
  friend Vec2 operator+(const float &lhs, Vec2 rhs) {
    return Vec2(lhs + rhs.x, lhs + rhs.y);
  };

  /** Subtraction + */
  friend Vec2 operator-(Vec2 lhs, const Vec2 &rhs) {
    return Vec2(lhs.x - rhs.x, lhs.y - rhs.y);
  };
  friend Vec2 operator-(Vec2 lhs, const float &rhs) {
    return Vec2(lhs.x - rhs, lhs.y - rhs);
  };

  /** Times * */
  friend Vec2 operator*(Vec2 lhs, const float &rhs) {
    return Vec2(lhs.x * rhs, lhs.y * rhs);
  };
  friend Vec2 operator*(const float &lhs, Vec2 rhs) {
    return Vec2(lhs * rhs.x, lhs * rhs.y);
  };
  friend Vec2 operator/(Vec2 lhs, const float &rhs) {
    return Vec2(lhs.x / rhs, lhs.y / rhs);
  };

  float get_magnitude() { return sqrt(x * x + y * y); }

  /** Normalization */
  void normalize() {
    float mag = get_magnitude();
    x /= mag;
    y /= mag;
  }

  Vec2 normalized() {
    float mag = get_magnitude();
    return Vec2(x / mag, y / mag);
  }

  /** Rotation */
  void rotate_rad(float r) {
    float newX = std::cos(r) * x + std::sin(r) * y;
    if (abs(newX) < 0.0001)
      newX = 0;
    float newY = std::sin(r) * x + std::cos(r) * y;
    if (abs(newY) < 0.0001)
      newY = 0;
    x = newX;
    y = newY;
  }

  void rotate(float degrees) { rotate_rad(degrees * 0.01745329); }

  Vec2 rotated_rad(float r) {
    float newX = std::cos(r) * x + std::sin(r) * y;
    if (abs(newX) < 0.0001)
      newX = 0;
    float newY = std::sin(r) * x + std::cos(r) * y;
    if (abs(newY) < 0.0001)
      newY = 0;
    return Vec2(newX, newY);
  }

  Vec2 rotated(float degrees) { return rotated_rad(degrees * 0.01745329); }

  // TODO: dot product or smn. DONT CARE!
};
