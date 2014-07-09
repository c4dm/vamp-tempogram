
// This is a skeleton file for use in creating your own plugin
// libraries.  Replace MyPlugin and myPlugin throughout with the name
// of your first plugin class, and fill in the gaps as appropriate.


// Remember to use a different guard symbol in each header!
#ifndef _TEMPOGRAM_H_
#define _TEMPOGRAM_H_

#include <vamp-sdk/Plugin.h>

using std::string;
using std::vector;

class Tempogram : public Vamp::Plugin
{
public:
    Tempogram(float inputSampleRate);
    virtual ~Tempogram();

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
    void initialiseForGRF();
    void cleanupForGRF();
    void reset();

    FeatureSet process(const float *const *inputBuffers,
                       Vamp::RealTime timestamp);

    FeatureSet getRemainingFeatures();

protected:
    // plugin-specific data and methods go here
    size_t m_blockSize;
    size_t m_stepSize;
    float compressionConstant;
    float *previousY;
    float *currentY;
    vector<float> noveltyCurve;
    
    unsigned int tN;
    unsigned int thopSize;
    double * fftInput;
    double * fftOutputReal;
    double * fftOutputImag;
    
    int ncLength;
    int hannN;
    float *hannWindow;
    float *hannWindowtN;
    
    vector<Vamp::RealTime> ncTimestamps;
};


#endif
