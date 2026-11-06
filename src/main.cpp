#define SW_VERSION (10)
// ************** features **************
#define NET
//#define GRAPH
// ************** platform : TODO use platformIO's macros **************
//#define S3MINIPRO
//#define S2MINI
#define S3ZERO
//#define C3OLED
// ************** parameters **************
#define LOCAL_IP (134)
#define DEF_BRIGHTNESS  (128)
#define MAX_NUM_LEDS    (1024)
#define NUM_LEDS_DEF (32)
#define LED_TYPE WS2812
#define COLOR_ORDER GRB // RGB for outside lights, GRB for tree lights, RGB for internal ws2812
#define BUT_THRESH (10)
#define U8G

#include <Arduino.h>
#include <Preferences.h>
#include <FastLED.h>

// be sure that graphics is supported
#ifndef ARDUINO_ESP32_C3_DEVKITM_1
#undef GRAPH
#endif

#if defined (NET)
#include "WiFi.h"
#endif

#if defined (GRAPH)
#if defined (U8G)
#include <U8g2lib.h>
#include <Wire.h>
#else // U8G
#include <Arduino_GFX_Library.h>
#endif // U8G
#endif // GRAPH

// 6 scroll
#define BASE_SCROLL (1)
// 16 pong
#define BASE_PONG (7)
// 14 solid
#define BASE_SOLID (23)
#define NUM_SCROLL (BASE_PONG-BASE_SCROLL)
#define NUM_PONG (BASE_SOLID-BASE_PONG)
#define NUM_SOLID (NUM_ANI - BASE_SOLID + 1)
#define NUM_ANI (36)

#define DEF_CLRFREQ (3)
#define DEF_DEL (15)
#define DEF_CYC (10000)

#if defined (S3ZERO)
#define LED_PIN (3)
#define GND_PIN (2)
//#define LED_PIN_AUX     (21)
#define NUM_LEDS_AUX    (1)
#define LED_TYPE_AUX    WS2812
#define COLOR_ORDER_AUX RGB
#define BUT_PIN     (0)
#endif // S3ZERO

#if defined (C3OLED)
#define LED_PIN     (2) 
#define BUT_PIN     (9)
#endif


// net

#if defined(NET)
//#define NET_SCAN
//#define LOOPBACK
#define DEBUG_PRINT
#define WIFI_CONNECT_TIMEOUT_MS (7000)
#define NET_STAT_NONE (0)
#define NET_STAT_CONN (1)
#define NET_STAT_CLIENT (2)
String loc_ipaddr_str = "";
String rem_ipaddr_str = "";
String ssid = "";
typedef struct
{
  int server_hasclient;
  int client_avail;
  int client_conn;
  int stat;
} t_net_status;
char infostr[1024];
char hdr[32];
uint8_t rxbuf[1024];
int button_down = 0;
int net_start;
//uint8_t ds8b_address[8];
t_net_status net_status = { 0, 0, 0, 0 };
#endif // NET


#ifdef GRAPH
#ifdef U8G
char printbuf[128];
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 6, 5);
int width = 72;
int height = 40;
int xOffset = 30; // = (132-w)/2
int yOffset = 12; // = (64-h)/2
#else // U8G
char printbuf[128];
// tft
Arduino_DataBus *bus = create_default_Arduino_DataBus();
Arduino_GFX *gfx = new Arduino_GC9107(bus, DF_GFX_RST, 1 /* rotation */, true /* IPS */);
#define GFX_BL DF_GFX_BL  // default backlight pin, you may replace DF_GFX_BL to actual backlight pin
#endif // U8G
#endif // GRAPH


CRGB ORANGE_1 { (0xff), (0x40), (0x00) };
CRGB GOLD { (0xff), (0xA0), (0x00) };
CRGB CRIMSON_RED { (0xc8), (0x00), (0x0a) };
CRGB ORANGE_3 { (0xff), (0x80), (0x00) };
CRGB MOSS_GREEN { (0x1e), (0x64), (0x00) };
CRGB CANDY_GREEN { (0x00), (0xff), (0x1e) };
CRGB RED_1 { (0xff), (0x00), (0x00) };
CRGB GREEN_1 { (0x00), (0xff), (0x00) };
CRGB BLUE_1 { (0x00), (0x00), (0xff) };
CRGB WHITE_1 { (0xff), (0xff), (0xff) };
CRGB COPPER { {0x87}, {0x28}, {0x00}};
CRGB MAGNENTA_1 { (0xff), (0x00), (0xff) };
CRGB YELLOW_1 { (0xff), (0xff), (0x00) };
CRGB CYAN_1 { (0x00), (0xff), (0xff) };



#ifdef SEAS_HALLOWEEN
#define SEAS_HUE_WAV HUE_ORANGE
#define SEAS_PAL_1 PartyColors_p
#define SEAS_PAL_2 FallCoolors_rgb_p
#define SEAS_PAL_3 HeatColors_p
#define SEAS_PAL_4 HalloweenColors_rgb_p
#define SEAS_PONG_COL1 ORANGE_1
#define SEAS_PONG_COL2 GOLD
#define SEAS_PONG_COL3 COPPER
#define SEAS_PONG_COL4 CANDY_GREEN
#define SEAS_SOLID_COL1 ORANGE_1
#define SEAS_SOLID_COL2 GOLD
#define SEAS_SOLID_COL3 CRIMSON_RED
#define SEAS_SOLID_COL4 ORANGE_3
#define SEAS_SOLID_COL5 MOSS_GREEN
#define SEAS_SOLID_COL6 ORANGE_1
#define SEAS_SOLID_COL7 CANDY_GREEN
#else
#define SEAS_HUE_WAV HUE_PURPLE
#define SEAS_PAL_1 PartyColors_p
#define SEAS_PAL_2 LavaColors_p
#define SEAS_PAL_3 OceanColors_p
#define SEAS_PAL_4 ChristmasColors_p
#define SEAS_PONG_COL1 RED_1
#define SEAS_PONG_COL2 GREEN_1
#define SEAS_PONG_COL3 BLUE_1
#define SEAS_PONG_COL4 WHITE_1
#define SEAS_SOLID_COL1 RED_1
#define SEAS_SOLID_COL2 GREEN_1
#define SEAS_SOLID_COL3 BLUE_1
#define SEAS_SOLID_COL4 WHITE_1
#define SEAS_SOLID_COL5 MAGNENTA_1
#define SEAS_SOLID_COL6 YELLOW_1
#define SEAS_SOLID_COL7 CYAN_1
#endif


