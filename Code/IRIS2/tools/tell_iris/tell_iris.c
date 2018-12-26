#include "global.h"

int main(int argc, char *argv[])
{
 struct sockaddr_un address;
 int  socket_fd, nbytes;
 char buffer[256];
 char response_buffer[4096];

 if(argc < 2)
  {
   printf("not enough parameters - give iris some command!\n");
   exit(-1);
  }


 socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);

 if(socket_fd < 2)
 {
  printf("socket() failed\n");
  return 1;
 }

 /* start with a clean address structure */
 memset(&address, 0, sizeof(struct sockaddr_un));
 
 address.sun_family = AF_UNIX;

 snprintf(address.sun_path, UNIX_PATH_MAX, "/var/run/iris_cmd");

 if(connect(socket_fd, 
            (struct sockaddr *) &address, 
            sizeof(struct sockaddr_un)) != 0)
 {
  printf("connect() failed - is Iris running?\n");
  return 1;
 }

 nbytes = snprintf(buffer, 256, argv[1]);
 write(socket_fd, buffer, nbytes);
 
 nbytes = read(socket_fd, response_buffer, 4096);

 if(nbytes == 0)
  printf("no response from Iris\n");
 else
  printf("Iris said: %s\n", response_buffer);

 close(socket_fd);

 return 0;
}


