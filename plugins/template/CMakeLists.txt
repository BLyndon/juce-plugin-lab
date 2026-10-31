# The target name is the directory name: build with --target empty_VST3
# See `docs/CMake API.md` in the JUCE repo for all config options
juce_add_plugin(empty
    COMPANY_NAME "${COMPANY_NAME}"
    BUNDLE_ID "com.blyndon.empty"

    # On MacOS, plugin is copied to ~/Library/Audio/Plug-Ins/VST3
    COPY_PLUGIN_AFTER_BUILD TRUE

    PLUGIN_MANUFACTURER_CODE ${PLUGIN_MANUFACTURER_CODE}

    # A unique four-character plugin id, at least one upper-case character
    PLUGIN_CODE Empt
    FORMATS VST3

    # How it's listed in the host
    PRODUCT_NAME "Empty")

# This lets us use our code in both the JUCE targets and our Test target
# Without running into ODR violations
add_library(empty_SharedCode INTERFACE)

# Enable fast math, C++23 and a few other target defaults
pamplejuce_shared_code_defaults(TARGET empty_SharedCode)

file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
target_sources(empty_SharedCode INTERFACE ${SourceFiles})

target_compile_definitions(empty_SharedCode
    INTERFACE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(empty_SharedCode
    INTERFACE
    juce_audio_utils
    juce_audio_processors
    juce_dsp
    juce_gui_basics
    juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)

target_link_libraries(empty PRIVATE empty_SharedCode)

pamplejuce_add_tests(
    TARGET empty_Tests
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test"
    PLUGIN_TARGET empty
    SHARED_CODE empty_SharedCode
    SOURCE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_pluginval_test(empty)
