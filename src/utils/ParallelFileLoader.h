#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <atomic>
#include <optional>
#include <cstring>
#include <cerrno>
#include <algorithm>

// ---------------------------------------------------------------------------
// Platform detection
// ---------------------------------------------------------------------------
#if defined(_WIN32) || defined(_WIN64)
    #define PFL_PLATFORM_WINDOWS 1
#elif defined(__linux__)
    #define PFL_PLATFORM_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
    #define PFL_PLATFORM_MACOS 1
#else
    #define PFL_PLATFORM_POSIX_GENERIC 1
#endif

// ---------------------------------------------------------------------------
// Platform headers
// ---------------------------------------------------------------------------
#ifdef PFL_PLATFORM_WINDOWS
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <Windows.h>
#else
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #ifdef PFL_PLATFORM_LINUX
        #include <linux/stat.h>
    #endif
#endif

// ---------------------------------------------------------------------------
// FileLoadResult
// ---------------------------------------------------------------------------

/// Result of loading a single file.
struct FileLoadResult
{
    std::string content;
    int         errnum{0};     // 0 = success

    [[nodiscard]] bool ok() const noexcept { return errnum == 0; }

    [[nodiscard]] std::string errorMessage() const
    {
        if (ok())
            return {};

#ifdef PFL_PLATFORM_WINDOWS
        char buf[256]{};
        strerror_s(buf, sizeof(buf), errnum);
        return std::string(buf);
#else
        return std::string(strerror(errnum));
#endif
    }
};

// ---------------------------------------------------------------------------
// ParallelFileLoader
// ---------------------------------------------------------------------------

/// Loads many files in parallel using a fixed-size thread pool with
/// platform-optimised I/O for maximum throughput.
///
/// Usage:
///     ParallelFileLoader loader(4);                          // 4 threads
///     loader.submit("/path/a.js");
///     loader.submit({"/path/b.js", "/path/c.js"});
///     loader.wait();                                         // blocks until done, pool stays alive
///     // — or —
///     loader.finish();                                       // blocks, then shuts down the pool
///
///     if (auto* r = loader.get("/path/a.js"); r && r->ok())
///         use(r->content);
///
///     for (auto& [path, result] : loader.results())
///         ...
///
class ParallelFileLoader
{
public:
    /// Construct with desired thread count. Threads are spawned immediately.
    /// If threadCount is 0, it defaults to std::thread::hardware_concurrency().
    /// If maxFileSize is 0, no file size limit is enforced.
    explicit ParallelFileLoader(unsigned threadCount = 0, size_t maxFileSize = 0)
        : m_maxFileSize(maxFileSize)
    {
        if (threadCount == 0)
            threadCount = std::max(1u, std::thread::hardware_concurrency());

        m_workers.reserve(threadCount);
        for (unsigned i = 0; i < threadCount; ++i)
            m_workers.emplace_back([this] { workerLoop(); });
    }

    ~ParallelFileLoader()
    {
        finish();
    }

    ParallelFileLoader(const ParallelFileLoader&)            = delete;
    ParallelFileLoader& operator=(const ParallelFileLoader&) = delete;
    ParallelFileLoader(ParallelFileLoader&&)                 = delete;
    ParallelFileLoader& operator=(ParallelFileLoader&&)      = delete;

    /// Submit a single file path for loading.
    void submit(std::string filePath)
    {
        {
            std::lock_guard lock(m_queueMutex);
            m_queue.push(std::move(filePath));
            ++m_pendingCount;
        }
        m_queueCV.notify_one();
    }

    /// Submit multiple file paths at once.
    void submit(std::vector<std::string> filePaths)
    {
        {
            std::lock_guard lock(m_queueMutex);
            for (auto& p : filePaths)
            {
                m_queue.push(std::move(p));
                ++m_pendingCount;
            }
        }
        m_queueCV.notify_all();
    }

    /// Block until all submitted files have been loaded and shut down the
    /// thread pool. Safe to call multiple times.
    void finish()
    {
        {
            std::lock_guard lock(m_queueMutex);
            if (m_shutdown)
                return;
            m_shutdown = true;
        }
        m_queueCV.notify_all();

        for (auto& t : m_workers)
        {
            if (t.joinable())
                t.join();
        }
    }

    /// Block until all currently-submitted work is complete, but keep the pool
    /// alive so more files can be submitted afterwards.
    void wait()
    {
        std::unique_lock lock(m_queueMutex);
        m_doneCV.wait(lock, [this] { return m_pendingCount == 0; });
    }