//#define MAKPIX_SCL(r, g, b, scl) ( ( (unsigned)(r*scl) << 16 ) | ( (unsigned)(g*scl) << 8 ) | ( (unsigned)(b*scl) ) )
//#define MAKPIX(r, g, b) ( ( r << 16 ) | ( g << 8 ) | ( b ) )

CRGB MAKPIX_SCL(CRGB clr, float scl)
{
    return CRGB(
        (uint8_t)(clr.r * scl),
        (uint8_t)(clr.g * scl),
        (uint8_t)(clr.b * scl)
    );
}

CRGB leds[MAX_NUM_LEDS];
#if defined(LED_PIN_AUX)
CRGB leds_aux[NUM_LEDS_AUX];
#endif

CRGBPalette16 currentPalette;

TBlendType    currentBlending;

// wifi
#if defined(NET)
WiFiClient wifi_client;
WiFiServer wifi_server(80);
#endif

extern const TProgmemRGBPalette16 RainbowColors1_p FL_PROGMEM =
{
    0xFF0000, 0xD52A00, 0xAB5500, 0xAB7F00,
    0xABAB00, 0x56D500, 0x00FF00, 0x00D52A,
    0x00AB55, 0x0056AA, 0x0000FF, 0x2A00D5,
    0x5500AB, 0x7F0081, 0xAB0055, 0xD5002B
};


extern const TProgmemRGBPalette16 ChristmasColors_p FL_PROGMEM =
{
    0xFF0000, 0xD52A00, 0xAB5500, 0xAB7F00,
    0xABAB00, 0x56D500, 0x00FF00, 0x00D52A,
    0x00AB55, 0x0056AA, 0x0000FF, 0x2A2AD5,
    0x8888AB, 0x7F7f81, 0xAB5555, 0xD52B2B
};

extern const TProgmemRGBPalette16 HalloweenColors_rgb_p FL_PROGMEM =
{
    0x1b1b1b, 0xff8f00, 0xf0e130, 0xe76d2c,
    0x000000, 0x242124, 0x90ee90, 0xb284be,
    0x612302, 0xda9100, 0xf8f8ff, 0x8a0707,
    0xe3dac9, 0x00a86b, 0x353839, 0xb87333
};

extern const TProgmemRGBPalette16 FallCoolors_rgb_p FL_PROGMEM =
{
    0xdc793e, 0xa2242f, 0xc48a69, 0x0f4c81,
    0xd9ce52, 0xd19c97, 0x006b54, 0x6a2e2a,
    0xe6af91, 0x6b264b, 0x63563b, 0x263056,
    0xf4e4c1, 0x92949b, 0x90ee90, 0xf0e130
};

extern const TProgmemRGBPalette16 HalloweenColors_grb_p FL_PROGMEM =
{
    0x1b1b1b, 0x8fff00, 0xe1f030, 0x6de72c,
    0x000000, 0x212424, 0xee9090, 0x84b2be,
    0x236102, 0x91da00, 0xf8f8ff, 0x078a07,
    0xdae3c9, 0xa8006b, 0x383539, 0x73b833
};

extern const TProgmemRGBPalette16 FallCoolors_grb_p FL_PROGMEM =
{
    0x79dc3e, 0x24a22f, 0x8ac469, 0x4c0f81,
    0xced952, 0x9cd197, 0x6b0054, 0x2e6a2a,
    0xafe691, 0x266b4b, 0x56633b, 0x302656,
    0xe4f4c1, 0x94929b, 0xee9090, 0xe1f030
};



extern const TProgmemRGBPalette16 solidcolor_p FL_PROGMEM =
{
    0x00FF00, 0x00FF00, 0x00FF00, 0x00FF00,
    0x00FF00, 0x00FF00, 0x00FF00, 0x00FF00, 
    0x00FF00, 0x00FF00, 0x00FF00, 0x00FF00, 
    0x00FF00, 0x00FF00, 0x00FF00, 0x00FF00
};

#define MODE_PLAY_RANDOM 1
#define MODE_BYP_SCROLL 2
#define MODE_BYP_PONG 4
#define MODE_BYP_SOLID 8

// persistence
Preferences prefs;  // global instance

// global
int start = BASE_SOLID;
unsigned num_leds = NUM_LEDS_DEF;
unsigned num_leds_pend = num_leds;
unsigned anim_no = start;
int timer_ct = 0;
int freq = DEF_CLRFREQ;
int del = DEF_DEL;
int cyc = DEF_CYC;
int ver = SW_VERSION;
int storecount = 0;
int brightness = DEF_BRIGHTNESS;
uint32_t mode = 0;
int anim_changed = 1;


// proto
void draw_text(String txt1, String txt2, String txt3);
void gfx_init(void);
void shift_color(CHSV& incolor, int color_freq);
void shift_val(CHSV& incolor, int color_freq);
void ani_scroll(unsigned changed);
void ani_pong(unsigned changed);
void ani_solid(unsigned changed);
void ani_blank(unsigned changed);
void sin_color(CHSV& incolor, int color_freq);
int network_sm();
void net_connect(String ssid);
void disp_net_status(void);
void wifi_server_process();
int set_if_changed(int *val_to_set, int val);
void check_client_connection();
void init_leds();
void readfs();
void putfs();
unsigned change_anim(unsigned anim_no, unsigned mode);



