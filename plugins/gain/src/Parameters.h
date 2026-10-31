#pragma once

// Parameter definitions from spec.md, the single place for IDs, ranges, and defaults.
// The ID and the range are part of the contract with the host: sessions and automation
// store the ID and a normalized value, so changing either breaks recall of saved projects.
namespace Parameters::Gain
{
    inline constexpr auto id = "gain";
    // Bump only when the meaning of the parameter changes
    inline constexpr int version = 1;
    inline constexpr auto name = "Gain";
    inline constexpr auto unit = "dB";

    // Level L in dB
    inline constexpr float minDb = -60.0f;
    inline constexpr float maxDb = 12.0f;
    inline constexpr float defaultDb = 0.0f;

    // Duration of the linear ramp in g after each change
    inline constexpr double rampTimeSeconds = 0.02;
}
