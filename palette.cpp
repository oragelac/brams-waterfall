#include "palette.h"

Palette::Palette()
{
    colors = new QColor[256];
    colors[0] = QColor(0, 0, 255);
    colors[255] = QColor(255, 0, 0);
    brightness = 0;
    contrast = 0;
    _f = 1.0;

    unsigned int i = 1;

    while(true) {

        int r = colors[i - 1].red();
        int g = colors[i - 1].green();
        int b = colors[i - 1].blue();

        if(i >= 255) break;

        else if(r <= 0 && g < 255 && b >= 255)
        {
            g += 4;
        }

        else if(r <= 0 && g >= 255 && b > 0)
        {
            b -= 4;
        }

        else if(r < 255 && g >= 255 && b <= 0)
        {
            r += 4;
        }

        else if(r >= 255 && g > 0 && b <= 0)
        {
            g -= 4;
        }

        else break;

        if(r > 255) r = 255;
        if(r < 0) r = 0;
        if(g > 255) g = 255;
        if(g < 0) g = 0;
        if(b > 255) b = 255;
        if(b < 0) b = 0;

        colors[i++] = QColor(r, g, b);
    }
}

QColor Palette::operator[](unsigned int i) {

    QColor color;

    if(i <= 255) {

        int r = colors[i].red() + brightness;
        int g = colors[i].green() + brightness;
        int b = colors[i].blue() + brightness;

        r = _f * (r - 128) + 128;
        g = _f * (g - 128) + 128;
        b = _f * (b - 128) + 128;

        if(r < 0) r = 0;
        if(g < 0) g = 0;
        if(b < 0) b = 0;
        if(r > 255) r = 255;
        if(g > 255) g = 255;
        if(b > 255) b = 255;

        color.setRgb(r, g, b);
    }

    return color;
}

void Palette::setBrightness(int brightness) {

    this->brightness = brightness;
}

void Palette::setContrast(int contrast) {

    this->contrast = contrast;
    _f = (double)(259 * (255 + contrast)) / (255 * (259 - contrast));
}


