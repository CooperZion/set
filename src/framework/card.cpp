#include "card.h"

using namespace std;

card::card(int s, int c, int f, int n, int p) {
    setShape(s);
    setColor(c);
    setFill(f);
    setNumber(n);
    setMapPos(p);
}

bool card::isSetWith(card *o1, card *o2) {
    if ((this->number == o1->number && this->number == o2->number && o1->number == o2->number) || ((this->number != o1->number && this->number != o2->number && o1->number != o2->number))) {
        if ((this->shape == o1->shape && this->shape == o2->shape && o1->shape == o2->shape) || ((this->shape != o1->shape && this->shape != o2->shape && o1->shape != o2->shape))) {
            if ((this->color == o1->color && this->color == o2->color && o1->color == o2->color) || ((this->color != o1->color && this->color != o2->color && o1->color != o2->color))) {
                if ((this->fill == o1->fill && this->fill == o2->fill && o1->fill == o2->fill) || ((this->fill != o1->fill && this->fill != o2->fill && o1->fill != o2->fill))) {
                    return true;
                }
            }
        }
    }
    return false;
}

int card::getColor() {
    return this->color;
}

int card::getShape() {
    return this->shape;
}

int card::getFill() {
    return this->fill;
}

int card::getNumber() {
    return this->number;
}

int card::getMapPos() {
    return this->mapPos;
}

void card::setColor(int col) {
    this->color = col;
}

void card::setShape(int sha) {
    this->shape = sha;
}

void card::setFill(int fil) {
    this->fill = fil;
}

void card::setNumber(int num) {
    this->number = num;
}

void card::setMapPos(int pos) {
    this->mapPos = pos;
}

bool operator==(card &o1, card &o2) {
    return o1.color == o2.color && o1.fill == o2.fill && o1.number == o2.number && o1.shape == o2.shape;
}

bool operator!=(card &o1, card &o2) {
    return !(o1.color == o2.color && o1.fill == o2.fill && o1.number == o2.number && o1.shape == o2.shape);
}
