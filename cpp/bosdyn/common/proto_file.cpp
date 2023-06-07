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
#include <unistd.h>


namespace {
// RAII for file descriptor
class unique_fd {
 public:
    unique_fd(int fd) : m_fd(fd) {}
    unique_fd(const unique_fd& other) = delete;  // non-copyable.
    unique_fd(unique_fd&& other) { std::swap(m_fd, other.m_fd); }
    ~unique_fd() {
        if (fd()) close();
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
}  // namespace

namespace bosdyn::common {

bool ParseMessageFromFile(const std::string& filename, google::protobuf::Message* message,
                          ParseOptions options) {
    int mode = options.update_access_time ? O_RDWR : O_RDONLY;
    unique_fd fd(open(filename.c_str(), mode));
    if (!fd) {
        if (!((mode | O_RDWR) && (errno == EACCES || errno == EROFS))) return false;
        // Try again, falling back to not updating access time.
        fd = unique_fd(open(filename.c_str(), O_RDONLY));
        if (!fd) return false;
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
        if (stat_buf.st_size == 0) return false;
    }
    return message->ParseFromFileDescriptor(fd.fd());
}

bool WriteMessageToFile(const std::string& filename, const google::protobuf::Message& message,
                        WriteOptions options) {
    unique_fd fd(open(filename.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0666));
    if (!fd) return false;
    if (!message.SerializeToFileDescriptor(fd.fd())) return false;
    if (options.fsync_file && fsync(fd.fd()) != 0) return false;
    return fd.close();  // Will return false if closing fails.
}
}  // namespace bosdyn::common
