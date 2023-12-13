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
    glDeleteBuffers(1, &VTBO);
}

void Tex::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Tex::initVectors() {
    this->vertices.insert(vertices.end(), {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left

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
    // Dynamic cast to check if the other shape is a Tex
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

void Tex::setVertices(int mapPos) {
    int v = ceil(mapPos / 9);
    int u = mapPos % 9;
    if (u == 0) {u = 9;}

    float top_right_u = CARD_X * u;
    float top_right_v = CARD_Y * v;

    float bottom_left_u = top_right_u - CARD_X;
    float bottom_left_v  = top_right_v - CARD_Y;

    float top_left_u = top_right_u - CARD_X;
    float top_left_v = top_right_v;

    float bottom_right_u = top_right_u;
    float bottom_right_v = top_right_v - CARD_Y;

    // top left
    vertices[6] = float(top_left_u / MAP_X);
    vertices[7] = float(top_left_v / MAP_Y);
    // top right
    vertices[14] = float(top_right_u / MAP_X);
    vertices[15] = float(top_right_v / MAP_Y);
    // bottom left
    vertices[22] = float(bottom_left_u / MAP_X);
    vertices[23] = float(bottom_left_v / MAP_Y);
    // bottom right
    vertices[30] = float(bottom_right_u / MAP_X);
    vertices[31] = float(bottom_right_v / MAP_Y);
}
