#include "communication.h"

static const int buffer_size = 1024;

static char buffer[buffer_size];

bool expect(const std::string& s) {
  in("%s", buffer);
  if (s != buffer) {
    fprintf(stderr, "\033[31mRead '%s', expected '%s'\033[0m\n",
            buffer, s.c_str());
    return false;
  }
  return true;
}

bool expectOK() {
  in("%s", buffer);
  if (buffer != std::string("OK")) {
    fprintf(stderr, "\033[31mexpectOK, got \"%s", buffer);
    char c;
    while (in("%c", &c) != EOF) {
      if (c == '\n') {
        break;
      }
      fprintf(stderr, "%c", c);
    }
    fprintf(stderr, "\"\033[0m\n");
    return false;
  }
  return true;
}

bool login(const std::string& l, const std::string& pass) {
  if (!expect("LOGIN")) {
    return false;
  }
  out("%s\n", l.c_str());
  if (!expect("PASS")) {
    return false;
  }
  out("%s\n", pass.c_str());
  if (!expectOK()) {
    return false;
  }
  return true;
}

bool cwait() {
  out("WAIT\n");
  if (!expectOK()) {
    return false;
  }
  if (!expectOK()) {
    return false;
  }
  return true;
}