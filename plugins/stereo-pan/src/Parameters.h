#pragma once

// Parameter definitions from spec.md, the single place for IDs, ranges, and defaults.
// The ID and the range are part of the contract with the host: sessions and automation
// store the ID and a normalized value, so changing either breaks recall of saved projects.
namespace Parameters
{
    namespace Width
    {
        inline constexpr auto id = "width";
        // Bump only when the meaning of the parameter changes
        inline constexpr int version = 1;
        inline constexpr auto name = "Width";

        // Width w, shown as 0 … 100 %
        inline constexpr float min = 0.0f;
        inline constexpr float max = 1.0f;
        inline constexpr float defaultValue = 1.0f;
    }

    namespace Position
    {
        inline constexpr auto id = "position";
        // Bump only when the meaning of the parameter changes
        inline constexpr int version = 1;
        inline constexpr auto name = "Position";

        // Position p, shown as 100 % L … C … 100 % R
        inline constexpr float min = -1.0f;
        inline constexpr float max = 1.0f;
        inline constexpr float defaultValue = 0.0f;
    }

    // Duration of the linear ramp in a, b, c, d after each change
    inline constexpr double rampTimeSeconds = 0.02;
}
