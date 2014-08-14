
// This is a skeleton file for use in creating your own plugin
// libraries.  Replace MyPlugin and myPlugin throughout with the name
// of your first plugin class, and fill in the gaps as appropriate.

//* Should I use initialiseForGRF()? I generally think it's nicer to initialise stuff before processing. It just means that for some reason if somebody needs to process quickly (and have preparation time before) it's a bit easier on the load.
//* I've taken this approach with NoveltyCurve, Spectrogram and FIRFilter too. Is this a good approach?
//* The names "cleanUpForGRF()" and "initialise...()" are horrible...
//* The "m_..." variable name thing (I've been quite inconsitent with that)
//* Using size_t and not unsigned int?
//* In Tempogram.h, should the protected methods be private?
//* NoveltyCurve::NoveltyCurve() calls initialise(). May be overdetermined with amount of info? i.e., constructor takes parameters fftLength, numberOfBlocks... these are dimensions of vector< vector<float> >spectrogram.
//* When to use function() const?
//* spectrogram continues for too long? see tempogram output
//* should WindowFunction::hanning be static? Justification: no initialisation needed (i.e., no need for a constructor!).


// Remember to use a different guard symbol in each header!
#ifndef _TEMPOGRAM_H_
#define _TEMPOGRAM_H_

#include <vamp-sdk/Plugin.h>
#include "FIRFilter.h"
#include "WindowFunction.h"
#include "NoveltyCurveProcessor.h"
#include "SpectrogramProcessor.h"
#include <vamp-sdk/FFT.h>

#include <cmath>
#include <fstream>
#include <cassert>
#include <string>

using std::string;
using std::vector;

class TempogramPlugin : public Vamp::Plugin
{
public:
    TempogramPlugin(float inputSampleRate);
    virtual ~TempogramPlugin();

    string getIdentifier() const;
    string getName() const;
    string getDescription() const;
    string getMaker() const;
    int getPluginVersion() const;
    string getCopyright() const;

    InputDomain getInputDomain() const;
    size_t getPreferredBlockSize() const;
    size_t getPreferredStepSize() const;
    size_t getMinChannelCount() const;
    size_t getMaxChannelCount() const;

    ParameterList getParameterDescriptors() const;
    float getParameter(string identifier) const;
    void setParameter(string identifier, float value);

    ProgramList getPrograms() const;
    string getCurrentProgram() const;
    void selectProgram(string name);

    OutputList getOutputDescriptors() const;
    
    bool initialise(size_t channels, size_t stepSize, size_t blockSize);
    void reset();

    FeatureSet process(const float *const *inputBuffers,
                       Vamp::RealTime timestamp);

    FeatureSet getRemainingFeatures();

protected:
    // plugin-specific data and methods go here
    size_t m_blockSize;
    size_t m_stepSize;
    float m_compressionConstant;
    SpectrogramTransposed m_spectrogram; //spectrogram data
    vector<float> m_noveltyCurve; //novelty curve data
    float m_minDB;
    
    void cleanup(); //used to release anything allocated in initialise()
    string floatToString(float value) const;
    void updateBPMParameters();
    
    //FFT params for noveltyCurve -> tempogra
    float m_log2WindowLength;
    size_t m_windowLength;
    float m_log2FftLength;
    size_t m_fftLength;
    float m_log2HopSize;
    size_t m_hopSize;
    
    float m_minBPM; // tempogram output bin range min
    float m_maxBPM; // tempogram output bin range max
    unsigned int m_minBin;
    unsigned int m_maxBin;
    
    vector<Vamp::RealTime> ncTimestamps;
};


#endif
