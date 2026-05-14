#pragma once

/**
 * @file Logging.hpp
 * @brief Centralized logging wrapper for CircuitGenGraph.
 */

#ifdef LOGFLAG
#include "easyloggingpp/easylogging++.h"

#define CG_LOG_INFO LOG(INFO)
#define CG_LOG_WARNING LOG(WARNING)
#define CG_LOG_ERROR LOG(ERROR)
#define CG_LOG_FATAL LOG(FATAL)
#define CG_LOG_DEBUG LOG(DEBUG)
#define CG_LOG_TRACE LOG(TRACE)
#define CG_VLOG(level) VLOG(level)
#else
#include <iostream>
#include <ostream>

/**
 * @brief Dummy stream buffer that discards all input.
 */
class NullBuffer : public std::streambuf {
public:
  int overflow(int c) override { return c; }
};

/**
 * @brief Dummy output stream that uses NullBuffer.
 */
class NullStream : public std::ostream {
public:
  NullStream() : std::ostream(&m_sb) {}

private:
  NullBuffer m_sb;
};

/**
 * @brief Returns a reference to a global NullStream instance.
 */
inline NullStream &getNullStream() {
  static NullStream ns;
  return ns;
}

#define CG_LOG_INFO getNullStream()
#define CG_LOG_WARNING getNullStream()
#define CG_LOG_ERROR getNullStream()
#define CG_LOG_FATAL getNullStream()
#define CG_LOG_DEBUG getNullStream()
#define CG_LOG_TRACE getNullStream()
#define CG_VLOG(level) getNullStream()
#endif