void setup() 
{
  Serial.begin(115200);
  delay( 300 ); // power-up safety delay
  // Serial.setDebugOutput(true);
  // while(!Serial);
  Serial.println("Starting..");

  // 1) Open a “namespace” in NVS
  //    Second arg: false = read/write, true = read-only
  if (!prefs.begin("string", false)) {
    Serial.println("Failed to init NVS");
    while (true) {}
  }  

  readfs();

  anim_no = start;

#if defined(GRAPH)
  gfx_init();
  draw_text("Starting..", "", "");
#endif



  // initialize wifi
#if defined(NET)

// *******************************************************************************    
// SETUP FOR FIXED IP, UNTESTED
  // Set your Static IP address
  IPAddress local_IP(192, 168, 1, LOCAL_IP);
  // Set your Gateway IP address
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 0, 0);
  IPAddress primaryDNS(8, 8, 8, 8);   //optional
  IPAddress secondaryDNS(8, 8, 4, 4); //optional
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) 
  {
    Serial.println("STA Failed to configure");
  }
// *******************************************************************************    
    
  // Set WiFi to station mode and disconnect from an AP if it was previously connected.
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);  // ?????
#endif


#if defined EN_PIN
    pinMode(EN_PIN, OUTPUT);
    digitalWrite(EN_PIN, 1);
#endif
    pinMode(LED_PIN, OUTPUT);
#if defined (GND_PIN)
    pinMode(GND_PIN, OUTPUT);
    digitalWrite(GND_PIN, 0); 
#endif

#if defined BUT_PIN
    pinMode(BUT_PIN, INPUT_PULLUP);
#endif

  init_leds();
    
}

void init_leds()
{
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, num_leds).setCorrection( TypicalLEDStrip );

#if defined(LED_PIN_AUX)
    FastLED.addLeds<LED_TYPE_AUX, LED_PIN_AUX, COLOR_ORDER_AUX>(leds_aux, NUM_LEDS_AUX).setCorrection( TypicalLEDStrip );
#endif    
    FastLED.setBrightness(  brightness );

}

void putfs()
{
  ++storecount;
  prefs.putUInt("storecount", storecount);
  prefs.putUInt("ver", ver);
  prefs.putUInt("num_leds", num_leds_pend);
  prefs.putInt("freq", freq);
  prefs.putInt("del", del);
  prefs.putInt("cyc", cyc);
  prefs.putInt("start", start);
  prefs.putInt("brightness", brightness);
  prefs.putUInt("mode", mode);
  

  Serial.printf("putfs: storect=%u, storedver=%u, leds=%u, new_num_leds=%u,freq=%d, del=%d, cyc=%d, brigt=%d,start=%d\n", 
    storecount, ver, num_leds, num_leds_pend, freq, del, cyc, brightness, start);
}

void readfs()
{
  storecount = prefs.getUInt("storecount", 0);  
  uint32_t storedver = prefs.getUInt("ver", 0);  
  num_leds = prefs.getUInt("num_leds", NUM_LEDS_DEF);  
  num_leds_pend = num_leds;
  freq = prefs.getInt("freq", DEF_CLRFREQ);  
  del = prefs.getInt("del", DEF_DEL);  
  cyc = prefs.getInt("cyc", DEF_CYC);
  start = prefs.getInt("start", BASE_SOLID);
  brightness = prefs.getInt("brightness", DEF_BRIGHTNESS);
  mode = prefs.getUInt("mode", 0);

  // legacy issue, used to be -1 to signal default
  freq = freq==-1 ? DEF_CLRFREQ:freq;
  del = del==-1 ? DEF_DEL:del;
  cyc = cyc==-1 ? DEF_CYC:cyc;




  Serial.printf("readfs: storect=%u, storedver=%u, leds=%u, freq=%d, del=%d, cyc=%d, bright=%d, start=%d\n", 
    storecount, storedver, num_leds, freq, del, cyc, brightness, start);
}


char b1[16], b2[16], b3[16];

void loop()
{
  static int but_ct = 0;

  // **** can anim_changed govern this entire if..then block? ***
  if (anim_no == 0)
    ani_blank(anim_changed);
  else if (anim_no < BASE_PONG)
    ani_scroll(anim_changed);
  else if (anim_no < BASE_SOLID)
    ani_pong(anim_changed);
  else  if (anim_no < NUM_ANI+1)
    ani_solid(anim_changed);

  else if (anim_no == 256)
  {
    if (anim_changed)
    {
      // special case, set to solid white
      for (int j=0; j<num_leds; ++j) 
        leds[j] = WHITE_1;
#if defined (LED_PIN_AUX)        
      leds_aux[0] = WHITE_1;
#endif
    }
  }
  else
  {
    ani_blank(anim_changed);
  }

  #ifdef GRAPH
    if (anim_changed)
    {
      if (net_status.stat == NET_STAT_CONN)
        sprintf(b1, "a%d i%d", anim_no, LOCAL_IP);
      else if (net_status.stat == NET_STAT_CLIENT)
        sprintf(b1, "a%d I%d", anim_no, LOCAL_IP);
      else  
        sprintf(b1, "a%d i%s", anim_no, "err");

      sprintf(b2, "v%d b%d", ver, brightness);
      sprintf(b3, "m%d n%d", mode, num_leds);
      draw_text(b1, b2, b3);
      Serial.printf("%s %s %s", b1, b2, b3);
    }
  #endif


  anim_changed = 0;

  #ifdef BUT_PIN
    if (digitalRead(BUT_PIN) == 0 && but_ct != -10)
    {
      but_ct = but_ct + 1;
      if (but_ct > BUT_THRESH)
      {
        anim_no = change_anim(anim_no, mode);
        anim_changed = 1;
        Serial.printf("button pressed, animation=%d\n", anim_no);
        but_ct = -BUT_THRESH;
      }
    }
    else if (but_ct == -BUT_THRESH)
    {
      if (digitalRead(BUT_PIN) != 0)
      {
        ++but_ct;
      // check if person let go of button 
      }
    }
    else
      but_ct = 0;
  #endif

#if defined (NET)
  network_sm();
#endif

  ++timer_ct;
  
  // auto increment
  if ( ( (timer_ct % cyc) ==0) && ( anim_no&255) > 0)
  {
    anim_no = change_anim(anim_no, mode);
    anim_changed = 1;
  }


  FastLED.delay(1);
}


