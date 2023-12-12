#ifndef GRAPHICS_CARD_H
#define GRAPHICS_CARD_H

#include "color.h"
#include <cstring>
#include <memory>

class card {
private:
    int color;
    int shape;
    int fill;
    int number;
    int mapPos;
public:
    card(int shape, int color, int fill, int number, int mapPos);
    int getColor();
    int getShape();
    int getFill();
    int getNumber();
    int getMapPos();
    void setColor(int col);
    void setShape(int sha);
    void setFill(int fil);
    void setNumber(int num);
    void setMapPos(int pos);
    bool isSetWith(card *o1, card *o2);
    friend bool operator == (card &o1, card &o2);
};

#endif //GRAPHICS_CARD_H
