
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
    m_inputBlockSize(0), //host parameter
    m_inputStepSize(0), //host parameter
    m_noveltyCurveMinDB(pow(10,(float)-74/20)), //set in initialise()
    m_noveltyCurveCompressionConstant(1000), //parameter
    m_tempogramLog2WindowLength(10), //parameter
    m_tempogramWindowLength(pow((float)2,m_tempogramLog2WindowLength)),
    m_tempogramLog2FftLength(m_tempogramLog2WindowLength), //parameter
    m_tempogramFftLength(m_tempogramWindowLength),
    m_tempogramLog2HopSize(6), //parameter
    m_tempogramHopSize(pow((float)2,m_tempogramLog2HopSize)),
    m_tempogramMinBPM(30), //parameter
    m_tempogramMaxBPM(480), //parameter
    m_tempogramMinBin(0), //set in initialise()
    m_tempogramMaxBin(0), //set in initialise()
    m_cyclicTempogramMinBPM(30), //reset in initialise()
    m_cyclicTempogramNumberOfOctaves(0), //set in initialise()
    m_cyclicTempogramOctaveDivider(30) //parameter

    // Also be sure to set your plugin parameters (presumably stored
    // in member variables) to their default values here -- the host
    // will not do that for you
{
}

TempogramPlugin::~TempogramPlugin()
{
    //delete stuff
    
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
    d1.name = "Novelty Curve Spectrogram Compression Constant";
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
    d5.name = "(Cyclic) Tempogram Minimum BPM";
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
    d6.name = "(Cyclic) Tempogram Maximum BPM";
    d6.description = "The maximum BPM of the tempogram output bins.";
    d6.unit = "";
    d6.minValue = 30;
    d6.maxValue = 2000;
    d6.defaultValue = 480;
    d6.isQuantized = true;
    d6.quantizeStep = 5;
    list.push_back(d6);
    
    ParameterDescriptor d7;
    d7.identifier = "octDiv";
    d7.name = "Cyclic Tempogram Octave Divider";
    d7.description = "The number bins within each octave.";
    d7.unit = "";
    d7.minValue = 5;
    d7.maxValue = 60;
    d7.defaultValue = 30;
    d7.isQuantized = true;
    d7.quantizeStep = 1;
    list.push_back(d7);

    return list;
}

float
TempogramPlugin::getParameter(string identifier) const
{
    if (identifier == "C") {
        return m_noveltyCurveCompressionConstant; // return the ACTUAL current value of your parameter here!
    }
    else if (identifier == "log2TN"){
        return m_tempogramLog2WindowLength;
    }
    else if (identifier == "log2HopSize"){
        return m_tempogramLog2HopSize;
    }
    else if (identifier == "log2FftLength"){
        return m_tempogramLog2FftLength;
    }
    else if (identifier == "minBPM") {
        return m_tempogramMinBPM;
    }
    else if (identifier == "maxBPM"){
        return m_tempogramMaxBPM;
    }
    else if (identifier == "octDiv"){
        return m_cyclicTempogramOctaveDivider;
    }
    
    return 0;
}

