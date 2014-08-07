//
//  NoveltyCurve.cpp
//  Tempogram
//
//  Created by Carl Bussey on 10/07/2014.
//  Copyright (c) 2014 Carl Bussey. All rights reserved.
//

#include "NoveltyCurve.h"
using namespace std;

NoveltyCurve::NoveltyCurve(float samplingFrequency, int fftLength, int numberOfBlocks, int compressionConstant) :
    m_samplingFrequency(samplingFrequency),
    m_fftLength(fftLength),
    m_blockSize(fftLength/2 + 1),
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
        m_bandBoundaries[band] = m_fftLength*lowFreq/m_samplingFrequency;
    }
    m_bandBoundaries[m_numberOfBands] = m_blockSize;
    
    m_bandSum = new float [m_numberOfBands];
}

void
NoveltyCurve::cleanup(){
    delete []m_hannWindow;
    m_hannWindow = NULL;
    delete []m_bandBoundaries;
    m_bandBoundaries = NULL;
    delete []m_bandSum;
    m_bandSum = NULL;
}

float NoveltyCurve::calculateMax(vector< vector<float> > &spectrogram){
    float max = 0;
    
    for (int j = 0; j < m_numberOfBlocks; j++){
        for (int i = 0; i < m_blockSize; i++){
            max = max > fabs(spectrogram[i][j]) ? max : fabs(spectrogram[i][j]);
        }
    }
    
    return max;
}

void NoveltyCurve::subtractLocalAverage(vector<float> &noveltyCurve){
    vector<float> localAverage(m_numberOfBlocks);
    
    FIRFilter *filter = new FIRFilter(m_numberOfBlocks, m_hannLength);
    filter->process(&noveltyCurve[0], m_hannWindow, &localAverage[0]);
    delete filter;
    filter = NULL;
    
    assert(noveltyCurve.size() == m_numberOfBlocks);
    for (int i = 0; i < m_numberOfBlocks; i++){
        noveltyCurve[i] -= localAverage[i];
        noveltyCurve[i] = noveltyCurve[i] >= 0 ? noveltyCurve[i] : 0;
    }
}

void NoveltyCurve::smoothedDifferentiator(vector< vector<float> > &spectrogram, int smoothLength){
    
    //need to make new hannWindow!!
    float * diffHannWindow = new float [smoothLength];
    WindowFunction::hanning(diffHannWindow, smoothLength, true);
    
    if(smoothLength%2) diffHannWindow[(smoothLength+1)/2 - 1] = 0;
    for(int i = (smoothLength+1)/2; i < smoothLength; i++){
        diffHannWindow[i] = -diffHannWindow[i];
    }
    
    FIRFilter *smoothFilter = new FIRFilter(m_numberOfBlocks, smoothLength);
    
    for (int i = 0; i < m_blockSize; i++){
        smoothFilter->process(&spectrogram[i][0], diffHannWindow, &spectrogram[i][0]);
    }
    
    delete smoothFilter;
    smoothFilter = NULL;
}

void NoveltyCurve::halfWaveRectify(vector< vector<float> > &spectrogram){ //should this return spectrogram??
    
    for (int block = 0; block < m_numberOfBlocks; block++){
        for (int k = 0; k < m_blockSize; k++){
            if (spectrogram[k][block] < 0.0) spectrogram[k][block] = 0.0;
        }
    }
}

vector<float>
NoveltyCurve::spectrogramToNoveltyCurve(vector< vector<float> > &spectrogram){
    
    assert(spectrogram.size() == m_blockSize);
    assert(spectrogram[0].size() == m_numberOfBlocks);
    
    float normaliseScale = calculateMax(spectrogram);
    
    for (int block = 0; block < m_numberOfBlocks; block++){
        for (int k = 0; k < m_blockSize; k++){
            if(normaliseScale != 0.0) spectrogram[k][block] /= normaliseScale; //normalise
            spectrogram[k][block] = log(1+m_compressionConstant*spectrogram[k][block]);
        }
    }

    smoothedDifferentiator(spectrogram, 5); //make smoothLength a parameter!
    halfWaveRectify(spectrogram);
    
    for (int block = 0; block < m_numberOfBlocks; block++){
        for (int band = 0; band < m_numberOfBands; band++){
            int k = m_bandBoundaries[band];
            int bandEnd = m_bandBoundaries[band+1];
            m_bandSum[band] = 0;
            
            while(k < bandEnd){
                m_bandSum[band] += spectrogram[k][block];
                k++;
            }
        }
        float total = 0;
        for(int band = 0; band < m_numberOfBands; band++){
            total += m_bandSum[band];
        }
        data[block] = total/m_numberOfBands;
    }
    
    subtractLocalAverage(data);
    
    return data;
}
