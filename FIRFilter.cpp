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

FIRFilter::FIRFilter(const size_t &lengthInput, const size_t &numberOfCoefficients) :
    m_lengthInput(lengthInput),
    m_numberOfCoefficients(numberOfCoefficients),
    m_pFftInput(0),
    m_pFftCoefficients(0),
    m_pFftReal1(0),
    m_pFftImag1(0),
    m_pFftReal2(0),
    m_pFftImag2(0),
    m_pFftFilteredReal(0),
    m_pFftFilteredImag(0),
    m_pFftOutputReal(0),
    m_pFftOutputImag(0)
{
    initialise();
}

FIRFilter::~FIRFilter()
{
    cleanup();
}

//allocate memory
void
FIRFilter::initialise()
{
    //next power of 2
    m_lengthFIRFFT = pow(2,(ceil(log2(m_lengthInput+m_numberOfCoefficients-1))));
    
    m_pFftInput = new double[m_lengthFIRFFT];
    m_pFftCoefficients = new double[m_lengthFIRFFT];
    m_pFftReal1 = new double[m_lengthFIRFFT];
    m_pFftImag1 = new double[m_lengthFIRFFT];
    m_pFftReal2 = new double[m_lengthFIRFFT];
    m_pFftImag2 = new double[m_lengthFIRFFT];
    m_pFftFilteredReal = new double[m_lengthFIRFFT];
    m_pFftFilteredImag = new double[m_lengthFIRFFT];
    m_pFftOutputReal = new double[m_lengthFIRFFT];
    m_pFftOutputImag = new double[m_lengthFIRFFT];
    
    for(unsigned int i = 0; i < m_lengthFIRFFT; i++){
        m_pFftInput[i] = m_pFftCoefficients[i] = m_pFftReal1[i] = m_pFftImag1[i] = m_pFftReal2[i] = m_pFftImag2[i] = m_pFftFilteredReal[i] = m_pFftFilteredImag[i] = m_pFftOutputReal[i] = m_pFftOutputImag[i] = 0.0;
    }
}

void
FIRFilter::process(const float* pInput, const float* pCoefficients, float* pOutput)
{
    //Copy to same length FFT buffers
    for(unsigned int i = 0; i < m_lengthFIRFFT; i++){
        m_pFftInput[i] = i < m_lengthInput ? pInput[i] : 0.0;
        m_pFftCoefficients[i] = i < m_numberOfCoefficients ? pCoefficients[i] : 0.0;
    }
    
    FFT::forward(m_lengthFIRFFT, m_pFftInput, 0, m_pFftReal1, m_pFftImag1);
    FFT::forward(m_lengthFIRFFT, m_pFftCoefficients, 0, m_pFftReal2, m_pFftImag2);
    
    //Multiply FFT coefficients. Multiplication in freq domain is convolution in time domain.
    for (unsigned int i = 0; i < m_lengthFIRFFT; i++){
        m_pFftFilteredReal[i] = (m_pFftReal1[i] * m_pFftReal2[i]) - (m_pFftImag1[i] * m_pFftImag2[i]);
        m_pFftFilteredImag[i] = (m_pFftReal1[i] * m_pFftImag2[i]) + (m_pFftReal2[i] * m_pFftImag1[i]);
    }
    
    FFT::inverse(m_lengthFIRFFT, m_pFftFilteredReal, m_pFftFilteredImag, m_pFftOutputReal, m_pFftOutputImag);
    
    //copy to output
    for (unsigned int i = 0; i < m_lengthInput; i++){
        pOutput[i] = m_pFftOutputReal[i];
    }
}

//remove memory allocations
void
FIRFilter::cleanup()
{
    delete []m_pFftInput;
    delete []m_pFftCoefficients;
    delete []m_pFftReal1;
    delete []m_pFftImag1;
    delete []m_pFftReal2;
    delete []m_pFftImag2;
    delete []m_pFftFilteredReal;
    delete []m_pFftFilteredImag;
    delete []m_pFftOutputReal;
    delete []m_pFftOutputImag;
    m_pFftInput = m_pFftCoefficients = m_pFftReal1 = m_pFftImag1 = m_pFftReal2 = m_pFftImag2 = m_pFftFilteredReal = m_pFftFilteredImag = m_pFftOutputReal = m_pFftOutputImag = 0;
}
