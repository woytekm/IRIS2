#include "global.h"
#include "setup.h"
#include "config.h"
#include "keyboard.h"
#include "WEH001602-lib.h"


setup_menu_item_t *new_menu_item(setup_menu_item_t *prev_item, char *menu_title, void *var, uint8_t var_type, uint32_t var_min, uint32_t var_max)
 {

  setup_menu_item_t *new_item;

  new_item = malloc(sizeof(setup_menu_item_t));

  new_item->title = malloc(strlen(menu_title)+1);
  strcpy(new_item->title, menu_title);

  new_item->variable = var;
  new_item->var_type = var_type;
  new_item->var_min = var_min;
  new_item->var_max = var_max;
  new_item->prev = prev_item;
  new_item->next = NULL;

  return new_item;

 }

uint8_t PL_connect_bt_source(void)
 {

 }

uint8_t PL_reconnect_wifi(void)
 {
  system("/usr/sbin/iris-restart-wifi.sh");
  return 1;
 }

uint8_t PL_reconnect_bt_speaker(void)
 {
  int exit_val;
  exit_val =  system("/usr/sbin/iris-connect-bt-speaker.sh");
  return 1;
 }

uint8_t PL_discover_bt_speaker(void)
 {
  int exit_val;
  exit_val =  system("/usr/sbin/iris-discover-bt-speaker.sh");
  return 1;
 }

uint8_t PL_do_reboot(void)
 {
  system("/sbin/iris-reboot");
  return 1;
 }

uint8_t PL_do_factory_init(void)
 {
  system("/usr/sbin/nvram zeroize_all");
  system("/usr/sbin/nvram reinit");
  system("/sbin/iris-reboot");
  return 1;
 }


