#define TTS_READ_RSS_NOW 1
#define TTS_READ_RSS_AFTER_10SEC 2

#define TTS_SAY_CURRENT_TIME 5
#define TTS_STOP 10

#define TTS_PLAYING 10
#define TTS_PREPARING 15
#define TTS_STOPPED 20

void PL_TTS_thread(void);
void PL_TTS_indicator_thread(void);

