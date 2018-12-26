#include "global.h"
#include "system.h"
#include "tts.h"
#include "WEH001602-lib.h"

int PL_command_socket_setup(void)
{
 int sock;
 socklen_t len;
 struct sockaddr_un local;

   if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
      PL_debug("PL_command_socket_setup: cannot create command socket %s (%d)",
              IRIS_COMMAND_SOCKET,errno);
      return NULL;
   }

   local.sun_family = AF_UNIX;
   strcpy(local.sun_path, IRIS_COMMAND_SOCKET);
   unlink(local.sun_path);
   len = SUN_LEN(&local);
   if (bind(sock, (struct sockaddr *)&local, len) == -1) {
        PL_debug("PL_command_socket_setup: cannot bind to command socket %s (%d)",
                IRIS_COMMAND_SOCKET,errno);
        return NULL;
    }

   if (listen(sock, 5) == -1) {
      PL_debug("PL_command_socket_setup: listen failed on command socket %s (%d)!",
              IRIS_COMMAND_SOCKET,errno);
      return NULL;
      }

  return sock;
}

// it's handy to have these here

#define  CMD_RANGE 11

#define CMD_STREAM_START 0
#define CMD_STREAM_STOP 1
#define CMD_STREAM_NEXT 2
#define CMD_STREAM_PREV 3
#define CMD_VOL_UP 4
#define CMD_VOL_DOWN 5
#define CMD_STREAM_RELOAD 6
#define CMD_RSS_RELOAD 7
#define CMD_TELL_TIME 8
#define CMD_READ_RSS 9
#define CMD_STREAM_SHOW 10


int8_t PL_parse_command(char *command_str)
 {

   uint8_t cmd_code, i;
   char *str_check;
   char *first_token;
   const char *separator = " ";
   const char *accepted_commands[] = {"stream_start","stream_stop","stream_next","stream_prev","vol_up","vol_down","stream_reload","rss_reload",
                                      "tell_time","read_rss","stream_show"};

   first_token = strtok(command_str,separator);

   for(i=0; i<CMD_RANGE; i++)
    {

      if( (str_check = strstr(first_token,accepted_commands[i]))  != NULL)
       if(strlen(str_check) == strlen(accepted_commands[i]))
        return i;
    }
   
   return -1;

 }


