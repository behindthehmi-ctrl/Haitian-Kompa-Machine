#include "PluginEditor.h"

static juce::Colour gold() { return juce::Colour(0xffe1b65f); }

HaitianKompaEditor::HaitianKompaEditor(HaitianKompaProcessor& p) : AudioProcessorEditor(&p), processor(p)
{
    setSize(820, 520);
    sound.addItemList({"Kompa Guitar", "Konpa Bass", "Keys & Organ", "Brass Lead", "Percussion Synth", "Modern Pluck"}, 1);
    sound.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff14231d));
    sound.setColour(juce::ComboBox::textColourId, gold());
    sound.setColour(juce::ComboBox::outlineColourId, gold());
    addAndMakeVisible(sound);
    soundA = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.parameters, "sound", sound);
    auto add = [this](Knob& k, const juce::String& name, const juce::String& id,
                      std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& a) {
        k.setName(name); k.setRange(0.0, 1.0, 0.001); k.setNumDecimalPlacesToDisplay(2);
        addAndMakeVisible(k);
        a = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, id, k);
    };
    add(brightness, "BRIGHTNESS", "brightness", brightnessA);
    add(groove, "GROOVE", "groove", grooveA);
    add(warmth, "WARMTH", "saturation", warmthA);
    add(delay, "DELAY", "delay", delayA);
    add(reverb, "REVERB", "reverb", reverbA);
    add(texture, "PERC TEXTURE", "crash", textureA);
    add(output, "OUTPUT", "volume", outputA);
}

void HaitianKompaEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff09120f));
    auto r = getLocalBounds().toFloat().reduced(8);
    g.setColour(juce::Colour(0xff16362b)); g.fillRoundedRectangle(r, 12);
    g.setColour(gold()); g.drawRoundedRectangle(r, 12, 2);
    g.setColour(juce::Colour(0xff0c1914)); g.fillRoundedRectangle(juce::Rectangle<float>(22, 18, 776, 95), 9);
    g.setColour(gold()); g.drawRoundedRectangle(juce::Rectangle<float>(22, 18, 776, 95), 9, 1.5f);
    g.setFont(juce::Font(juce::FontOptions(38.0f, juce::Font::bold)));
    g.drawText("HAITIAN KOMPA", 38, 28, 520, 45, juce::Justification::centredLeft);
    g.setFont(juce::Font(juce::FontOptions(14.0f)));
    g.drawText("ISLAND GROOVE MACHINE  •  GUITAR / BASS / KEYS / PERCUSSION", 42, 74, 700, 22, juce::Justification::centredLeft);
    auto panel = [&](int x, int y, int w, int h, juce::String title) {
        g.setColour(juce::Colour(0xff0d1b15)); g.fillRoundedRectangle((float)x, (float)y, (float)w, (float)h, 8);
        g.setColour(juce::Colour(0xff6d8b55)); g.drawRoundedRectangle((float)x, (float)y, (float)w, (float)h, 8, 1.2f);
        g.setColour(gold()); g.setFont(juce::Font(juce::FontOptions(15.0f, juce::Font::bold)));
        g.drawText(title, x + 12, y + 7, w - 24, 22, juce::Justification::centredLeft);
    };
    panel(24, 126, 772, 126, "INSTRUMENT SELECTOR");
    panel(24, 266, 772, 151, "GROOVE & CHARACTER");
    panel(24, 431, 772, 65, "ISLAND OUTPUT");
    g.setColour(juce::Colour(0xffa7b89e)); g.setFont(juce::Font(juce::FontOptions(12.0f)));
    g.drawText("Play MIDI from FL Studio's piano roll or a MIDI keyboard. Delay and reverb are active effects. MIDI notes follow note-on and note-off events.", 36, 466, 745, 20, juce::Justification::centred);
}

void HaitianKompaEditor::resized()
{
    sound.setBounds(42, 164, 220, 36);
    brightness.setBounds(290, 144, 105, 102);
    groove.setBounds(415, 144, 105, 102);
    warmth.setBounds(540, 144, 105, 102);
    output.setBounds(665, 144, 105, 102);
    delay.setBounds(150, 288, 105, 112);
    reverb.setBounds(300, 288, 105, 112);
    texture.setBounds(450, 288, 105, 112);
}
