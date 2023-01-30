//
//  scd40.h
//


#ifndef scd40_h
#define scd40_h

void scd40_setup();

bool scd40_data_ready();

void scd40_get_data(float&, float&, float&);


#endif /* scd40_h */