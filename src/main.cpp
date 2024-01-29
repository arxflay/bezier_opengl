//By Alexej Fedorenko, 37676

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <nlohmann/json.hpp>
#include <fstream>

#include "Shader.h"
#include "AppConfiguration.h"
#include "MathExt.h"

void WindowCloseCallback(GLFWwindow *window);
void FramebufferResizedCallback(GLFWwindow *, int width, int height);
AppConfiguration CreateDefaultConfig(std::string_view filename);
void ProcessInput(GLFWwindow *window);

int main()
{
    //0) load config
    AppConfiguration appcfg = LoadConfig("config.json", CreateDefaultConfig);
    
    //1) init glfw
    if (!glfwInit())
        throw std::runtime_error("failed to initialize glfw");
    
    //2) set opengl flags
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 4); //multisampling
    
    //3) create window, set opengl context and set callbacks
    GLFWwindow *window = glfwCreateWindow((int)appcfg.canvasSize.width, (int)appcfg.canvasSize.height, "bezier", nullptr, nullptr);
    if(window == nullptr)
        throw std::runtime_error("failed to create window");;
    glfwMakeContextCurrent(window);
    glfwSetWindowCloseCallback(window, WindowCloseCallback);

    //4) load glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("failed to initialize glad");

    //5.1) Create and bind point VAO
    unsigned int pointVAO;
    glGenVertexArrays(1, &pointVAO);
    glBindVertexArray(pointVAO);
    
    //5.2) Create, bind, and configure point VBO
    unsigned int pointVBO;
    glGenBuffers(1, &pointVBO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    float pointVerticies[] = {
         0.5f,  0.5f,
         0.0f,  0.5f,
         0.33f, 1.0f,
         0.66f, 1.0f, 
         0.99f, 0.5f,
         0.66f, 0.0f,
         0.33f, 0.0f,
         0.0f,  0.5f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointVerticies), pointVerticies, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)(0));
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    //5.3) Create and bind rect VAO
    unsigned int rectVAO;
    glGenVertexArrays(1, &rectVAO);
    glBindVertexArray(rectVAO);
    
    //5.4) Create, bind, and configure rect VBO
    unsigned int rectVBO;
    glGenBuffers(1, &rectVBO);
    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    float rectVerticies[] = {
         0.0f,  1.0f,  
         0.0f,  0.0f,
         1.0f,  0.0f, 
         1.0f,  0.0f,          
         1.0f,  1.0f,   
         0.0f,  1.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectVerticies), rectVerticies, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)(0));
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    //5.3) load rect shaders
    Shader rectShader = Shader::FromFiles("shaders/rect.vert", "shaders/rect.frag", std::nullopt);
    glm::mat4 projectionMatrix = glm::ortho(0.0f, (float)appcfg.canvasSize.width, 0.0f, (float)appcfg.canvasSize.height);
    
    //5.4) calculate curve discretePoints
    //formula https://en.wikipedia.org/wiki/B%C3%A9zier_curve
    //quadratic example = (1-t)^2p_0 + 2(1-t)tP_1 + t^2P_2
    size_t canvasSizeAverage = std::max<size_t>((appcfg.canvasSize.height + appcfg.canvasSize.width) / 4, 10);
    std::vector<float> curveDiscretePoints(canvasSizeAverage * 4);
    for (size_t curveParameter = 0; curveParameter < canvasSizeAverage; curveParameter++)
    {
        float t = (float)curveParameter / (float)(canvasSizeAverage - 1);
        glm::vec2 coords(0.0f, 0.0f);

        /* calculate values for first to pre last point */
        size_t maxExponent = appcfg.points.size() - 1;
        for (size_t pointIndex = 0; pointIndex < appcfg.points.size() - 1; pointIndex++)
        {
            float combination = Combination(appcfg.points.size(), pointIndex);
            float one_minus_t_pow = (float)std::pow(1 - t, maxExponent - pointIndex);
            float tPow = (float)std::pow(t, pointIndex);

            coords += combination * one_minus_t_pow * tPow * appcfg.points[pointIndex].ToVec2(); 
        }
        /*calculate coords last point*/
        coords += (float)glm::pow(t, maxExponent) * appcfg.points[appcfg.points.size() - 1].ToVec2();

        curveDiscretePoints[curveParameter << 2] = coords.x;
        curveDiscretePoints[(curveParameter << 2) + 1] = coords.y;
        
        //set thickness to prev point
        if (curveParameter > 0)
        {
            size_t prevPointIndex = (curveParameter - 1) << 2; 
            glm::vec2 prevPointPos(curveDiscretePoints[prevPointIndex], curveDiscretePoints[prevPointIndex + 1]);
            glm::vec2 vecToThisPoint = glm::normalize(coords - prevPointPos);
            glm::vec2 perpVec(-vecToThisPoint.y, vecToThisPoint.x); //use perVec as coefiecent 
            
            curveDiscretePoints[prevPointIndex + 2] = prevPointPos.x + perpVec.x * (float)appcfg.thicknessOfBezierCurve;
            curveDiscretePoints[prevPointIndex + 3] = prevPointPos.y + perpVec.y * (float)appcfg.thicknessOfBezierCurve;
        }
    }
    
    { 
        //calculate last point offset (approximate)
        size_t lastPointIndexBegin = (curveDiscretePoints.size() - 4);
        glm::vec2 lastPoint(curveDiscretePoints[lastPointIndexBegin], curveDiscretePoints[lastPointIndexBegin + 1]);
        glm::vec2 prevPoint(curveDiscretePoints[lastPointIndexBegin - 4], curveDiscretePoints[lastPointIndexBegin - 3]);
        glm::vec2 vecToThisPoint = glm::normalize(lastPoint - prevPoint);
        glm::vec2 perpVec(-vecToThisPoint.y, vecToThisPoint.x); //use perVec as coefiecent
        curveDiscretePoints[lastPointIndexBegin + 2] = lastPoint.x + perpVec.x * (float)appcfg.thicknessOfBezierCurve;
        curveDiscretePoints[lastPointIndexBegin + 3] = lastPoint.y + perpVec.y * (float)appcfg.thicknessOfBezierCurve;
    }

    //5.5) Create and bind discretePoints VAO
    unsigned int discretePointsVAO;
    glGenVertexArrays(1, &discretePointsVAO);
    glBindVertexArray(discretePointsVAO);
    
    //5.6) Create, bind, and configure discretePoints VBO
    unsigned int discretePointsVBO;
    glGenBuffers(1, &discretePointsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, discretePointsVBO);

    glBufferData(GL_ARRAY_BUFFER, (GLsizei)(curveDiscretePoints.size() * sizeof(float)), curveDiscretePoints.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)(0));
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    //5.7) load bezier curve shaders
    Shader curveShader = Shader::FromFiles("shaders/curve.vert", "shaders/rect.frag", std::nullopt);

    //5.7)set callbacks and configure gl
    glfwSetFramebufferSizeCallback(window, FramebufferResizedCallback);
    glViewport(0, 0, (GLsizei)appcfg.canvasSize.width, (GLsizei)appcfg.canvasSize.height);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_MULTISAMPLE);

    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();
        ProcessInput(window);
        rectShader.UseShader();
        rectShader.SetUniformMat4("projectionMatrix", projectionMatrix);
        
        if (appcfg.enableLineBetweenPoints)
        {
            glBindVertexArray(rectVAO);
            //5.7) draw line between two points
            for (size_t i = 1; i < appcfg.points.size(); i++)
            {
                const Point &currentPoint = appcfg.points[i - 1];
                const Point &nextPoint = appcfg.points[i];
                
                //calculate rotation angle between two points
                glm::vec2 vecToNextPoint = glm::vec2(nextPoint.x - currentPoint.x, nextPoint.y - currentPoint.y);
                float vecLen = glm::sqrt(vecToNextPoint.x * vecToNextPoint.x + vecToNextPoint.y * vecToNextPoint.y);
                float angleInRads = 0;
                if (glm::sign(vecToNextPoint.y) == -1) //more than 180 degrees
                    angleInRads = glm::pi<float>() + (glm::pi<float>() - glm::acos(vecToNextPoint.x / vecLen));
                else
                    angleInRads = glm::acos(vecToNextPoint.x / vecLen);

                //set line model matrix
                float xOffset = (float)appcfg.pointSize / 2;
                float yOffset = ((float)appcfg.pointSize - (float)appcfg.thicknessOfLineBetweenPoints) / 2;
                glm::mat4 lineModelMatrix = glm::mat4(1.0f);
                lineModelMatrix = glm::translate(lineModelMatrix, glm::vec3(currentPoint.x + xOffset, currentPoint.y + yOffset, 0.0f));
                lineModelMatrix = glm::rotate(lineModelMatrix, angleInRads, glm::vec3(0.0f, 0.0f, 1.0f));
                lineModelMatrix = glm::scale(lineModelMatrix, glm::vec3(vecLen, appcfg.thicknessOfLineBetweenPoints, 1.0f));
                
                //set uniforms
                rectShader.SetUniformMat4("modelMatrix", lineModelMatrix);
                rectShader.SetUniformVec4("elementColor", appcfg.lineBetweenPointsColor.ToGLColor());
                
                //draw line
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
        
        if(appcfg.enablePoints)
        {
            glBindVertexArray(pointVAO);
            //5.8) draw points
            for (size_t i = 0; i < appcfg.points.size(); i++)
            {
                Point p = appcfg.points[i];
                
                //set point model matrix
                glm::mat4 pointModelMatrix = glm::mat4(1.0f);
                pointModelMatrix = glm::translate(pointModelMatrix, glm::vec3(p.x, p.y, 0.0f));
                pointModelMatrix = glm::scale(pointModelMatrix, glm::vec3(appcfg.pointSize, appcfg.pointSize, 0.0f));
                
                //set uniforms
                rectShader.SetUniformMat4("modelMatrix", pointModelMatrix);
                rectShader.SetUniformVec4("elementColor", appcfg.pointColor.ToGLColor());
                
                //draw point
                glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
            }
        }
        if(appcfg.enableBezierCurve)
        {
            //5.9) draw curve
            glBindVertexArray(discretePointsVAO);
            curveShader.UseShader();
            curveShader.SetUniformMat4("projectionMatrix", projectionMatrix);
            curveShader.SetUniformVec4("elementColor", appcfg.bezierCurveColor.ToGLColor());
            glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)(curveDiscretePoints.size() / 2));
        }
        glfwSwapBuffers(window);
    }
    glfwTerminate();

    return 0;
}

void WindowCloseCallback(GLFWwindow *window)
{
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void FramebufferResizedCallback(GLFWwindow *, int width, int height)
{
    glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_L))
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else if (glfwGetKey(window, GLFW_KEY_F))
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

AppConfiguration CreateDefaultConfig(std::string_view filename)
{
    // 1) create config
    AppConfiguration config;
    config.canvasSize = {640, 360};
    config.pointSize = 6;
    config.thicknessOfLineBetweenPoints = 2;
    config.thicknessOfBezierCurve = 3;
    config.pointColor = Color(255, 0, 0);
    config.bezierCurveColor = Color(0, 0, 0);
    config.lineBetweenPointsColor = Color(127, 127, 127, 127);
    config.points.resize(2);
    config.points[0] = Point{0, 0};
    config.points[1] = Point{(float)config.canvasSize.width - config.pointSize, (float)config.canvasSize.height - config.pointSize};
    config.enablePoints = true;
    config.enableBezierCurve = true;
    config.enableLineBetweenPoints = true;
    
    // 2)create file
    std::ofstream file(filename.data());
    nlohmann::json json;
    nlohmann::to_json(json, config);
    file << json.dump(4);
    file.close();

    return config;
}