void PL_command_thread()
 {
    
   // writes less than 4kb are atomic, we have 512 bytes, so we should always receive entire command in one recv cycle

   int n, s2, t, socket, i;
   int8_t cmd_code;
   char command[MAX_CMD_LEN];
   char *cmd_copy;
   char command_response[MAX_RESP_LEN];
   char volume_msg[17];
   uint8_t TTS_thread_msg[2];
   uint8_t RSS_index;
   struct sockaddr_un remote;
   char *second_param;
   char *third_param;
   char *fourth_param;
   
   PL_debug("PL_command_thread: starting");

   socket = PL_command_socket_setup();

   if(socket == NULL)
    {
     PL_debug("PL_command_thread: cannot create command socket - external commands disabled");
     return;
    }

   while (1)
     {

       if ((s2 = accept(socket, (struct sockaddr *)&remote, &t)) != -1 ) 
        {
          PL_debug("PL_command_thread: accepted a connection on command channel.");
          bzero(command,MAX_CMD_LEN);
          n = recv(s2, command, MAX_CMD_LEN, 0);

          if (n <= 0)
           { 
             if (n < 0)
               PL_debug("PL_command_thread: receive error %d", errno); 
             continue; 
           }
          else
           {
             PL_debug("PL_command_thread: received external command: %s",command);

             cmd_copy = strdup(command);
             cmd_code = PL_parse_command(cmd_copy); 
             free(cmd_copy);
  
             if(cmd_code == -1)
              {

                snprintf(command_response,MAX_RESP_LEN,"NOK unknown command");
                send(s2,(void *)command_response, strlen(command_response),0);

                PL_debug("PL_command_thread: unknown command - ignoring");
                continue;
              }
             else
              {
              PL_debug("PL_command_thread: servicing command %d", cmd_code);
              switch(cmd_code)
               {

                 case CMD_STREAM_STOP:
                  G_player_mode = PLAYER_STOP;
                  if(G_scroll_meta_active)
                   G_kill_meta_scroll = 1;

                  snprintf(command_response,MAX_RESP_LEN,"OK");
                  send(s2,(void *)command_response, strlen(command_response),0);

                  break;

                 case CMD_STREAM_START:
                  G_player_mode = PLAYER_STREAM;

                  snprintf(command_response,MAX_RESP_LEN,"OK");
                  send(s2,(void *)command_response, strlen(command_response),0);

                  break;

                 case CMD_STREAM_NEXT:
                  if(G_stream_index == G_stream_count-1)
                   G_stream_index = 0;
                  else
                   G_stream_index++;
                  if(G_scroll_meta_active)
                   {
                    G_kill_meta_scroll = 1;
                    strcpy(G_current_stream_META,"\0");
                   }
                  G_search_direction = SEARCH_FORWARD;

                  snprintf(command_response,MAX_RESP_LEN,"OK streaming %s %s",G_streams[G_stream_index]->name, G_streams[G_stream_index]->url);
                  send(s2,(void *)command_response, strlen(command_response),0);

                  break;

                 case CMD_STREAM_PREV:
                  if(G_stream_index == 0)
                   G_stream_index = G_stream_count-1;
                  else
                   G_stream_index--;
                  if(G_scroll_meta_active)
                   {
                    G_kill_meta_scroll = 1;
                    strcpy(G_current_stream_META,"\0");
                   }
                  G_search_direction = SEARCH_BACK;

                  snprintf(command_response,MAX_RESP_LEN,"OK streaming %s %s",G_streams[G_stream_index]->name, G_streams[G_stream_index]->url);
                  send(s2,(void *)command_response, strlen(command_response),0);

                  break;

                 case CMD_VOL_UP:
                  if(G_config.volume_level < 1)
                   G_config.volume_level += 0.02;

                  if(G_config.volume_level == 0.98)
                   sprintf(volume_msg,"[volume max]",G_config.volume_level);
                  else
                   sprintf(volume_msg,"[volume %1.2f]",G_config.volume_level);

                  pthread_mutex_lock(&G_display_lock);
                  my_spi_WEH001602_out_text(TOP_ROW,"                ");
                  my_spi_WEH001602_out_text(TOP_ROW,volume_msg);
                  usleep(200000);
                  my_spi_WEH001602_out_text(TOP_ROW,"                ");
                  pthread_mutex_unlock(&G_display_lock);

                  snprintf(command_response,MAX_RESP_LEN,"OK volume now %1.2f",G_config.volume_level);
                  send(s2,(void *)command_response, strlen(command_response),0);

                  break;

                 case CMD_VOL_DOWN:
                  if(G_config.volume_level > 0.02)
                    G_config.volume_level -= 0.02;

                  if(G_config.volume_level == 0.02)
                   sprintf(volume_msg,"[volume min]",G_config.volume_level);
                  else
                   sprintf(volume_msg,"[volume %1.2f]",G_config.volume_level);

                  pthread_mutex_lock(&G_display_lock);
                  my_spi_WEH001602_out_text(TOP_ROW,"                ");
                  my_spi_WEH001602_out_text(TOP_ROW,volume_msg);
                  usleep(200000);
                  my_spi_WEH001602_out_text(TOP_ROW,"                ");
                  pthread_mutex_unlock(&G_display_lock);

                  snprintf(command_response,MAX_RESP_LEN,"OK volume now %1.2f",G_config.volume_level);
                  send(s2,(void *)command_response, strlen(command_response),0);

                  break;
               
                 case CMD_TELL_TIME:
                  TTS_thread_msg[0] = TTS_SAY_CURRENT_TIME;
                  TTS_thread_msg[1] = 0; // does not matter
                  write(G_TTS_thread_input[1],&TTS_thread_msg,2);

                  snprintf(command_response,MAX_RESP_LEN,"OK");
                  send(s2,(void *)command_response, strlen(command_response),0);

                  break;
                 
                case CMD_READ_RSS:

                  strtok(command," ");
                  second_param = strtok(NULL," ");

                  if(second_param != NULL)
                   {
                    RSS_index = atoi(second_param);
                    if(RSS_index <= G_rss_feed_count)
                     TTS_thread_msg[1] = RSS_index;
                    else
                     {
                      PL_debug("PL_command_thread: invalid RSS index given (%d)", RSS_index);
                      break;
                     }
                   } 
                  else
                   TTS_thread_msg[1] = G_rss_feed_index;

                  TTS_thread_msg[0] = TTS_READ_RSS_NOW;

                  write(G_TTS_thread_input[1],&TTS_thread_msg,2); 

                  snprintf(command_response,MAX_RESP_LEN,"OK preparing RSS at index %d",TTS_thread_msg[1]);
                  send(s2,(void *)command_response, strlen(command_response),0);

                  break;

               case CMD_STREAM_SHOW:

                 if(G_player_mode == PLAYER_STREAM)
                  snprintf(command_response,MAX_RESP_LEN,"OK streaming %s [%s] META: %s",G_streams[G_stream_index]->name, G_streams[G_stream_index]->url, G_current_stream_META);
                 else
                  snprintf(command_response,MAX_RESP_LEN,"OK not streaming. Stream index: %s %s",G_streams[G_stream_index]->name, G_streams[G_stream_index]->url);
                  send(s2,(void *)command_response, strlen(command_response),0);

                  break;

               default:

                  snprintf(command_response,MAX_RESP_LEN,"NOK not implemented");
                  send(s2,(void *)command_response, strlen(command_response),0);

                  PL_debug("PL_command_thread: %d - command not implemented (yet?)",cmd_code);

               }
             }
 
           }

        }
     }

 }
