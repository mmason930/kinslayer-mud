#pragma once

#include "../jsoncpp/json.h"

// JsonCpp's member access asserts on a non-object; check before indexing.
inline bool readJsonCommand(const std::string &input, Json::Value &command)
{
    Json::Reader reader;
    return reader.parse(input, command, false) && command.isObject()
        && command["method"].isString();
}
