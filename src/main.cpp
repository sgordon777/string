#include <Arduino.h>
#include <Preferences.h>
#include <FastLED.h>
//#if defined (NET)
#include "WiFi.h"
//#endif

//#if defined (GRAPH)
//#include <Arduino_GFX_Library.h>
//#endif

//#define S3MINIPRO
//#define S2MINI
#define S3ZERO
#define NET
//#define GRAPH
#define LOCAL_IP (135)
#define DEF_BRIGHTNESS  (128)
//#define EN_PIN      (12)
//#define DEB_PIN     (42)
#define BUT_PIN     (0)   //0 for ESP32-S3ZERO, 47 on S3MINPO, 9 for SEED C3, 0 for s2-mini
#define MAX_NUM_LEDS    (1024)
#define NUM_LEDS_DEF (32)
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB // GRB for 2812 strip, RGB for christmas lights
#define BUT_THRESH (10)
#define NUM_ANI (12)

#define DEF_CLRFREQ (3)
#define DEF_DEL (15)
#define DEF_CYC (10000)

#if defined (S3ZERO)
#define LED_PIN     (3)  // 8 for ESP32-S3ZERO (21 builtin), 14 for s3mini-pro, 16 for s2 mini
#define GND_PIN (2)
#define LED_PIN_AUX     (21)  // 8 for ESP32-S3ZERO (21 builtin), 14 for s3mini-pro. C3 flaky, C5 nothing
#define NUM_LEDS_AUX    (1)
#define LED_TYPE_AUX    WS2812
#define COLOR_ORDER_AUX RGB
#endif // S3ZERO


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
char printbuf[128];
// tft
Arduino_DataBus *bus = create_default_Arduino_DataBus();
Arduino_GFX *gfx = new Arduino_GC9107(bus, DF_GFX_RST, 1 /* rotation */, true /* IPS */);
#define GFX_BL DF_GFX_BL  // default backlight pin, you may replace DF_GFX_BL to actual backlight pin
#endif



CRGB leds[MAX_NUM_LEDS];
#ifdef S3ZERO
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

extern const TProgmemRGBPalette16 solidcolor_p FL_PROGMEM =
{
    0x00FF00, 0x00FF00, 0x00FF00, 0x00FF00,
    0x00FF00, 0x00FF00, 0x00FF00, 0x00FF00, 
    0x00FF00, 0x00FF00, 0x00FF00, 0x00FF00, 
    0x00FF00, 0x00FF00, 0x00FF00, 0x00FF00
};

// persistence
Preferences prefs;  // global instance

// global
int num_leds = NUM_LEDS_DEF;
int anim_no = 9;
int timer_ct = 0;
int ovrfreq = -1;
int ovrdel = -1;
int ovrcyc = -1;
int ver = 3;
int storecount = 0;
int brightness = DEF_BRIGHTNESS;

void gfx_init(void);
void shift_color(CHSV& incolor, int color_freq);
void ani_scroll(void);
void ani_pong(void);
void ani_solid(void);
void sin_color(CHSV& incolor, int color_freq);
void network_sm();
void net_connect(String ssid);
void disp_net_status(void);
void wifi_server_process();
int set_if_changed(int *val_to_set, int val);
void check_client_connection();
void init_leds();
void readfs();
void putfs();



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

#if defined(GRAPH)
  gfx_init();
  draw_text("Starting....", "Hello");
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

#if defined(S3ZERO)
    FastLED.addLeds<LED_TYPE_AUX, LED_PIN_AUX, COLOR_ORDER_AUX>(leds_aux, NUM_LEDS_AUX).setCorrection( TypicalLEDStrip );
#endif    
    FastLED.setBrightness(  brightness );

}

void putfs()
{
  ++storecount;
  prefs.putUInt("storecount", storecount);
  prefs.putUInt("ver", ver);
  prefs.putUInt("num_leds", num_leds);
  prefs.putInt("ovrfreq", ovrfreq);
  prefs.putInt("ovrdel", ovrdel);
  prefs.putInt("ovrcyc", ovrcyc);
  prefs.putInt("brightness", brightness);

  Serial.printf("putfs: storect=%u, storedver=%u, leds=%u, ovrfreq=%d, ovrdel=%d, ovrcyc=%d\n", 
    storecount, ver, num_leds, ovrfreq, ovrdel, ovrcyc);
}

