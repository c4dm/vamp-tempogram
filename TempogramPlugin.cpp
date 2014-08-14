
// This is a skeleton file for use in creating your own plugin
// libraries.  Replace MyPlugin and myPlugin throughout with the name
// of your first plugin class, and fill in the gaps as appropriate.


#include "TempogramPlugin.h"
#include <sstream>
#include <stdexcept>

using Vamp::FFT;
using Vamp::RealTime;
using namespace std;

TempogramPlugin::TempogramPlugin(float inputSampleRate) :
    Plugin(inputSampleRate),
    m_blockSize(0),
    m_stepSize(0),
    m_compressionConstant(1000), //parameter
    m_minDB(0),
    m_log2WindowLength(10), //parameter
    m_windowLength(pow((float)2,m_log2WindowLength)),
    m_log2FftLength(m_log2WindowLength),
    m_fftLength(m_windowLength),
    m_log2HopSize(6), //parameter
    m_hopSize(pow((float)2,m_log2HopSize)),
    m_minBPM(30),
    m_maxBPM(480),
    m_minBin(0), //set in initialise()
    m_maxBin(0) //set in initialise()

    // Also be sure to set your plugin parameters (presumably stored
    // in member variables) to their default values here -- the host
    // will not do that for you
{
}

TempogramPlugin::~TempogramPlugin()
{
    //delete stuff
    cleanup();
}

string
TempogramPlugin::getIdentifier() const
{
    return "tempogram";
}

string
TempogramPlugin::getName() const
{
    return "Tempogram";
}

string
TempogramPlugin::getDescription() const
{
    // Return something helpful here!
    return "Cyclic Tempogram as described by Peter Grosche and Meinard Muller";
}

string
TempogramPlugin::getMaker() const
{
    //Your name here
    return "Carl Bussey";
}

int
TempogramPlugin::getPluginVersion() const
{
    // Increment this each time you release a version that behaves
    // differently from the previous one
    return 1;
}

string
TempogramPlugin::getCopyright() const
{
    // This function is not ideally named.  It does not necessarily
    // need to say who made the plugin -- getMaker does that -- but it
    // should indicate the terms under which it is distributed.  For
    // example, "Copyright (year). All Rights Reserved", or "GPL"
    return "";
}

TempogramPlugin::InputDomain
TempogramPlugin::getInputDomain() const
{
    return FrequencyDomain;
}

size_t
TempogramPlugin::getPreferredBlockSize() const
{
    return 2048; // 0 means "I can handle any block size"
}

size_t 
TempogramPlugin::getPreferredStepSize() const
{
    return 1024; // 0 means "anything sensible"; in practice this
              // means the same as the block size for TimeDomain
              // plugins, or half of it for FrequencyDomain plugins
}

size_t
TempogramPlugin::getMinChannelCount() const
{
    return 1;
}

size_t
TempogramPlugin::getMaxChannelCount() const
{
    return 1;
}

TempogramPlugin::ParameterList
TempogramPlugin::getParameterDescriptors() const
{
    ParameterList list;

    // If the plugin has no adjustable parameters, return an empty
    // list here (and there's no need to provide implementations of
    // getParameter and setParameter in that case either).

    // Note that it is your responsibility to make sure the parameters
    // start off having their default values (e.g. in the constructor
    // above).  The host needs to know the default value so it can do
    // things like provide a "reset to default" function, but it will
    // not explicitly set your parameters to their defaults for you if
    // they have not changed in the mean time.

    ParameterDescriptor d1;
    d1.identifier = "C";
    d1.name = "C";
    d1.description = "Spectrogram compression constant, C, used when retrieving the novelty curve from the audio.";
    d1.unit = "";
    d1.minValue = 2;
    d1.maxValue = 10000;
    d1.defaultValue = 1000;
    d1.isQuantized = false;
    list.push_back(d1);

    ParameterDescriptor d2;
    d2.identifier = "log2TN";
    d2.name = "Tempogram Window Length";
    d2.description = "FFT window length when analysing the novelty curve and extracting the tempogram time-frequency function.";
    d2.unit = "";
    d2.minValue = 7;
    d2.maxValue = 12;
    d2.defaultValue = 10;
    d2.isQuantized = true;
    d2.quantizeStep = 1;
    for (int i = d2.minValue; i <= d2.maxValue; i++){
        d2.valueNames.push_back(floatToString(pow((float)2,(float)i)));
    }
    list.push_back(d2);
    
    ParameterDescriptor d3;
    d3.identifier = "log2HopSize";
    d3.name = "Tempogram Hopsize";
    d3.description = "FFT hopsize when analysing the novelty curve and extracting the tempogram time-frequency function.";
    d3.unit = "";
    d3.minValue = 6;
    d3.maxValue = 12;
    d3.defaultValue = 6;
    d3.isQuantized = true;
    d3.quantizeStep = 1;
    for (int i = d3.minValue; i <= d3.maxValue; i++){
        d3.valueNames.push_back(floatToString(pow((float)2,(float)i)));
    }
    list.push_back(d3);
    
    ParameterDescriptor d4;
    d4.identifier = "log2FftLength";
    d4.name = "Tempogram FFT Length";
    d4.description = "FFT length when analysing the novelty curve and extracting the tempogram time-frequency function. This parameter determines the amount of zero padding.";
    d4.unit = "";
    d4.minValue = 6;
    d4.maxValue = 12;
    d4.defaultValue = d2.defaultValue;
    d4.isQuantized = true;
    d4.quantizeStep = 1;
    for (int i = d4.minValue; i <= d4.maxValue; i++){
        d4.valueNames.push_back(floatToString(pow((float)2,(float)i)));
    }
    list.push_back(d4);
    
    ParameterDescriptor d5;
    d5.identifier = "minBPM";
    d5.name = "Minimum BPM";
    d5.description = "The minimum BPM of the tempogram output bins.";
    d5.unit = "";
    d5.minValue = 0;
    d5.maxValue = 2000;
    d5.defaultValue = 30;
    d5.isQuantized = true;
    d5.quantizeStep = 5;
    list.push_back(d5);
    
    ParameterDescriptor d6;
    d6.identifier = "maxBPM";
    d6.name = "Maximum BPM";
    d6.description = "The minimum BPM of the tempogram output bins.";
    d6.unit = "";
    d6.minValue = 30;
    d6.maxValue = 2000;
    d6.defaultValue = 480;
    d6.isQuantized = true;
    d6.quantizeStep = 5;
    list.push_back(d6);

    return list;
}

