#include "global.h"
#include "curlget.h"
 
 
static size_t
curl_wr_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct curl_received_data *mem = (struct curl_received_data *)userp;
 
  mem->data = realloc(mem->data, mem->size + realsize + 1);

  if(mem->data == NULL) {
    return 0;
  }
 
  memcpy(&(mem->data[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->data[mem->size] = 0;
 
  return realsize;
}
 
size_t curl_get_url_into_memory(char *url, struct curl_received_data *chunk)
{
  CURL *curl_handle;
  CURLcode res;
 
  chunk->data = malloc(1);
  chunk->size = 0;
 
  curl_global_init(CURL_GLOBAL_ALL);
  curl_handle = curl_easy_init();
 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_wr_callback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)chunk);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  curl_easy_setopt(curl_handle, CURLOPT_DNS_CACHE_TIMEOUT, 0);
  curl_easy_setopt(curl_handle, CURLOPT_DNS_USE_GLOBAL_CACHE, 0);
  curl_easy_setopt(curl_handle, CURLOPT_DNS_SERVERS,"8.8.8.8,8.8.4.4");
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
 
  res = curl_easy_perform(curl_handle);
 
  if(res != CURLE_OK) {
   PL_debug("curl_get_url_into_memory: CURL error %d",res);
   chunk->size = 0;
  }
 
  curl_easy_cleanup(curl_handle);
  curl_global_cleanup();
 
  return chunk->size;
}
 
 
