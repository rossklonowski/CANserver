//
//  Value.h
//


#ifndef value_h
#define value_h

#include <Arduino.h> // for string


class Value {
    
    private:
        int _intValue;
        double _doubleValue;
        float _floatValue;

        String _noScale;
        String _k;

        bool _set = false;
        
    public:
        Value(double, String, String);
        Value(double, String);
        void setValue(double);
        String getString();
        String getString(bool);
        double getValue();
        bool isSet();
};

#endif /* value_h */