/**
 * @file NamedPipeServer.cpp
 * @author DuckyLi
 * @date 2023-09-13 17:02:53
 * @description
 */
#ifdef _WIN32

#include <ff/NamedPipeServer.h>

NS_FF_BEG

NamedPipeServer::NamedPipeServer() {}

NamedPipeServer::~NamedPipeServer() {}

bool NamedPipeServer::start(const std::string& pipeName);
void NamedPipeServer::stop();

NS_FF_END

#endif
