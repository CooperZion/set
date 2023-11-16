#include "card.h"

using namespace std;

card::card(colors c, shapes s, fills f, number n) {
    setColor(c);
    setShape(s);
    setFill(f);
    setNumber(n);
}

bool card::isSetWith(card *o1, card *o2) {
    if ((this->n == o1->n && this->n == o2->n && o1->n == o2->n) || ((this->n != o1->n && this->n != o2->n && o1->n != o2->n))) {
        if ((this->s == o1->s && this->s == o2->s && o1->s == o2->s) || ((this->s != o1->s && this->s != o2->s && o1->s != o2->s))) {
            if ((this->c == o1->c && this->c == o2->c && o1->c == o2->c) || ((this->c != o1->c && this->c != o2->c && o1->c != o2->c))) {
                if ((this->f == o1->f && this->f == o2->f && o1->f == o2->f) || ((this->f != o1->f && this->f != o2->f && o1->f != o2->f))) {
                    return true;
                }
            }
        }
    }
    return false;
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

number card::getNumber() {
    return this->n;
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

void card::setNumber(number num) {
    this->n = num;
}
