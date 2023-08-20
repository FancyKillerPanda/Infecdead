#pragma once

#include "common.hpp"

namespace log_ {

    void info(const u8* message, ...);
    void warn(const u8* message, ...);
    void error(const u8* message, ...);

} // namespace log_
