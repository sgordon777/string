
/*
 build: 
  esp32
    LOLIN S3 MINI PRO
      Settings
        USB-CVC on boot: Enabled
        USB MODE: CDC and JTAB
        Firmware MSC: Disabled
      Libs (tested)
        FASTLED 3.94
        esp32  1.0.13
      Chips, tested with WS2811 string christmax light:
        ESP32 DOIT Devkit V1
          fails (red is yellow,flicker)
        ESP32-C3SEED
          fails (red is yellow,flicker)
        ESP32-C6SEED
          fails (red is yellow,flicker)
        S2 mini
          fails (red is yellow,flicker)
        S3 Zero
          works
        S3 mini pro
          works
        Arduino Mega
          works
        RasPi
          ???
                      
  arduino uno


bugs:
  cannot reconnect

todo: 
  x -add starfield sim: sin(x[] with randomized freq and/or phase) v4
  x -more "trad" v3
  x -set count remotel v3
  x -store settings in nvram v3
  x -gLobal brightness control v3
  x -command to write v3
  x -more pong animations v4
  x -randomize v4
  -change "cycles" to be based time 
  x -c3-OLD board support
  x -Way to select subset of patterns
  x remove "ovr" scheme, give eacj value defai;tested, 6
  x add "start" param to start on a particular animation, 6
  optional PIR sensor
  Different colorsets (halloween, christmas)

version:

  ver7: 
    fixed reconnect bug
    Haloween colors
  ver8: 
    blank aux for mode 0
    mode 256 all white
  ver9:
    bugfix, making array size (num_leds) crashes system
  ver10:
    compile-time season select


power (s3 zero), current @5v
  I@5v    *@12v   Power(w)    Power per bulb
all_off, disconnected, only aux connected
  .052            .26
aux LED full white, no string
  .060            0.3
string of 50 tiny LED all_off
  .067
string of 50 tiny LED all_on
  .676
string of 50 house LED all_off
  0.85    0.049   4
string of 50 house LED all_on
  0.38    0.3     5
string of 200 house LED all_off
  0.16    0.202   3.2
string of 200 house LED all_on
  0.77     .815   13.5

CN1=connector
U1=WAVESHRE_S3MINI_ZERO
U2=74LSHCT125N

CN1_1=STRING_5V           U1_1 (jumpered)
CN1_2=STRING_GND          U1_2
CN1_3=STRING_DATA         U2_8

U1_1=5v                   U2_14, CN1_1(jumpered)
U1_2=GND                  U2_7, CN1_2
U1_5=IO2                  U2_10 (output_ground)
U1_6=IO3                  U2_9 (output)

U2_7=GND                  U1_2
U2_8=3Y                   CN1_3
U2_9=3A                   U1_6
U2_10=3OE'                U1_5
U2_14=VCC                 U1_1



*/
