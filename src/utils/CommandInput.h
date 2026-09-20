#pragma once

#include <string>

// Limit the text after escaping, without splitting an escaped dollar sign.
inline std::string prepareLegacyCommand(const std::string &input, bool escapeDollars,
                                       size_t limit, bool &truncated)
{
    std::string output;
    output.reserve(input.size() < limit ? input.size() : limit);
    truncated = false;
    for (char character : input)
    {
        const size_t width = escapeDollars && character == '$' ? 2 : 1;
        if (width > limit - output.size())
        {
            truncated = true;
            break;
        }
        output.append(width, character);
    }
    return output;
}
