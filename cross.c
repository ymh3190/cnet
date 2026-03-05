#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define CLOSESOCKET closesocket
#define READSOCKET(s, b, l) recv(s, b, l, 0)
#else
#include <arpa/inet.h>
#include <unistd.h>
#define CLOSESOCKET close
#define READSOCKET(s, b, l) read(s, b, l)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 3030
#define BUFFER_SIZE 4096

void send_response(int client_socket, const char* status,
                   const char* content_type, const char* body) {
  char response[BUFFER_SIZE];

  sprintf(response,
          "HTTP/1.1 %s\r\n"
          "Content-Type: %s\r\n"
          "Content-Length: %ld\r\n"
          "Connection: close\r\n"
          "\r\n"
          "%s",
          status, content_type, strlen(body), body);

  send(client_socket, response, strlen(response), 0);
}

void handle_request(int client_socket, char* request) {
  printf("Request:\n%s\n", request);

  if (strncmp(request, "GET / ", 6) == 0) {
    send_response(client_socket, "200 OK", "application/json",
                  "{ \"message\": \"Hello from C backend\" }");
  } else if (strncmp(request, "GET /health ", 12) == 0) {
    send_response(client_socket, "200 OK", "application/json",
                  "{ \"status\": \"ok\" }");
  } else if (strncmp(request, "GET /v1/c ", 10) == 0) {
    send_response(client_socket, "200 OK", "application/json",
                  "{ \"status\": \"ok\" }");
  } else {
    send_response(client_socket, "404 Not Found", "application/json",
                  "{ \"error\": \"Not Found\" }");
  }
}

int main() {
#ifdef _WIN32
  WSADATA wsa;
  WSAStartup(MAKEWORD(2, 2), &wsa);
#endif

  int server_fd, client_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[BUFFER_SIZE];

  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 10) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  printf("Server is listening on port %d\n", PORT);

  while (1) {
    client_socket =
        accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

    if (client_socket < 0) {
      perror("accept");
      continue;
    }

    memset(buffer, 0, BUFFER_SIZE);
    READSOCKET(client_socket, buffer, BUFFER_SIZE);

    handle_request(client_socket, buffer);

    CLOSESOCKET(client_socket);
  }

#ifdef _WIN32
  WSACleanup();
#endif

  return 0;
}