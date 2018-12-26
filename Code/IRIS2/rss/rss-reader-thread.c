#include "global.h"
#include "newslist.h"
#include "curlget.h"
#include "rss.h"


void PL_rss_reader_thread()
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    xmlErrorPtr pErr;

    struct curl_received_data my_data;
    uint32_t curl_data_size;

    LIBXML_TEST_VERSION

    G_rss_item_count = 0;

    sleep(10);

    PL_debug("PL_rss_reader_thread: starting");

    while(1)
    {

      while( (G_display_mode_upper_row != DISPLAY_MODE_RSS) &&
        (G_display_mode_lower_row != DISPLAY_MODE_RSS) )
      usleep(50*1000);
 
      xmlInitParser();

      PL_debug("PL_rss_reader_thread: DISPLAY_MODE_RSS - scrolling current RSS channel (%s)",G_rss_feeds[G_rss_feed_index]->url);

      curl_data_size = curl_get_url_into_memory(G_rss_feeds[G_rss_feed_index]->url,&my_data);
      PL_debug("PL_rss_reader_thread: curl got %d bytes from network",curl_data_size);

      if(curl_data_size == 0)
       {
        PL_debug("PL_rss_reader_thread: error: curl returned 0 bytes - will retry in 3 seconds");
        free(my_data.data);
        xmlCleanupParser();
        sleep(3);
        continue;
       }

      doc = xmlReadMemory(my_data.data, curl_data_size, "rss.xml", NULL, 0);

      if (doc == NULL) {
        pErr = xmlGetLastError();
        PL_debug("PL_rss_reader_thread: error: could not parse curl buffer (libXML error: %s)",pErr->message);
        free(my_data.data);
        xmlCleanupParser();
        sleep(3);
        continue;
      }

      /*Get the root element node */
      root_element = xmlDocGetRootElement(doc);

      parse_xml_rss(root_element,PREPARE_FOR_DISPLAY);

      //dump_rss_items(G_rss_news_list);
      WEH001602_play_rss_items(G_rss_news_list);
      destroy_rss_news_list(G_rss_news_list);

      /*free the document */
      xmlFreeDoc(doc);
      free(my_data.data);

      PL_debug("PL_rss_reader_thread: parser cleanup");
      xmlCleanupParser();

      sleep(2);  // prevent hammering feed server which can happen if something goes wrong with xml parser

    }

}


