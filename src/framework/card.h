#ifndef GRAPHICS_CARD_H
#define GRAPHICS_CARD_H

#include "color.h"
#include <cstring>
#include <memory>

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

enum numbers {
    one,
    two,
    three
};

class card {
private:
    int color;
    int shape;
    int fill;
    int number;
public:
    card(int color, int shape, int fill, int number);
    int getColor();
    int getShape();
    int getFill();
    int getNumber();
    void setColor(int col);
    void setShape(int sha);
    void setFill(int fil);
    void setNumber(int num);
    bool isSetWith(card *o1, card *o2);
    friend bool operator == (card &o1, card &o2);
};

#endif //GRAPHICS_CARD_H