void readfs()
{
  storecount = prefs.getUInt("storecount", 0);  
  uint32_t storedver = prefs.getUInt("ver", 0);  
  num_leds = prefs.getUInt("num_leds", NUM_LEDS_DEF);  
  ovrfreq = prefs.getInt("ovrfreq", -1);  
  ovrdel = prefs.getInt("ovrdel", -1);  
  ovrcyc = prefs.getInt("ovrcyc", -1);
  brightness = prefs.getInt("brightness", DEF_BRIGHTNESS);


  Serial.printf("readfs: storect=%u, storedver=%u, leds=%u, ovrfreq=%d, ovrdel=%d, ovrcyc=%d, brightness=%d\n", 
    storecount, storedver, num_leds, ovrfreq, ovrdel, ovrcyc, brightness);
}


void loop()
{
  static int but_ct = 0;
  
  switch(anim_no)
  {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      ani_scroll();
      break;
    case 7:
    case 8:
      ani_pong();
      break;
    case 0:
    case 9:
    case 10:
    case 11:
    case 12:
      ani_solid();
      break;

  }


  #ifdef BUT_PIN
    if (digitalRead(BUT_PIN) == 0 && but_ct != -10)
    {
      but_ct = but_ct + 1;
      if (but_ct > BUT_THRESH)
      {
        ++anim_no;
        if (anim_no > NUM_ANI) anim_no = 2;
        //Serial.printf("button pressed, animation=%d\n", anim_no);
#if defined (GRAPH)
        sprintf(printbuf,"anim=%d\n", anim_no);
        draw_text("change anim", printbuf);
#endif        
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
  int cycthrs = (ovrcyc < 0) ? DEF_CYC:ovrcyc;
  if ( ( (timer_ct % cycthrs) ==0) && anim_no > 0)
  {
    ++anim_no;
    if (anim_no > NUM_ANI) anim_no = 1;  
  }

  FastLED.delay(1);
}


void ani_solid(void)
{
  CRGB pix1;
  static int oldani = 0;

  if (anim_no == oldani)
    return;

  oldani = anim_no;
  for (int j=0; j<num_leds; ++j) 
  {
    // modes 1-4, palleted colors
    switch (anim_no) {
      case 0:
        pix1 = 0x000000;
        break;
      case 9:
        pix1 = 0xFF0000;
        break;
      case 10:
        pix1 = 0x00FF00;
        break;
      case 11:
        pix1 = 0xFFFFFF;
        break;
      case 12:
        int16_t n = random16(5);
        switch (n) {
          case 0:
            pix1 = 0xFF0000;
            break;
          case 1:
            pix1 = 0x00FF00;
            break;
          case 2:
            pix1 = 0x0000FF;
            break;
          case 3:
            pix1 = 0xFFFFFF;
            break;
          case 4:
            pix1 = 0xFF00FF;
            break;
        }
    }
    leds[j] = pix1;
  }
  FastLED.show();
  leds_aux[0] = pix1;
}



void ani_pong(void)
{
  static int lpdelay = 1;
  static int dir = 1;
  static int startIndex = 0;


  // assumption is that this is called onece every ms
  if (--lpdelay > 0) return;

  for (int j=0; j<num_leds; ++j) leds[j] = 0;
  if (startIndex >= 0 && startIndex < num_leds)
    leds[startIndex] = 0xFFFFFF;
  else
    Serial.println("OOB\n");
  if (dir == 1)
  {
    if (startIndex > 0) leds[startIndex-1] = 0xc0c0c0;
    if (startIndex > 1) leds[startIndex-2] = 0xa0a0a0;
    if (startIndex > 2) leds[startIndex-3] = 0x808080;
    if (startIndex > 3) leds[startIndex-4] = 0x606060;
    if (startIndex > 4) leds[startIndex-5] = 0x404040;
    if (startIndex > 5) leds[startIndex-6] = 0x202020;
  }
  else
  {
    if (startIndex < num_leds-1) leds[startIndex+1] = 0xc0c0c0;
    if (startIndex < num_leds-2) leds[startIndex+2] = 0xa0a0a0;
    if (startIndex < num_leds-3) leds[startIndex+3] = 0x808080;
    if (startIndex < num_leds-4) leds[startIndex+4] = 0x606060;
    if (startIndex < num_leds-5) leds[startIndex+5] = 0x404040;
    if (startIndex < num_leds-6) leds[startIndex+6] = 0x202020;
  }
  FastLED.show();
  if (anim_no == 7)
  {
    if (startIndex >= (num_leds-1)) dir = -1;
    if (startIndex <= 0)  dir = 1;
    startIndex = startIndex + dir; 
  }
  else if (anim_no==8)
  {
    startIndex = startIndex + 1;  
    if (startIndex >= num_leds) startIndex = 0;
  }
  lpdelay = 1;
  leds_aux[0] = 0xFFFFFF;
}

void ani_scroll(void)
{
  static int startIndex = 0;
  static int colorIndex = 0;
  static int lpdelay = 50;
  static CRGB leds2[MAX_NUM_LEDS];
  static CHSV pixhsv_col =  CHSV( HUE_PURPLE, 255, 255);
  static CHSV pixhsv_wht =  CHSV( HUE_PURPLE, 0, 255);
  CRGB pix1;
  int clrfreq;

  TBlendType blend = LINEARBLEND;
  CRGBPalette16 currentPalette;


  // assumption is that this is called onece every ms
  if (--lpdelay > 0) return;

//    currentPalette = RainbowColors_p;  // CloudColors_p, LavaColors_p, OceanColors_p, ForestColors_p, RainbowColors_p, RainbowStripeColors_p, PartyColors_p, HeatColors_p

  // modes 1-4, palleted colors
  switch (anim_no) {
    case 1:
      currentPalette = PartyColors_p;
      pix1 = ColorFromPalette( currentPalette , colorIndex, 255, blend);
      clrfreq = (ovrfreq < 0) ? DEF_CLRFREQ:ovrfreq;
      colorIndex = colorIndex + clrfreq;
      lpdelay = (ovrdel < 0) ? DEF_DEL:ovrdel;
      break;
    case 2:
      currentPalette = LavaColors_p;
      pix1 = ColorFromPalette( currentPalette, colorIndex, 255, blend);
      clrfreq = (ovrfreq < 0) ? DEF_CLRFREQ:ovrfreq;
      colorIndex = colorIndex + clrfreq;
      lpdelay = (ovrdel < 0) ? DEF_DEL:ovrdel;
      break;
    case 3:
      currentPalette = OceanColors_p;
      pix1 = ColorFromPalette( currentPalette, colorIndex, 255, blend);
      clrfreq = (ovrfreq < 0) ? DEF_CLRFREQ:ovrfreq;
      colorIndex = colorIndex + clrfreq;
      lpdelay = (ovrdel < 0) ? DEF_DEL:ovrdel;
      break;
    case 4:
      currentPalette = ChristmasColors_p;
      pix1 = ColorFromPalette( currentPalette, colorIndex, 255, blend);
      clrfreq = (ovrfreq < 0) ? DEF_CLRFREQ:ovrfreq;
      colorIndex = colorIndex + clrfreq;
      lpdelay = (ovrdel < 0) ? DEF_DEL:ovrdel;
      break;
    case 5:
      sin_color(pixhsv_wht, (ovrfreq < 0) ? 10:ovrfreq);  // freq must be high(10) for starfield sim
      pix1 = pixhsv_wht;
      lpdelay = (ovrdel < 0) ? DEF_DEL:ovrdel;
      break;
    case 6:
      shift_color(pixhsv_col, (ovrfreq < 0) ? DEF_CLRFREQ:ovrfreq);
      pix1 = pixhsv_col;
      lpdelay = (ovrdel < 10) ? DEF_DEL:ovrdel;
      break;
  }
  leds2[startIndex] = pix1;

  int n = 0;
  for (int j=startIndex; j<num_leds; ++j) leds[n++] = leds2[j];
  for (int j=0; j<startIndex; ++j)    leds[n++] = leds2[j];
  FastLED.show();
  startIndex = startIndex - 1;  if (startIndex < 0) startIndex += num_leds;
  leds_aux[0] = pix1;
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



void sin_color(CHSV& incolor, int color_freq)
{
  int val = 127 * ( sin( (timer_ct / 100.0) * color_freq) + 1 );


  if (val > 255) val = 255;
  if (val < 0) val = 0;
  incolor.val = val;
}






#if defined (NET)

void network_sm() {
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
  if (net_status.stat & NET_STAT_CONN) {
    // if connected to network, check client connected or dropped
    check_client_connection();
  }

  wifi_server_process();

  ch = 0;


  ch += set_if_changed(&net_status.server_hasclient, wifi_server.hasClient());
  ch += set_if_changed(&net_status.client_avail, wifi_client.available());
  ch += set_if_changed(&net_status.client_conn, wifi_client.connected());

  if (ch > 0) disp_net_status();
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
  ? = status
  N = set animatino
  freq=N = set freq
  del=N = set del
  cycdel=N = set switching delay
*/

#if 1

    for (int i=0; i<rxd; ++i) infostr[i] = rxbuf[i];
    infostr[rxd] = 0;
    Serial.printf("str=%s len=%d\n",infostr, rxd);

    if (rxbuf[0] == '?') 
    {
      // remote status request
      sprintf(infostr, "animation=%d, ovrfreq=%d, ovrdel=%d, ovrcyc=%d, leds=%d, ver=%d, bright=%d [?,n,o,w]\n", 
        anim_no, ovrfreq, ovrdel, ovrcyc, num_leds, ver, brightness);
      wifi_client.write(infostr, strlen(infostr));
      Serial.println(infostr);
    } 
    else if (rxbuf[0] == 'O' || rxbuf[0] == 'o' )
    {
      // remote status request
      Serial.printf("override...\n");
      int chk = sscanf(infostr+1, "%d,%d,%d", &ovrfreq, &ovrdel, &ovrcyc );
      if (chk)
      {
        sprintf(infostr, "OK freq=%d, del=%d, cyc=%d\n", ovrfreq, ovrdel, ovrcyc);
      }
      else
      {
        sprintf(infostr, "ERR freq=%d, del=%d, cyc=%d\n", ovrfreq, ovrdel, ovrcyc);
      }
      Serial.printf("OK freq=%d, del=%d, cyc=%d\n", ovrfreq, ovrdel, ovrcyc);
      wifi_client.write(infostr, strlen(infostr));
    } 
    else if (rxbuf[0] == 'N' || rxbuf[0] == 'n' )
    {
        int test_leds = atoi(infostr+1);
        if (test_leds > 0 && test_leds <= MAX_NUM_LEDS)
        {
          num_leds = test_leds;
          sprintf(infostr, "OK num_leds=%d\n", num_leds);
          wifi_client.write(infostr, strlen(infostr));
          init_leds();
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
    else 
    {
        // intepret as animation#
        anim_no = atoi(infostr);
        sprintf(infostr, "OK animation=%d\n", anim_no);
        wifi_client.write(infostr, strlen(infostr));
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
#ifdef GRAPH
  sprintf(infostr, "%s\stat=%d\n%s, %s\nst=%d,%d,%d\n",
          "netgear777", net_status.stat, loc_ipaddr_str.c_str(), rem_ipaddr_str.c_str(), net_status.server_hasclient, net_status.client_avail, net_status.client_conn);
  draw_text("stat      ", infostr);
#endif

}

void check_client_connection() {
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
      net_status.stat = 3;
    }
    disp_net_status();
  }
}

void net_connect(String ssid) {
  int try_ct = 0;

  Serial.printf("Connecting to WiFi ssid=%s", ssid.c_str());
  sprintf(infostr, "connect: %s", ssid.c_str());
#ifdef GRAPH
  draw_text("info", infostr);
#endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, "princess777");

  while (WiFi.status() != WL_CONNECTED && ++try_ct < WIFI_CONNECT_TIMEOUT_MS * 0.01) {
    Serial.print('.');
    delay(50);
  }
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED) {
    loc_ipaddr_str = WiFi.localIP().toString();
    sprintf(infostr, "ssid=%s\nip=%s", ssid.c_str(), loc_ipaddr_str.c_str());
    Serial.println(infostr);
#ifdef GRAPH
    draw_text("connected", infostr);
#endif    
    net_status.stat = 1;
  } else {
    sprintf(infostr, "stat=%d", WiFi.status());
    Serial.println(String("Error") + infostr);
#ifdef GRAPH
    draw_text("Error", infostr);
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
#ifdef GRAPH
  draw_text(hdr, s_infostr);
#endif
  if (n > 0) retval = WiFi.SSID(0);

  // Delete the scan result to free memory for code below.
  WiFi.scanDelete();

  return retval;
}

#endif // NET



#if defined(GRAPH)

void gfx_init(void) {

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
}



void draw_text(String txt1, String txt2) {
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
}
#endif // GRAPH
