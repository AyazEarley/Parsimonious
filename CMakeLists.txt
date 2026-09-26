cmake_minimum_required(VERSION 3.22)

project(Parsimonious VERSION 1.0.0)

# Pull in JUCE (assumes it's a submodule at ./JUCE)
add_subdirectory(JUCE)

juce_add_plugin(Parsimonious
    COMPANY_NAME "Ayaz Earley"
    IS_SYNTH TRUE                         # loads as an Instrument, works reliably in FL
    IS_MIDI_EFFECT FALSE
    NEEDS_MIDI_INPUT FALSE                # no live MIDI I/O — generation is file-based
    NEEDS_MIDI_OUTPUT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE         # auto-installs to the system VST3 folder
    PLUGIN_MANUFACTURER_CODE Ymco         # 4 chars, first must be uppercase
    PLUGIN_CODE Mypl                      # 4 chars, unique per plugin
    FORMATS VST3 Standalone
    VST3_CATEGORIES "Instrument"
    PRODUCT_NAME "Parsimonious")

juce_generate_juce_header(Parsimonious)

# JUCE modules that generate the standard juce_audio_plugin_client boilerplate
target_sources(Parsimonious
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp)

target_compile_definitions(Parsimonious
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(Parsimonious
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)