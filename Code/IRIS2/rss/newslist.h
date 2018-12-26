
#include <stdint.h>
#include <string.h>


#define NEWS_LIST_LEN 15


uint8_t G_rss_item_count;

typedef struct _rss_item_t{
 unsigned char *title;
 unsigned char *body;
 struct rss_item_t *next;
 } rss_item_t;

rss_item_t *G_rss_news_list, *G_last_rss_item;