unsigned change_anim(unsigned anim_no, unsigned mode)
{
//ssssssssssssssssssssssssss
//#define BASE_SCROLL (1)
//#define BASE_PONG (7)
//#define BASE_SOLID (23)


  if ( (mode & MODE_BYP_SCROLL) && (mode & MODE_BYP_PONG) && (mode & MODE_BYP_SOLID) )
    return BASE_SOLID;
  
  
  unsigned anim_cat = 0;
  unsigned num_cat = 0;
  if (mode & MODE_PLAY_RANDOM)
  {
    if ((mode & MODE_BYP_SCROLL) == 0)
      num_cat += (NUM_SCROLL);
    if ((mode & MODE_BYP_PONG) == 0)
      num_cat += (NUM_PONG);
    if ((mode & MODE_BYP_SOLID) == 0)
      num_cat += (NUM_SOLID);

    anim_cat = (random() % num_cat) + 1;

    if (anim_cat < BASE_PONG &&  ((mode & MODE_BYP_SCROLL) == 0) ) return anim_cat;
    if (mode & MODE_BYP_SCROLL)  anim_cat += NUM_SCROLL;
    if (anim_cat < BASE_SOLID && ((mode & MODE_BYP_PONG) == 0) )   return anim_cat;
    if (mode & MODE_BYP_PONG)  anim_cat += NUM_PONG;
    return anim_cat;
  }
  else
  {
    anim_cat = anim_no + 1;
    if (anim_cat > NUM_ANI) anim_cat = 1;
    if (anim_cat == BASE_SCROLL)
        if (mode & MODE_BYP_SCROLL)  anim_cat = BASE_PONG;
    if (anim_cat == BASE_PONG)
        if (mode & MODE_BYP_PONG)    anim_cat = BASE_SOLID;
    if (anim_cat == BASE_SOLID)
        if (mode & MODE_BYP_SOLID)   anim_cat = BASE_SCROLL;
    if (anim_cat == BASE_SCROLL)
        if (mode & MODE_BYP_SCROLL)  anim_cat = BASE_PONG;


    return anim_cat;

  }

}

void ani_blank(unsigned changed)
{
  if (changed == 1)
  {
    for (int j=0; j<num_leds; ++j) 
      leds[j] = 0;
#if defined(LED_PIN_AUX)  
    leds_aux[0] = 0;
#endif      
  }
}

void ani_solid(unsigned changed)
{
  CRGB pix1;
  static float freq[MAX_NUM_LEDS];
  static float phs[MAX_NUM_LEDS];
  static float y[MAX_NUM_LEDS];
  static uint8_t sel1[MAX_NUM_LEDS];
  int newani = 0;
  int anim = anim_no - BASE_SOLID;

//  0, 1, 2, 3, 4, 5 = solid red, gren, blue, white, mix3, mix7
//  6, 7 = mix3_regen, mix7_regen
//  8, 9, 10, 11, 12, 13 = twinkle red, green, blue, white, mix3, mix7

  newani = changed;

  if (newani == false && (anim == 6 || anim == 7) && (timer_ct%100)==0 )
    newani = true;

  if (newani)
  {
    if (anim > 7)
    {
      for (int i=0; i<num_leds; ++i)
      {
        freq[i] = 1.0/100.0 * random16(10);
        phs[i] = 1.0 * random16(100);
        y[i] = 0;
        sel1[i] = random16(7);
      }
    }
    else
    {
      for (int i=0; i<num_leds; ++i)
      {
        y[i] = 1.0;
        sel1[i] = random16(7);
        //Serial.printf("i=%d, sel=%d\n", i, sel1[i]);
      }
    }
  }


  if (newani == false && anim < 8)
    return;

  // compute sinsoid for brightness (twinkle)
  if (anim > 7)
  {
    for (int i = 0; i< num_leds; ++i)
    {
      y[i] = 0.5 * ( sin( timer_ct * freq[i] + phs[i] ) + 1.0 );
    }
  }


  for (int j=0; j<num_leds; ++j) 
  {
    // modes 1-4, palleted colors
    switch (anim) 
    {
      case 0:
      case 8:
        pix1 = MAKPIX_SCL(SEAS_SOLID_COL1,y[j]);
        break;
      case 1:
      case 9:
        pix1 = MAKPIX_SCL(SEAS_SOLID_COL2,y[j]);
        break;
      case 2:
      case 10:
        pix1 = MAKPIX_SCL(SEAS_SOLID_COL3,y[j]);  // crimson red
        break;
      case 3:
      case 11:
        pix1 = MAKPIX_SCL(SEAS_SOLID_COL4,y[j]);
        break;
      case 4:
      case 6:
      case 12:
      switch ( sel1[j] ) 
        {
          case 0:
          case 1:
            pix1 = MAKPIX_SCL(SEAS_SOLID_COL1,y[j]);
            break;
          case 2:
          case 3:
            pix1 = MAKPIX_SCL(SEAS_SOLID_COL2,y[j]);  // goled
            break;
          case 4:
          case 5:
          case 6:
            pix1 = MAKPIX_SCL(SEAS_SOLID_COL3,y[j]); // crimson red
            break;
        }
        break;
      case 5:
      case 7:
      case 13:
        switch ( sel1[j] ) 
        {
          case 0:
            pix1 = MAKPIX_SCL(SEAS_SOLID_COL1,y[j]);
            break;
          case 1:
            pix1 = MAKPIX_SCL(SEAS_SOLID_COL2,y[j]);
            break;
          case 2:
            pix1 = MAKPIX_SCL(SEAS_SOLID_COL3,y[j]);
            break;
          case 3:
            pix1 = MAKPIX_SCL(SEAS_SOLID_COL4,y[j]);
            break;
          case 4:
            pix1 = MAKPIX_SCL(SEAS_SOLID_COL5,y[j]);  // moss green
            break;
          case 5:
            pix1 = MAKPIX_SCL(SEAS_SOLID_COL6,y[j]);
            break;
          case 6:
            pix1 = MAKPIX_SCL(SEAS_SOLID_COL7,y[j]);
            break;
        }        
        break;

    }
    leds[j] = pix1;
  }
  FastLED.show();
#ifdef LED_PIN_AUX
  leds_aux[0] = pix1;
#endif
 
}


