//By Alexej Fedorenko, 37676
#pragma once
#include <glm/vec2.hpp>
#include <vector>
#include <nlohmann/json.hpp>
#include <string_view>
#include "Color.h"
#include "Point.h"
#include "Size.h"
/*
{
    "pointSize" : 10,
    "thicknessOfBezierCurve" : 10,
    "thicknessOfLineBetweenPoints" : 5,

    "pointColor" : {
        "r" : 255,
        "g" : 255,
        "b" : 255
    },
    "lineBetweenPointsColor" : {
        "r" : 255,
        "g" : 255,
        "b" : 255,
        "a" : 255
    },
    "bezierCurveColor" : {
        "r" : 255,
        "g" : 255,
        "b" : 255,
        "a" : 255
    },
    "canvasSize" : {
        "width" : 10, 
        "height" : 10
    },
    "points" : [ 
        {
            "x" : 10, 
            "y" : 20
        }
    ]
}
*/

struct AppConfiguration
{
    size_t pointSize;
    size_t thicknessOfBezierCurve;
    size_t thicknessOfLineBetweenPoints;
    bool enablePoints;
    bool enableBezierCurve;
    bool enableLineBetweenPoints;
    Size canvasSize;
    std::vector<Point> points;
    Color pointColor;
    Color lineBetweenPointsColor;
    Color bezierCurveColor;
};
using CreateDefaultAppConfigCallback = AppConfiguration(*)(std::string_view filename);
AppConfiguration LoadConfig(std::string_view filename, CreateDefaultAppConfigCallback cb);

void from_json(const nlohmann::json &j, AppConfiguration &cfg);
void to_json(nlohmann::json &j, const AppConfiguration &cfg); 

