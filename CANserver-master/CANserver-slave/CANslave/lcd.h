//
//  lcd.h
//
//  Created by Ross Klonowski on January 16 2021.
//

#ifndef lcd_h
#define lcd_h

void lcdPrint(String content);

void sendToLCD(uint8_t row, int value1, String field1, String units1, int value2, String field2, String units2);

void sendToLCD(uint8_t row, int value1, String field1, String units1, int value2, String field2, String units2, int value3, String field3, String units3);

void sendToLCD(uint8_t row, int value, String field, String units);

void sendToLCD(uint8_t row, double value, String field, String units);

void sendToLCD(uint8_t row, String string);

void sendToLCD(uint8_t row1, String string1, uint8_t row2, String string2);

void setupLCD();

void displayLoadingAnimationLCD();

#endif /* lcd_h */
