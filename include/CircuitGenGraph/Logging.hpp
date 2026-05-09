#pragma once

/**
 * @file Logging.hpp
 * @brief Centralized logging wrapper for CircuitGenGraph.
 */

#ifdef LOGFLAG
    #include "easyloggingpp/easylogging++.h"

    #define CG_LOG_INFO    LOG(INFO)
    #define CG_LOG_WARNING LOG(WARNING)
    #define CG_LOG_ERROR   LOG(ERROR)
    #define CG_LOG_DEBUG   LOG(DEBUG)
    #define CG_VLOG(level) VLOG(level)
#else
    #include <iostream>
    #include <ostream>

    // Dummy stream to swallow log output when logging is disabled
    class NullBuffer : public std::streambuf {
    public:
        int overflow(int c) override { return c; }
    };

    class NullStream : public std::ostream {
    public:
        NullStream() : std::ostream(&m_sb) {}
    private:
        NullBuffer m_sb;
    };

    inline NullStream& getNullStream() {
        static NullStream ns;
        return ns;
    }

    #define CG_LOG_INFO    getNullStream()
    #define CG_LOG_WARNING getNullStream()
    #define CG_LOG_ERROR   getNullStream()
    #define CG_LOG_DEBUG   getNullStream()
    #define CG_VLOG(level) getNullStream()
#endif
