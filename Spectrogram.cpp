//
//  Spectrogram.cpp
//  Tempogram
//
//  Created by Carl Bussey on 07/08/2014.
//  Copyright (c) 2014 Carl Bussey. All rights reserved.
//

#include "Spectrogram.h"
#include <iostream>
using namespace std;
using Vamp::FFT;

Spectrogram::Spectrogram(unsigned int inputLength, unsigned int fftLength, unsigned int hopSize) :
    m_inputLength(inputLength),
    m_fftLength(fftLength),
    m_hopSize(hopSize),
    m_numberOfOutputBins(floor(fftLength/2 + 0.5) + 1),
    fftInput(NULL),
    fftOutputReal(NULL),
    fftOutputImag(NULL)
{
    initialise();
}

Spectrogram::~Spectrogram(){
    cleanup();
}

void Spectrogram::initialise(){
    fftInput = new double [m_fftLength];
    fftOutputReal = new double [m_fftLength];
    fftOutputImag = new double [m_fftLength];
    
    int numberOfBlocks = ceil(m_inputLength/m_hopSize) + m_fftLength/m_hopSize-1;
    spectrogramOutput = vector< vector<float> >(m_numberOfOutputBins, vector<float>(numberOfBlocks));
}

void Spectrogram::cleanup(){
    delete []fftInput;
    delete []fftOutputReal;
    delete []fftOutputImag;
    
    fftInput = fftOutputReal = fftOutputImag = NULL;
    
    cerr << "Spectrogram" << endl;
}

vector< vector<float> > Spectrogram::audioToMagnitudeSpectrogram(const float * const input, const float * window){
    
    int readPointerBeginIndex = m_hopSize-m_fftLength;
    int writeBlockPointer = 0;
    
    while(readPointerBeginIndex < m_inputLength){
        
        int readPointer = readPointerBeginIndex;
        for (int n = 0; n < m_fftLength; n++){
            if(readPointer < 0 || readPointer >= m_inputLength){
                fftInput[n] = 0.0; //pad with zeros
            }
            else{
                fftInput[n] = input[readPointer] * window[n];
            }
            readPointer++;
        }
        
        FFT::forward(m_fftLength, fftInput, NULL, fftOutputReal, fftOutputImag);
        
        //@todo: sample at logarithmic spacing? Leave for host?
        for(int k = 0; k < m_numberOfOutputBins; k++){
            spectrogramOutput[k][writeBlockPointer] = (fftOutputReal[k]*fftOutputReal[k] + fftOutputImag[k]*fftOutputImag[k]); //Magnitude or power?
        }
        
        readPointerBeginIndex += m_hopSize;
        writeBlockPointer++;
    }
    
    return spectrogramOutput;
}