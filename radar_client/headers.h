#pragma once

#define WIN32_LEAN_AND_MEAN
#define _WINSOCKAPI_
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <cmath>
#include <chrono>
#include <winioctl.h>

struct vec3
{
    float x, y, z;

    // Default constructor
    vec3() : x(0), y(0), z(0) {}
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Compute the length (magnitude) of the vector
    float Length() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Compute the squared length (useful for comparisons, avoids sqrt)
    float LengthSquared() const
    {
        return x * x + y * y + z * z;
    }

    // Normalize the vector (returns a unit vector)
    vec3 Normalized() const
    {
        float len = Length();
        return (len > 0) ? vec3(x / len, y / len, z / len) : vec3(0, 0, 0);
    }

    // Overloaded operators for common vector operations
    vec3 operator+(const vec3& v) const { return vec3(x + v.x, y + v.y, z + v.z); }
    vec3 operator-(const vec3& v) const { return vec3(x - v.x, y - v.y, z - v.z); }
    vec3 operator*(float scalar) const { return vec3(x * scalar, y * scalar, z * scalar); }
    vec3 operator/(float scalar) const { return (scalar != 0) ? vec3(x / scalar, y / scalar, z / scalar) : vec3(0, 0, 0); }

    // Dot product
    float Dot(const vec3& v) const { return x * v.x + y * v.y + z * v.z; }

    // Cross product
    vec3 Cross(const vec3& v) const
    {
        return vec3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }

    // Assignment operators
    vec3& operator+=(const vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    vec3& operator-=(const vec3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    vec3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    vec3& operator/=(float scalar) { if (scalar != 0) { x /= scalar; y /= scalar; z /= scalar; } return *this; }
};

namespace sdk
{
	uint64_t module_base{};
}



