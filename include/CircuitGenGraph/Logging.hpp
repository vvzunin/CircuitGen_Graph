#pragma once

/**
 * @file Logging.hpp
 * \~english
 * @brief Centralized logging wrapper for CircuitGenGraph.
 *
 * \~russian
 * @brief Централизованная обертка логирования для CircuitGenGraph.
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
 * \~english
 * @brief Dummy stream buffer that discards all input.
 *
 * \~russian
 * @brief Временный буфер потока, отбрасывающий все входные данные.
 */
class NullBuffer : public std::streambuf {
public:
  int overflow(int c) override { return c; }
};

/**
 * \~english
 * @brief Dummy output stream that uses NullBuffer.
 *
 * \~russian
 * @brief Временный выходной поток, использующий NullBuffer.
 */
class NullStream : public std::ostream {
public:
  NullStream() : std::ostream(&m_sb) {}

private:
  NullBuffer m_sb;
};

/**
 * \~english
 * @brief Returns a reference to a global NullStream instance.
 *
 * \~russian
 * @brief Возвращает ссылку на глобальный экземпляр NullStream.
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
