#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdint.h>

#include "newslist.h"

int
main(int argc, char **argv)
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    if (argc != 2)
        return(1);

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    if(!my_spi_WEH001602_init())
     {
      printf("libbcm2835 initialization error.\n");
      exit(0);
     }

    G_rss_item_count = 0;

    /*parse the file and get the DOM */
    doc = xmlReadFile(argv[1], NULL, 0);

    if (doc == NULL) {
        printf("error: could not parse file %s\n", argv[1]);
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    parse_xml_rss(root_element);

    dump_rss_items(G_rss_news_list);

    WEH001602_play_rss_items(G_rss_news_list);

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

    return 0;
}