float
TempogramPlugin::getParameter(string identifier) const
{
    if (identifier == "C") {
        return m_compressionConstant; // return the ACTUAL current value of your parameter here!
    }
    else if (identifier == "log2TN"){
        return m_log2WindowLength;
    }
    else if (identifier == "log2HopSize"){
        return m_log2HopSize;
    }
    else if (identifier == "log2FftLength"){
        return m_log2FftLength;
    }
    else if (identifier == "minBPM") {
        return m_minBPM;
    }
    else if (identifier == "maxBPM"){
        return m_maxBPM;
    }
    
    return 0;
}

void
TempogramPlugin::setParameter(string identifier, float value) 
{
    
    if (identifier == "C") {
        m_compressionConstant = value; // set the actual value of your parameter
    }
    else if (identifier == "log2TN") {
        m_windowLength = pow(2,value);
        m_log2WindowLength = value;
    }
    else if (identifier == "log2HopSize"){
        m_hopSize = pow(2,value);
        m_log2HopSize = value;
    }
    else if (identifier == "log2HopFftLength"){
        m_fftLength = pow(2,value);
        m_log2FftLength = value;
    }
    else if (identifier == "minBPM") {
        m_minBPM = value;
    }
    else if (identifier == "maxBPM"){
        m_maxBPM = value;
    }
    
}

void TempogramPlugin::updateBPMParameters(){

}

TempogramPlugin::ProgramList
TempogramPlugin::getPrograms() const
{
    ProgramList list;

    // If you have no programs, return an empty list (or simply don't
    // implement this function or getCurrentProgram/selectProgram)

    return list;
}

string
TempogramPlugin::getCurrentProgram() const
{
    return ""; // no programs
}

void
TempogramPlugin::selectProgram(string name)
{
}

string TempogramPlugin::floatToString(float value) const
{
    ostringstream ss;
    
    if(!(ss << value)) throw runtime_error("TempogramPlugin::floatToString(): invalid conversion from float to string");
    return ss.str();
}

TempogramPlugin::OutputList
TempogramPlugin::getOutputDescriptors() const
{
    OutputList list;

    // See OutputDescriptor documentation for the possibilities here.
    // Every plugin must have at least one output.
    
    OutputDescriptor d;
    float d_sampleRate;
    float tempogramInputSampleRate = (float)m_inputSampleRate/m_stepSize;
    
    d.identifier = "tempogram";
    d.name = "Tempogram";
    d.description = "Tempogram";
    d.unit = "BPM";
    d.hasFixedBinCount = true;
    d.binCount = m_maxBin - m_minBin + 1;
    d.hasKnownExtents = false;
    d.isQuantized = false;
    d.sampleType = OutputDescriptor::FixedSampleRate;
    d_sampleRate = tempogramInputSampleRate/m_hopSize;
    d.sampleRate = d_sampleRate > 0.0 && !isnan(d_sampleRate) ? d_sampleRate : 0.0;
    for(int i = m_minBin; i <= (int)m_maxBin; i++){
        float w = ((float)i/m_fftLength)*(tempogramInputSampleRate);
        d.binNames.push_back(floatToString(w*60));
    }
    d.hasDuration = false;
    list.push_back(d);
    
    d.identifier = "nc";
    d.name = "Novelty Curve";
    d.description = "Novelty Curve";
    d.unit = "";
    d.hasFixedBinCount = true;
    d.binCount = 1;
    d.hasKnownExtents = false;
    d.isQuantized = false;
    d.sampleType = OutputDescriptor::FixedSampleRate;
    d_sampleRate = tempogramInputSampleRate;
    d.sampleRate = d_sampleRate > 0 && !isnan(d_sampleRate) ? d_sampleRate : 0.0;
    d.hasDuration = false;
    list.push_back(d);
    
    return list;
}

