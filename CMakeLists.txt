cmake_minimum_required(VERSION 3.22)
project(HaitianKompaMachine VERSION 1.1.0)
if (DEFINED JUCE_SOURCE_DIR AND EXISTS "${JUCE_SOURCE_DIR}/CMakeLists.txt")
    add_subdirectory("${JUCE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/JUCE")
else()
    find_package(JUCE CONFIG REQUIRED)
endif()
juce_add_plugin(HaitianKompaMachine
    COMPANY_NAME "Island Groove Audio"
    BUNDLE_ID "audio.islandgroove.haitiankompa"
    PLUGIN_MANUFACTURER_CODE IsGr
    PLUGIN_CODE HKom
    FORMATS VST3 Standalone
    PRODUCT_NAME "Haitian Kompa Machine"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)
juce_generate_juce_header(HaitianKompaMachine)
target_sources(HaitianKompaMachine PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)
target_compile_features(HaitianKompaMachine PRIVATE cxx_std_17)
target_compile_definitions(HaitianKompaMachine PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(HaitianKompaMachine PRIVATE
    juce::juce_audio_utils juce::juce_audio_processors juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags juce::juce_recommended_warning_flags
)