void ani_pong(unsigned changed)
{
  static int lpdelay = 1;
  static int dir = 1;
  static unsigned startIndex = 0;
  int anim = anim_no - BASE_PONG;
  int tracelen = 6;
  int strps = (num_leds /  tracelen) / 2;
  static int pausectr = 0;
  int bounce = 1;
  int cross = 0;
  CRGB color;

  // anim;
  // 0-3 = single bar bounce (red, green, blue, white)
  // 4-7 = single bar through (red, gree, blue, white)
  // 8-11 = multibar bounce 
  // 12-15 = cross pattern bounce

  if (pausectr > 0)
  {
    --pausectr;
    return;
  }

  int animclr = anim & 0x03;
  

  if (animclr == 0) 
    color = SEAS_PONG_COL1;     // orange
  else if (animclr == 1) 
    color = SEAS_PONG_COL2;     // yellow/gold
  else if (animclr == 2) 
    color = SEAS_PONG_COL1;     // copper
  else // (animclr == 3) 
    color = SEAS_PONG_COL4;   // green

  int animmode = anim >> 2;
  if (animmode == 0) 
  {
    // single bar, bounce
    strps = 1;
  }
  else if (animmode == 1) 
  {
    // single bar, overflow
    strps = 1;
    bounce = 0;
  }
  else if (animmode == 2) 
  {
    // multibar
    strps = (num_leds /  tracelen) / 2;
    pausectr = 5;
  }
  else // (animmode == 3) 
  {
    // cross mode
    strps = 1;
    cross = 1;
  }

  

  // assumption is that this is called onece every ms
  if (--lpdelay > 0) return;

  for (int j=0; j<num_leds; ++j) leds[j] = 0;
  for (unsigned i=0; i<strps; ++i) 
  {
    for (unsigned j=0; j<tracelen; ++j)
    {
      leds[ (startIndex + i*2*tracelen + j) % num_leds ] = color;
      if (cross) leds[ (num_leds - tracelen - startIndex + i*2*tracelen + j) % num_leds ] = color;
    }
  }

  FastLED.show();
  if (bounce == 1)
  {
    // bounce mode
    if (startIndex >= (num_leds-tracelen)) dir = -1;
    if (startIndex <= 0)  dir = 1;
    startIndex = startIndex + dir; 
  }
  else
  {
    // overflow mode
    startIndex = startIndex + 1;  
    if (startIndex >= num_leds) startIndex = 0;
  }
  lpdelay = 1;
#ifdef LED_PIN_AUX
  leds_aux[0] = 0xFFFFFF;
#endif  
}


void ani_scroll(unsigned changed)
{
  static int startIndex = 0;
  static int colorIndex = 0;
  static int lpdelay = 50;
  static CRGB leds2[MAX_NUM_LEDS];
  static CHSV pixhsv_col =  CHSV( HUE_PURPLE, 255, 255);
  static CHSV pixhsv_wht =  CHSV( HUE_PURPLE, 0, 255);
  static CHSV pixhsv_wav =  CHSV( SEAS_HUE_WAV, 0, 255);
  CRGB pix1;
  int clrfreq;
  int anim = anim_no - BASE_SCROLL;

  TBlendType blend = LINEARBLEND;
  CRGBPalette16 currentPalette;


  // assumption is that this is called onece every ms
  if (--lpdelay > 0) return;

  // modes 1-4, palleted colors
  switch (anim) {
    case 0:
      currentPalette = SEAS_PAL_1;
      pix1 = ColorFromPalette( currentPalette , colorIndex, 255, blend);
      colorIndex = colorIndex + freq;
      lpdelay = del;
      break;
    case 1:
      currentPalette = SEAS_PAL_2;
      pix1 = ColorFromPalette( currentPalette, colorIndex, 255, blend);
      colorIndex = colorIndex + freq;
      lpdelay = del;
      break;
    case 2:
      currentPalette = SEAS_PAL_3;
      pix1 = ColorFromPalette( currentPalette, colorIndex, 255, blend);
      colorIndex = colorIndex + freq;
      lpdelay = del;
      break;
    case 3:
      currentPalette = SEAS_PAL_4;
      pix1 = ColorFromPalette( currentPalette, colorIndex, 255, blend);
      colorIndex = colorIndex + freq;
      lpdelay = del;
      break;
    case 4:
      sin_color(pixhsv_wav, 3.33333 * freq);  // freq must be high(10) for starfield sim
      pix1 = pixhsv_wav;
      lpdelay = del;
      break;
    case 5:
      shift_val(pixhsv_col, freq);
      pix1 = pixhsv_col;
      lpdelay = del;
      break;
  }
  leds2[startIndex] = pix1;

  int n = 0;
  for (int j=startIndex; j<num_leds; ++j) leds[n++] = leds2[j];
  for (int j=0; j<startIndex; ++j)    leds[n++] = leds2[j];
  FastLED.show();
  startIndex = startIndex - 1;  if (startIndex < 0) startIndex += num_leds;
#ifdef LED_PIN_AUX
  leds_aux[0] = pix1;
#endif
}

