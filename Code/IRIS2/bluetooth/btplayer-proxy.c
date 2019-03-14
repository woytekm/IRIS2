#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include "btctl-proxy.h"
#include "global.h"


uint8_t bt_playerproxy_get_info(int sockfd)
 {

   uint16_t numbytes;
   char devbuf[BTMGR_MAXDATASIZE];
   char *token;
   char *cmd="show\n";
   char *info_lines[64];
   uint8_t i,found=0;
   char found_title[128];
   char found_album[128];
   char found_artist[128];
   
   bzero(found_title,128);
   bzero(found_album,128);
   bzero(found_artist,128);

   send(sockfd, cmd, strlen(cmd)+1, 0);
   sleep(1);

   if ((numbytes=recv(sockfd, devbuf, BTMGR_MAXDATASIZE, 0)) == -1) {
     return 0;
   }

   devbuf[numbytes] = '\0';

   token = strtok(devbuf,"\n");

   while(token != NULL)
    {
     if(strstr(token,"No default player available"))
      {
       bzero(G_bt_found_title,128);
       bzero(G_bt_found_artist,128);
       bzero(G_bt_found_album,128);
       return 0;
      }
      
     if(strstr(token,"Title"))
      {
       strncpy(found_title,token,strlen(token));
       found = 1;
      }
     if(strstr(token,"Album"))
      {
       strncpy(found_album,token,strlen(token));
       found = 1;
      }
     if(strstr(token,"Artist"))
      {
       strncpy(found_artist,token,strlen(token));
       found = 1;
      }
     token = strtok(NULL,"\n");
    }

   if(found)
    {
    if(strlen(found_title)>0)
     {
      token = strtok(found_title,":");
      token = strtok(NULL,":");
      if(token != NULL)
       if(strstr(token,"bluez") == NULL)   // filter out garbage from player command line
        strcpy(G_bt_found_title,token);
     }
    if(strlen(found_artist)>0)
     {
      token = strtok(found_artist,":");
      token = strtok(NULL,":");
      if(token != NULL)
       if(strstr(token,"bluez") == NULL)  // filter out garbage from player command line
         strcpy(G_bt_found_artist,token);
     }
    if(strlen(found_album)>0)
     {
      token = strtok(found_album,":");
      token = strtok(NULL,":");
      if(token != NULL)
       if(strstr(token,"bluez") == NULL)  // filter out garbage from player command line
        strcpy(G_bt_found_album,token);
     }
    }

   if(found) return 1;
   else return 0;

 }


