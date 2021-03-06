#include "connect.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <netdb.h>
#include <netinet/tcp.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static void syserr(const char* fmt, ...) {
  va_list args;
  fprintf(stderr, "\033[41m\033[97mERROR: ");
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, " (%d; %s)\033[0m\n", errno, strerror(errno));
  exit(EXIT_FAILURE);
}

static FILE *to_server, *from_server;
static int sock;

void Connect(const std::string& host, int port) {
  struct addrinfo addr_hints;
  struct addrinfo *addr_result;
  (void) memset(&addr_hints, 0, sizeof(struct addrinfo));
  addr_hints.ai_family = AF_INET;
  addr_hints.ai_socktype = SOCK_STREAM;
  addr_hints.ai_protocol = IPPROTO_TCP;
  addr_hints.ai_flags = 0;
  addr_hints.ai_addrlen = 0;
  addr_hints.ai_addr = NULL;
  addr_hints.ai_canonname = NULL;
  addr_hints.ai_next = NULL;
  if (getaddrinfo(host.c_str(), NULL, &addr_hints, &addr_result) != 0) {
    syserr("getaddrinfo");
  }

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr =
      ((struct sockaddr_in*) (addr_result->ai_addr))->sin_addr.s_addr;
  address.sin_port = htons(port);

  freeaddrinfo(addr_result);

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    syserr("socket");
  }

  if (connect(sock, (struct sockaddr*) &address, sizeof(address)) == -1) {
    syserr("connect");
  }

  int sso_flag = 1;
  if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
      (char*) &sso_flag, sizeof(int)) == -1) {
    syserr("setsockopt");
  }

  to_server = fdopen(sock, "w");
  from_server = fdopen(sock, "r");
  if (to_server == NULL || from_server == NULL) {
    syserr("fdopen");
  }

  if (setvbuf(to_server, NULL, _IOLBF, 0) != 0) {
    syserr("setvbuf");
  }
}

void Disconnect() {
  if (close(sock) != 0) {
    syserr("close");
  }
}

int in(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int ret = vfscanf(from_server, fmt, args);
  va_end(args);
  return ret;
}

static char buffer[1 << 20];
static char buffer2[1 << 20];
static bool do_print_out = true;

int out(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsprintf(buffer, fmt, args);
  va_end(args);
  if (do_print_out) {
    int pos = 0;
    for (int i = 0; ; i++) {
      if (buffer[i] == '\0') {
        buffer2[pos++] = '\0';
        break;
      } else if (buffer[i] == '\n') {
        buffer2[pos++] = '\\';
        buffer2[pos++] = 'n';
      } else if (buffer[i] == '\t') {
        buffer2[pos++] = '\\';
        buffer2[pos++] = 't';
      } else {
        buffer2[pos++] = buffer[i];
      }
    }
    printf("You> \033[34m%s\033[0m\n", buffer2);
  }
  int ret = fprintf(to_server, "%s", buffer);
  return ret;
}

void SetPrintOut(bool do_print) {
  do_print_out = do_print;
}
