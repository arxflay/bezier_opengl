//By Alexej Fedorenko, 37676
#pragma once
#include <nlohmann/json.hpp>

struct Size
{
    size_t width;
    size_t height;
};

void from_json(const nlohmann::json &j, Size &s);
void to_json(nlohmann::json &j, const Size &s); 
