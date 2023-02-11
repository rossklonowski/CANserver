//
//  Value.cpp
//  To be used with CANserver created by Josh Wardell
//
//  Created by Ross Klonowski on September 3 2021.
//

#include "Value.h"

Value::Value(String noScale, String k) {
    _doubleValue = 0.0;
    _noScale = noScale;
    _k = k;
}

Value::Value(String noScale) {
    _doubleValue = 0.0;
    _noScale = noScale;
}

void Value::setValue(double value) {
    _doubleValue = value;
    _set = true;
}

String Value::getString() {
    double returnValue = _doubleValue;
    String phrase = "";
    String unit = "";
    int decimalPlaces = 0;
    double absoluteValue = abs(_doubleValue);

    if (_set) {
        if (absoluteValue >= 100e3) { // 101 KW
            returnValue = returnValue / 1000.00;
            unit = _k;
            decimalPlaces = 0;
        } else if ( (absoluteValue < 100e3) && (absoluteValue >= 10e3) ) { // 98 KW
            returnValue = returnValue / 1000.00;
            unit = _k;
            decimalPlaces = 1;
        } else if ( (absoluteValue < 10e3) && (absoluteValue >= 1e3) ) { //2.23KW
            returnValue = returnValue / 1000.00;
            unit = _k;
            decimalPlaces = 2;
        } else if (absoluteValue < 1e3) { // 900W, 1W
            unit = _noScale;
            decimalPlaces = 0;
        }
        phrase = String(returnValue, decimalPlaces) + unit;
    } else {
        phrase = "-" + _noScale; 
    }

    return phrase;
}

String Value::getString(int decimal) {

    double returnValue = _doubleValue;
    String phrase = "";
    String unit = _noScale;
    if (_set) {
        if (decimal == 0) {
            String temp_string = String(returnValue, 1);
            phrase = temp_string.substring(0,temp_string.length()-2) + unit;
        } else {
            phrase = String(returnValue, decimal) + unit;
        }
    } else {
        phrase = "-" + _noScale;
    }

    return phrase;
}

bool Value::isSet() {
    return _set;
}

double Value::getValue() {
    return _doubleValue;
}
