//By Alexej Fedorenko, 37676
#include "AppConfiguration.h"
#include <fstream>
#include <filesystem>

void from_json(const nlohmann::json &j, AppConfiguration &cfg)
{
    /*gl params*/
    j.at("pointSize").get_to(cfg.pointSize);
    j.at("thicknessOfBezierCurve").get_to(cfg.thicknessOfBezierCurve);
    j.at("thicknessOfLineBetweenPoints").get_to(cfg.thicknessOfLineBetweenPoints);
    if (cfg.pointSize == 0)
        throw std::runtime_error("Point size must be highier than 0");
    if(cfg.thicknessOfLineBetweenPoints == 0)
        throw std::runtime_error("thickness of line between points must be highier than 0");
    if(cfg.thicknessOfBezierCurve == 0)
        throw std::runtime_error("thickness of bezier curve must be highier than 0");
    if (cfg.thicknessOfLineBetweenPoints > cfg.pointSize)
        throw std::runtime_error("thickness of line between the points must be less than pointSize");
    
    j.at("enablePoints").get_to(cfg.enablePoints);
    j.at("enableLineBetweenPoints").get_to(cfg.enableLineBetweenPoints);
    j.at("enableBezierCurve").get_to(cfg.enableBezierCurve); 
    size_t minimumCanvasSize = std::max(std::max(cfg.pointSize, cfg.thicknessOfLineBetweenPoints), cfg.thicknessOfBezierCurve) * 2;
    j.at("pointColor").get_to(cfg.pointColor);
    j.at("lineBetweenPointsColor").get_to(cfg.lineBetweenPointsColor);
    j.at("bezierCurveColor").get_to(cfg.bezierCurveColor);
    
    /*canvas*/
    j.at("canvasSize").get_to(cfg.canvasSize);

    if (cfg.canvasSize.width < minimumCanvasSize || cfg.canvasSize.height < minimumCanvasSize)
        throw std::runtime_error("canvasSize is less than drawing parameters");
    else if (cfg.canvasSize.height == 0 || cfg.canvasSize.width == 0)
        throw std::runtime_error("canvasSize must be highier than 0");

    /*points*/
    const auto &arr = j.at("points");
    if (arr.size() < 1)
        throw std::runtime_error("points array must contain at least two points");
    cfg.points.resize(arr.size());
    for (size_t i = 0; i < cfg.points.size(); i++)
        arr[i].get_to(cfg.points[i]);
}


AppConfiguration LoadConfig(std::string_view filename, CreateDefaultAppConfigCallback cb)
{
    if (!std::filesystem::exists(filename))
        return cb(filename);

    std::ifstream f(filename.data());
    nlohmann::json data = nlohmann::json::parse(f);
    f.close();
    AppConfiguration cfg;
    nlohmann::from_json(data, cfg);

    return cfg;
}

void to_json(nlohmann::json &j, const AppConfiguration &cfg)
{
    if (cfg.pointSize == 0)
        throw std::runtime_error("Point size must be highier than 0");
    if(cfg.thicknessOfLineBetweenPoints == 0)
        throw std::runtime_error("thickness of line between points must be highier than 0");
    if(cfg.thicknessOfBezierCurve == 0)
        throw std::runtime_error("thickness of bezier curve must be highier than 0");
    if (cfg.thicknessOfLineBetweenPoints > cfg.pointSize)
        throw std::runtime_error("thickness of line between the points must be less than pointSize");
    
    size_t minimumCanvasSize = std::max(std::max(cfg.pointSize, cfg.thicknessOfLineBetweenPoints), cfg.thicknessOfBezierCurve) * 2;
    if (cfg.canvasSize.width < minimumCanvasSize || cfg.canvasSize.height < minimumCanvasSize)
        throw std::runtime_error("canvasSize is less than drawing parameters");
    else if (cfg.canvasSize.height == 0 || cfg.canvasSize.width == 0)
        throw std::runtime_error("canvasSize must be highier than 0");

    j["pointSize"] = cfg.pointSize;
    j["thicknessOfBezierCurve"] = cfg.thicknessOfBezierCurve;
    j["thicknessOfLineBetweenPoints"] = cfg.thicknessOfLineBetweenPoints;
    j["enablePoints"] = cfg.enablePoints;
    j["enableBezierCurve"] = cfg.enableBezierCurve;
    j["enableLineBetweenPoints"] = cfg.enableLineBetweenPoints;
    j["pointColor"] = cfg.pointColor;
    j["lineBetweenPointsColor"] = cfg.lineBetweenPointsColor;
    j["bezierCurveColor"] = cfg.bezierCurveColor;
    
    /*canvas*/
    j["canvasSize"] = cfg.canvasSize;
    
    if (cfg.points.size() < 1)
        throw std::runtime_error("points array must contain at least two points");
    
    /*points*/
    auto &arr = j["points"];
    for (size_t i = 0; i < cfg.points.size(); i++)
        arr.push_back(cfg.points[i]);
}
