#pragma once

#include "common.hpp"

namespace log {

    void info(const u8* message, ...);
    void warn(const u8* message, ...);
    void error(const u8* message, ...);

} // namespace log
