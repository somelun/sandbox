#pragma once

#include <iostream>
#include "vec.h"

using mat_t = float;

constexpr float kPI = 3.14159265359f;
constexpr float kPI180 = kPI / 180.0f;
constexpr float k180PI = 180.0f / kPI;

#define degreesToRadians(x) (x * kPI180)
#define radiansToDegrees(x) (x * k180PI)

class mat4 {
public:
    mat4() {};
    mat4(mat_t a00, mat_t a01, mat_t a02, mat_t a03,
         mat_t a10, mat_t a11, mat_t a12, mat_t a13,
         mat_t a20, mat_t a21, mat_t a22, mat_t a23,
         mat_t a30, mat_t a31, mat_t a32, mat_t a33);

    mat_t& at(int row, int column);
    const mat_t& at(int row, int column) const;

    mat4 operator*(const mat4& m) const;
    vec4f operator*(const vec4f& v) const;
    vec3f operator*(const vec3f& v) const;

    mat4 transpose();
    mat4 inverse();

    friend std::ostream& operator<<(std::ostream& s, const mat4& m);

private:
    mat4 adjoint();
    mat_t cofactor(size_t r, size_t c);
    mat_t minor(size_t r, size_t c);

    mat_t data_[16] = {0};
};

[[maybe_unused]]
static mat4 mat4_identity() {
    mat4 m = {};
    m.at(0, 0) = m.at(1, 1) = m.at(2, 2) = m.at(3, 3) = 1.0f;
    return m;
}

[[maybe_unused]]
static mat4 mat4_orthographic(
        const float& left,
        const float& right,
        const float& top,
        const float& bottom,
        const float &near,
        const float& far
    ) {
    mat4 m = {};

    m.at(0, 0) = 2.0f / (right - left);
    m.at(1, 1) = 2.0f / (top - bottom);
    m.at(2, 2) = -2.0f / (far - near);
    m.at(3, 3) = 1.0f;
    m.at(0, 3) = -(right + left) / (right - left);
    m.at(1, 3) = -(top + bottom) / (top - bottom);
    m.at(2, 3) = -(far + near) / (far - near);

    return m;
}

[[maybe_unused]]
static mat4 mat4_perspective(
        const float& fov,
        const float &near,
        const float& far
    ) {
    mat4 m = {};

    const float scale = 1.0f / tan(fov * 0.5f * kPI / 180.0f);

    m.at(0, 0) = scale;
    m.at(1, 1) = scale;
    m.at(2, 2) = -far / (far - near);
    m.at(2, 3) = -(far * near) / (far - near);
    m.at(3, 2) = -1.0f;
    m.at(3, 3) = 0.0f;

    return m;
}

[[maybe_unused]]
static mat4 mat4_translate(const vec3f& v) {
    mat4 m = mat4_identity();
    m.at(0, 3) = v.x;
    m.at(1, 3) = v.y;
    m.at(2, 3) = v.z;
    return m;
}

[[maybe_unused]]
static mat4 mat4_scale(const vec3f& v) {
    mat4 m = {};
    m.at(0, 0) = v.x;
    m.at(1, 1) = v.y;
    m.at(2, 2) = v.z;
    m.at(3, 3) = 1.0f;
    return m;
}

[[maybe_unused]]
static mat4 mat4_rotate_x(const float degrees) {
    mat4 m = mat4_identity();

    const float radians = degreesToRadians(degrees);

    m.at(1, 1) = cosf(radians);
    m.at(2, 1) = sinf(radians);
    m.at(1, 2) = -m.at(1, 2);
    m.at(2, 2) = m.at(1, 1);
    m.at(3, 3) = 1.0f;

    return m;
}

[[maybe_unused]]
static mat4 mat4_rotate_y(const float degrees) {
    mat4 m = mat4_identity();

    const float radians = degreesToRadians(degrees);

    m.at(0, 0) = cosf(radians);
    m.at(0, 2) = sinf(radians);
    m.at(2, 0) = -m.at(0, 0);
    m.at(2, 2) = m.at(0, 0);
    m.at(3, 3) = 1.0f;

    return m;
}

[[maybe_unused]]
static mat4 mat4_rotate_z(const float degrees) {
    mat4 m  = mat4_identity();

    const float radians = degreesToRadians(degrees);

    m.at(0, 0) = cosf(radians);
    m.at(1, 0) = sinf(radians);
    m.at(0, 1) = -m.at(1, 0);
    m.at(1, 1) = m.at(0, 0);
    m.at(3, 3) = 1.0f;

    return m;
}

// [[maybe_unused]]
// static mat4 mat4_rotate(const vec3f& v) {
//     mat4 m;
//     return m;
// }

class mat3 {
public:
    friend std::ostream& operator<<(std::ostream& s, const mat3& m);

private:
    friend class mat4;

    mat_t determinant();

    mat_t data_[9] = {0};
};
