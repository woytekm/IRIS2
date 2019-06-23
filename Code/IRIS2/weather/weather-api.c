#include <bcm2835.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <jansson.h>
#include <string.h>
#include <curl/curl.h>
#include <math.h>

#include "global.h"

#define API_URL "https://community-open-weather-map.p.rapidapi.com/forecast?q=%s&units=metric"
#define URL_SIZE 256

#define BUFFER_SIZE  (256 * 1024)  

struct write_result
{
    char *data;
    int pos;
};



static size_t write_response(void *ptr, size_t size, size_t nmemb, void *stream)
{
    struct write_result *result = (struct write_result *)stream;

    if(result->pos + size * nmemb >= BUFFER_SIZE - 1)
    {
        fprintf(stderr, "error: too small buffer\n");
        return 0;
    }

    memcpy(result->data + result->pos, ptr, size * nmemb);
    result->pos += size * nmemb;

    return size * nmemb;
}

static char *weather_api_request(const char *url)
{
    CURL *curl = NULL;
    CURLcode status;
    struct curl_slist *headers = NULL;
    char *data = NULL;
    long code;
    char rapidapi_key_header[255];

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(!curl)
        goto error;

    data = malloc(BUFFER_SIZE);
    if(!data)
        goto error;


    struct write_result write_result = {
        .data = data,
        .pos = 0
    };

    curl_easy_setopt(curl, CURLOPT_URL, url);

    sprintf(rapidapi_key_header,"X-RapidAPI-Key: %s",G_config.rapidapi_weather_key);
    /* GitHub commits API v3 requires a User-Agent header */
    headers = curl_slist_append(headers, "User-Agent: IRIS2");
    headers = curl_slist_append(headers, "X-RapidAPI-Host: community-open-weather-map.p.rapidapi.com");
    headers = curl_slist_append(headers, rapidapi_key_header);
    
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &write_result);

    status = curl_easy_perform(curl);
    if(status != 0)
    {
        PL_debug("PL_weather_api_thread: error: unable to request data from %s:\n", url);
        PL_debug("PL_weather_api_thread: curl error: %s\n", curl_easy_strerror(status));
        goto error;
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    char *ct;
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &ct);
    if(code != 200)
     {
        PL_debug("PL_weather_api_thread: error: server responded with code %ld (url: %s)", code, ct);
        goto error;
     }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    curl_global_cleanup();

    /* zero-terminate the result */
    data[write_result.pos] = '\0';
   
    return data;

error:
    if(data)
        free(data);
    if(curl)
        curl_easy_cleanup(curl);
    if(headers)
        curl_slist_free_all(headers);
    curl_global_cleanup();
    return NULL;
}


weather_t PL_parse_weather_description(const char *description)
 {

  if(strstr(description,"clear sky") != NULL)
   return CLEAR_SKY;
  else if(strstr(description,"few clouds") != NULL)
   return FEW_CLOUDS;
  else if(strstr(description,"scattered clouds") != NULL)
   return SCATTERED_CLOUDS;
  else if(strstr(description,"broken clouds") != NULL)
   return BROKEN_CLOUDS;
  else if(strstr(description,"overcast clouds") != NULL)
   return OVERCAST_CLOUDS;
  else if(strstr(description,"light rain") != NULL)
   return LIGHT_RAIN;
  else if(strstr(description,"moderate rain") != NULL)
   return MODERATE_RAIN;
  else if(strstr(description,"heavy intensity rain") != NULL)
   return HEAVY_RAIN;
  else if(strstr(description,"thunderstorm") != NULL)
   return STORM;
  else if(strstr(description,"fog") != NULL)
   return FOG;
  else if(strstr(description,"snow") != NULL)
   return SNOW;
  else return UNKNOWN;

 }


void PL_weather_api_thread(void)
 {

  uint8_t failed = 0;;
  char url[URL_SIZE];
  char *api_return_data, *key;
  double api_3h_temp, api_3h_windspeed;
  const char *api_3h_weather_desc;

  json_t *root, *forecast_list, *forecast_3h, *forecast_main_3h, *forecast_weather_3h, *forecast_clouds_3h, *forecast_wind_3h, *weather_array;
  json_t *temperature, *wind_speed, *weather_description;

  json_error_t error;

  G_last_weather_API_call_status = 0;

  if((strlen(G_config.rapidapi_weather_key) < 10) || (strlen(G_config.weather_location) < 5))
   {
     PL_debug("PL_weather_api_thread: no API key or no location (or both) in nvram. Disabling weather feature.");
     G_config.get_weather = 0;
     G_config.matrix_weather = 0;
     G_config.matrix_temperature = 0;
   }
  else if(G_config.get_weather == 1)
   {
     PL_debug("PL_weather_api_thread: enabling weather feature for %s.",G_config.weather_location);
   }

  while(1)
   {

    if(G_config.get_weather == 0)
     {
      sleep(600);
      continue;
     }

    if(failed) 
     {
       PL_debug("PL_weather_api_thread: failed to get proper Open WeatherMap response via Rapid API. Sleeping 10 mins.");
       G_last_weather_API_call_status = 0;
       G_weather = UNKNOWN;
       G_temperature = 0;
       G_windspeed = 0;
       sleep(600);
     }

    if((G_config.get_weather) && (strlen(G_config.weather_location) > 5))
    {
     snprintf(url, URL_SIZE, API_URL, G_config.weather_location);  
     api_return_data = weather_api_request(url);

     if(!api_return_data)
      {
       PL_debug("PL_weather_api_thread: API error. Couldn't get weather information.");
       failed = 1;
       continue;
      }
     else
      failed = 0;
      

    root = json_loads(api_return_data, 0, &error);
 
    if(root == NULL)
    {
      PL_debug("PL_weather_api_thread: couldn't parse json response: error: %s",error.text);   
      failed = 1;
      continue;
    }

    free(api_return_data); 

    if(!json_is_object(root))
    {
        PL_debug("PL_weather_api_thread: error: json root is not a json object\n");
        failed = 1;
        json_decref(root);
        continue;
    }

      
    forecast_list = json_object_get(root, "list");
    
    if(!json_is_array(forecast_list))
     {
      PL_debug("PL_weather_api_thread: error: forecast_list is not a json array object\n");
      failed = 1;
      continue;
     }

    forecast_3h = json_array_get(forecast_list,1);

    forecast_main_3h = json_object_get(forecast_3h,"main");
    forecast_weather_3h = json_array_get(json_object_get(forecast_3h,"weather"),0);
    forecast_clouds_3h = json_object_get(forecast_3h,"clouds");
    forecast_wind_3h = json_object_get(forecast_3h,"wind");

    temperature = json_object_get(forecast_main_3h,"temp");
    wind_speed = json_object_get(forecast_wind_3h,"speed");
    weather_description = json_object_get(forecast_weather_3h,"description");

    if(!json_is_real(temperature) || !json_is_real(wind_speed) || !json_is_string(weather_description))
     {
      PL_debug("PL_weather_api_thread: error: failed to get JSON keys for 3h forecast.\n");
      failed = 1;
      continue;
     }

    api_3h_temp = json_real_value(temperature);
    api_3h_windspeed = json_real_value(wind_speed);
    api_3h_weather_desc = json_string_value(weather_description);
   
    G_last_weather_API_call_status = 1;

    G_temperature = api_3h_temp;
    G_windspeed = round(api_3h_windspeed);
    G_weather = PL_parse_weather_description(api_3h_weather_desc);

    json_decref(root);
 
   }

   sleep(1200);

  }

 }


