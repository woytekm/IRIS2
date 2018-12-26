#include "global.h"

struct MHD_Response *PL_service_REST_request(char *url, struct MHD_Connection *connection, const char *method)
 {

   struct MHD_Response *response;

   if(!strcmp(url,WEBSRV_API_RES_STREAM))
     {

      if(!strcmp(method,MHD_HTTP_METHOD_GET))
       {

       }

     }
   else if(!strcmp(url,WEBSRV_API_RES_RSS))
     {

     }
   else if(!strcmp(url,WEBSRV_API_RES_VOL))
    {

    }

   return response;

 }


