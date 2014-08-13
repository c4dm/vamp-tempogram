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
    FIRFilter(const size_t &lengthInput, const size_t &numberOfCoefficients);
    ~FIRFilter();
    void process(const float *pInput, const float *pCoefficients, float * pOutput);
private:
    size_t m_lengthInput;
    size_t m_numberOfCoefficients;
    int m_lengthFIRFFT;
    
    double *m_pFftInput;
    double *m_pFftCoefficients;
    double *m_pFftReal1;
    double *m_pFftImag1;
    double *m_pFftReal2;
    double *m_pFftImag2;
    double *m_pFftFilteredReal;
    double *m_pFftFilteredImag;
    double *m_pFftOutputReal;
    double *m_pFftOutputImag;
    
    void initialise();
    void cleanup();
};

#endif /* defined(__Tempogram__FIRFilter__) */
