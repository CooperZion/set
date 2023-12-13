#include "shape.h"
#include "../framework/shader.h"
#include "../framework/config.h"
#include <iostream>
using glm::vec2, glm::vec3;

#ifndef GRAPHICS_TEX_H
#define GRAPHICS_TEX_H


class Tex : public Shape {
private:
    /// @brief Initializes the vertices and indices of the square
    void initVectors();
public:
    /// @brief Construct a new Square object
    /// @details This constructor will call the InitRenderData function.
    /// @param shader The shader to use
    /// @param pos The position of the square
    /// @param size The size of the square
    /// @param color The color of the square
    Tex(Shader & shader, vec2 pos, vec2 size, struct color color);

    // Overloaded constructor with only width (assuming square) using struct color
    Tex(Shader &shader, vec2 pos, float width, struct color color);

    // Overloaded constructor with only width (assuming square) using vec4 color
    Tex(Shader &shader, vec2 pos, float width, vec4 color);

    Tex(Tex const& other);

    /// @brief Destroy the Square object and delete it's VAO and VBO
    ~Tex();

    /// @brief Binds the VAO and calls the virtual draw function
    void draw() const override;

    float getLeft() const override;
    float getRight() const override;
    float getTop() const override;
    float getBottom() const override;

    static bool isOverlapping(const Tex &r1, const Tex &r2);
    bool isOverlapping(const Tex &other) const;
    bool isOverlapping(const Shape &other) const;

    void setVertices(int mapPos);
};


#endif //GRAPHICS_TEX_H

