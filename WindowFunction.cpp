//
//  WindowFunction.cpp
//  Tempogram
//
//  Created by Carl Bussey on 26/06/2014.
//  Copyright (c) 2014 Carl Bussey. All rights reserved.
//

#include "WindowFunction.h"
using std::vector;

//static function
void
WindowFunction::hanning(float *window, const unsigned int N, const bool normalise){
    
    float sum = 0;
    for(int i = 0; i < N; i++){
        window[i] = 0.5*(1-cos((float)2*M_PI*i/N));
        sum += window[i];
    }
    if (normalise){
        for(int i = 0; i < N; i++){
            window[i] /= sum;
        }
    }
}