# Haitian Kompa Machine

JUCE-based Windows VST3 instrument for Haitian kompa music production in FL Studio.

## Version 1.1
- Six selectable synthesized sounds
- MIDI input and note playback
- Delay and reverb processing
- Parameter controls for tone, groove, warmth and volume

## Source package
The v1.1 source is prepared as `Haitian_Kompa_Machine_v1_1_GitHub_Ready.zip` in the ChatGPT conversation. The ZIP contains `CMakeLists.txt`, `Source/` and `.github/workflows/windows-vst3.yml`. Upload the ZIP's **contents** (not the ZIP itself) to this repository at their original paths.

## Build
After the source and workflow files have been uploaded, GitHub Actions can build a Windows VST3 and publish an artifact. Do not assume the plugin is compiled or FL Studio-tested until the workflow succeeds and the VST3 is checked in FL Studio.
