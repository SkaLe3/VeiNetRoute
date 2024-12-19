#pragma once
#include <imgui.h>
#include <glm/glm.hpp>


inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
	return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}
inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
	return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
}
inline ImVec2 operator*(const ImVec2& vec, float scalar)
{
	return ImVec2(vec.x * scalar, vec.y * scalar);
}
inline ImVec2 operator/(const ImVec2& vec, float scalar)
{
	return ImVec2(vec.x / scalar, vec.y / scalar);
}
inline ImVec2 operator+(const ImVec2& vec, float scalar)
{
	return ImVec2(vec.x + scalar, vec.y + scalar);
}
inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs)
{
	return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y);
}
inline ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs)
{
	return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y);
}
inline ImVec2 operator-(const ImVec2& vec)
{
	return ImVec2(-vec.x, -vec.y);
}

inline ImVec2 ToImVec2(const glm::vec2& vec)
{
	return ImVec2(vec.x, vec.y);
}
inline glm::vec2 ToGlmVec2(const ImVec2& vec)
{
	return glm::vec2(vec.x, vec.y);
}