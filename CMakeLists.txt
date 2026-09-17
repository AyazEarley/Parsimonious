cmake_minimum_required(VERSION 3.22)

project(Parsimonious VERSION 1.0.0)

add_subdirectory(JUCE) # path to your JUCE checkout, e.g. a submodule

juce_add_plugin(MyPlugin
    COMPANY_NAME "Ayaz Earley"
    PLUGIN_MANUFACTURER_CODE Ymco
    PLUGIN_CODE Mypl
    FORMATS VST3 Standalone
    PRODUCT_NAME "Parsimonious")

juce_generate_juce_header(MyPlugin)

target_sources(MyPlugin
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp)

target_compile_definitions(MyPlugin
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0)

target_link_libraries(MyPlugin
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)