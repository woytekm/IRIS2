#include "global.h"

/* Read a STREAM and return a newly allocated string with the content,
   and set *LENGTH to the length of the string.  The string is
   zero-terminated, but the terminating zero byte is not counted in
   *LENGTH.  On errors, *LENGTH is undefined, errno preserves the
   values set by system functions (if any), and NULL is returned.  (from old version of libmicrohttpd)*/
char *
fread_file (FILE * stream, size_t * length)
{
  char *buf = NULL;
  size_t alloc = 0;
  size_t size = 0;
  int save_errno;

  for (;;)
    {
      size_t count;
      size_t requested;

      if (size + BUFSIZ + 1 > alloc)
        {
          char *new_buf;

          alloc += alloc / 2;
          if (alloc < size + BUFSIZ + 1)
            alloc = size + BUFSIZ + 1;

          new_buf = realloc (buf, alloc);
          if (!new_buf)
            {
              save_errno = errno;
              break;
            }

          buf = new_buf;
        }

      requested = alloc - size - 1;
      count = fread (buf + size, 1, requested, stream);
      size += count;

      if (count != requested)
        {
          save_errno = errno;
          if (ferror (stream))
            break;
          buf[size] = '\0';
          *length = size;
          return buf;
        }
    }

  free (buf);
  errno = save_errno;
  return NULL;
}

static char *
read_file (const char *filename, size_t * length)
{
  FILE *stream = fopen (filename, "r");
  char *out;
  int save_errno;

  if (!stream)
    return NULL;

  out = fread_file (stream, length);

  save_errno = errno;

  if (fclose (stream) != 0)
    {
      if (out)
        {
          save_errno = errno;
          free (out);
        }
      errno = save_errno;
      return NULL;
    }

  return out;
}

//
// Iris uses passwd credentials for web access
//

uint8_t PL_check_webauth_pass(char *got_pass, char *got_user)
{
 
   //struct spwd *pw;
   struct passwd *pw;
   char *admin_pw = NULL, *salt, *encrypted, *username;
   uint8_t i,j;
   
 
   if(got_pass == NULL)
    return 0;
   
   if(got_user == NULL)
    username = strdup(WEBSRV_ADMIN_LOGIN);
   else
    username = strdup(got_user);

   //pw = getspent();
   pw = getpwent();
 
   while(pw != NULL)
    {
     //if(!strcmp(pw->sp_namp,username))
     if(!strcmp(pw->pw_name,username))
     //admin_pw = strdup(pw->sp_pwdp);
      admin_pw = strdup(pw->pw_passwd);
     //pw = gespent();
     pw = getpwent();
    }
 
   if(admin_pw == NULL)
    {
     PL_debug("PL_check_webauth_pass: no such user: %s", username);
     free(username);
     endpwent();
     return 0;
    }

   salt = strdup(admin_pw);
 
   i = j = 0;
 
   while(j<3)
    {
     if(salt[i] == '$')
      j++;
     i++;
     if(i >= strlen(admin_pw))
      {
        PL_debug("PL_check_webauth_pass: password check failed for %s", username);
        free(username);
        free(admin_pw);
        endpwent();
        return 0;
      }
    }
 
   salt[i] = 0x0;
 
   encrypted = crypt(got_pass,salt);
 
   //endspent();
   endpwent();

   free(salt);
   
   if(!strcmp(encrypted,admin_pw))
    {
     PL_debug("PL_check_webauth_pass: auth succeeded for %s", username);
     free(admin_pw);
     free(username);
     return 1;
    }
   else
    {
     PL_debug("PL_check_webauth_pass: password check failed for %s", username);
     free(admin_pw);
     free(username);
     return 0;
    }
}


