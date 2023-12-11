#include "tex.h"

Tex::Tex(Shader & shader, vec2 pos, vec2 size, struct color color) : Shape(shader, pos, size, color) {
    initVectors();
    initVAO();
    initVTBO();
    initEBO();
}

Tex::Tex(Shader &shader, vec2 pos, float width, struct color color)
        : Tex(shader, pos, vec2(width, width), color) {}

Tex::Tex(Shader &shader, vec2 pos, float width, vec4 color)
        : Tex(shader, pos, vec2(width, width), color) {}

Tex::~Tex() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Tex::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Tex::initVectors() {
    this->vertices.insert(vertices.end(), {
            -0.5f, 0.5f, 0.0f, 0.0f,  // Top left
            0.5f, 0.5f, 0.0f, 0.0f,   // Top right
            -0.5f, -0.5f, 0.0f, 0.0f, // Bottom left
            0.5f, -0.5f, 0.0f, 0.0f   // Bottom right

    });

    this->indices.insert(indices.end(), {
            0, 1, 2, // First triangle
            1, 2, 3  // Second triangle
    });
}
bool Tex::isOverlapping(const Tex &r1, const Tex &r2) {
    if (r1.getLeft() > r2.getRight() || r2.getLeft() > r1.getRight()) {
        return false;
    } else if (r1.getBottom() > r2.getTop() || r2.getBottom() > r1.getTop()) {
        return false;
    } else {
        return true;
    }
}

bool Tex::isOverlapping(const Tex &other) const {
    return isOverlapping(*this, other);
}

bool Tex::isOverlapping(const Shape &other) const {
    // Dynamic cast to check if the other shape is a Rect
    const Tex *otherTex = dynamic_cast<const Tex *>(&other);
    if (otherTex) {
        return isOverlapping(*this, *otherTex);
    }
    return false;
}
// Overridden Getters from Shape
float Tex::getLeft() const        { return pos.x - (size.x / 2); }
float Tex::getRight() const       { return pos.x + (size.x / 2); }
float Tex::getTop() const         { return pos.y + (size.y / 2); }
float Tex::getBottom() const      { return pos.y - (size.y / 2); }

void Tex::setUniforms(float u, float v) {
    // top left
    vertices[2] = u;
    vertices[3] = v;
    // top right
    vertices[2] = u;
    vertices[3] = v;
    // bottom left
    vertices[2] = u;
    vertices[3] = v;
    // bottom right
    vertices[2] = u;
    vertices[3] = v;
}