    /// Retrieve the result for a given file path (read-only).
    /// Returns nullptr if the path was never submitted or hasn't finished yet.
    [[nodiscard]] const FileLoadResult* get(const std::string& filePath) const
    {
        std::lock_guard lock(m_resultsMutex);
        auto it = m_results.find(filePath);
        return (it != m_results.end()) ? &it->second : nullptr;
    }

    /// Move the result out for a given path. Returns std::nullopt if not found.
    [[nodiscard]] std::optional<FileLoadResult> take(const std::string& filePath)
    {
        std::lock_guard lock(m_resultsMutex);
        auto it = m_results.find(filePath);
        if (it == m_results.end())
            return std::nullopt;
        auto result = std::move(it->second);
        m_results.erase(it);
        return result;
    }

    /// Access all results. Only safe to call after finish() or wait().
    [[nodiscard]] const std::unordered_map<std::string, FileLoadResult>& results() const noexcept
    {
        return m_results;
    }

    [[nodiscard]] size_t successCount() const noexcept { return m_successCount.load(std::memory_order_relaxed); }
    [[nodiscard]] size_t failureCount() const noexcept { return m_failureCount.load(std::memory_order_relaxed); }

private:
    // -----------------------------------------------------------------------
    // Worker loop (platform-independent)
    // -----------------------------------------------------------------------
    void workerLoop()
    {
        for (;;)
        {
            std::string filePath;
            {
                std::unique_lock lock(m_queueMutex);
                m_queueCV.wait(lock, [this] { return !m_queue.empty() || m_shutdown; });

                if (m_queue.empty())
                    return;

                filePath = std::move(m_queue.front());
                m_queue.pop();
            }

            FileLoadResult result = loadFile(filePath);

            if (result.ok())
                m_successCount.fetch_add(1, std::memory_order_relaxed);
            else
                m_failureCount.fetch_add(1, std::memory_order_relaxed);

            {
                std::lock_guard lock(m_resultsMutex);
                m_results.emplace(std::move(filePath), std::move(result));
            }

            {
                std::lock_guard lock(m_queueMutex);
                --m_pendingCount;
                if (m_pendingCount == 0)
                    m_doneCV.notify_all();
            }
        }
    }

