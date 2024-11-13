#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#if defined(_WIN32) || defined(_WIN64)
   #define WINDOWS_PLATFORM
#endif

#ifdef WINDOWS_PLATFORM
    #pragma comment(lib, "ws2_32.lib") 
    #include <winsock2.h>
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <arpa/inet.h> // clinet
#endif 

typedef enum {
  HTTP_GET,
  HTTP_POST
}HTTP_Method;

#define MAX_MESSAGE_CAPACITY 1024

typedef struct {
  char message[MAX_MESSAGE_CAPACITY];
  size_t message_size;
}HTTP;

const char* get_method_as_cstr(HTTP_Method method) {
  switch(method) {
  case HTTP_GET:
    return "GET";
  case HTTP_POST:
    return "POST";

  default:
    assert(0 && "Unreachable method: get_method_as_cstr()\n");
  }

  return NULL;
}

void http_write(HTTP* http, const char* chunk) {
  size_t chunk_size = strlen(chunk);
  assert(http->message_size + chunk_size < MAX_MESSAGE_CAPACITY);

  for(size_t i =0;i < chunk_size; ++i) {
    http->message[http->message_size++] = chunk[i];
  }
}

void http_append_header(HTTP* http, const char* header, const char* value) {
  http_write(http, header);
  http_write(http, ": ");
  http_write(http, value);
  http_write(http, "\r\n");
}

void http_request_start(HTTP* http, HTTP_Method method, const char *message) {
  const char *method_cstr = get_method_as_cstr(method);
  http_write(http, method_cstr);
  // const char* page = get_page()
  // http_write(http, page);
  http_write(http, " / ");
  char* http_version = "HTTP/1.1\r\n";
  http_write(http, http_version);

  if (message) {
    http_write(http, message);
  }
}

int main(void) {

  int s = -1;
  struct sockaddr_in server;
  int recv_size = 0;

#ifdef WINDOWS_PLATFORM
   WSADATA wsa;
   if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
     fprintf(stderr, "[ERROR]: Failed to initalized Windows socket\n");
     exit(1);
   }
#endif

  if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)    {
    printf("[ERROR]: Could not create socket: %s\n", strerror(errno));
    exit(1);
  }

  memset(&server, 0, sizeof(server));
  server.sin_addr.s_addr = inet_addr("ip");
  server.sin_family = AF_INET;
  server.sin_port = htons(80);

  if (connect(s, (struct sockaddr *)(&server), sizeof(server)) < 0) {
     printf("[ERROR]: Could not connect to server: %s\n", strerror(errno));
     exit(1);
  }

  HTTP http = {0};
  http_request_start(&http, HTTP_GET, NULL);
  http_append_header(&http, "Host", "google.com");
  printf("%s\n", http.message);
  
  if(send(s, http.message, http.message_size, 0) < 0) {
     printf("[ERROR]: Could not send http request to server: %s\n", strerror(errno));
     exit(1);
  }
    

  if((recv_size = recv(s, server_reply, REPLY_SIZE, 0)) < 0) {
     printf("[ERROR]: Something wrong happened: %s\n", strerror(errno));
     exit(1);
  }

  server_reply[REPLY_SIZE - 1] = 0;

  printf("\nServer Reply:\n\n");
  printf("%s\n", server_reply);

    // Close the socket
#if defined(_WIN32) || defined(_WIN64)  
    closesocket(s);
    WSACleanup();
#else
    close(s);
#endif
    
  return 0;
}