void shift_color(CHSV& incolor, int color_freq)
{
  int huinc = random8() - 128;
  huinc = huinc >> (8-color_freq);
  huinc = color_freq;

  int hue = incolor.hue;
  hue = hue + huinc;
  if (hue > 255) hue = 0;
  if (hue < 0) hue = 255;

  incolor.hue = hue;
}

void shift_val(CHSV& incolor, int color_freq)
{
  int valinc = color_freq;

  int val = incolor.val;
  val = val + valinc;
  if (val > 255) val = 0;
  if (val < 0) val = 255;

  incolor.val = val;
}

void sin_color(CHSV& incolor, int color_freq)
{
  int val = 127 * ( sin( (timer_ct / 100.0) * color_freq) + 1 );


  if (val > 255) val = 255;
  if (val < 0) val = 0;
  incolor.val = val;
}






#if defined (NET)

int network_sm() {
  int test_x;
  int ch;

#if defined(NET_SCAN)
  // scan network
  if (net_status.stat == 0) {
    ssid = net_scan();
    delay(250);
  }
#else   // NET_SCAN
  ssid = "netgear777";
  button_down = 1;
#endif  // NET_SCAN

  // log onto network
  if (net_status.stat == 0 && button_down) {
    net_connect(ssid);
    delay(1000);
    if (net_status.stat == NET_STAT_CONN) {
      // begin tcp server
      wifi_server.begin();
      disp_net_status();
    }
    button_down = 0;
  }

  // check for client connections
  if (net_status.stat == NET_STAT_CONN || net_status.stat == NET_STAT_CLIENT) {
    // if connected to network, check client connected or dropped
    check_client_connection();
  }

  wifi_server_process();

  ch = 0;


  ch += set_if_changed(&net_status.server_hasclient, wifi_server.hasClient());
  ch += set_if_changed(&net_status.client_avail, wifi_client.available());
  ch += set_if_changed(&net_status.client_conn, wifi_client.connected());

  if (ch > 0) disp_net_status();
  return ch;
}

int set_if_changed(int *val_to_set, int val) {
  int ch;
  ch = ~(*val_to_set == val);
  //Serial.printf("%d,%d,%d\n", *val_to_set, val, ch);
  *val_to_set = val;
  return (ch);
}


void wifi_server_process() 
{
#if defined(LOOPBACK)
  // loopback
  while (wifi_client.connected() && wifi_client.available()) 
  {
    int rxd = wifi_client.read(rxbuf, sizeof(rxbuf));
    wifi_client.write(rxbuf, rxd);
  }
#else
  int sumrxd = 0;
  while (wifi_client.connected() && wifi_client.available()) 
  {
    int rxd = wifi_client.read(rxbuf, sizeof(rxbuf));
    sumrxd += rxd;
#if 0
    if (rxd > 0)
    {
      Serial.println(rxd);
      for (int i=0; i<rxd; ++i) infostr[i] = rxbuf[i];
      infostr[rxd] = 0;
      Serial.println(infostr);
      for (int i=0; i<rxd; ++i) 
        Serial.printf("%.2x-", infostr[i]);
      Serial.printf("\n");

      delay(250);
      disp_net_status();

    }
#endif

/*
  ?      : get status
  {val}  : set animatinon
  b{val} : set brightness
  n{val} : set number of lights
  m{val} : set mode
  f{val} : frequency for anumations: lower makes gradients repeat more often
  d{val} : delay for gradients
  c{val} : number of animation cycles to wait before switching to next animation
  s{val} : start at animation $val
  w      : write settings to persistent memeory   

*/

#if 1

    for (int i=0; i<rxd; ++i) infostr[i] = rxbuf[i];
    infostr[rxd] = 0;
    Serial.printf("str=%s len=%d\n",infostr, rxd);

    if (rxbuf[0] == '?') 
    {
      // remote status request
      sprintf(infostr, "anim#=%d, bright=%d, leds=%d, mode=%u freq=%d, del=%d, cyc=%d, start=%d, ver=%d \n [ ? | w | {x} | b{x} | n{x} | m{x} | f{x} | d{x} | c{x} ]\n", 
                        anim_no,  brightness,num_leds,mode,   freq,    del,    cyc,    start,    ver);
      wifi_client.write(infostr, strlen(infostr));
      Serial.println(infostr);
    } 
    else if (rxbuf[0] == 'N' || rxbuf[0] == 'n' )
    {
        int test_leds = atoi(infostr+1);
        if (test_leds > 0 && test_leds <= MAX_NUM_LEDS)
        {
          num_leds_pend = test_leds;
          sprintf(infostr, "OK num_leds(save+restart)=%d\n", num_leds_pend);
          wifi_client.write(infostr, strlen(infostr));
        }
    }
    else if (rxbuf[0] == 'B' || rxbuf[0] == 'b' )
    {
        int test_bright = atoi(infostr+1);
        if (test_bright >= 0 && test_bright <= 255)
        {
          brightness = test_bright;
          sprintf(infostr, "OK brigh=%d\n", brightness);
          wifi_client.write(infostr, strlen(infostr));
          init_leds();
        }
    }
    else if (rxbuf[0] == 'W' or rxbuf[0] == 'w')
    {
      putfs();
      sprintf(infostr, "OK wrote config\n");
      wifi_client.write(infostr, strlen(infostr));
    }
    else if (rxbuf[0] == 'M' or rxbuf[0] == 'm')
    {
      mode = atoi(infostr+1);
      sprintf(infostr, "OK mode=%u\n", mode);
      wifi_client.write(infostr, strlen(infostr));
    }
    else if (rxbuf[0] == 'F' || rxbuf[0] == 'f' )
    {
        freq = atoi(infostr+1);
        sprintf(infostr, "OK freq=%d\n", freq);
        wifi_client.write(infostr, strlen(infostr));
    }
    else if (rxbuf[0] == 'D' || rxbuf[0] == 'd' )
    {
        del = atoi(infostr+1);
        sprintf(infostr, "OK del=%d\n", del);
        wifi_client.write(infostr, strlen(infostr));
    }
    else if (rxbuf[0] == 'C' || rxbuf[0] == 'c' )
    {
        cyc = atoi(infostr+1);
        sprintf(infostr, "OK cyc=%d\n", cyc);
        wifi_client.write(infostr, strlen(infostr));
    }
    else if (rxbuf[0] == 'S' || rxbuf[0] == 's' )
    {
        start = atoi(infostr+1);
        sprintf(infostr, "OK start=%d\n", start);
        wifi_client.write(infostr, strlen(infostr));
    }
    else 
    {
        // intepret as animation#
        anim_no = atoi(infostr);
        sprintf(infostr, "OK animation=%d\n", anim_no);
        wifi_client.write(infostr, strlen(infostr));
        anim_changed = 1;
    }
#endif
  }// while
#endif  // LOOPBACK
  if (sumrxd)
  {
  }
}