void init_setup_menu(void)
 {

   setup_menu_item_t *next_item;
   // at least two alarms should be allocated at startup, even if there were no alarm definitions in nvram

   G_setup_menu = new_menu_item(NULL,"[Set SYS time]",  NULL, VAR_TYPE_GET_CURR_TIME, 0, 0);
   next_item = G_setup_menu;  

   next_item->next = new_menu_item(next_item,"[Set ALM1 time]",  (void*)G_alarms, VAR_TYPE_GET_ALARM_TIME, 0, 0);
   next_item = next_item->next;

   next_item->next = new_menu_item(next_item,"[Set ALM1 days]", (void*)&G_alarms->days_of_week, VAR_TYPE_GET_WEEKDAYS, 0, 0);
   next_item = next_item->next;

   next_item->next = new_menu_item(next_item,"[Set ALM1 type]", (void*)&G_alarms->alarm_type, VAR_TYPE_GET_UINT8, 0, 2);
   next_item = next_item->next;

   next_item->next = new_menu_item(next_item,"[Set ALM1 active]", (void*)&G_alarms->alarm_state, VAR_TYPE_GET_BOOL, 0, 0);
   next_item = next_item->next;

   next_item->next = new_menu_item(next_item,"[Set ALM2 time]", (void*)G_alarms->next, VAR_TYPE_GET_ALARM_TIME, 0, 0);
   next_item = next_item->next;

   next_item->next = new_menu_item(next_item,"[Set ALM2 days]", (void*)&G_alarms->next->days_of_week, VAR_TYPE_GET_WEEKDAYS, 0, 0);
   next_item = next_item->next;

   next_item->next = new_menu_item(next_item,"[Set ALM2 type]", (void*)&G_alarms->next->alarm_type, VAR_TYPE_GET_UINT8, 0, 2);
   next_item = next_item->next;

   next_item->next = new_menu_item(next_item,"[Set ALM2 active]", (void*)&G_alarms->next->alarm_state, VAR_TYPE_GET_BOOL, 0, 0);
   next_item = next_item->next;

   next_item->next = new_menu_item(next_item,"[Sleep timer]", (void*)&G_config.sleep_timer_time, VAR_TYPE_GET_UINT32, 0, 720);
   next_item = next_item->next;

   next_item->next = new_menu_item(next_item,"[Snooze timer]", (void*)&G_config.snooze_timer_time, VAR_TYPE_GET_UINT32, 10, 600);
   next_item = next_item->next;

   next_item->next = new_menu_item(next_item,"[Tell time]", (void*)&G_config.tell_time_when_on, VAR_TYPE_GET_UINT8, 0, 2);
   next_item = next_item->next;

   if(!G_config.basic_menu)
    {
     next_item->next = new_menu_item(next_item,"[WiFi SSID]", (void*)&G_config.wireless_ssid, VAR_TYPE_GET_STRING, 0, 15);
     next_item = next_item->next;

     next_item->next = new_menu_item(next_item,"[WiFi password]", (void*)&G_config.wireless_password, VAR_TYPE_SECURE_GET_STRING, 0, 15);
     next_item = next_item->next;

     next_item->next = new_menu_item(next_item,"[WiFi driver]", (void*)&G_config.wireless_driver, VAR_TYPE_GET_STRING, 0, 15);
     next_item = next_item->next;

     next_item->next = new_menu_item(next_item,"[WiFi type]",  (void*)&G_config.wireless_type, VAR_TYPE_GET_UINT8, 0, 2);
     next_item = next_item->next;

     next_item->next = new_menu_item(next_item,"[IP address]", (void*)&G_config.ip_address, VAR_TYPE_GET_STRING, 0, 15);
     next_item = next_item->next;

     next_item->next = new_menu_item(next_item,"[IP netmask]", (void*)&G_config.ip_netmask, VAR_TYPE_GET_STRING, 0, 15);
     next_item = next_item->next;

     next_item->next = new_menu_item(next_item,"[IP gateway]", (void*)&G_config.ip_gateway, VAR_TYPE_GET_STRING, 0, 15);
     next_item = next_item->next;

     next_item->next = new_menu_item(next_item,"[Timezone]", (void*)&G_config.timezone, VAR_TYPE_GET_STRING, 0, 15);
     next_item = next_item->next;

     next_item->next = new_menu_item(next_item,"[Use NTP]", (void*)&G_config.use_ntp, VAR_TYPE_GET_BOOL, 0, 0);
     next_item = next_item->next;

     next_item->next = new_menu_item(next_item,"[NTP server]", (void*)&G_config.ntp_server, VAR_TYPE_GET_STRING, 0, 15);
     next_item = next_item->next;

     next_item->next = new_menu_item(next_item,"[BT speaker]", (void*)&G_config.bt_spk, VAR_TYPE_GET_BOOL, 0, 0);
     next_item = next_item->next;

     if(!G_config.bt_spk)
      {
        next_item->next = new_menu_item(next_item,"[BT sink]", (void*)&G_config.bt_sink, VAR_TYPE_GET_BOOL, 0, 0);
        next_item = next_item->next;
      }

     next_item->next = new_menu_item(next_item,"[SSH access]", (void*)&G_config.ssh_access, VAR_TYPE_GET_BOOL, 0, 0);
     next_item = next_item->next;

     next_item->next = new_menu_item(next_item,"[WWW access]", (void*)&G_config.www_access, VAR_TYPE_GET_BOOL, 0, 0);
     next_item = next_item->next;

     next_item->next = new_menu_item(next_item,"[Autoplay]", (void*)&G_config.autoplay, VAR_TYPE_GET_BOOL, 0, 0);
     next_item = next_item->next;

    } 

   next_item->next = new_menu_item(next_item,"[Save config?]", (void*)&PL_save_config, VAR_TYPE_EXEC, 0, 0);
   next_item = next_item->next;

   next_item->next = new_menu_item(next_item,"[Restart WiFi?]", (void*)&PL_reconnect_wifi, VAR_TYPE_EXEC, 0, 0);
   next_item = next_item->next;

   if(G_config.bt_spk)
    {
     next_item->next = new_menu_item(next_item,"[Conn BT spkr?]", (void*)&PL_reconnect_bt_speaker, VAR_TYPE_EXEC, 0, 0);
     next_item = next_item->next;

     next_item->next = new_menu_item(next_item,"[Find BT spkr?]", (void*)&PL_discover_bt_speaker, VAR_TYPE_EXEC, 0, 0);
     next_item = next_item->next;
    }

   if((!G_config.bt_spk) && (G_config.bt_sink))
    {
     next_item->next = new_menu_item(next_item,"[BT src]", (void*)&G_config.bt_source, VAR_TYPE_GET_STRING, 0, 0);
     next_item = next_item->next;

     next_item->next = new_menu_item(next_item,"[Conn BT src?]", (void*)&PL_connect_bt_source, VAR_TYPE_EXEC, 0, 0);
     next_item = next_item->next;
    }

   next_item->next = new_menu_item(next_item,"[Reboot?]", (void*)&PL_do_reboot, VAR_TYPE_EXEC, 0, 0);
   next_item = next_item->next;

   next_item->next = new_menu_item(next_item,"[Factory init?]", (void*)&PL_do_factory_init, VAR_TYPE_EXEC, 0, 0);
   next_item = next_item->next;

   next_item->next = G_setup_menu; // wrap linked list
   G_setup_menu->prev = next_item; // wrap linked list

 }

 
