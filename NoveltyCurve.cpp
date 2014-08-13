//
//  NoveltyCurve.cpp
//  Tempogram
//
//  Created by Carl Bussey on 10/07/2014.
//  Copyright (c) 2014 Carl Bussey. All rights reserved.
//

//Spectrogram dimensions should be flipped?

#include "NoveltyCurve.h"
#include <memory>
using namespace std;

NoveltyCurve::NoveltyCurve(const float &samplingFrequency, const size_t &fftLength, const size_t &numberOfBlocks, const size_t &compressionConstant) :
    m_samplingFrequency(samplingFrequency),
    m_fftLength(fftLength),
    m_blockSize(fftLength/2 + 1),
    m_numberOfBlocks(numberOfBlocks),
    m_compressionConstant(compressionConstant),
    m_numberOfBands(5),
    m_pBandBoundaries(0),
    m_pBandSum(0)
{
    initialise();
}

NoveltyCurve::~NoveltyCurve(){
    cleanup();
}

//allocate all space and set variable
void
NoveltyCurve::initialise(){
    
    // for bandwise processing, the band is split into 5 bands. m_pBandBoundaries contains the upper and lower bin boundaries for each band.
    m_pBandBoundaries = new int[m_numberOfBands+1];
    m_pBandBoundaries[0] = 0;
    for (unsigned int band = 1; band < m_numberOfBands; band++){
        float lowFreq = 500*pow(2.5, (int)band-1);
        m_pBandBoundaries[band] = m_fftLength*lowFreq/m_samplingFrequency;
    }
    m_pBandBoundaries[m_numberOfBands] = m_blockSize;
    
    m_pBandSum = new float [m_numberOfBands];
}

//delete space allocated in initialise()
void
NoveltyCurve::cleanup(){
    delete []m_pBandBoundaries;
    m_pBandBoundaries = 0;
    delete []m_pBandSum;
    m_pBandSum = 0;
}

//calculate max of spectrogram
float NoveltyCurve::calculateMax(const vector< vector<float> > &spectrogram) const
{
    float max = 0;
    
    for (unsigned int j = 0; j < m_numberOfBlocks; j++){
        for (unsigned int i = 0; i < m_blockSize; i++){
            max = max > fabs(spectrogram[i][j]) ? max : fabs(spectrogram[i][j]);
        }
    }
    
    return max;
}

//subtract local average of novelty curve
//uses m_hannWindow as filter
void NoveltyCurve::subtractLocalAverage(vector<float> &noveltyCurve, const size_t &smoothLength) const
{
    vector<float> localAverage(m_numberOfBlocks);
    
    float * m_hannWindow = new float[smoothLength];
    WindowFunction::hanning(m_hannWindow, smoothLength, true);
    
    FIRFilter filter(m_numberOfBlocks, smoothLength);
    filter.process(&noveltyCurve[0], m_hannWindow, &localAverage[0]);
    
    assert(noveltyCurve.size() == m_numberOfBlocks);
    for (unsigned int i = 0; i < m_numberOfBlocks; i++){
        noveltyCurve[i] -= localAverage[i];
        noveltyCurve[i] = noveltyCurve[i] >= 0 ? noveltyCurve[i] : 0;
    }
    
    delete []m_hannWindow;
    m_hannWindow = 0;
}

//smoothed differentiator filter. Flips upper half of hanning window about y-axis to create coefficients.
void NoveltyCurve::smoothedDifferentiator(vector< vector<float> > &spectrogram, const size_t &smoothLength) const
{
    
    float * diffHannWindow = new float [smoothLength];
    WindowFunction::hanning(diffHannWindow, smoothLength, true);
    
    if(smoothLength%2) diffHannWindow[(smoothLength+1)/2 - 1] = 0;
    for(unsigned int i = (smoothLength+1)/2; i < smoothLength; i++){
        diffHannWindow[i] = -diffHannWindow[i];
    }
    
    FIRFilter smoothFilter(m_numberOfBlocks, smoothLength);
    
    for (unsigned int i = 0; i < m_blockSize; i++){
        smoothFilter.process(&spectrogram[i][0], diffHannWindow, &spectrogram[i][0]);
    }
}

//half rectification (set negative to zero)
void NoveltyCurve::halfWaveRectify(vector< vector<float> > &spectrogram) const
{
    for (unsigned int block = 0; block < m_numberOfBlocks; block++){
        for (unsigned int k = 0; k < m_blockSize; k++){
            if (spectrogram[k][block] < 0.0) spectrogram[k][block] = 0.0;
        }
    }
}

//process method
vector<float>
NoveltyCurve::spectrogramToNoveltyCurve(Spectrogram spectrogram) const
{
    std::vector<float> noveltyCurve(m_numberOfBlocks);
    
    assert(spectrogram.size() == m_blockSize);
    assert(spectrogram[0].size() == m_numberOfBlocks);
    
    //normalise and log spectrogram
    float normaliseScale = calculateMax(spectrogram);
    for (unsigned int block = 0; block < m_numberOfBlocks; block++){
        for (unsigned int k = 0; k < m_blockSize; k++){
            if(normaliseScale != 0.0) spectrogram[k][block] /= normaliseScale; //normalise
            spectrogram[k][block] = log(1+m_compressionConstant*spectrogram[k][block]);
        }
    }

    //smooted differentiator
    smoothedDifferentiator(spectrogram, 5); //make smoothLength a parameter!
    //halfwave rectification
    halfWaveRectify(spectrogram);
    
    //bandwise processing
    for (unsigned int block = 0; block < m_numberOfBlocks; block++){
        for (unsigned int band = 0; band < m_numberOfBands; band++){
            int k = m_pBandBoundaries[band];
            int bandEnd = m_pBandBoundaries[band+1];
            m_pBandSum[band] = 0;
            
            while(k < bandEnd){
                m_pBandSum[band] += spectrogram[k][block];
                k++;
            }
        }
        float total = 0;
        for(unsigned int band = 0; band < m_numberOfBands; band++){
            total += m_pBandSum[band];
        }
        noveltyCurve[block] = total/m_numberOfBands;
    }
    
    //subtract local averages
    subtractLocalAverage(noveltyCurve, 65);
    
    return noveltyCurve;
}