void disp_net_status(void) {

  sprintf(infostr, "timer_ct=%d, net_stat=%d, loc_ip=%s, cli_ip=%s  srv_hascli=%d, cli_av=%d cli_con=%d \n",
          timer_ct, net_status.stat, loc_ipaddr_str.c_str(), rem_ipaddr_str.c_str(), net_status.server_hasclient, net_status.client_avail, net_status.client_conn);
  Serial.printf(infostr);

//  sprintf(infostr, "ssid=%s\nnet_stat=%d\nloc_ip=%s\ncli_ip=%s\nsrv_hascli=%d\ncli_av=%d\ncli_con=%d \n",
//          "netgear777", net_status.stat, loc_ipaddr_str.c_str(), rem_ipaddr_str.c_str(), net_status.server_hasclient, net_status.client_avail, net_status.client_conn);
//#ifdef GRAPH
//  sprintf(infostr, "%s\stat=%d\n%s, %s\nst=%d,%d,%d\n",
//          "netgear777", net_status.stat, loc_ipaddr_str.c_str(), rem_ipaddr_str.c_str(), net_status.server_hasclient, net_status.client_avail, net_status.client_conn);
//  draw_text("stat      ", infostr);
//#endif

}

void check_client_connection() {
  if (net_status.stat == NET_STAT_CLIENT && !wifi_client.connected()) {
    wifi_client.stop();
    rem_ipaddr_str = "";
    net_status.stat = NET_STAT_CONN;
    Serial.println("Client disconnected");
    disp_net_status();
  }

  if (wifi_server.hasClient()) {
    // If we are already connected to another computer,
    // then reject the new connection. Otherwise accept
    // the connection.
    if (wifi_client.connected()) {
      Serial.println("Connection rejected");
      wifi_server.available().stop();
    } else {
      wifi_client = wifi_server.available();
      rem_ipaddr_str = wifi_client.remoteIP().toString();
      Serial.printf("Connection accepted from %s\n", rem_ipaddr_str.c_str());
      net_status.stat = NET_STAT_CLIENT;
    }
    disp_net_status();
  }
}

void net_connect(String ssid) {
  int try_ct = 0;

  Serial.printf("Connecting to WiFi ssid=%s", ssid.c_str());
  sprintf(infostr, "%s", ssid.c_str());
#ifdef GRAPH
  char b1[8];
  sprintf(b1, "ip=%d", LOCAL_IP);
  draw_text("connect", infostr, b1);
#endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, "princess777");

  while (WiFi.status() != WL_CONNECTED && ++try_ct < WIFI_CONNECT_TIMEOUT_MS * 0.01) 
  {
    Serial.print('.');
    delay(50);
  }
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED) 
  {
    loc_ipaddr_str = WiFi.localIP().toString();
    sprintf(infostr, "ssid=%s\nip=%s", ssid.c_str(), loc_ipaddr_str.c_str());
    Serial.println(infostr);
#ifdef GRAPH
    char b1[8];
    sprintf(b1, "ip=%d", LOCAL_IP);
    draw_text("connect", "success", b1);
#endif    
    net_status.stat = NET_STAT_CONN;
  } 
  else 
  {
    sprintf(infostr, "st=%d", WiFi.status());
    Serial.println(String("Error") + infostr);
#ifdef GRAPH
    draw_text("connect", "error", infostr);
#endif    
  }
}


