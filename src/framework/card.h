#ifndef GRAPHICS_CARD_H
#define GRAPHICS_CARD_H

#include "color.h"
#include <cstring>

enum colors {
    cardRed,
    cardGreen,
    cardBlue
};

enum shapes {
    rectangle,
    square,
    diamond
};

enum fills {
    cardEmpty,
    cardLined,
    cardFilled
};

class card {
private:
    colors c;
    shapes s;
    fills f;
public:
    card(colors c, shapes s, fills f);
    colors getColor();
    shapes getShape();
    fills getFill();
    void setColor(colors col);
    void setShape(shapes sha);
    void setFill(fills fi);
};

#endif //GRAPHICS_CARD_H
