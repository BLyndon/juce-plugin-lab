#include <PluginProcessor.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

TEST_CASE ("Plugin instance", "[instance]")
{
    PluginProcessor testPlugin;

    SECTION ("name")
    {
        // Must match PRODUCT_NAME in CMakeLists.txt
        CHECK_THAT (testPlugin.getName().toStdString(),
            Catch::Matchers::Equals ("Gain"));
    }

    SECTION ("program name")
    {
        // Steinberg's VST3 validator fails plugins whose programs have no name
        CHECK (testPlugin.getProgramName (0).isNotEmpty());
    }
}
