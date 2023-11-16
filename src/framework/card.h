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

enum number {
    one,
    two,
    three
};

class card {
private:
    colors c;
    shapes s;
    fills f;
    number n;
public:
    card(colors c, shapes s, fills f, number n);
    colors getColor();
    shapes getShape();
    fills getFill();
    number getNumber();
    void setColor(colors col);
    void setShape(shapes sha);
    void setFill(fills fi);
    void setNumber(number num);
    bool isSetWith(card* o1, card* o2);
};

#endif //GRAPHICS_CARD_H
