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
        Value(String, String);
        Value(String);
        void setValue(double);
        String getString();
        String getString(int);
        double getValue();
        bool isSet();
        void resetValue();
};

#endif /* value_h */