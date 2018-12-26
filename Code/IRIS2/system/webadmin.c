#include "global.h"
#include "webadmin.h"


char *PL_build_admin_panel()
 {

    char *panel_code;
    static char *response_data = "<html><body><b><h2>Admin panel here</b></h2></body></html>";

    panel_code = malloc(strlen(response_data)+1);
    strcpy(panel_code,response_data);

    return panel_code;

 }


int PL_service_admin_panel_req(char *url, struct MHD_Connection *connection, const char *method)  
 {

  struct MHD_Response *response;
  char *next_token;
  char *real_path;
  char *tmp_url;
  char *file_path;
  char *file_contents;
  char *mime_type = NULL,*ext;
  char *main_panel_code;
  int fd, ret;
  struct stat fstat;
  unsigned int status_code;
  
  

  static char *code_400_response = "<html><head><title>invalid request</title></head><body>400 invalid request</body>";
  static char *code_404_response = "<html><head><title>not found</title></head><body>404 not found</body>";
  static char *code_500_response = "<html><head><title>500</title></head><body>500 internal server error</body>";

  tmp_url = strdup(url);

  if(!strcmp(method, "GET"))
   {
     next_token = strtok(tmp_url,"/"); // cut off "admin" string
     next_token = strtok(NULL,"/");

     if(next_token == NULL) //  "GET /admin/ " - we service this request separately
      {
        main_panel_code = PL_build_admin_panel();
        response = MHD_create_response_from_buffer(strlen(main_panel_code),main_panel_code,MHD_RESPMEM_MUST_FREE);   //show default admin panel
        MHD_add_response_header(response, "Content-Type", "text/html");
        status_code = MHD_HTTP_OK;
      }
    else
     {
       if(strstr(url,"..") != NULL)
        {
          PL_debug("URL path traversal detected - discarding request");
          response = MHD_create_response_from_buffer(strlen(code_400_response),(void *)code_400_response,MHD_RESPMEM_PERSISTENT);
          status_code = MHD_HTTP_BAD_REQUEST;
        }
      else
       { 

       file_path = malloc(strlen(url) + strlen(WEBSRV_DOCUMENT_ROOT)+2);
       sprintf(file_path,"%s%s",WEBSRV_DOCUMENT_ROOT,url);
       
       if( stat(file_path,&fstat) == -1 )
        {
          response = MHD_create_response_from_buffer(strlen(code_404_response),(void *)code_404_response,MHD_RESPMEM_PERSISTENT);
          status_code = MHD_HTTP_NOT_FOUND;
          PL_debug("PL_service_admin_panel_req: 404: no such file %s",url);
        }
       else
        {
          if( (fd = open(file_path,O_RDONLY)) == -1)
           {
             PL_debug("PL_service_admin_panel_req: cannot open %s ",file_path);
             response = MHD_create_response_from_buffer(strlen(code_404_response),(void *)code_404_response,MHD_RESPMEM_PERSISTENT);
             status_code = MHD_HTTP_NOT_FOUND;
           }
          else
           {
             response = MHD_create_response_from_fd(fstat.st_size,fd);
             if(response == NULL)
              {
               PL_debug("PL_service_admin_panel_req: cannot create response from file descriptor (%s)",file_path);
               response = MHD_create_response_from_buffer(strlen(code_404_response),(void *)code_404_response,MHD_RESPMEM_PERSISTENT);
               status_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
              }
             else
              { 
                // mime type detection stolen from pom-ng code
  		mime_type = "binary/octet-stream";
		char *ext = strrchr(file_path, '.');
    	        if (ext) {
		ext++;
		if (!strcasecmp(ext, "html") || !strcasecmp(ext, "htm"))
		 mime_type = "text/html";
		else if (!strcasecmp(ext, "png"))
		 mime_type = "image/png";
		else if (!strcasecmp(ext, "jpg") || !strcasecmp(ext, "jpeg"))
		 mime_type = "image/jpeg";
		else if (!strcasecmp(ext, "js"))
		 mime_type = "application/javascript";
		else if (!strcasecmp(ext, "css"))
		 mime_type = "text/css";
		}                
                MHD_add_response_header(response, "Content-Type", mime_type);
                status_code = MHD_HTTP_OK;
                PL_debug("PL_service_admin_panel_req: 200 OK, serving %s (%d bytes)",file_path, fstat.st_size);
              }

           }
         }

        }
     }

    free(tmp_url);
    free(file_path);

    ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
   }

  return ret;

 }


