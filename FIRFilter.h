//
//  FIRFilter.h
//  Tempogram
//
//  Created by Carl Bussey on 25/06/2014.
//  Copyright (c) 2014 Carl Bussey. All rights reserved.
//

#ifndef __Tempogram__FIRFilter__
#define __Tempogram__FIRFilter__

class FIRFilter{
public:
    FIRFilter(const unsigned int lengthInput, const unsigned int numberOfCoefficients);
    ~FIRFilter();
    void process(const float *input, const float *coefficients, float *output);
private:
    unsigned int _lengthInput;
    unsigned int _numberOfCoefficients;
    unsigned int _lengthFIRFFT;
    
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
