#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdint.h>

#include "curlget.h"
#include "newslist.h"
#include "rss.h"

// stolen from stack overflow and converted to c:
char *remove_tags(char *potential_html)
 {
        char *returnStr = malloc(strlen(potential_html));  
        uint8_t insideTag = 0,c, add_space=0;
        uint16_t i,j=0,html_len;

        html_len = strlen(potential_html);

        for (i = 0; i < html_len; i++)
        {
            c = potential_html[i];
            if((c == '\r') || (c == '\n'))
             {
              c = '.';
              add_space = 1;
             }
            if (c == '<')    
                insideTag = 1;

            if (!insideTag)
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

void dump_rss_items(rss_item_t *rss_list_start)
 {

  rss_item_t *current_item;

  current_item = rss_list_start;

    while(current_item)
     {
      printf("title:\n%s\n",current_item->title);
      printf("body:\n%s\n",current_item->body);
      printf("===============\n");
      current_item = current_item->next;
     }

 }

void tts_play_rss_items(rss_item_t *rss_list_start)
 {

  rss_item_t *current_item;
  uint8_t end = 0;
  char *tts_cmd;
  char aplay_cmd[1024] = "aplay test.wav";
 
  current_item = rss_list_start;

    while(current_item)
     {
      tts_cmd = malloc(strlen(current_item->body) + 100);
      sprintf(tts_cmd,"pico2wave -w test.wav \"%s\"",current_item->body);
      system(tts_cmd);
      system(aplay_cmd);
      current_item = current_item->next;
     }

 }

rss_item_t *new_rss_item(unsigned char *title, unsigned char *body)
 {

  rss_item_t *new_item;
  unsigned char *tag_stripped_body;
  uint16_t title_len, body_len, ctr;


  tag_stripped_body = remove_tags(body);

  new_item = malloc(sizeof(rss_item_t));
  new_item->title = (char *)malloc(strlen(title)+9); /* additional characters for trailing 0x0 and formatting */
  new_item->body = (char *)malloc(strlen(tag_stripped_body)+9);
  new_item->next = 0;

  printf("new_rss_item: body after stripping tags: %s\n",tag_stripped_body);

  sprintf(new_item->title,"%s", title);
  sprintf(new_item->body," %s", tag_stripped_body);

  free(tag_stripped_body);  // malloc'ed in remove_tags 

  return new_item;

 }


int
main(int argc, char **argv)
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    struct curl_received_data my_data;
    uint32_t curl_data_size;

    LIBXML_TEST_VERSION

    G_rss_item_count = 0;

    /*parse the file and get the DOM */

     curl_data_size = curl_get_url_into_memory(argv[1],&my_data);

     doc = xmlReadMemory(my_data.data, curl_data_size, "rss.xml", NULL, 0);

     if (doc == NULL) {
        printf("error: could not parse curl buffer\n");
     }

     /*Get the root element node */
     root_element = xmlDocGetRootElement(doc);

     parse_xml_rss(root_element, PREPARE_FOR_DISPLAY);

     dump_rss_items(G_rss_news_list);
     //tts_play_rss_items(G_rss_news_list);

     /*free the document */
     xmlFreeDoc(doc);

    xmlCleanupParser();

    return 0;
}


