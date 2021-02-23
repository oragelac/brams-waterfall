#ifndef PALETTE_H
#define PALETTE_H

#include <QColor>
#include <iostream>

class Palette {
    
public:

    Palette();

    void setBrightness(int);
    void setContrast(int contrast);

    QColor *colors;
    QColor operator[](unsigned int i);

private:

    int brightness;
    int contrast;
    double _f;
};

#endif // PALETTE_H
