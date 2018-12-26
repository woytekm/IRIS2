#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdint.h>

#include "newslist.h"


void parse_xml_rss(xmlNode * a_node, uint8_t formatting_type)
{

    xmlNode *cur_node = NULL;
    xmlChar *xml_node_data;
    rss_item_t *rss_item;
    uint8_t in_item_list = 1;
    uint16_t title_len, content_len;
    char *last_news_title = NULL; 

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if ((cur_node->type == XML_ELEMENT_NODE) && (G_rss_item_count < NEWS_LIST_LEN ))  
         {


           if( (!strcmp(cur_node->name,"title")) && (!strcmp(cur_node->parent->name,"item")) )
             {
              
              xml_node_data = xmlNodeGetContent(cur_node);
              title_len = strlen(xml_node_data);
 
              if(last_news_title != NULL)
               free(last_news_title);

              last_news_title = malloc(title_len+1);
              strcpy(last_news_title, xml_node_data);
              xmlFree(xml_node_data);

             }

           if((!strcmp(cur_node->name,"description")) && (!strcmp(cur_node->parent->name,"item")))
            {

             xml_node_data = xmlNodeGetContent(cur_node);
             content_len = strlen(xml_node_data);
              
             //PL_debug("parse_xml_rss: new RSS item: %s, %s",last_news_title, xml_node_data); 

             rss_item = new_rss_item(last_news_title, xml_node_data, formatting_type);

             if(G_rss_item_count == 0)
               {
                G_rss_news_list = rss_item;
                G_last_rss_item = rss_item;
               }
             else
               {
                G_last_rss_item->next = rss_item;
                G_last_rss_item = rss_item;
               }
              
             xmlFree(xml_node_data);
             free(last_news_title);
             last_news_title = NULL;

             G_rss_item_count++;

           }

        }

        parse_xml_rss(cur_node->children, formatting_type);
       
    }
}



 
