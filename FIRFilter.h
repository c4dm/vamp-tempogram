//
//  FIRFilter.h
//  Tempogram
//
//  Created by Carl Bussey on 25/06/2014.
//  Copyright (c) 2014 Carl Bussey. All rights reserved.
//

#ifndef __Tempogram__FIRFilter__
#define __Tempogram__FIRFilter__

#include <cmath>
#include <vamp-sdk/FFT.h>
#include <assert.h>

class FIRFilter{
public:
    FIRFilter(const unsigned int lengthInput, const unsigned int numberOfCoefficients);
    ~FIRFilter();
    void process(const float *input, const float *coefficients, float *output);
private:
    unsigned int m_lengthInput;
    unsigned int m_numberOfCoefficients;
    unsigned int m_lengthFIRFFT;
    
    double *fftInput;
    double *fftCoefficients;
    double *fftReal1;
    double *fftImag1;
    double *fftReal2;
    double *fftImag2;
    double *fftFilteredReal;
    double *fftFilteredImag;
    double *fftOutputReal;
    double *fftOutputImag;
    
    void initialise();
    void cleanup();
};

#endif /* defined(__Tempogram__FIRFilter__) */
