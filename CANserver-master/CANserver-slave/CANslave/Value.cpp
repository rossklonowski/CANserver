//
//  Value.cpp
//  To be used with CANserver created by Josh Wardell
//
//  Created by Ross Klonowski on September 3 2021.
//

#include "Value.h"

Value::Value(double doubleValue, String noScale, String k) {
    _doubleValue = doubleValue;
    _noScale = noScale;
    _k = k;
    _set = true;
}

void Value::setValue(double value) {
    _doubleValue = value;
}

String Value::getValue() {

    double returnValue = _doubleValue;
    String unit = "";
    int decimalPlaces = 0;
    double absoluteValue = abs(_doubleValue);

    if (absoluteValue > 100e3) { // 101 KW
        returnValue = returnValue / 1000.00;
        unit = _k;
        decimalPlaces = 0;
    } else if ( (absoluteValue < 100e3) && (absoluteValue > 10e3) ) { // 98 KW
        returnValue = returnValue / 1000.00;
        unit = _k;
        decimalPlaces = 1;
    } else if ( (absoluteValue < 10e3) && (absoluteValue > 1e3) ) { //2.23KW
        returnValue = returnValue / 1000.00;
        unit = _k;
        decimalPlaces = 2;
    } else if (absoluteValue < 1e3) { // 900W, 1W
        unit = _noScale;
        decimalPlaces = 0;
    }

    return String(returnValue, decimalPlaces) + unit;
}

bool Value::isSet() {
    return _set;
}
