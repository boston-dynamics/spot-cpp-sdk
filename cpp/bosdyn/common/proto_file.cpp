/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "proto_file.h"

#include <fcntl.h>
#include <sys/stat.h>

#ifdef WIN32
#    include <io.h>
#    define open _open
#    define close(a) _close(a)
#    define fsync(a) _commit(a)
#else
#    include <unistd.h>
#endif


namespace {
// RAII for file descriptor
class unique_fd {
 public:
    unique_fd(int fd) : m_fd(fd) {}
    unique_fd(const unique_fd& other) = delete;  // non-copyable.
    unique_fd(unique_fd&& other) { std::swap(m_fd, other.m_fd); }
    ~unique_fd() {
        if (*this) close();
    }
    inline int fd() { return m_fd; }
    operator bool() { return m_fd >= 0; }
    unique_fd& operator=(const unique_fd& other) = delete;
    unique_fd& operator=(unique_fd&& other) {
        std::swap(m_fd, other.m_fd);
        return *this;
    }
    bool close() {
        bool result = (::close(m_fd) == 0);
        m_fd = -1;
        return result;
    }

 private:
    int m_fd = -1;
};

struct ReadErrorCategory : public std::error_category {
    const char* name() const noexcept override { return "bosdyn::common::ReadError"; }
    std::string message(int ev) const override {
        switch (static_cast<bosdyn::common::ReadError>(ev)) {
            case bosdyn::common::ReadError::kSuccess:
                return "Success";
            case bosdyn::common::ReadError::kParseError:
                return "Failed to parse the message.";
            case bosdyn::common::ReadError::kEmptyFile:
                return "File was empty.";
        }
        return "Unknown error";
    }
    //    bool equivalent(int valcode, const std::error_condition& cond) const noexcept override {
    //    return false;}
};
struct WriteErrorCategory : public std::error_category {
    const char* name() const noexcept override { return "bosdyn::common::WriteError"; }
    std::string message(int ev) const override {
        switch (static_cast<bosdyn::common::WriteError>(ev)) {
            case bosdyn::common::WriteError::kSuccess:
                return "Success";
            case bosdyn::common::WriteError::kSerializationError:
                return "Failed to serialize the message.";
        }
        return "Unknown error";
    }
    //    bool equivalent(int valcode, const std::error_condition& cond) const noexcept override {
    //    return false;}
};

const ReadErrorCategory ReadErrorCategory_category{};
const WriteErrorCategory WriteErrorCategory_category{};

}  // namespace

namespace bosdyn::common {

bool ParseMessageFromFile(const std::string& filename, google::protobuf::Message* message,
                          ParseOptions options) {
    // error_code is true if there is an error.
    return !ParseMessageFromFileWithError(filename, message, options);
}

bool WriteMessageToFile(const std::string& filename, const google::protobuf::Message& message,
                        WriteOptions options) {
    // error_code is true if there is an error.
    return !WriteMessageToFileWithError(filename, message, options);
}

std::error_code make_error_code(ReadError value) {
    return {static_cast<int>(value), ReadErrorCategory_category};
}
std::error_code make_error_code(WriteError value) {
    return {static_cast<int>(value), WriteErrorCategory_category};
}
std::error_code ParseMessageFromFileWithError(const std::string& filename,
                                              google::protobuf::Message* message,
                                              ParseOptions options) {
    int mode = options.update_access_time ? O_RDWR : O_RDONLY;
    unique_fd fd(open(filename.c_str(), mode));
    if (!fd) {
        if (!((mode | O_RDWR) && (errno == EACCES || errno == EROFS)))
            return std::make_error_code(std::errc(errno));
        // Try again, falling back to not updating access time.
        fd = unique_fd(open(filename.c_str(), O_RDONLY));
        if (!fd) return std::make_error_code(std::errc(errno));
        ;
        options.update_access_time = false;
    }
#ifdef IS_LINUX
    if (options.update_access_time) {
        // Update access time but not modification time.
        timespec times[2] = {UTIME_NOW, UTIME_OMIT};
        futimens(fd.fd(), times);  // Don't bother with error checking on this one.
    }
#endif
    if (options.ensure_non_empty) {
        struct stat stat_buf;
        fstat(fd.fd(), &stat_buf);
        if (stat_buf.st_size == 0) return ReadError::kEmptyFile;
    }
    if (message->ParseFromFileDescriptor(fd.fd())) {
        return ReadError::kSuccess;
    } else {
        return ReadError::kParseError;
    }
}
std::error_code WriteMessageToFileWithError(const std::string& filename,
                                            const google::protobuf::Message& message,
                                            WriteOptions options) {
    unique_fd fd(open(filename.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0666));
    if (!fd) return std::make_error_code(std::errc(errno));
    if (!message.SerializeToFileDescriptor(fd.fd())) return WriteError::kSerializationError;
    if (options.fsync_file && fsync(fd.fd()) != 0) return std::make_error_code(std::errc(errno));
    if (fd.close()) {  // Will return false if closing fails.
        return WriteError::kSuccess;
    } else {
        return std::make_error_code(std::errc(errno));
    }
}
}  // namespace bosdyn::common