int PL_service_web_connection(void *cls, struct MHD_Connection *connection,
        const char *url,
        const char *method, const char *version,
        const char *upload_data,
        size_t *upload_data_size, void **con_cls)
{
 
        int ret;
        static int aptr;
        struct MHD_Response *response;
        char *url_copy, *next_token;
        char *response_buffer;
        char *web_user, *web_pass;
        char *api_key;

        static char *response_redirect = "<html><head><meta http-equiv=\"refresh\" content=\"0;URL=\'/admin/\'\" /></head><body></body></html>";
        static char *response_auth_failed = "<html><head><title>Authorization failed</title></head><body>Authorization failed.</body></html>";
        static char *response_api_key_err = "<html><head><title>Authorization failed</title></head><body>Invalid API key.</body></html>";
        static char *code_404_response = "<html><head><title>not found</title></head><body>404 not found</body>";

        // TODO: understand what the fuck does this do:
         if (&aptr != *con_cls)
          {
           /* do never respond on first call */
           *con_cls = &aptr;
           return MHD_YES;
          }

        *con_cls = NULL;
 
        PL_debug("PL_service_web_connection: %s %s", method, url); // code_debug
 
        if ( (strcmp(method, "GET")) && (strcmp(method, "POST"))){
                return MHD_NO;
        }
 
        //if (strlen(url) == 1){
        //        return MHD_NO;
        //}
 
        url_copy = strdup(url);

        next_token = strtok(url_copy,"/");  // get first segment of URL

        if(next_token != NULL)
         {
          if(!strcmp(next_token,WEBSRV_API_URL))
           {

            api_key = MHD_lookup_connection_value(connection,MHD_GET_ARGUMENT_KIND,"IRISAPIKEY");

            if(api_key != NULL)
             {
               response = PL_service_REST_request(url, connection, method); 
             }
            else
             {
               response_buffer = response_api_key_err;
               response = MHD_create_response_from_buffer(strlen(response_buffer),(void*) response_buffer, MHD_RESPMEM_PERSISTENT);
             }

             ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
             MHD_destroy_response(response);

           }
          else if(!strcmp(next_token,WEBSRV_ADMIN_URL))
           {

             // check for session cookie here
             // if no cookie:
             web_pass = NULL;
             web_user = MHD_basic_auth_get_username_password(connection, &web_pass);
             if(!PL_check_webauth_pass(web_pass, web_user))
              {
               response_buffer = response_auth_failed;
               response = MHD_create_response_from_buffer (strlen (response_buffer),
                                                       (void *) response_buffer,
                                                       MHD_RESPMEM_PERSISTENT);
               ret = MHD_queue_basic_auth_fail_response (connection,"Iris Admin Panel",response);
               MHD_destroy_response(response);
               return ret;
              }
             else
              {
               // set session cookie
              }
            
              // if we got valid cookie from client:

              ret = PL_service_admin_panel_req(url, connection, method);

           }
          else
           {
             response_buffer = code_404_response;  // we have received "GET /" request - redirect browser to admin panel login

             response = MHD_create_response_from_buffer(
                strlen(response_buffer),
                (void*)response_buffer, MHD_RESPMEM_PERSISTENT);

             ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
             MHD_destroy_response(response);
           }

         }
        else
         {
           response_buffer = response_redirect;  // we have received "GET /" request - redirect browser to admin panel login

           response = MHD_create_response_from_buffer(
                strlen(response_buffer),
                (void*)response_buffer, MHD_RESPMEM_PERSISTENT);
 
           ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
           MHD_destroy_response(response);
         }
    
        free(url_copy);
        return ret;
 
}


void PL_websrv_thread(void)
 {
 
  char *cert_buffer, *key_buffer;
  uint16_t cert_len, key_len;
 
  if(G_config.websrv_mode == WEBSRV_SECURE)
   PL_debug("PL_websrv_thread: web server starting (secure)");
  else
   PL_debug("PL_websrv_thread: web server starting (plain)");

  struct MHD_Daemon *daemon;
   
  if(G_config.websrv_mode == WEBSRV_SECURE)
   {
    cert_buffer = read_file(WEBSRV_CERT_FILE,&cert_len);
    key_buffer = read_file(WEBSRV_KEY_FILE, &key_len);

    if((cert_buffer == NULL) || (key_buffer == NULL))
     {
      PL_debug("PL_websrv_thread: cannot load web server key/certificate - web server disabled");
      return;
     }
   }

   if(G_config.websrv_mode == WEBSRV_SECURE)
    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY|MHD_USE_SSL, WEBSRV_PORT, NULL, NULL,
               &PL_service_web_connection, NULL,
                          MHD_OPTION_HTTPS_PRIORITIES,"PERFORMANCE",
                          MHD_OPTION_HTTPS_MEM_KEY, key_buffer,
                          MHD_OPTION_HTTPS_MEM_CERT, cert_buffer, MHD_OPTION_END);
   else if(G_config.websrv_mode == WEBSRV_HTTP)
    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, WEBSRV_PORT, NULL, NULL,
               &PL_service_web_connection, NULL,
                          MHD_OPTION_END);

        if (daemon == NULL) 
         PL_debug("PL_websrv_thread: cannot start web server (%d). WWW access will be disabled.", errno);


 }

