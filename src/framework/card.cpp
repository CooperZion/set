#include "card.h"

using namespace std;

card::card(colors c, shapes s, fills f) {
    setColor(c);
    setShape(s);
    setFill(f);
}

colors card::getColor() {
    return this->c;
}

shapes card::getShape() {
    return this->s;
}

fills card::getFill() {
    return this->f;
}

void card::setColor(colors col) {
    this->c = col;
}

void card::setShape(shapes sha) {
    this->s = sha;
}

void card::setFill(fills fi) {
    this->f = fi;
}
