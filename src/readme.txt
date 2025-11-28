
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
  -c3-OLD board support
  -Way to select subset of patterns


*/
