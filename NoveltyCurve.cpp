//
//  NoveltyCurve.cpp
//  Tempogram
//
//  Created by Carl Bussey on 10/07/2014.
//  Copyright (c) 2014 Carl Bussey. All rights reserved.
//

#include <cmath>
#include <vector>
#include <iostream>
#include "FIRFilter.h"
#include "WindowFunction.h"
using namespace std;
#include "NoveltyCurve.h"

NoveltyCurve::NoveltyCurve(float samplingFrequency, int blockSize, int numberOfBlocks, int compressionConstant) :
    m_samplingFrequency(samplingFrequency),
    m_blockSize(blockSize),
    m_numberOfBlocks(numberOfBlocks),
    m_compressionConstant(compressionConstant),
    m_numberOfBands(5),
    m_bandBoundaries(NULL),
    m_hannLength(65),
    m_hannWindow(NULL),
    m_bandSum(NULL)
{
    initialise();
}

NoveltyCurve::~NoveltyCurve(){
    cleanup();
}

void
NoveltyCurve::initialise(){
    data = vector<float>(m_numberOfBlocks);
    
    m_hannWindow = new float[m_hannLength];
    WindowFunction::hanning(m_hannWindow, m_hannLength, true);
    
    m_bandBoundaries = new int[m_numberOfBands+1]; //make index variable
    
    m_bandBoundaries[0] = 0;
    for (int band = 1; band < m_numberOfBands; band++){
        float lowFreq = 500*pow(2.5, band-1);
        m_bandBoundaries[band] = m_blockSize*lowFreq/m_samplingFrequency;
    }
    m_bandBoundaries[m_numberOfBands] = m_blockSize/2 + 1;
    
    m_bandSum = new float * [m_numberOfBands];
    for (int i = 0; i < m_numberOfBands; i++){
        m_bandSum[i] = new float[m_numberOfBlocks];
    }
}

void
NoveltyCurve::cleanup(){
    delete []m_hannWindow;
    m_hannWindow = NULL;
    delete []m_bandBoundaries;
    m_bandBoundaries = NULL;
    
    for(int i = 0; i < m_numberOfBands; i++){
        delete []m_bandSum[i];
        m_bandSum[i] = NULL;
    }
    delete []m_bandSum;
    m_bandSum = NULL;
}

float NoveltyCurve::calculateMax(float ** spectrogram){
    int specLength = (m_blockSize/2 + 1);
    float max = 0;
    
    for (int j = 0; j < m_numberOfBlocks; j++){
        for (int i = 0; i < specLength; i++){
            max = max > spectrogram[i][j] ? max : spectrogram[i][j];
        }
    }
    
    return max;
}

void NoveltyCurve::subtractLocalAverage(float * noveltyCurve){
    vector<float> localAverage(m_numberOfBlocks);
    
    FIRFilter *filter = new FIRFilter(m_numberOfBlocks, m_hannLength);
    filter->process(&noveltyCurve[0], m_hannWindow, &localAverage[0]);
    delete filter;
    
    for (int i = 0; i < m_numberOfBlocks; i++){
        noveltyCurve[i] -= localAverage[i];
        noveltyCurve[i] = noveltyCurve[i] >= 0 ? noveltyCurve[i] : 0;
    }
}

vector<float>
NoveltyCurve::spectrogramToNoveltyCurve(float ** spectrogram){
    
    float normaliseScale = calculateMax(spectrogram);
    
    for (int block = 0; block < m_numberOfBlocks; block++){
        
        for (int band = 0; band < m_numberOfBands; band++){
            
            int specIndex = m_bandBoundaries[band];
            int bandEnd = m_bandBoundaries[band+1];
            
            while(specIndex < bandEnd){
                
                spectrogram[specIndex][block] /= normaliseScale; //normalise
                spectrogram[specIndex][block] = log(1+m_compressionConstant*spectrogram[specIndex][block]);
                
                int currentY = spectrogram[specIndex][block];
                int prevBlock = block-1;
                int previousY = prevBlock >= 0 ? spectrogram[specIndex][prevBlock] : 0;
                
                if(currentY > previousY){
                    m_bandSum[band][block] += (currentY - previousY);
                }
                
                //cout << specIndex << endl;
                specIndex++;
            }
        }
        
        float total = 0;
        for(int band = 0; band < m_numberOfBands; band++){
            total += m_bandSum[band][block];
        }
        float average = total/m_numberOfBands;
        data[block] = average;
    }
    
    subtractLocalAverage(&data[0]);
    
    return data;
}