String net_scan() {
  String s_infostr;
  String retval = "";

  Serial.println("Scan start");

  // WiFi.scanNetworks will return the number of networks found.
  int n = WiFi.scanNetworks();
  int n1 = n;
  if (n1 > 7) n1 = 7;
  Serial.println("Scan done");
  s_infostr = "";
  if (n1 == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
    for (int i = 0; i < n1; ++i) {
      // Print SSID and RSSI for each network found
      Serial.printf("%2d", i + 1);
      //sprintf(infostr,"%2d",i + 1); s_infostr += infostr;
      Serial.print("|");
      //sprintf(infostr,"|");  s_infostr += infostr;
      Serial.printf("%s", WiFi.SSID(i).c_str());
      sprintf(infostr, "%-10.10s", WiFi.SSID(i).c_str());
      s_infostr += infostr;
      Serial.print("|");
      sprintf(infostr, "|");
      s_infostr += infostr;
      Serial.printf("%d", WiFi.RSSI(i));
      sprintf(infostr, "%d", WiFi.RSSI(i));
      s_infostr += infostr;
      Serial.print("|");
      sprintf(infostr, "|");
      s_infostr += infostr;
      Serial.printf("%d", WiFi.channel(i));
      sprintf(infostr, "%d", WiFi.channel(i));
      s_infostr += infostr;
      Serial.print("|");
      sprintf(infostr, "|");
      s_infostr += infostr;
      switch (WiFi.encryptionType(i)) {
        case WIFI_AUTH_OPEN:
          Serial.print("open");
          sprintf(infostr, "open");
          s_infostr += infostr;
          break;
        case WIFI_AUTH_WEP:
          Serial.print("WEP");
          sprintf(infostr, "wep ");
          s_infostr += infostr;
          break;
        case WIFI_AUTH_WPA_PSK:
          Serial.print("WPA");
          sprintf(infostr, "wpa ");
          s_infostr += infostr;
          break;
        case WIFI_AUTH_WPA2_PSK:
          Serial.print("WPA2");
          sprintf(infostr, "wpa2");
          s_infostr += infostr;
          break;
        case WIFI_AUTH_WPA_WPA2_PSK:
          Serial.print("WPA+WPA2");
          sprintf(infostr, "wwp2");
          s_infostr += infostr;
          break;
        case WIFI_AUTH_WPA2_ENTERPRISE:
          Serial.print("WPA2-EAP");
          sprintf(infostr, "wp2e");
          s_infostr += infostr;
          break;
        case WIFI_AUTH_WPA3_PSK:
          Serial.print("WPA3");
          sprintf(infostr, "wpa3");
          s_infostr += infostr;
          break;
        case WIFI_AUTH_WPA2_WPA3_PSK:
          Serial.print("WPA2+WPA3");
          sprintf(infostr, "wp23");
          s_infostr += infostr;
          break;
        case WIFI_AUTH_WAPI_PSK:
          Serial.print("WAPI");
          sprintf(infostr, "wapi");
          s_infostr += infostr;
          break;
        default:
          Serial.print("unknown");
          sprintf(infostr, "????");
          s_infostr += infostr;
      }
      Serial.println();
      sprintf(infostr, "\n");
      s_infostr += infostr;
      delay(10);
    }
  }
  Serial.println("");
  sprintf(hdr, "%d found:", n);
//#ifdef GRAPH
//  draw_text(hdr, s_infostr);
//#endif
  if (n > 0) retval = WiFi.SSID(0);

  // Delete the scan result to free memory for code below.
  WiFi.scanDelete();

  return retval;
}

#endif // NET



#if defined(GRAPH)

void gfx_init(void) {

#ifdef U8G
  delay(1000);
  u8g2.begin();
  u8g2.setContrast(255); // set contrast to maximum 
  u8g2.setBusClock(400000); //400kHz I2C 
  // tf: full font / all glyphs includedtr: Transparent background
  // ncen = new centry schoolbook, helv, courier, etc
  
  
  u8g2.setFont(u8g2_font_helvB14_tf);
  //u8g2.setFont(u8g2_font_courB12_tf);
  //u8g2.setFont(u8g2_font_ncenB12_tr);
  //u8g2.setFont(u8g2_font_ncenB10_tr);
  //u8g2.setFont(u8g2_font_ncenB08_tf);
#else
  #ifdef GFX_EXTRA_PRE_INIT
    GFX_EXTRA_PRE_INIT();
  #endif
    // Init Display
    if (!gfx->begin()) {
      Serial.println("gfx->begin() failed!");
    }
    gfx->fillScreen(BLACK);
    // backlight
  #ifdef GFX_BL
    pinMode(GFX_BL, OUTPUT);
    digitalWrite(GFX_BL, HIGH);
  #endif
#endif
}



void draw_text(String txt1, String txt2, String txt3) 
{

#ifdef U8G
  u8g2.clearBuffer(); // clear the internal memory


  //u8g2.drawFrame(xOffset+0, yOffset+0, width, height); //draw a frame around the border
  u8g2.setCursor(xOffset+0, yOffset+24);
  u8g2.printf(txt1.c_str());
  u8g2.setCursor(xOffset+0, yOffset+38);
  u8g2.printf(txt2.c_str());
  u8g2.setCursor(xOffset+0, yOffset+52);
  u8g2.printf(txt3.c_str());
  u8g2.sendBuffer(); // transfer internal memory to the display     
#else // U8G

#define SPC (8)
#define TXT_SZ (1)
#define SPC2 (16)
#define TXT_SZ_2 (2)
  //gfx->setFont("u8g2_font_cubic11_h_cjk");
  //  gfx->setTextWrap(false);

//  gfx->fillRect(0, 0, 127, SPC, WHITE);
  gfx->setTextSize(TXT_SZ);
  gfx->setCursor(0, 0);
  gfx->setTextColor(BLACK, WHITE);
  gfx->println(txt1);

  gfx->fillRect(0, SPC
   + 1, 127, 127, BLACK);
  gfx->setTextSize(TXT_SZ_2);
  gfx->setCursor(0, SPC + 1);
  gfx->setTextColor(WHITE, BLACK);
  gfx->println(txt2);
#endif // U8G  
}
#endif // GRAPH
