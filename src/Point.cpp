//By Alexej Fedorenko, 37676
#include "Point.h"

void from_json(const nlohmann::json &j, Point &p)
{
    j.at("x").get_to(p.x);
    j.at("y").get_to(p.y);
}

void to_json(nlohmann::json &j, const Point &p)
{
    j["x"] = p.x;
    j["y"] = p.y;
}


glm::vec2 Point::ToVec2()
{ 
    return glm::vec2(x, y); 
}

