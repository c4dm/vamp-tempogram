//
//  WindowFunction.h
//  Tempogram
//
//  Created by Carl Bussey on 26/06/2014.
//  Copyright (c) 2014 Carl Bussey. All rights reserved.
//

#ifndef __Tempogram__WindowFunction__
#define __Tempogram__WindowFunction__

#include <iostream>
#include <cmath>
#include <vector>

class WindowFunction{
public:
    static void hanning(float *signal, const unsigned int N, const bool normalise = false);
};

#endif /* defined(__Tempogram__WindowFunction__) */
