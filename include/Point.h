//By Alexej Fedorenko, 37676
#pragma once
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

struct Point
{
    float x;
    float y;
    glm::vec2 ToVec2();
};

void from_json(const nlohmann::json &j, Point &p);
void to_json(nlohmann::json &j, const Point &p); 
