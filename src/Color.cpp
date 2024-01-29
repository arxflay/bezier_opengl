//By Alexej Fedorenko, 37676
#include <cstdint>
#include <glm/vec4.hpp>
#include "Color.h"

Color::Color()
    : Color(255, 255, 255)
{
}

Color::Color(uint8_t rIn, uint8_t gIn, uint8_t bIn, uint8_t aIn )
    : r(rIn), g(gIn), b(bIn), a(aIn)
{

}

glm::vec4 Color::ToGLColor()
{
    return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

void from_json(const nlohmann::json &j, Color &col)
{
    j.at("r").get_to(col.r);
    j.at("g").get_to(col.g);
    j.at("b").get_to(col.b);
    if (j.contains("a"))
        j.at("a").get_to(col.a);
}

void to_json(nlohmann::json &j, const Color &col)
{
    j["r"] = col.r;
    j["g"] = col.g;
    j["b"] = col.b;
    j["a"] = col.a;
}
