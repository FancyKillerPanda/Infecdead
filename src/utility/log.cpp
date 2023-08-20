#include "utility/log.hpp"

#include <stdio.h>
#include <string.h>
#include <time.h>

namespace log_ {

    constexpr const u8* YELLOW = "\x1b[33m";
    constexpr const u8* RED = "\x1b[31m";
    constexpr const u8* RESET = "\x1b[0m";

    u32 currentHeaderLength = 0;

    void print_current_time() {
        time_t current = time(NULL);
        tm* local = localtime(&current);

        printf("[%02d:%02d:%02d]", local->tm_hour, local->tm_min, local->tm_sec);
    }

    void print_string(const u8* message) {
        while (*message) {
            usize numBytes = strcspn(message, "\n");
            printf("%.*s\n", (s32) numBytes, message);

            // Skip the part we printed, and the newline.
            message += numBytes;
            if (*message == '\n') {
                message += 1;
            }

            if (*message) {
                for (u32 i = 0; i < currentHeaderLength; i++) {
                    printf(" ");
                }
            }
        }
    }

    void log_internal(const u8* colour, const u8* identifier, const u8* message, va_list args) {
        // Time is 10 characters long
        currentHeaderLength = 10 + strlen(identifier);

        printf("%s", colour);
        print_current_time();
        printf("%s", identifier);

        va_list argsCopy;
        va_copy(argsCopy, args);

        usize bufferLength = vsnprintf(nullptr, 0, message, args) + 1;
        u8* buffer = (u8*) calloc(bufferLength, 1);
        vsnprintf(buffer, bufferLength, message, argsCopy);

        print_string(buffer);
        printf(RESET);
        free(buffer);

        va_end(argsCopy);
    }

    void info(const u8* message, ...) {
        va_list args;
        va_start(args, message);

        log_internal("", "    INFO: ", message, args);

        va_end(args);
    }

    void warn(const u8* message, ...) {
        va_list args;
        va_start(args, message);

        log_internal(YELLOW, " WARNING: ", message, args);

        va_end(args);
    }

    void error(const u8* message, ...) {
        va_list args;
        va_start(args, message);

        log_internal(RED, "   ERROR: ", message, args);

        va_end(args);
    }

} // namespace log_
