#ifndef COMMUNICATION_COMMUNICATION_H_
#define COMMUNICATION_COMMUNICATION_H_

#include <string>

#include "connect.h"

void SetCommandLimit(int limit);
int GetCommandLimit();

bool expect(const std::string& s);
bool expectOK();

bool login(const std::string& l, const std::string& pass);
bool cwait();

#endif  // COMMUNICATION_COMMUNICATION_H_
