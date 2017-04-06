#ifndef COMMUNICATION_CONNECT_H_
#define COMMUNICATION_CONNECT_H_

#include <cstdarg>
#include <string>

void Connect(const std::string& host, int port);
void Disconnect();

int in(const char* fmt, ...);
int out(const char* fmt, ...);

#endif  // COMMUNICATION_CONNECT_H_