bool
TempogramPlugin::initialise(size_t channels, size_t stepSize, size_t blockSize)
{
    if (channels < getMinChannelCount() ||
	channels > getMaxChannelCount()) return false;
    
    // Real initialisation work goes here!
    m_blockSize = blockSize;
    m_stepSize = stepSize;
    m_minDB = pow(10,(float)-74/20);
    
    if (m_fftLength < m_windowLength){
        m_fftLength = m_windowLength;
    }
    if (m_minBPM > m_maxBPM){
        m_minBPM = 30;
        m_maxBPM = 480;
    }
    float tempogramInputSampleRate = (float)m_inputSampleRate/m_stepSize;
    m_minBin = (unsigned int)(max(floor(((m_minBPM/60)/tempogramInputSampleRate)*m_fftLength), (float)0.0));
    m_maxBin = (unsigned int)(min(ceil(((m_maxBPM/60)/tempogramInputSampleRate)*m_fftLength), (float)m_fftLength/2));
    
    m_spectrogram = SpectrogramTransposed(m_blockSize/2 + 1);
    
    return true;
}

void TempogramPlugin::cleanup(){

}

void
TempogramPlugin::reset()
{
    // Clear buffers, reset stored values, etc
    ncTimestamps.clear();
    m_spectrogram = SpectrogramTransposed(m_blockSize/2 + 1);
}

TempogramPlugin::FeatureSet
TempogramPlugin::process(const float *const *inputBuffers, Vamp::RealTime timestamp)
{
    size_t n = m_blockSize/2 + 1;
    
    FeatureSet featureSet;
    Feature feature;
    
    const float *in = inputBuffers[0];

    //calculate magnitude of FrequencyDomain input
    for (unsigned int i = 0; i < n; i++){
        float magnitude = sqrt(in[2*i] * in[2*i] + in[2*i + 1] * in[2*i + 1]);
        magnitude = magnitude > m_minDB ? magnitude : m_minDB;
        m_spectrogram[i].push_back(magnitude);
    }
    
    ncTimestamps.push_back(timestamp); //save timestamp

    return featureSet;
}

TempogramPlugin::FeatureSet
TempogramPlugin::getRemainingFeatures()
{
    
    float * hannWindow = new float[m_windowLength];
    for (unsigned int i = 0; i < m_windowLength; i++){
        hannWindow[i] = 0.0;
    }
    
    FeatureSet featureSet;
    
    //initialise m_noveltyCurve processor
    size_t numberOfBlocks = m_spectrogram[0].size();
    NoveltyCurveProcessor nc(m_inputSampleRate, m_blockSize, numberOfBlocks, m_compressionConstant);
    m_noveltyCurve = nc.spectrogramToNoveltyCurve(m_spectrogram); //calculate novelty curve from magnitude data
    
    //push novelty curve data to featureset 1 and set timestamps
    for (unsigned int i = 0; i < numberOfBlocks; i++){
        Feature feature;
        feature.values.push_back(m_noveltyCurve[i]);
        feature.hasTimestamp = false;
        //feature.timestamp = ncTimestamps[i];
        featureSet[1].push_back(feature);
    }
    
    //window function for spectrogram
    WindowFunction::hanning(hannWindow, m_windowLength);
    
    //initialise spectrogram processor
    SpectrogramProcessor spectrogramProcessor(m_windowLength, m_fftLength, m_hopSize);
    //compute spectrogram from novelty curve data (i.e., tempogram)
    Spectrogram tempogram = spectrogramProcessor.process(&m_noveltyCurve[0], numberOfBlocks, hannWindow);
    
    int timePointer = m_hopSize-m_windowLength/2;
    int tempogramLength = tempogram.size();
    
    //push tempogram data to featureset 0 and set timestamps.
    for (int block = 0; block < tempogramLength; block++){
        Feature feature;
        
        //int timeMS = floor(1000*(m_stepSize*timePointer)/m_inputSampleRate + 0.5);
        
        assert(tempogram[block].size() == (m_fftLength/2 + 1));
        for(int k = m_minBin; k < (int)m_maxBin; k++){
            feature.values.push_back(tempogram[block][k]);
            //cout << tempogram[k][block] << endl;
        }
        feature.hasTimestamp = false;
        //feature.timestamp = RealTime::fromMilliseconds(timeMS);
        featureSet[0].push_back(feature);
        
        timePointer += m_hopSize;
    }
    
    delete []hannWindow;
    hannWindow = 0;
    
    return featureSet;
}
