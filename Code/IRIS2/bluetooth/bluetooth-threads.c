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


void PL_bluetoothplayer_proxy_thread()
  {

      int sockfd, numbytes;
      struct hostent *he;
      struct sockaddr_in their_addr; /* connector's address information */

      if ((he=gethostbyname("127.0.0.1")) == NULL) {  /* get the host info */
          herror("gethostbyname");
          exit(1);
      }

      if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
          perror("socket");
          exit(1);
      }

      their_addr.sin_family = AF_INET;      /* host byte order */
      their_addr.sin_port = htons(BTPLAYERPROXY_PORT);    /* short, network byte order */
      their_addr.sin_addr = *((struct in_addr *)he->h_addr);
      bzero(&(their_addr.sin_zero), 8);     /* zero the rest of the struct */

      if (connect(sockfd, (struct sockaddr *)&their_addr, \
                                           sizeof(struct sockaddr)) == -1) {
        PL_debug("PL_bluetoothplayer_proxy_thread: error while connecting to bluetooth-player control channel. Sleeping forever...");
        while (1) {sleep(1);}
       }

      PL_debug("PL_bluetoothplayer_proxy_thread: connected to bluetooth-player control channel.");

      while(1)
       {
        bt_playerproxy_get_info(sockfd);
        //PL_debug("PL_bluetoothplayer_proxy_thread: artist: %s, album: %s, song: %s",G_bt_found_artist,G_bt_found_album,G_bt_found_title);
        sleep(3);
       }

  }


void PL_bluetoothctl_proxy_thread()
  {
      uint8_t tries=0;
      int sockfd, numbytes;  
      struct hostent *he;
      struct sockaddr_in their_addr; /* connector's address information */

      if ((he=gethostbyname("127.0.0.1")) == NULL) {  /* get the host info */
          herror("gethostbyname");
          exit(1);
      }

      if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
          perror("socket");
          exit(1);
      }

      their_addr.sin_family = AF_INET;      /* host byte order */
      their_addr.sin_port = htons(BTCTLPROXY_PORT);    /* short, network byte order */
      their_addr.sin_addr = *((struct in_addr *)he->h_addr);
      bzero(&(their_addr.sin_zero), 8);     /* zero the rest of the struct */

      if (connect(sockfd, (struct sockaddr *)&their_addr, \
                                           sizeof(struct sockaddr)) == -1) {
        PL_debug("PL_bluetoothctl_proxy_thread: error while connecting to bluetoothctl control channel. Sleeping forever...");
        while (1) {sleep(1);}
       }

      PL_debug("PL_bluetoothctl_proxy_thread: connected to bluetoothctl control channel.");

      while(1)
       {

        if(btctlproxy_is_connected(sockfd,G_config.bt_pair_with))
         G_bt_source_connected = 1;
        else
         G_bt_source_connected = 0;

        if(G_bt_pair_now)
         if(strlen(G_config.bt_pair_with) > 3)
          {

           tries++;

           if(tries == 4)
             G_bt_pair_now = 0;

           if(btctlproxy_is_connected(sockfd,G_config.bt_pair_with))
            {
              PL_debug("PL_bluetoothctl_proxy_thread: %s is already connected\n",G_config.bt_pair_with);
              G_bt_pair_now = 0;
              continue;
            }

           PL_debug("PL_bluetoothctl_proxy_thread: starting scanning and pairing");

           if(!btctlproxy_power_on(sockfd))
            {
             PL_debug("PL_bluetoothctl_proxy_thread: error while communicating with bluetoothctl (power on). Stopping pairing.");
             G_bt_pair_now = 0;
             continue;
            }

           if(!btctlproxy_set_system_alias(sockfd,"IRIS2"))
            {
             PL_debug("PL_bluetoothctl_proxy_thread: error while communicating with bluetoothctl (set system alias). Stopping pairing.");
             G_bt_pair_now = 0;
             continue;
            }

           if(!btctlproxy_pairable_on(sockfd))
            {
             PL_debug("PL_bluetoothctl_proxy_thread: error while communicating with bluetoothctl (pairable on). Stopping pairing.");
             G_bt_pair_now = 0;
             continue;
            }

           if(!btctlproxy_discoverable_on(sockfd))
            {
             PL_debug("PL_bluetoothctl_proxy_thread: error while communicating with bluetoothctl (discoverable on). Stopping pairing.");
             G_bt_pair_now = 0;
             continue;
            }

           if(!btctlproxy_scan_on(sockfd))
            {
             PL_debug("PL_bluetoothctl_proxy_thread: error while communicating with bluetoothctl (scan on). Stopping pairing.");
             G_bt_pair_now = 0;
             continue;
            }

           sleep(3);

           if(!btctlproxy_find_and_pair(sockfd,G_config.bt_pair_with))
            {
             PL_debug("PL_bluetoothctl_proxy_thread: %s cannot be found. Will restart scanning.\n",G_config.bt_pair_with);
             continue;
            }

           sleep(5);

           if(btctlproxy_is_connected(sockfd,G_config.bt_pair_with))
            {   
             PL_debug("PL_bluetoothctl_proxy_thread: %s is connected.\n",G_config.bt_pair_with);
             G_bt_pair_now = 0;
            }

          }

        sleep(8);
       }

      close(sockfd);
    }


