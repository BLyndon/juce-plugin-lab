# The target name is the directory name: build with --target <dir>_VST3 <dir>_Tests
get_filename_component(PLUGIN_TARGET "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

# See `docs/CMake API.md` in the JUCE repo for all config options
juce_add_plugin(${PLUGIN_TARGET}
    COMPANY_NAME "${COMPANY_NAME}"
    BUNDLE_ID "com.blyndon.${PLUGIN_TARGET}"

    # On MacOS, plugin is copied to ~/Library/Audio/Plug-Ins/VST3
    COPY_PLUGIN_AFTER_BUILD TRUE

    PLUGIN_MANUFACTURER_CODE ${PLUGIN_MANUFACTURER_CODE}

    PLUGIN_CODE Gain
    FORMATS VST3

    PRODUCT_NAME "Gain")

# This lets us use our code in both the JUCE targets and our Test target
# Without running into ODR violations
add_library(${PLUGIN_TARGET}_SharedCode INTERFACE)

# Enable fast math, C++23 and a few other target defaults
pamplejuce_shared_code_defaults(TARGET ${PLUGIN_TARGET}_SharedCode)

file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
target_sources(${PLUGIN_TARGET}_SharedCode INTERFACE ${SourceFiles})

target_compile_definitions(${PLUGIN_TARGET}_SharedCode
    INTERFACE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(${PLUGIN_TARGET}_SharedCode
    INTERFACE
    juce_audio_utils
    juce_audio_processors
    juce_dsp
    juce_gui_basics
    juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)

target_link_libraries(${PLUGIN_TARGET} PRIVATE ${PLUGIN_TARGET}_SharedCode)

pamplejuce_add_tests(
    TARGET ${PLUGIN_TARGET}_Tests
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test"
    PLUGIN_TARGET ${PLUGIN_TARGET}
    SHARED_CODE ${PLUGIN_TARGET}_SharedCode
    SOURCE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_pluginval_test(${PLUGIN_TARGET})