void
TempogramPlugin::setParameter(string identifier, float value) 
{
    
    if (identifier == "C") {
        m_noveltyCurveCompressionConstant = value; // set the actual value of your parameter
    }
    else if (identifier == "log2TN") {
        m_tempogramWindowLength = pow(2,value);
        m_tempogramLog2WindowLength = value;
    }
    else if (identifier == "log2HopSize"){
        m_tempogramHopSize = pow(2,value);
        m_tempogramLog2HopSize = value;
    }
    else if (identifier == "log2FftLength"){
        m_tempogramFftLength = pow(2,value);
        m_tempogramLog2FftLength = value;
    }
    else if (identifier == "minBPM") {
        m_tempogramMinBPM = value;
    }
    else if (identifier == "maxBPM"){
        m_tempogramMaxBPM = value;
    }
    else if (identifier == "octDiv"){
        m_cyclicTempogramOctaveDivider = value;
    }
    
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

TempogramPlugin::OutputList
TempogramPlugin::getOutputDescriptors() const
{
    OutputList list;

    // See OutputDescriptor documentation for the possibilities here.
    // Every plugin must have at least one output.
    
    float d_sampleRate;
    float tempogramInputSampleRate = (float)m_inputSampleRate/m_inputStepSize;
    
    OutputDescriptor d3;
    d3.identifier = "cyclicTempogram";
    d3.name = "Cyclic Tempogram";
    d3.description = "Cyclic Tempogram";
    d3.unit = "";
    d3.hasFixedBinCount = true;
    d3.binCount = m_cyclicTempogramOctaveDivider > 0 && !isnan(m_cyclicTempogramOctaveDivider) ? m_cyclicTempogramOctaveDivider : 0;
    d3.hasKnownExtents = false;
    d3.isQuantized = false;
    d3.sampleType = OutputDescriptor::FixedSampleRate;
    d_sampleRate = tempogramInputSampleRate/m_tempogramHopSize;
    d3.sampleRate = d_sampleRate > 0.0 && !isnan(d_sampleRate) ? d_sampleRate : 0;
    d3.hasDuration = false;
    list.push_back(d3);
    
    OutputDescriptor d1;
    d1.identifier = "tempogram";
    d1.name = "Tempogram";
    d1.description = "Tempogram";
    d1.unit = "BPM";
    d1.hasFixedBinCount = true;
    d1.binCount = m_tempogramMaxBin - m_tempogramMinBin + 1;
    d1.hasKnownExtents = false;
    d1.isQuantized = false;
    d1.sampleType = OutputDescriptor::FixedSampleRate;
    d_sampleRate = tempogramInputSampleRate/m_tempogramHopSize;
    d1.sampleRate = d_sampleRate > 0.0 && !isnan(d_sampleRate) ? d_sampleRate : 0.0;
    for(int i = m_tempogramMinBin; i <= (int)m_tempogramMaxBin; i++){
        float w = ((float)i/m_tempogramFftLength)*(tempogramInputSampleRate);
        d1.binNames.push_back(floatToString(w*60));
    }
    d1.hasDuration = false;
    list.push_back(d1);
    
    OutputDescriptor d2;
    d2.identifier = "nc";
    d2.name = "Novelty Curve";
    d2.description = "Novelty Curve";
    d2.unit = "";
    d2.hasFixedBinCount = true;
    d2.binCount = 1;
    d2.hasKnownExtents = false;
    d2.isQuantized = false;
    d2.sampleType = OutputDescriptor::FixedSampleRate;
    d_sampleRate = tempogramInputSampleRate;
    d2.sampleRate = d_sampleRate > 0 && !isnan(d_sampleRate) ? d_sampleRate : 0;
    d2.hasDuration = false;
    list.push_back(d2);
    
    return list;
}

bool
TempogramPlugin::initialise(size_t channels, size_t stepSize, size_t blockSize)
{
    if (channels < getMinChannelCount() ||
	channels > getMaxChannelCount()) return false;
    
    // Real initialisation work goes here!
    m_inputBlockSize = blockSize;
    m_inputStepSize = stepSize;
    
    if (!handleParameterValues()) return false;
    //cout << m_cyclicTempogramOctaveDivider << endl;
    
    return true;
}

void
TempogramPlugin::reset()
{
    // Clear buffers, reset stored values, etc
    m_spectrogram.clear();
    handleParameterValues();
}

TempogramPlugin::FeatureSet
TempogramPlugin::process(const float *const *inputBuffers, Vamp::RealTime timestamp)
{
    //cerr << "Here" << endl;

    size_t n = m_inputBlockSize/2 + 1;
    
    FeatureSet featureSet;
    Feature feature;
    
    const float *in = inputBuffers[0];

    //calculate magnitude of FrequencyDomain input
    vector<float> fftCoefficients;
    for (int i = 0; i < (int)n; i++){
        float magnitude = sqrt(in[2*i] * in[2*i] + in[2*i + 1] * in[2*i + 1]);
        magnitude = magnitude > m_noveltyCurveMinDB ? magnitude : m_noveltyCurveMinDB;
        fftCoefficients.push_back(magnitude);
    }
    m_spectrogram.push_back(fftCoefficients);
    
    return featureSet;
}

TempogramPlugin::FeatureSet
TempogramPlugin::getRemainingFeatures()
{
    
    float * hannWindow = new float[m_tempogramWindowLength];
    for (int i = 0; i < (int)m_tempogramWindowLength; i++){
        hannWindow[i] = 0.0;
    }
    
    FeatureSet featureSet;
    
    //initialise novelty curve processor
    size_t numberOfBlocks = m_spectrogram.size();
    //cerr << numberOfBlocks << endl;
    NoveltyCurveProcessor nc(m_inputSampleRate, m_inputBlockSize, m_noveltyCurveCompressionConstant);
    vector<float> noveltyCurve = nc.spectrogramToNoveltyCurve(m_spectrogram); //calculate novelty curvefrom magnitude data
    //if(noveltyCurve.size() > 50) for (int i = 0; i < 50; i++) cerr << noveltyCurve[i] << endl;
    
    //push novelty curve data to featureset 1 and set timestamps
    for (int i = 0; i < (int)numberOfBlocks; i++){
        Feature noveltyCurveFeature;
        noveltyCurveFeature.values.push_back(noveltyCurve[i]);
        noveltyCurveFeature.hasTimestamp = false;
        featureSet[2].push_back(noveltyCurveFeature);
        assert(!isnan(noveltyCurveFeature.values.back()));
    }
    
    //window function for spectrogram
    WindowFunction::hanning(hannWindow, m_tempogramWindowLength);
    
    //initialise spectrogram processor
    SpectrogramProcessor spectrogramProcessor(m_tempogramWindowLength, m_tempogramFftLength, m_tempogramHopSize);
    //compute spectrogram from novelty curve data (i.e., tempogram)
    Tempogram tempogram = spectrogramProcessor.process(&noveltyCurve[0], numberOfBlocks, hannWindow);
    delete []hannWindow;
    hannWindow = 0;
    
    int tempogramLength = tempogram.size();
    
    //push tempogram data to featureset 0 and set timestamps.
    for (int block = 0; block < tempogramLength; block++){
        Feature tempogramFeature;
        
        assert(tempogram[block].size() == (m_tempogramFftLength/2 + 1));
        for(int k = m_tempogramMinBin; k < (int)m_tempogramMaxBin; k++){
            tempogramFeature.values.push_back(tempogram[block][k]);
            assert(!isnan(tempogramFeature.values.back()));
        }
        tempogramFeature.hasTimestamp = false;
        featureSet[1].push_back(tempogramFeature);
    }
    
    //Calculate cyclic tempogram
    vector< vector<unsigned int> > logBins = calculateTempogramNearestNeighbourLogBins();
    
    //assert((int)logBins.size() == m_cyclicTempogramOctaveDivider*m_cyclicTempogramNumberOfOctaves);
    for (int block = 0; block < tempogramLength; block++){
        Feature cyclicTempogramFeature;
        
        for (int i = 0; i < (int)m_cyclicTempogramOctaveDivider; i++){
            float sum = 0;
            
            for (int j = 0; j < (int)m_cyclicTempogramNumberOfOctaves; j++){
                sum += tempogram[block][logBins[j][i]];
            }
            cyclicTempogramFeature.values.push_back(sum/m_cyclicTempogramNumberOfOctaves);
            assert(!isnan(cyclicTempogramFeature.values.back()));
        }

        cyclicTempogramFeature.hasTimestamp = false;
        featureSet[0].push_back(cyclicTempogramFeature);
    }
    
    return featureSet;
}

vector< vector<unsigned int> > TempogramPlugin::calculateTempogramNearestNeighbourLogBins() const
{
    vector< vector<unsigned int> > logBins;
    
    for (int octave = 0; octave < (int)m_cyclicTempogramNumberOfOctaves; octave++){
        vector<unsigned int> octaveBins;
        
        for (int bin = 0; bin < (int)m_cyclicTempogramOctaveDivider; bin++){
            float bpm = m_cyclicTempogramMinBPM*pow(2.0f, octave+(float)bin/m_cyclicTempogramOctaveDivider);
            
            octaveBins.push_back(bpmToBin(bpm));
        }
        logBins.push_back(octaveBins);
    }
    
    //cerr << logBins.size() << endl;
    
    return logBins;
}

unsigned int TempogramPlugin::bpmToBin(const float &bpm) const
{
    float w = (float)bpm/60;
    float sampleRate = m_inputSampleRate/m_inputStepSize;
    int bin = floor((float)m_tempogramFftLength*w/sampleRate + 0.5);
    
    if(bin < 0) bin = 0;
    else if(bin > m_tempogramFftLength/2.0f) bin = m_tempogramFftLength;
    
    return bin;
}

float TempogramPlugin::binToBPM(const int &bin) const
{
    float sampleRate = m_inputSampleRate/m_inputStepSize;
    
    return (bin*sampleRate/m_tempogramFftLength)*60;
}

bool TempogramPlugin::handleParameterValues(){
    
    if (m_tempogramHopSize <= 0) return false;
    if (m_tempogramLog2FftLength <= 0) return false;
    
    if (m_tempogramFftLength < m_tempogramWindowLength){
        m_tempogramFftLength = m_tempogramWindowLength;
    }
    if (m_tempogramMinBPM >= m_tempogramMaxBPM){
        m_tempogramMinBPM = 30;
        m_tempogramMaxBPM = 480;
    }
    
    float tempogramInputSampleRate = (float)m_inputSampleRate/m_inputStepSize;
    m_tempogramMinBin = (max(floor(((m_tempogramMinBPM/60)/tempogramInputSampleRate)*m_tempogramFftLength), (float)0.0));
    m_tempogramMaxBin = (min(ceil(((m_tempogramMaxBPM/60)/tempogramInputSampleRate)*m_tempogramFftLength), (float)m_tempogramFftLength/2));
    
    if (m_tempogramMinBPM > m_cyclicTempogramMinBPM) m_cyclicTempogramMinBPM = m_tempogramMinBPM;
    float cyclicTempogramMaxBPM = 480;
    if (m_tempogramMaxBPM < cyclicTempogramMaxBPM) cyclicTempogramMaxBPM = m_tempogramMaxBPM;
    
    m_cyclicTempogramNumberOfOctaves = floor(log2(cyclicTempogramMaxBPM/m_cyclicTempogramMinBPM));
    
    return true;
}

string TempogramPlugin::floatToString(float value) const
{
    ostringstream ss;
    
    if(!(ss << value)) throw runtime_error("TempogramPlugin::floatToString(): invalid conversion from float to string");
    return ss.str();
}
