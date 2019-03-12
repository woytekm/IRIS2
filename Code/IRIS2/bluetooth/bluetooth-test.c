#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h>

#define PORT 8888    /* the port client will be connecting to */

#define MAXDATASIZE 2048 /* max number of bytes we can get at once */


uint8_t btctlproxy_trust(int sockfd, char *bt_address)
 {

   uint16_t numbytes;
   char buf[MAXDATASIZE];

   char cmd[20];

   sprintf(cmd,"trust %s\n",bt_address);

   send(sockfd, cmd, strlen(cmd)+1, 0);

   sleep(1);

   if ((numbytes=recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
     return 0;
   }

  buf[numbytes] = '\0';

  if(strstr(buf,"succeeded"))
   return 1;
  else return 0;

 }


uint8_t btctlproxy_pair(int sockfd, char *bt_address)
 {

   uint16_t numbytes;
   char buf[MAXDATASIZE];

   char cmd[20];

   sprintf(cmd,"pair %s\n",bt_address);

   send(sockfd, cmd, strlen(cmd)+1, 0);

   sleep(1);

   if ((numbytes=recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
     return 0;
   }

  buf[numbytes] = '\0';

  if(strstr(buf,"succeeded"))
   return 1;
  else return 0;

 }


uint8_t btctlproxy_is_connected(int sockfd, char *endpoint_name)
 {
 
   uint16_t numbytes;
   char infobuf[MAXDATASIZE]; 
   char *cmd="info\n";
   
   send(sockfd, cmd, strlen(cmd)+1, 0);

   sleep(1);

   if ((numbytes=recv(sockfd, infobuf, MAXDATASIZE, 0)) == -1) {
     return 0;
   }

   if(strstr(infobuf,"Missing"))
    return 0;
   else if(strstr(infobuf,"Connected: yes"))
    if(strstr(infobuf,endpoint_name))
     return 1;
    else
     return 0;
 }


uint8_t btctlproxy_find_and_pair(int sockfd, char *endpoint_name)
 {

   uint16_t numbytes;
   char devbuf[MAXDATASIZE];
   char *token;
   char *cmd="devices\n";
   char *found_devices[64];
   char found_bt_addr[20];
   char found_bt_name[128];
   uint8_t device_count=0;
   uint8_t i,found=0,addr_provided=0;

   
   if(strstr(endpoint_name,":")) addr_provided = 1;

   if(!addr_provided)
    {

     send(sockfd, cmd, strlen(cmd)+1, 0);
     sleep(1);

     if ((numbytes=recv(sockfd, devbuf, MAXDATASIZE, 0)) == -1) {
       return 0;
      }

     devbuf[numbytes] = '\0';

     token = strtok(devbuf,"\n");

     while(token != NULL)
      {

     if(strstr(token,"Device"))
      {
       found_devices[device_count] = malloc(strlen(token)+1);
       strncpy(found_devices[device_count],token,strlen(token));
       device_count++;
      }

     token = strtok(NULL,"\n");

    }

   for(i=0;i<=device_count; i++)
   {
    token = strtok(found_devices[i]," ");
    if(token == NULL) return 0; 
    token = strtok(NULL," ");
    if(token != NULL)
     strcpy(found_bt_addr,token);
    else continue;
    token = strtok(NULL," ");
    if(token != NULL)
     strcpy(found_bt_name,token);
    else continue;

    if(strstr(found_bt_name,endpoint_name))
     {
       if(strlen(found_bt_name) == strlen(endpoint_name))
        {
          found = 1;
          break;
        }
     }
   }
  if(found)
   {
     btctlproxy_trust(sockfd,found_bt_addr);
     btctlproxy_pair(sockfd,found_bt_addr);
   }
  else
   return 0;
  }
 else
  {
    btctlproxy_trust(sockfd,endpoint_name);
    btctlproxy_pair(sockfd,endpoint_name);
  }

 return 1;

 }

uint8_t btctlproxy_power_on(int sockfd)
 {

   uint16_t numbytes;
   char buf[MAXDATASIZE];
   char *cmd="power on\n";

   send(sockfd, cmd, strlen(cmd)+1, 0);

   sleep(1);

   if ((numbytes=recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
     return 0;
   }

  buf[numbytes] = '\0';

  if(strstr(buf,"succeeded"))
   return 1;
  else return 0;

 }

uint8_t btctlproxy_scan_on(int sockfd)
 {

   uint16_t numbytes;
   char buf[MAXDATASIZE];
   char *cmd="scan on\n";

   send(sockfd, cmd, strlen(cmd)+1, 0);

   sleep(1);

   if ((numbytes=recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
     return 0;
   }

  buf[numbytes] = '\0';

  if(strstr(buf,"started"))
   return 1;
  else return 0;

 }


uint8_t btctlproxy_pairable_on(int sockfd)
 {

   uint16_t numbytes;
   char buf[MAXDATASIZE];
   char *cmd="pairable on\n";

   send(sockfd, cmd, strlen(cmd)+1, 0);

   sleep(1);

   if ((numbytes=recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
     return 0;
   }

  buf[numbytes] = '\0';

  if(strstr(buf,"succeeded"))
   return 1;
  else return 0;

 }


uint8_t btctlproxy_discoverable_on(int sockfd)
 {

   uint16_t numbytes;
   char buf[MAXDATASIZE];
   char *cmd="discoverable on\n";

   send(sockfd, cmd, strlen(cmd)+1, 0);

   sleep(1);

   if ((numbytes=recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
     return 0;
   }

  buf[numbytes] = '\0';

  if(strstr(buf,"succeeded"))
   return 1;
  else return 0;

 }

uint8_t btctlproxy_set_system_alias(int sockfd, char *newalias)
 {

   uint16_t numbytes;
   char buf[MAXDATASIZE];

   char cmd[20];

   sprintf(cmd,"system-alias %s\n",newalias);

   send(sockfd, cmd, strlen(cmd)+1, 0);

   sleep(1);

   if ((numbytes=recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
     return 0;
   }

  buf[numbytes] = '\0';

  if(strstr(buf,"succeeded"))
   return 1;
  else return 0;

 }




int main(int argc, char *argv[])
  {
      int sockfd, numbytes;  
      struct hostent *he;
      struct sockaddr_in their_addr; /* connector's address information */
      char *pair_with="Redmi";

      if (argc != 3) {
            fprintf(stderr,"usage: client hostname bt_devname\n");
            exit(1);
      }

      if ((he=gethostbyname(argv[1])) == NULL) {  /* get the host info */
          herror("gethostbyname");
          exit(1);
      }

      if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
          perror("socket");
          exit(1);
      }

      their_addr.sin_family = AF_INET;      /* host byte order */
      their_addr.sin_port = htons(PORT);    /* short, network byte order */
      their_addr.sin_addr = *((struct in_addr *)he->h_addr);
      bzero(&(their_addr.sin_zero), 8);     /* zero the rest of the struct */

      if (connect(sockfd, (struct sockaddr *)&their_addr, \
                                           sizeof(struct sockaddr)) == -1) {
        perror("connect");
        exit(1);
       }


      if(btctlproxy_is_connected(sockfd,argv[2]))
       {
        printf("already connected\n");
        exit(0);
       }
      
      if(btctlproxy_power_on(sockfd))
        printf("BT power on successfull\n");

      if(btctlproxy_set_system_alias(sockfd,"IRIS2"))
        printf("alias successfull\n");

      if(btctlproxy_pairable_on(sockfd))
        printf("pairable successfull\n");

      if(btctlproxy_discoverable_on(sockfd))
        printf("discoverable successfull\n");

      if(btctlproxy_scan_on(sockfd))
        printf("scan start successfull\n");

      sleep(3);

      if(!btctlproxy_find_and_pair(sockfd,argv[2]))
       {
        printf("device not found!\n");
        exit(0);
       }

      sleep(5);

      if(btctlproxy_is_connected(sockfd,argv[2]))
       printf("connected\n");

      close(sockfd);
      return 0;
    }


