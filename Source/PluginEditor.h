#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class HaitianKompaEditor : public juce::AudioProcessorEditor
{
public:
    explicit HaitianKompaEditor(HaitianKompaProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    HaitianKompaProcessor& processor;
    juce::ComboBox sound;
    struct Knob : juce::Slider {
        Knob() {
            setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
            setTextBoxStyle(juce::Slider::TextBoxBelow, false, 66, 18);
            setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffd8a84e));
            setColour(juce::Slider::thumbColourId, juce::Colour(0xffffd98a));
            setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffffdfaa));
            setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff171d18));
            setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff7b6131));
        }
    };
    Knob brightness, groove, warmth, delay, reverb, texture, output;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> soundA;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> brightnessA, grooveA, warmthA, delayA, reverbA, textureA, outputA;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HaitianKompaEditor)
};
