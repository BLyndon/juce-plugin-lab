cmake_minimum_required(VERSION 3.25)

# This tells cmake we have goodies in the /cmake folder
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (PamplejuceVersion)

# Modern concise way to add dependencies to your project
include (CPM)

# Configures universal binaries and decides which version of macOS to support
include(PamplejuceMacOS)

# PamplejuceMacOS sets 10.14, but current libc++ warns below macOS 11
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Support macOS down to Big Sur" FORCE)

# Couple tweaks that IMO should be JUCE defaults
include(JUCEDefaults)

# Force JUCE to work with sccache
# instruct MSVC to embed debug info (/Z7) instead of emitting a .pdb
set(CMAKE_POLICY_DEFAULT_CMP0141        NEW      CACHE STRING "" FORCE)
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT Embedded CACHE STRING "" FORCE)

# One CMake project for all plugins; each plugin is its own set of targets
project(juce-plugin-lab VERSION ${CURRENT_VERSION})

# Log build environment info (after project() so compiler info is available)
include(PamplejuceLog)

# Defines the pamplejuce_* functions the plugins call (also fetches Catch2 at include time)
include(PamplejuceFunctions)

# JUCE is setup as a submodule in the /JUCE folder
# Locally, you must run `git submodule update --init --recursive` once
# and later `git submodule update --remote --merge` to keep it up to date
# On Github Actions, this is done as a part of actions/checkout
add_subdirectory(JUCE)

# pluginval is downloaded into the build folder, nothing is installed system-wide
if (APPLE)
    set(PLUGINVAL_PLATFORM macOS)
elseif (WIN32)
    set(PLUGINVAL_PLATFORM Windows)
else ()
    set(PLUGINVAL_PLATFORM Linux)
endif ()

CPMAddPackage(
    NAME pluginval
    URL "https://github.com/Tracktion/pluginval/releases/download/v1.0.4/pluginval_${PLUGINVAL_PLATFORM}.zip"
    DOWNLOAD_ONLY YES)

# The macOS zip holds pluginval.app; extraction drops that top-level folder
if (APPLE)
    set(PLUGINVAL_EXECUTABLE "${pluginval_SOURCE_DIR}/Contents/MacOS/pluginval")
elseif (WIN32)
    set(PLUGINVAL_EXECUTABLE "${pluginval_SOURCE_DIR}/pluginval.exe")
else ()
    set(PLUGINVAL_EXECUTABLE "${pluginval_SOURCE_DIR}/pluginval")
endif ()

# Registers pluginval on the plugin's VST3 bundle as a CTest test in the calling directory
function(add_pluginval_test plugin)
    add_test(NAME ${plugin}_pluginval
        COMMAND "${PLUGINVAL_EXECUTABLE}" --strictness-level 10 --validate
            "$<GENEX_EVAL:$<TARGET_PROPERTY:${plugin}_VST3,JUCE_PLUGIN_ARTEFACT_FILE>>")
endfunction()

# Shared by all plugins. Each plugin sets its own PLUGIN_CODE, BUNDLE_ID and PRODUCT_NAME
set(COMPANY_NAME "juce-plugin-lab")
set(PLUGIN_MANUFACTURER_CODE Jplb)

# One line per plugin
add_subdirectory(plugins/template)
add_subdirectory(plugins/gain)