    // -----------------------------------------------------------------------
    // Platform-specific file loading
    // -----------------------------------------------------------------------

#ifdef PFL_PLATFORM_WINDOWS
    // -----------------------------------------------------------------------
    // Windows: CreateFileW + GetFileSizeEx + ReadFile
    //   - FILE_FLAG_SEQUENTIAL_SCAN hints the cache manager to read-ahead
    //   - Wide string conversion for full Unicode path support
    // -----------------------------------------------------------------------
    [[nodiscard]] FileLoadResult loadFile(const std::string& filePath) const
    {
        FileLoadResult result;

        // UTF-8 → UTF-16 for the Win32 wide API.
        const int wideLen = MultiByteToWideChar(
            CP_UTF8, 0,
            filePath.c_str(), static_cast<int>(filePath.size()),
            nullptr, 0);

        if (wideLen <= 0)
        {
            result.errnum = EINVAL;
            return result;
        }

        std::wstring widePath(static_cast<size_t>(wideLen), L'\0');
        MultiByteToWideChar(
            CP_UTF8, 0,
            filePath.c_str(), static_cast<int>(filePath.size()),
            widePath.data(), wideLen);

        HANDLE hFile = CreateFileW(
            widePath.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
            nullptr);

        if (hFile == INVALID_HANDLE_VALUE)
        {
            result.errnum = static_cast<int>(GetLastError());
            return result;
        }

        LARGE_INTEGER liSize;
        if (!GetFileSizeEx(hFile, &liSize))
        {
            result.errnum = static_cast<int>(GetLastError());
            CloseHandle(hFile);
            return result;
        }

        const auto fileSize = static_cast<size_t>(liSize.QuadPart);

        if (m_maxFileSize > 0 && fileSize > m_maxFileSize)
        {
            result.errnum = EFBIG;
            CloseHandle(hFile);
            return result;
        }

        result.content.resize(fileSize);

        size_t totalRead = 0;
        while (totalRead < fileSize)
        {
            const auto chunkSize = static_cast<DWORD>(
                std::min<size_t>(fileSize - totalRead, 0x7FFF'FFFFu));

            DWORD bytesRead = 0;
            if (!ReadFile(hFile, result.content.data() + totalRead,
                          chunkSize, &bytesRead, nullptr))
            {
                result.errnum = static_cast<int>(GetLastError());
                CloseHandle(hFile);
                return result;
            }

            if (bytesRead == 0)
            {
                result.content.resize(totalRead);
                break;
            }

            totalRead += bytesRead;
        }

        CloseHandle(hFile);
        return result;
    }

#elif defined(PFL_PLATFORM_LINUX)
    // -----------------------------------------------------------------------
    // Linux: open + statx + posix_fadvise + read
    //   - statx(AT_EMPTY_PATH, STATX_SIZE) for minimal-overhead size query
    //   - POSIX_FADV_SEQUENTIAL | POSIX_FADV_WILLNEED for aggressive readahead
    //   - POSIX_FADV_DONTNEED after read to release page cache pressure
    // -----------------------------------------------------------------------
    [[nodiscard]] FileLoadResult loadFile(const std::string& filePath) const
    {
        FileLoadResult result;

        const int fd = open(filePath.c_str(), O_RDONLY);
        if (fd < 0)
        {
            result.errnum = errno;
            return result;
        }

        struct statx stx{};
        if (statx(fd, "", AT_EMPTY_PATH, STATX_SIZE, &stx) != 0)
        {
            result.errnum = errno;
            close(fd);
            return result;
        }

        const auto fileSize = static_cast<size_t>(stx.stx_size);

        if (m_maxFileSize > 0 && fileSize > m_maxFileSize)
        {
            result.errnum = EFBIG;
            close(fd);
            return result;
        }

        posix_fadvise(fd, 0, static_cast<off_t>(fileSize),
                      POSIX_FADV_SEQUENTIAL | POSIX_FADV_WILLNEED);

        result.content.resize(fileSize);

        size_t totalRead = 0;
        while (totalRead < fileSize)
        {
            const ssize_t n = read(fd, result.content.data() + totalRead,
                                   fileSize - totalRead);
            if (n > 0)
            {
                totalRead += static_cast<size_t>(n);
            }
            else if (n == 0)
            {
                result.content.resize(totalRead);
                break;
            }
            else
            {
                if (errno == EINTR)
                    continue;
                result.errnum = errno;
                close(fd);
                return result;
            }
        }

        posix_fadvise(fd, 0, static_cast<off_t>(fileSize), POSIX_FADV_DONTNEED);

        close(fd);
        return result;
    }

#else
    // -----------------------------------------------------------------------
    // macOS / generic POSIX: open + fstat + read
    //   - macOS: fcntl(F_RDAHEAD) for sequential read-ahead hint
    //   - No posix_fadvise on macOS (not supported)
    // -----------------------------------------------------------------------
    [[nodiscard]] FileLoadResult loadFile(const std::string& filePath) const
    {
        FileLoadResult result;

        const int fd = open(filePath.c_str(), O_RDONLY);
        if (fd < 0)
        {
            result.errnum = errno;
            return result;
        }

        struct stat st{};
        if (fstat(fd, &st) != 0)
        {
            result.errnum = errno;
            close(fd);
            return result;
        }

        const auto fileSize = static_cast<size_t>(st.st_size);

        if (m_maxFileSize > 0 && fileSize > m_maxFileSize)
        {
            result.errnum = EFBIG;
            close(fd);
            return result;
        }

#ifdef PFL_PLATFORM_MACOS
        // macOS read-ahead hint (equivalent to POSIX_FADV_SEQUENTIAL).
        fcntl(fd, F_RDAHEAD, 1);
#endif

        result.content.resize(fileSize);

        size_t totalRead = 0;
        while (totalRead < fileSize)
        {
            const ssize_t n = read(fd, result.content.data() + totalRead,
                                   fileSize - totalRead);
            if (n > 0)
            {
                totalRead += static_cast<size_t>(n);
            }
            else if (n == 0)
            {
                result.content.resize(totalRead);
                break;
            }
            else
            {
                if (errno == EINTR)
                    continue;
                result.errnum = errno;
                close(fd);
                return result;
            }
        }

        close(fd);
        return result;
    }

#endif // platform

    // -----------------------------------------------------------------------
    // Members
    // -----------------------------------------------------------------------

    // Thread pool
    std::vector<std::thread>    m_workers;

    // Work queue
    std::queue<std::string>     m_queue;
    std::mutex                  m_queueMutex;
    std::condition_variable     m_queueCV;
    std::condition_variable     m_doneCV;
    size_t                      m_pendingCount{0};
    bool                        m_shutdown{false};

    // Results
    std::unordered_map<std::string, FileLoadResult> m_results;
    mutable std::mutex                              m_resultsMutex;

    // Stats
    std::atomic<size_t>         m_successCount{0};
    std::atomic<size_t>         m_failureCount{0};

    // Config
    size_t                      m_maxFileSize;
};