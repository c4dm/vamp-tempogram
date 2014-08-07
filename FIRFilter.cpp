//
//  FIRFilter.cpp
//  Tempogram
//
//  Created by Carl Bussey on 25/06/2014.
//  Copyright (c) 2014 Carl Bussey. All rights reserved.
//

#include "FIRFilter.h"

using namespace std;
using Vamp::FFT;

FIRFilter::FIRFilter(const unsigned int lengthInput, const unsigned int numberOfCoefficients) :
    m_lengthInput(lengthInput),
    m_numberOfCoefficients(numberOfCoefficients),
    fftInput(NULL),
    fftCoefficients(NULL),
    fftReal1(NULL),
    fftImag1(NULL),
    fftReal2(NULL),
    fftImag2(NULL),
    fftFilteredReal(NULL),
    fftFilteredImag(NULL),
    fftOutputReal(NULL),
    fftOutputImag(NULL)
{
    initialise();
}

FIRFilter::~FIRFilter()
{
    cleanup();
}

void
FIRFilter::initialise()
{
    m_lengthFIRFFT = pow(2,(ceil(log2(m_lengthInput+m_numberOfCoefficients-1))));
    
    fftInput = new double[m_lengthFIRFFT];
    fftCoefficients = new double[m_lengthFIRFFT];
    fftReal1 = new double[m_lengthFIRFFT];
    fftImag1 = new double[m_lengthFIRFFT];
    fftReal2 = new double[m_lengthFIRFFT];
    fftImag2 = new double[m_lengthFIRFFT];
    fftFilteredReal = new double[m_lengthFIRFFT];
    fftFilteredImag = new double[m_lengthFIRFFT];
    fftOutputReal = new double[m_lengthFIRFFT];
    fftOutputImag = new double[m_lengthFIRFFT];
    
    for(int i = 0; i < m_lengthFIRFFT; i++){
        fftInput[i] = fftCoefficients[i] = fftReal1[i] = fftImag1[i] = fftReal2[i] = fftImag2[i] = fftFilteredReal[i] = fftFilteredImag[i] = fftOutputReal[i] = fftOutputImag[i] = 0.0;
    }
}

void
FIRFilter::process(const float* input, const float* coefficients, float* output)
{
    for(int i = 0; i < m_lengthFIRFFT; i++){
        fftInput[i] = i < m_lengthInput ? input[i] : 0.0;
        fftCoefficients[i] = i < m_numberOfCoefficients ? coefficients[i] : 0.0;
    }
    
    FFT::forward(m_lengthFIRFFT, fftInput, NULL, fftReal1, fftImag1);
    FFT::forward(m_lengthFIRFFT, fftCoefficients, NULL, fftReal2, fftImag2);
    
    for (int i = 0; i < m_lengthFIRFFT; i++){
        fftFilteredReal[i] = (fftReal1[i] * fftReal2[i]) - (fftImag1[i] * fftImag2[i]);
        fftFilteredImag[i] = (fftReal1[i] * fftImag2[i]) + (fftReal2[i] * fftImag1[i]);
    }
    FFT::inverse(m_lengthFIRFFT, fftFilteredReal, fftFilteredImag, fftOutputReal, fftOutputImag);
    
    for (int i = 0; i < m_lengthInput; i++){
        output[i] = fftOutputReal[i];
    }
}

void
FIRFilter::cleanup()
{
    delete []fftInput;
    delete []fftCoefficients;
    delete []fftReal1;
    delete []fftImag1;
    delete []fftReal2;
    delete []fftImag2;
    delete []fftFilteredReal;
    delete []fftFilteredImag;
    delete []fftOutputReal;
    delete []fftOutputImag;
    fftInput = fftCoefficients = fftReal1 = fftImag1 = fftReal2 = fftImag2 = fftFilteredReal = fftFilteredImag = fftOutputReal = fftOutputImag = NULL;
}