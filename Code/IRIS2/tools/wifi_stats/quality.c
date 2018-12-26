#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include "wireless.h"

typedef uint8_t __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;

#define PROC_NET_WIRELESS       "/proc/net/wireless"
#define IFNAMSIZ        16

#ifndef ifr_name
#define ifr_name ifr_ifrn.ifrn_name
#endif

typedef struct iw_statistics    iwstats;

/*------------------------------------------------------------------*/
/*
 * Compare two ethernet addresses
 */
static inline int
iw_ether_cmp(const struct ether_addr* eth1, const struct ether_addr* eth2)
{
  return memcmp(eth1, eth2, sizeof(*eth1));
}

/*------------------------------------------------------------------*/
/*
 * Display an Ethernet address in readable format.
 */
void
iw_ether_ntop(const struct ether_addr * eth,
              char *                    buf)
{
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
          eth->ether_addr_octet[0], eth->ether_addr_octet[1],
          eth->ether_addr_octet[2], eth->ether_addr_octet[3],
          eth->ether_addr_octet[4], eth->ether_addr_octet[5]);
}


/*------------------------------------------------------------------*/
/*
 * Display an Wireless Access Point Socket Address in readable format.
 * Note : 0x44 is an accident of history, that's what the Orinoco/PrismII
 * chipset report, and the driver doesn't filter it.
 */
char *
iw_sawap_ntop(const struct sockaddr *   sap,
              char *                    buf)
{
  const struct ether_addr ether_zero = {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};
  const struct ether_addr ether_bcast = {{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }};
  const struct ether_addr ether_hack = {{ 0x44, 0x44, 0x44, 0x44, 0x44, 0x44 }};
  const struct ether_addr * ether_wap = (const struct ether_addr *) sap->sa_data;

  if(!iw_ether_cmp(ether_wap, &ether_zero))
    sprintf(buf, "Not-Associated");
  else
    if(!iw_ether_cmp(ether_wap, &ether_bcast))
      sprintf(buf, "Invalid");
    else
      if(!iw_ether_cmp(ether_wap, &ether_hack))
        sprintf(buf, "None");
      else
        iw_ether_ntop(ether_wap, buf);
  return(buf);
}


/*------------------------------------------------------------------*/
/*
 * Display an Wireless Access Point Socket Address in readable format.
 * Note : 0x44 is an accident of history, that's what the Orinoco/PrismII
 * chipset report, and the driver doesn't filter it.
 */
uint8_t
iw_is_ap_addr(const struct sockaddr *sap)
{
  const struct ether_addr ether_zero = {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};
  const struct ether_addr ether_bcast = {{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }};
  const struct ether_addr ether_hack = {{ 0x44, 0x44, 0x44, 0x44, 0x44, 0x44 }};
  const struct ether_addr * ether_wap = (const struct ether_addr *) sap->sa_data;

  if(!iw_ether_cmp(ether_wap, &ether_zero))
   return 0;
  else
    if(!iw_ether_cmp(ether_wap, &ether_bcast))
     return 0;
    else
      if(!iw_ether_cmp(ether_wap, &ether_hack))
       return 0;
      else
       return 1;
} 

/*------------------------------------------------------------------*/
/*
 * Wrapper to extract some Wireless Parameter out of the driver
 */
static inline int
iw_get_ext(int                  skfd,           /* Socket to the kernel */
           const char *         ifname,         /* Device name */
           int                  request,        /* WE ID */
           struct iwreq *       pwrq)           /* Fixed part of the request */
{
  /* Set device name */
  strncpy(pwrq->ifr_name, ifname, IFNAMSIZ);
  /* Do the request */
  return(ioctl(skfd, request, pwrq));
}

struct sockaddr *get_wifi_connected_ap_addr(char *ifname, struct sockaddr *addrinfo, int skfd)
  {

   struct iwreq wrq;

   if(iw_get_ext(skfd, ifname, SIOCGIWAP, &wrq) >= 0)
     {
      memcpy(addrinfo, &(wrq.u.ap_addr), sizeof (struct sockaddr));
      return addrinfo;
     }
   else
    return NULL;
 }