int8_t PL_get_setup_menu_item(setup_menu_item_t *item)
 {

  unsigned char vartext[16];
  unsigned char set_time_cmd[255];

  unsigned char *edit_string;
  uint8_t key_pressed = 0;
  uint8_t i;
  uint8_t cursor_pos = 0;
  uint8_t exec_result;
  uint8_t tmp_min, tmp_hour, saved_min, saved_hour;
  uint8_t tmp_day, tmp_month, saved_day, saved_month;
  int tmp_year, saved_year;
  alarm_data_t *alarm;

  my_spi_WEH001602_out_text(TOP_ROW,item->title);
  
  if(item->var_type == VAR_TYPE_EXEC)
   {
     uint8_t (*exec_routine)(void);

     exec_routine = item->variable;
	
     while((key_pressed != KEY_SETUP) && (key_pressed != KEY_TUNING_UP_LONG) && (key_pressed != KEY_TUNING_DOWN_LONG))
     {
      read(G_setup_key_input[0], &key_pressed, sizeof(uint8_t));
      if(key_pressed == KEY_VOL_UP)
       {
        sprintf(vartext,">%s","executing...");
        my_spi_WEH001602_out_text(BOTTOM_ROW,vartext);

 	exec_result = exec_routine();

	if(exec_result == 1)
         sprintf(vartext,">%s","executed ok");
        else  
         sprintf(vartext,">%s","exec error!");

        my_spi_WEH001602_out_text(BOTTOM_ROW,vartext);
       }
     }
   }

  else if(item->var_type == VAR_TYPE_GET_CURR_TIME)
   {

     tmp_min = saved_min = G_tm->tm_min;
     tmp_hour = saved_hour = G_tm->tm_hour;
     tmp_day = saved_day = G_tm->tm_mday;
     tmp_month = saved_month = G_tm->tm_mon+1;
     tmp_year = saved_year = G_tm->tm_year - 100;

     cursor_pos = 1;

     while((key_pressed != KEY_SETUP) && (key_pressed != KEY_TUNING_UP_LONG) && (key_pressed != KEY_TUNING_DOWN_LONG))
      {
       read(G_setup_key_input[0], &key_pressed, sizeof(uint8_t));

       if((key_pressed == KEY_VOL_UP)||(key_pressed == KEY_VOL_UP_LONG))
        {
         if(cursor_pos == 0)
          {
           if(tmp_hour == 23)
            tmp_hour = 0;
           else
            tmp_hour++;
           }
         else if(cursor_pos == 1)
           {
            if(tmp_min == 59)
             tmp_min = 0;
            else
             tmp_min++;
           }
         else if(cursor_pos == 2)
           {
            if(tmp_day == 31)
             tmp_day = 1;
            else
             tmp_day++;
           }
         else if(cursor_pos == 3)
           {
            if(tmp_month == 12)
             tmp_month = 1;
            else
             tmp_month++;
           }
         else if(cursor_pos == 4)
           {
            if(tmp_year == 99)
             tmp_year = 0;
            else
             tmp_year++;
           }
        }

      else if((key_pressed == KEY_VOL_DOWN)||(key_pressed == KEY_VOL_DOWN_LONG))
        {
         if(cursor_pos == 0)
          {
           if(tmp_hour == 0)
            tmp_hour = 23;
           else
            tmp_hour--;
           }
         else if(cursor_pos == 1)
           {
            if(tmp_min == 0)
             tmp_min = 59;
            else
             tmp_min--;
           }
         else if(cursor_pos == 2)
           {
            if(tmp_day == 1)
             tmp_day = 31;
            else
             tmp_day--;
           }
         else if(cursor_pos == 3)
           {
            if(tmp_month == 1)
             tmp_month = 12;
            else
             tmp_month--;
           }
         else if(cursor_pos == 4)
           {
            if(tmp_year == 00)
             tmp_year = 99;
            else
             tmp_year--;
           }
        }

      else if(key_pressed == KEY_TUNING_UP)
        {
         if(cursor_pos == 4)
          cursor_pos = 0;
         else
          cursor_pos++;
        }

      else if(key_pressed == KEY_TUNING_DOWN)
        {
         if(cursor_pos == 0)
          cursor_pos = 4;
         else
          cursor_pos--;
        }

       sprintf(vartext,">%02d:%02d %02d/%02d/%02d",tmp_hour,tmp_min,tmp_day,tmp_month,tmp_year);

       my_spi_WEH001602_out_cmd(0b00001100); // turn cursor blinking off
       my_spi_WEH001602_out_text(BOTTOM_ROW,vartext);

       my_spi_WEH001602_out_cmd(0b00001111); // turn cursor+blinking on

       switch(cursor_pos) 
        {
         case 0:
          my_spi_WEH001602_move_cursor_at_col(BOTTOM_ROW,2);
          break;
         case 1:
          my_spi_WEH001602_move_cursor_at_col(BOTTOM_ROW,5);
          break;
         case 2:
          my_spi_WEH001602_move_cursor_at_col(BOTTOM_ROW,8);
          break;
         case 3:
          my_spi_WEH001602_move_cursor_at_col(BOTTOM_ROW,11);
          break;
         case 4:
          my_spi_WEH001602_move_cursor_at_col(BOTTOM_ROW,14);
          break;
        }

     }

     my_spi_WEH001602_out_cmd(0b00001100); // turn cursor blinking off

     if((tmp_min != saved_min) || (tmp_hour != saved_hour) ||
        (tmp_day != saved_day) || (tmp_month != saved_month) || (tmp_year != saved_year))  // user edited this - set the clock accordingly
      {
        sprintf(set_time_cmd,"/usr/sbin/iris-set-hwclock.sh %02d %02d %02d %02d %04d",tmp_hour, tmp_min, tmp_day, tmp_month, tmp_year+2000);
        system(set_time_cmd);
      }

   }


  else if(item->var_type == VAR_TYPE_GET_ALARM_TIME)
   {
     alarm = (alarm_data_t *)(item->variable);
     tmp_min = alarm->minute;
     tmp_hour = alarm->hour;
     cursor_pos = 1;

     while((key_pressed != KEY_SETUP) && (key_pressed != KEY_TUNING_UP_LONG) && (key_pressed != KEY_TUNING_DOWN_LONG))
      {   
       read(G_setup_key_input[0], &key_pressed, sizeof(uint8_t));
      
       if((key_pressed == KEY_VOL_UP)||(key_pressed == KEY_VOL_UP_LONG))
        {
         if(cursor_pos == 0)
          {
           if(tmp_hour == 23)
            tmp_hour = 0;
           else 
            tmp_hour++;
           }
         else if(cursor_pos == 1)
           {
            if(tmp_min == 59)
             tmp_min = 0;
            else
             tmp_min++;
           }
        }

      else if((key_pressed == KEY_VOL_DOWN)||(key_pressed == KEY_VOL_DOWN_LONG))
        {
         if(cursor_pos == 0)
          {
           if(tmp_hour == 0)
            tmp_hour = 23;
           else
            tmp_hour--;
           }
         else if(cursor_pos == 1)
           {
            if(tmp_min == 0)
             tmp_min = 59;
            else
             tmp_min--;
           }
        }

      else if((key_pressed == KEY_TUNING_UP)||(key_pressed == KEY_TUNING_DOWN))
        {
         if(cursor_pos == 0)
          cursor_pos = 1;
         else
          cursor_pos = 0;
        }

       sprintf(vartext,">%02d:%02d",tmp_hour,tmp_min);

       my_spi_WEH001602_out_cmd(0b00001100); // turn cursor blinking off
       my_spi_WEH001602_out_text(BOTTOM_ROW,vartext); 

       my_spi_WEH001602_out_cmd(0b00001111); // turn cursor+blinking on

       if(cursor_pos == 0)
        my_spi_WEH001602_move_cursor_at_col(BOTTOM_ROW,2);
       else
        my_spi_WEH001602_move_cursor_at_col(BOTTOM_ROW,5);

     }

     my_spi_WEH001602_out_cmd(0b00001100); // turn cursor blinking off
    
     if((alarm->minute != tmp_min) || (alarm->hour != tmp_hour))
      {
       alarm->minute = tmp_min;
       alarm->hour = tmp_hour;
       alarm->triggered_today = 0;
      }

   }

  else if(item->var_type == VAR_TYPE_GET_UINT8)
   {

    while((key_pressed != KEY_SETUP) && (key_pressed != KEY_TUNING_UP_LONG) && (key_pressed != KEY_TUNING_DOWN_LONG))
     {
      read(G_setup_key_input[0], &key_pressed, sizeof(uint8_t));

      if( ((key_pressed == KEY_VOL_UP)||(key_pressed == KEY_VOL_UP_LONG)) && (*(uint8_t *)item->variable < item->var_max))
       (*(uint8_t *)item->variable)++;

      else if( ((key_pressed == KEY_VOL_UP)||(key_pressed == KEY_VOL_UP_LONG)) && (*(uint8_t *)item->variable == item->var_max))
       (*(uint8_t *)item->variable) = item->var_min;

      else if( ((key_pressed == KEY_VOL_DOWN)||(key_pressed == KEY_VOL_DOWN_LONG)) && (*(uint8_t *)item->variable > item->var_min))
       (*(uint8_t *)item->variable)--;

      else if( ((key_pressed == KEY_VOL_DOWN)||(key_pressed == KEY_VOL_DOWN_LONG)) && (*(uint8_t *)item->variable == item->var_min))
       (*(uint8_t *)item->variable) = item->var_max;

      sprintf(vartext,">%02d",*(uint8_t *)item->variable);
      my_spi_WEH001602_out_text(BOTTOM_ROW,vartext);
     }

   }

 else if(item->var_type == VAR_TYPE_GET_UINT32)
   {

    while((key_pressed != KEY_SETUP) && (key_pressed != KEY_TUNING_UP_LONG) && (key_pressed != KEY_TUNING_DOWN_LONG))
     {
      read(G_setup_key_input[0], &key_pressed, sizeof(uint8_t));

      if( ((key_pressed == KEY_VOL_UP)||(key_pressed == KEY_VOL_UP_LONG)) && (*(uint32_t *)item->variable < item->var_max))
       (*(uint32_t *)item->variable)++;

      else if( ((key_pressed == KEY_VOL_UP)||(key_pressed == KEY_VOL_UP_LONG)) && (*(uint32_t *)item->variable == item->var_max))
       (*(uint32_t *)item->variable) = item->var_min;

      else if( ((key_pressed == KEY_VOL_DOWN)||(key_pressed == KEY_VOL_DOWN_LONG)) && (*(uint32_t *)item->variable > item->var_min))
       (*(uint32_t *)item->variable)--;

      else if( ((key_pressed == KEY_VOL_DOWN)||(key_pressed == KEY_VOL_DOWN_LONG)) && (*(uint32_t *)item->variable == item->var_min))
       (*(uint32_t *)item->variable) = item->var_max;

      sprintf(vartext,">%04d",*(uint32_t *)item->variable);
      my_spi_WEH001602_out_text(BOTTOM_ROW,vartext);
     }

   }

  else if(item->var_type == VAR_TYPE_GET_BOOL)
   {

    while((key_pressed != KEY_SETUP) && (key_pressed != KEY_TUNING_UP_LONG) && (key_pressed != KEY_TUNING_DOWN_LONG))
     {
      read(G_setup_key_input[0], &key_pressed, sizeof(uint8_t));

      if((key_pressed == KEY_VOL_UP) || (key_pressed == KEY_VOL_DOWN))
       {
        if((*(uint8_t *)item->variable) == 0)
         (*(uint8_t *)item->variable) = 1;
        else
         (*(uint8_t *)item->variable) = 0;
       }

      if((*(uint8_t *)item->variable) == 0)
        my_spi_WEH001602_out_text(BOTTOM_ROW,">no ");
      else if((*(uint8_t *)item->variable) == 1)
        my_spi_WEH001602_out_text(BOTTOM_ROW,">yes");
     }

   }

  else if(item->var_type == VAR_TYPE_GET_WEEKDAYS)
   {

    while((key_pressed != KEY_SETUP) && (key_pressed != KEY_TUNING_UP_LONG) && (key_pressed != KEY_TUNING_DOWN_LONG))
     {
      read(G_setup_key_input[0], &key_pressed, sizeof(uint8_t));

      if(key_pressed == KEY_TUNING_UP)
       if(cursor_pos < 6)
        cursor_pos++;
       else
        cursor_pos = 0;

      if(key_pressed == KEY_TUNING_DOWN)
       if(cursor_pos > 0)
        cursor_pos--;
       else
        cursor_pos = 6;

      if((key_pressed == KEY_VOL_UP) || (key_pressed == KEY_VOL_DOWN))
       if(  ( (  (*(uint8_t *)item->variable) >> cursor_pos) & 1 )  == 1)
        (*(uint8_t *)item->variable) ^= (1 << cursor_pos);
       else if(  ( (  (*(uint8_t *)item->variable) >> cursor_pos) & 1 )  == 0)
        (*(uint8_t *)item->variable) |= (1 << cursor_pos);

      my_spi_WEH001602_out_cmd(0b00001100);

      my_spi_WEH001602_out_text_at_col(BOTTOM_ROW,0,">Su[");

      for(i = 0; i < 7; i++)
       {
        if( ( ((*(uint8_t *)item->variable) >> i) & 1 )  == 1)  // check every bit of this variable: bits 0 - 6 represent days of week
         my_spi_WEH001602_out_text_at_col(BOTTOM_ROW,4+i,"x");
        else my_spi_WEH001602_out_text_at_col(BOTTOM_ROW,4+i,"-");
       }

      my_spi_WEH001602_out_text_at_col(BOTTOM_ROW,11,"]Sa");

      my_spi_WEH001602_out_cmd(0b00001111); // turn cursor+blinking on
      my_spi_WEH001602_move_cursor_at_col(BOTTOM_ROW,4+cursor_pos);

     }

     my_spi_WEH001602_out_cmd(0b00001100); // turn cursor blinking off


    }

  else if(item->var_type == VAR_TYPE_SECURE_GET_STRING)
   {

    char edit_string[255];
    char display_string[255];
    char *compare;
    uint8_t i;
	
    bzero(edit_string,255);
	
    strncpy(edit_string,(char *)item->variable,255);
	
    //edit_string = (char *)item->variable;

    while((key_pressed != KEY_SETUP) && (key_pressed != KEY_TUNING_UP_LONG) && (key_pressed != KEY_TUNING_DOWN_LONG))
     {
      read(G_setup_key_input[0], &key_pressed, sizeof(uint8_t));

      if(key_pressed == KEY_TUNING_UP)
       if(cursor_pos < 14)
        cursor_pos++;
       else
        cursor_pos = 0;

      if(key_pressed == KEY_TUNING_DOWN)
       if(cursor_pos > 0)
        cursor_pos--;
       else
        cursor_pos = 14;

      if( ((key_pressed == KEY_VOL_UP)||(key_pressed == KEY_VOL_UP_LONG)) && (edit_string[cursor_pos] < 126))
       edit_string[cursor_pos]++;
       
      if( ((key_pressed == KEY_VOL_DOWN)||(key_pressed == KEY_VOL_DOWN_LONG)) && (edit_string[cursor_pos] > 32))
       edit_string[cursor_pos]--;

      i = 0;
   
       while(edit_string[i] != 0x0)
        {
 	 if(i == cursor_pos)  
	  display_string[i] = edit_string[i];
	 else  
	  display_string[i] = '*';
         i++;
        }   
	  
      my_spi_WEH001602_out_cmd(0b00001100);
	  
      my_spi_WEH001602_out_text_at_col(BOTTOM_ROW,0,">");
      my_spi_WEH001602_out_text_at_col(BOTTOM_ROW,1,display_string);

      my_spi_WEH001602_out_cmd(0b00001111); // turn cursor+blinking on
      my_spi_WEH001602_move_cursor_at_col(BOTTOM_ROW,1+cursor_pos);
     }
	
    my_spi_WEH001602_out_cmd(0b00001100);
 
    compare = strstr((char *)item->variable,edit_string);

    if(compare == NULL)
     strncpy((char *)item->variable,edit_string,255);
    else if(strlen(compare) != strlen(edit_string))
     strncpy((char *)item->variable,edit_string,255);
    
   }
   
  else if(item->var_type == VAR_TYPE_GET_STRING)
   {

    edit_string = (char *)item->variable;

    while((key_pressed != KEY_SETUP) && (key_pressed != KEY_TUNING_UP_LONG) && (key_pressed != KEY_TUNING_DOWN_LONG))
     {
      read(G_setup_key_input[0], &key_pressed, sizeof(uint8_t));

      if(key_pressed == KEY_TUNING_UP)
       if(cursor_pos < 14)
        cursor_pos++;
       else
        cursor_pos = 0;

      if(key_pressed == KEY_TUNING_DOWN)
       if(cursor_pos > 0)
        cursor_pos--;
       else
        cursor_pos = 14;

      if( ((key_pressed == KEY_VOL_UP)||(key_pressed == KEY_VOL_UP_LONG)) && (edit_string[cursor_pos] < 126))
       edit_string[cursor_pos]++;
       
      if( ((key_pressed == KEY_VOL_DOWN)||(key_pressed == KEY_VOL_DOWN_LONG)) && (edit_string[cursor_pos] > 32))
       edit_string[cursor_pos]--;

      my_spi_WEH001602_out_cmd(0b00001100);
	  
      my_spi_WEH001602_out_text_at_col(BOTTOM_ROW,0,">");
      my_spi_WEH001602_out_text_at_col(BOTTOM_ROW,1,edit_string);

      my_spi_WEH001602_out_cmd(0b00001111); // turn cursor+blinking on
      my_spi_WEH001602_move_cursor_at_col(BOTTOM_ROW,1+cursor_pos);
     }
   }

  my_spi_WEH001602_out_cmd(0b00001100);
  
  my_spi_WEH001602_out_text(TOP_ROW,"                ");
  my_spi_WEH001602_out_text(BOTTOM_ROW,"                ");
  
  if(key_pressed == KEY_TUNING_UP_LONG)
   return 1;
  else if(key_pressed == KEY_TUNING_DOWN_LONG)
   return -1;
  else 
   return 0;

 }

void PL_setup_thread(void)
 {

   setup_menu_item_t *next_item;
   int8_t direction;

   init_setup_menu();

   while(1)
   {

    while(G_global_mode != GLOBAL_MODE_SETUP)
     {
      usleep(50000);
     }

    next_item = G_setup_menu;

    PL_debug("PL_setup_thread: entered setup");

    while(G_global_mode == GLOBAL_MODE_SETUP)
     {
      pthread_mutex_lock(&G_display_lock);
      direction = PL_get_setup_menu_item(next_item);
      pthread_mutex_unlock(&G_display_lock);
	  
      if(direction == 1)
       next_item = next_item->next;
      else if(direction == -1)
       next_item = next_item->prev;
     }

    PL_debug("PL_setup_thread: exited setup");

   }

 }
