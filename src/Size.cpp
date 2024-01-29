//By Alexej Fedorenko, 37676
#include "Size.h"

void from_json(const nlohmann::json &j, Size &s)
{
    j.at("width").get_to(s.width);
    j.at("height").get_to(s.height);
}

void to_json(nlohmann::json &j, const Size &s)
{
    j["width"] = s.width;
    j["height"] = s.height;
}
