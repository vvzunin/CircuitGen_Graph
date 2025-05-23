#ifdef LOGFLAG

#include "easyloggingpp/easylogging++.h"
INITIALIZE_EASYLOGGINGPP

#include "string.h"

void initLogging(std::string testName, std::string subtestName) {
  el::Configurations conf("easylogging++.conf");
  el::Loggers::reconfigureAllLoggers(conf);
  el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Filename,
                                     "logs/" + testName + "/" + subtestName +
                                         ".log");
  el::Loggers::addFlag(el::LoggingFlag::NewLineForContainer);
  el::Loggers::addFlag(el::LoggingFlag::LogDetailedCrashReason);
  el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
  // LOG(INFO) << "Logging started\n\n";
}

#endif
