//
//  program_settings.h
//
//  Created by Ross Klonowski on January 16 2021.
//

// I2C
#define I2C_SDA 33
#define I2C_SCL 32


const uint8_t masterMacAddress[] = { 0x24, 0x62, 0xAB, 0xFB, 0x01, 0x7C };
// const uint8_t masterMacAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

const static int page_button_pin = 36;
const static int reset_data_button_pin = 39;
const static int invert_color_button_pin = 34;
