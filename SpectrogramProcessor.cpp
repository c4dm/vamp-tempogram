//
//  SpectrogramProcessor.cpp
//  Tempogram
//
//  Created by Carl Bussey on 07/08/2014.
//  Copyright (c) 2014 Carl Bussey. All rights reserved.
//

#include "SpectrogramProcessor.h"
using namespace std;
using Vamp::FFT;
#include <iostream>

SpectrogramProcessor::SpectrogramProcessor(const size_t &windowLength, const size_t &fftLength, const size_t &hopSize) :
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

SpectrogramTransposed SpectrogramProcessor::transpose(const Spectrogram &spectrogram){
    int numberOfBlocks = spectrogram.size();
    int numberOfBins = spectrogram[0].size();
    
    SpectrogramTransposed spectrogramT(numberOfBins, vector<float>(numberOfBlocks));
    
    for (int i = 0; i < numberOfBlocks; i++){
        for (int j = 0; j < numberOfBins; j++){
            spectrogramT[j][i] = spectrogram[i][j];
        }
    }
    
    return spectrogramT;
}

//process method
Spectrogram SpectrogramProcessor::process(const float * const pInput, const size_t &inputLength, const float * pWindow, const bool &transposeOutput) const
{
    Spectrogram spectrogram;
    
    int readBlockPointerIndex = 0;
    int writeBlockPointer = 0;
    
    //cout << m_hopSize << endl;
    while(readBlockPointerIndex <= (int)inputLength) {
        
        int readPointer = readBlockPointerIndex - m_windowLength/2;
        for (int n = 0; n < (int)m_windowLength; n++){
            if(readPointer < 0 || readPointer >= (int)inputLength){
                m_pFftInput[n] = 0.0; //pad with zeros
            }
            else{
                m_pFftInput[n] = pInput[readPointer] * pWindow[n];
            }
            readPointer++;
        }
        for (int n = m_windowLength; n < (int)m_fftLength; n++){
            m_pFftInput[n] = 0.0;
        }
        
        //cerr << m_fftLength << endl;
        FFT::forward(m_fftLength, m_pFftInput, 0, m_pFftOutputReal, m_pFftOutputImag);
        
        vector<float> binValues;
        //@todo: sample at logarithmic spacing? Leave for host?
        for(int k = 0; k < (int)m_numberOfOutputBins; k++){
            binValues.push_back(m_pFftOutputReal[k]*m_pFftOutputReal[k] + m_pFftOutputImag[k]*m_pFftOutputImag[k]); //Magnitude or power?
            //std::cout << spectrogram[k][writeBlockPointer] << std::endl;
        }
        spectrogram.push_back(binValues);
        
        readBlockPointerIndex += m_hopSize;
        writeBlockPointer++;
    }
    
    if(transposeOutput) return transpose(spectrogram);
    else return spectrogram;
}
