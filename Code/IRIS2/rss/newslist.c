#include "rss.h"
#include "global.h"
#include "newslist.h"
#include "WEH001602-lib.h"

// stolen from stack overflow and converted to c:
char *remove_tags(char *potential_html)
 {
        char *returnStr = malloc(strlen(potential_html)+1);
        uint8_t insideTag = 0,c, add_space=0;
        uint16_t i,j=0,html_len;

        html_len = strlen(potential_html);

        for (i = 0; i < html_len; i++)
        {
            c = potential_html[i];
            if((c == '\r') || (c == '\n'))
             {
              c = ' ';
              //add_space = 1;
             }
            if (c == '<')
                insideTag = 1;

            if( (!insideTag) && (c != '"') && (c != '\'') )
              {
                returnStr[j++] = c;
                if(add_space)
                 {
                  returnStr[j++] = ' ';
                  add_space = 0;
                 }
              }

            if (c == '>')
                insideTag = 0;
        }

       returnStr[j++] = 0x0;

       return returnStr;
  }


rss_item_t *new_rss_item(unsigned char *title, unsigned char *body, uint8_t formatting_type)
 {

  rss_item_t *new_item;
  char *tag_stripped_body;

  uint16_t title_len, body_len, ctr, rss_channel_descr_len;

  new_item = malloc(sizeof(rss_item_t));

  tag_stripped_body = remove_tags(body);

  rss_channel_descr_len = strlen(G_rss_feeds[G_rss_feed_index]->name);  // ugly hack on global variables

  new_item->title = (char *)malloc(strlen(title)+20+rss_channel_descr_len); /* additional characters for trailing 0x0 and formatting */
  new_item->body = (char *)malloc(strlen(tag_stripped_body)+20+rss_channel_descr_len);
  new_item->next = 0;

  if(formatting_type == PREPARE_FOR_DISPLAY)
   {
    sprintf(new_item->title," \xce\xfe\xfe: [%s] %s ", G_rss_feeds[G_rss_feed_index]->name, title);
    sprintf(new_item->body," \xce\xfe\xfe: [%s] %s ", G_rss_feeds[G_rss_feed_index]->name, tag_stripped_body);
   }
  else if(formatting_type == PREPARE_FOR_TTS)
   {
    sprintf(new_item->title,"%s", title);
    sprintf(new_item->body,"%s", tag_stripped_body);
   }

  //PL_debug("new_rss_item: about to free tag_stripped_body [0x%x]", tag_stripped_body);
  free(tag_stripped_body); // malloc'ed in remove_tags() above

  return new_item;

 }


dump_rss_items(rss_item_t *rss_list_start)
 {

  rss_item_t *current_item;

  current_item = rss_list_start;

  while(current_item)
   {
    printf("[%s] \n%s \n ----- \n",current_item->title, current_item->body);
    current_item = current_item->next;
   }

 }

void destroy_rss_news_list(rss_item_t *rss_list_start)
 {
  
   uint8_t counter = 0;
   rss_item_t *current_item, *next_item;

   current_item = rss_list_start;

   while(current_item)
    {
     next_item = current_item->next;
     free(current_item->title);
     free(current_item->body);
     free(current_item);
     current_item = next_item;
     counter++;
    }

   PL_debug("destroy_rss_news_list: freed %d items",counter);

   G_rss_item_count = 0;

  }

void WEH001602_play_rss_items(rss_item_t *rss_list_start)
 {

  rss_item_t *current_item;
  uint8_t end = 0, row, prev_rss_index;

  if(G_display_mode_upper_row == DISPLAY_MODE_RSS) 
   row = TOP_ROW;
  else if(G_display_mode_lower_row == DISPLAY_MODE_RSS)
   row = BOTTOM_ROW;
  else
   return;
 
  current_item = rss_list_start;
  prev_rss_index = G_rss_feed_index;

    while(current_item)
     {

      if( (G_display_mode_upper_row != DISPLAY_MODE_RSS) &&
          (G_display_mode_lower_row != DISPLAY_MODE_RSS) )
        {
         //my_spi_WEH001602_clear();         
         return;
        }
      else
       my_spi_WEH001602_scroll_rss_once(current_item->body, row);

      if(G_rss_feed_index != prev_rss_index) // rss feed index changed - go to new rss feed
       return;

      current_item = current_item->next;
     }

 }

