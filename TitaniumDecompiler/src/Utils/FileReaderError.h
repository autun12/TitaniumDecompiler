#pragma once

#include <stdexcept>

class FileReaderError {
public:
    FileReaderError(const char* message) : message_(message) {}
    const char* what() const throw() { return message_; }

private:
    const char* message_;
};