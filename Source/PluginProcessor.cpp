#include "PluginProcessor.h"
#include "PluginEditor.h"

HaitianKompaProcessor::HaitianKompaProcessor()
 : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
   parameters(*this, nullptr, "PARAMETERS", createParameterLayout()) {}

juce::AudioProcessorValueTreeState::ParameterLayout HaitianKompaProcessor::createParameterLayout()
{
    using F = juce::AudioParameterFloat;
    juce::AudioProcessorValueTreeState::ParameterLayout p;
    p.add(std::make_unique<juce::AudioParameterChoice>("sound", "Sound",
        juce::StringArray{"Kompa Guitar", "Konpa Bass", "Keys & Organ", "Brass Lead", "Percussion Synth", "Modern Pluck"}, 0));
    p.add(std::make_unique<F>("brightness", "Brightness", juce::NormalisableRange<float>(0.0f, 1.0f), 0.58f));
    p.add(std::make_unique<F>("groove", "Groove", juce::NormalisableRange<float>(0.0f, 1.0f), 0.55f));
    p.add(std::make_unique<F>("saturation", "Warmth", juce::NormalisableRange<float>(0.0f, 1.0f), 0.20f));
    p.add(std::make_unique<F>("delay", "Delay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.18f));
    p.add(std::make_unique<F>("reverb", "Reverb", juce::NormalisableRange<float>(0.0f, 1.0f), 0.22f));
    p.add(std::make_unique<F>("crash", "Percussion Texture", juce::NormalisableRange<float>(0.0f, 1.0f), 0.12f));
    p.add(std::make_unique<F>("volume", "Output", juce::NormalisableRange<float>(0.0f, 1.0f), 0.75f));
    return p;
}

void HaitianKompaProcessor::prepareToPlay(double sampleRate, int)
{
    sr = sampleRate;
    for (auto& v : voices) v = Voice{};
    noiseState = 0.0f;
    reverbEngine.setSampleRate(sr);
    reverbEngine.reset();
    const int delaySize = juce::jmax(1, (int)(sr * 2.0));
    delayL.assign((size_t) delaySize, 0.0f);
    delayR.assign((size_t) delaySize, 0.0f);
    delayPosition = 0;
}

bool HaitianKompaProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    auto out = layouts.getMainOutputChannelSet();
    return out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo();
}

void HaitianKompaProcessor::handleMidi(const juce::MidiMessage& m)
{
    if (m.isAllNotesOff() || m.isAllSoundOff())
    {
        for (auto& v : voices) v.active = false;
        return;
    }
    if (m.isNoteOn())
    {
        Voice* chosen = nullptr;
        for (auto& v : voices) if (!v.active) { chosen = &v; break; }
        if (chosen == nullptr)
        {
            chosen = &voices[0];
            for (auto& v : voices) if (v.env < chosen->env) chosen = &v;
        }
        *chosen = Voice{};
        chosen->note = m.getNoteNumber();
        chosen->freq = (float) juce::MidiMessage::getMidiNoteInHertz(chosen->note);
        chosen->env = m.getFloatVelocity();
        chosen->active = true;
    }
    else if (m.isNoteOff())
    {
        for (auto& v : voices)
            if (v.active && v.note == m.getNoteNumber()) v.releasing = true;
    }
}

void HaitianKompaProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    const int sound = (int) parameters.getRawParameterValue("sound")->load();
    const float bright = parameters.getRawParameterValue("brightness")->load();
    const float groove = parameters.getRawParameterValue("groove")->load();
    const float warm = parameters.getRawParameterValue("saturation")->load();
    const float texture = parameters.getRawParameterValue("crash")->load();
    const float volume = parameters.getRawParameterValue("volume")->load();
    const float delayMix = parameters.getRawParameterValue("delay")->load();
    const float reverbMix = parameters.getRawParameterValue("reverb")->load();
    reverbParameters.roomSize = 0.25f + 0.55f * reverbMix;
    reverbParameters.damping = 0.45f;
    reverbParameters.wetLevel = reverbMix * 0.36f;
    reverbParameters.dryLevel = 1.0f;
    reverbParameters.width = 0.85f;
    reverbParameters.freezeMode = 0.0f;
    reverbEngine.setParameters(reverbParameters);

    const int numSamples = buffer.getNumSamples();
    const int channels = buffer.getNumChannels();
    auto* left = buffer.getWritePointer(0);
    auto* right = channels > 1 ? buffer.getWritePointer(1) : nullptr;
    juce::MidiBuffer::Iterator iterator(midi);
    juce::MidiMessage message;
    int nextSample = 0;
    bool hasEvent = iterator.getNextEvent(message, nextSample);
    const int delayLength = (int) delayL.size();
    const int delaySamples = juce::jlimit(1, delayLength - 1, (int)(sr * (0.16 + 0.20 * groove)));
    for (int n = 0; n < numSamples; ++n)
    {
        while (hasEvent && nextSample <= n)
        {
            handleMidi(message);
            hasEvent = iterator.getNextEvent(message, nextSample);
        }
        float mix = 0.0f;
        for (auto& v : voices)
        {
            if (!v.active) continue;
            const float fundamental = (float) std::sin(v.phase);
            const float second = (float) std::sin(v.phase * 2.0);
            const float third = (float) std::sin(v.phase * 3.0);
            float sample = fundamental;
            switch (sound)
            {
                case 0: sample = 0.72f * fundamental + (0.18f + bright * 0.18f) * second + 0.08f * third; break;
                case 1: sample = 0.85f * fundamental + 0.22f * second; break;
                case 2: sample = 0.58f * fundamental + 0.30f * second + 0.12f * third; break;
                case 3: sample = std::tanh((1.6f + bright * 3.0f) * (fundamental + 0.25f * second)); break;
                case 4:
                    noiseState = 0.94f * noiseState + 0.06f * (rng.nextFloat() * 2.0f - 1.0f);
                    sample = 0.28f * fundamental + texture * noiseState;
                    break;
                case 5: sample = 0.68f * fundamental + 0.32f * third; break;
            }
            mix += sample * v.env * 0.13f;
            v.phase += juce::MathConstants<double>::twoPi * v.freq / sr;
            if (v.phase >= juce::MathConstants<double>::twoPi) v.phase -= juce::MathConstants<double>::twoPi;
            const float decay = v.releasing ? 0.997f : (sound == 1 ? 0.99992f : 0.99965f);
            v.env *= decay;
            if (++v.age > (int)(sr * 8.0) || v.env < 0.0004f) v.active = false;
        }
        const float dry = std::tanh(mix * (1.0f + warm * 4.0f)) * volume;
        const int readPos = (delayPosition + delayLength - delaySamples) % delayLength;
        const float echoL = delayL[(size_t) readPos];
        const float echoR = delayR[(size_t) readPos];
        delayL[(size_t) delayPosition] = dry + echoL * 0.35f;
        delayR[(size_t) delayPosition] = dry + echoR * 0.35f;
        delayPosition = (delayPosition + 1) % delayLength;
        left[n] = dry + delayMix * echoL * 0.45f;
        if (right != nullptr) right[n] = dry + delayMix * echoR * 0.45f;
    }
    if (channels > 1) reverbEngine.processStereo(left, right, numSamples);
    else reverbEngine.processMono(left, numSamples);
}

juce::AudioProcessorEditor* HaitianKompaProcessor::createEditor() { return new HaitianKompaEditor(*this); }

void HaitianKompaProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, dest);
}
void HaitianKompaProcessor::setStateInformation(const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size));
    if (xml && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new HaitianKompaProcessor(); }
