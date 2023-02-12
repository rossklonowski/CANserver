//
//  ProgressBar.cpp
//  To be used with CANserver created by Josh Wardell
//

#include "ProgressBar.h"

ProgressBar::ProgressBar() {
    _percentFilled = 0.0;
    _set = false;
}

ProgressBar::ProgressBar(int xPos, int yPos, int barWidth, int barHeight, bool border) {

    _xPos = xPos;
    _yPos = yPos;
    _barWidth = barWidth;
    _barHeight = barHeight;
    _border = border;

    _percentFilled = 0.0;
    _set = false;  

    _borderX = xPos - 1;
    _borderY = yPos - 1;
    _borderWidth = barWidth + 2;
    _borderHeight= barHeight + 2;
}

void ProgressBar::setPercentFill(double percentFilled) {
    _set = true;
    if (percentFilled > 1) { // if not a ratio, make it so 
        percentFilled = percentFilled / 100.00;
    }
    if ( (percentFilled <= 1) && (percentFilled >= 0) ) { 
	    _percentFilled = percentFilled;
    }
}

double ProgressBar::getPercentFill() {
    return _percentFilled;
}

int ProgressBar::getxPos() {
    return _xPos;
}

int ProgressBar::getyPos() {
    return _yPos;
}

int ProgressBar::getBarWidth() {
    return _barWidth;
}

int ProgressBar::getBarHeight() {
    return _barHeight;
}

int ProgressBar::getBorderWidth() {
    return _borderWidth;
}

int ProgressBar::getBorderHeight() {
    return _borderHeight;
}

int ProgressBar::getBorderX() {
    return _borderX;
}

int ProgressBar::getBorderY() {
    return _borderY;
}

bool ProgressBar::getSet() {
    return _set;
}