uint8_t iw_are_we_connected_or_not(char *ifname, struct sockaddr *addrinfo, int skfd)
  {

   struct iwreq wrq;

   if(iw_get_ext(skfd, ifname, SIOCGIWAP, &wrq) >= 0)
     {
      memcpy(addrinfo, &(wrq.u.ap_addr), sizeof (struct sockaddr));
     }
   else
    return 0;

   return iw_is_ap_addr(addrinfo);

 }

int iw_get_stats(const char *ifname, iwstats *stats)
 {
      FILE *    f = fopen(PROC_NET_WIRELESS, "r");
      char      buf[256];
      char *    bp;
      int       t;

      if(f==NULL)
        return -1;
      /* Loop on all devices */
      while(fgets(buf,255,f))
        {
          bp=buf;
          while(*bp&&isspace(*bp))
            bp++;
          /* Is it the good device ? */
          if(strncmp(bp,ifname,strlen(ifname))==0 && bp[strlen(ifname)]==':')
            {
              /* Skip ethX: */
              bp=strchr(bp,':');
              bp++;
              /* -- status -- */
              bp = strtok(bp, " ");
              sscanf(bp, "%X", &t);
              stats->status = (unsigned short) t;
              /* -- link quality -- */
              bp = strtok(NULL, " ");
              if(strchr(bp,'.') != NULL)
                stats->qual.updated |= 1;
              sscanf(bp, "%d", &t);
              stats->qual.qual = (unsigned char) t;
              /* -- signal level -- */
              bp = strtok(NULL, " ");
              if(strchr(bp,'.') != NULL)
                stats->qual.updated |= 2;
              sscanf(bp, "%d", &t);
              stats->qual.level = (unsigned char) t;
              /* -- noise level -- */
              bp = strtok(NULL, " ");
          if(strchr(bp,'.') != NULL)
                stats->qual.updated += 4;
              sscanf(bp, "%d", &t);
              stats->qual.noise = (unsigned char) t;
              /* -- discarded packets -- */
              bp = strtok(NULL, " ");
              sscanf(bp, "%d", &stats->discard.nwid);
              bp = strtok(NULL, " ");
              sscanf(bp, "%d", &stats->discard.code);
              bp = strtok(NULL, " ");
              sscanf(bp, "%d", &stats->discard.misc);
              fclose(f);
              /* No conversion needed */
              return 0;
            }
        }
      fclose(f);
      return 1;
    }

/*------------------------------------------------------------------*/
/*
 * Open a socket.
 * Depending on the protocol present, open the right socket. The socket
 * will allow us to talk to the driver.
 */
int
iw_sockets_open(void)
{
  static const int families[] = {
    AF_INET, AF_IPX, AF_AX25, AF_APPLETALK
  };
  unsigned int  i;
  int           sock;

  /*
   * Now pick any (exisiting) useful socket family for generic queries
   * Note : don't open all the socket, only returns when one matches,
   * all protocols might not be valid.
   * Workaround by Jim Kaba <jkaba@sarnoff.com>
   * Note : in 99% of the case, we will just open the inet_sock.
   * The remaining 1% case are not fully correct...
   */

  /* Try all families we support */
  for(i = 0; i < sizeof(families)/sizeof(int); ++i)
    {
      /* Try to open the socket, if success returns it */
      sock = socket(families[i], SOCK_DGRAM, 0);
      if(sock >= 0)
        return sock;
    }
  }


main()
 {

  iwstats wifi_stats;

  int skfd;             /* generic raw socket desc.     */
  int goterr = 0;
  struct sockaddr ap_addr, *return_ap_addr;
  char ap_addr_outcome[255];
  uint8_t connection_test;

  /* Create a channel to the NET kernel. */
  if((skfd = iw_sockets_open()) < 0)
    {
      perror("socket");
      exit(-1);
    }

  iw_get_stats("wlan0",&wifi_stats);
  printf("wifi quality: %d ",wifi_stats.qual.level);

  connection_test = iw_are_we_connected_or_not("wlan0",&ap_addr,skfd);
 
  if(connection_test == 1)
   printf(", connected\n");
  else
   printf(", disconnected\n");

 }


