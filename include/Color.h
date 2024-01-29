//By Alexej Fedorenko, 37676
#pragma once
#include <cstdint>
#include <glm/vec4.hpp>
#include <nlohmann/json.hpp>

struct Color
{
    Color();
    Color(uint8_t rIn, uint8_t gIn, uint8_t bIn, uint8_t aIn = 255);
    glm::vec4 ToGLColor();
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

void from_json(const nlohmann::json &j, Color &col);
void to_json(nlohmann::json &j, const Color &col); 
