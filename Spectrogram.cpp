//
//  Spectrogram.cpp
//  Tempogram
//
//  Created by Carl Bussey on 07/08/2014.
//  Copyright (c) 2014 Carl Bussey. All rights reserved.
//

#include "Spectrogram.h"
using namespace std;
using Vamp::FFT;
#include <iostream>

SpectrogramProcessor::SpectrogramProcessor(const size_t &inputLength, const size_t &windowLength, const size_t &fftLength, const size_t &hopSize) :
    m_inputLength(inputLength),
    m_windowLength(windowLength),
    m_fftLength(fftLength),
    m_hopSize(hopSize),
    m_numberOfOutputBins(ceil(fftLength/2) + 1),
    m_pFftInput(0),
    m_pFftOutputReal(0),
    m_pFftOutputImag(0)
{
    initialise();
}

SpectrogramProcessor::~SpectrogramProcessor(){
    cleanup();
}

void SpectrogramProcessor::initialise(){
    m_pFftInput = new double [m_fftLength];
    m_pFftOutputReal = new double [m_fftLength];
    m_pFftOutputImag = new double [m_fftLength];
}

void SpectrogramProcessor::cleanup(){
    delete []m_pFftInput;
    delete []m_pFftOutputReal;
    delete []m_pFftOutputImag;
    
    m_pFftInput = m_pFftOutputReal = m_pFftOutputImag = 0;
}

//process method
Spectrogram SpectrogramProcessor::process(const float * const pInput, const float * pWindow) const
{
    int numberOfBlocks = ceil(m_inputLength/m_hopSize) + 2*(ceil(m_windowLength/m_hopSize)-1); //The last term corresponds to overlaps at the beginning and end with padded zeros. I.e., if m_hopSize = m_windowLength/2, there'll be 1 overlap at each end. If m_hopSize = m_windowLength/4, there'll be 3 overlaps at each end, etc...
    Spectrogram spectrogram(m_numberOfOutputBins, vector<float>(numberOfBlocks));
    
    int readPointerBeginIndex = m_hopSize-m_windowLength;
    unsigned int writeBlockPointer = 0;
    
    while(readPointerBeginIndex < (int)m_inputLength){
        
        int readPointer = readPointerBeginIndex;
        for (unsigned int n = 0; n < m_windowLength; n++){
            if(readPointer < 0 || readPointer >= (int)m_inputLength){
                m_pFftInput[n] = 0.0; //pad with zeros
            }
            else{
                m_pFftInput[n] = pInput[readPointer] * pWindow[n];
            }
            readPointer++;
        }
        for (unsigned int n = m_windowLength; n < m_fftLength; n++){
            m_pFftInput[n] = 0.0;
        }
        
        FFT::forward(m_fftLength, m_pFftInput, 0, m_pFftOutputReal, m_pFftOutputImag);
        
        //@todo: sample at logarithmic spacing? Leave for host?
        for(unsigned int k = 0; k < m_numberOfOutputBins; k++){
            spectrogram[k][writeBlockPointer] = (m_pFftOutputReal[k]*m_pFftOutputReal[k] + m_pFftOutputImag[k]*m_pFftOutputImag[k]); //Magnitude or power?
            //std::cout << spectrogram[k][writeBlockPointer] << std::endl;
        }
        
        readPointerBeginIndex += m_hopSize;
        writeBlockPointer++;
    }
    
    return spectrogram;
}
