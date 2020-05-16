//----------------------------------------//
//  gps.ino
//
//  created 06/09/2018
//  update to esp32 - 05/06/2019
//  by Luiz H. Cassettari
//----------------------------------------//


#include <SoftwareSerial.h>
#include <stdlib.h>

#define RXPin 10  
#define TXPin 11
#define GPSBaud 9600

char buffer[80];
byte b;

unsigned char lat[3];
unsigned char lon[4];
unsigned short alt[1];
unsigned char hdop[1];
unsigned long ts = 0;

SoftwareSerial ss(RXPin, TXPin);

int is_gga(char *buffer) {
    if ((strncmp(buffer, "GNGGA", 5) == 0) || (strncmp(buffer, "GPGGA", 5) == 0)) {
        return 1;
    }
    return 0;
}

/*
unsigned short encode_time(char *buffer) {
    char buf[3];
    char* token = strtok(buffer, ",");
    token = strtok(NULL, ",");
    if (strlen(token) < 6) return 0;
    
    strncpy(buf, &token[3], 3);
    return (unsigned short)atoi(buf);
}
*/

void encode_lat(char *buffer, unsigned char *lat) {
    // 24 bits long, MSB is sign, LSB are second fractions
    char buf[3];
    unsigned short i;
    
    char* token = strtok(buffer, ",");
    for (byte i = 0; i < 2; i++) {
      token = strtok(NULL, ",");
    }
    if (strlen(token) < 8) return 0;
    memset(buf, 0, sizeof buf);
    strncpy(buf, &token[5], 3);
    i = atoi(buf);

    memset(buf, 0, sizeof buf);
    strncpy(buf, &token[2], 2);
    i += atoi(buf) << 10;
    lat[2] = i & 0xFF;
    lat[1] = (i & 0xFF00) >> 8;
    
    memset(buf, 0, sizeof buf);
    strncpy(buf, token, 2);
    i = atoi(buf);
    lat[0] = i;

    token = strtok(NULL, ",");

    if (strncmp(token, "S", 1) == 0) {
        lat[0] += 0x80;
    }
}

void encode_lon(char *buffer, unsigned char *lon) {
    // 25 bits long, MSB is sign, LSB are second fractions    
    char buf[4];
    unsigned short i;
    
    char* token = strtok(NULL, ",");
    if (strlen(token) < 9) return 0;
    
    memset(buf, 0, sizeof buf);
    strncpy(buf, &token[6], 3);  // fractions
    i = atoi(buf);

    memset(buf, 0, sizeof buf);
    strncpy(buf, &token[3], 2);  // minutes
    i += atoi(buf) << 10;

    lon[3] = i & 0xFF;
    lon[2] = (i & 0xFF00) >> 8;
    
    memset(buf, 0, sizeof buf);
    strncpy(buf, token, 3);  // degrees
    i = atoi(buf);
    lon[1] = i & 0xFF;

    token = strtok(NULL, ",");
    if (strncmp(token, "W", 1) == 0) {
        lon[0] = 1;
    }
}

void encode_hdop(char *buffer, unsigned char *hdop) {
    // 8 bits long max 255
    // to decode: hdop = val / 10
    float f;
    unsigned char i;
    char* token = strtok(NULL, ",");
    token = strtok(NULL, ",");
    token = strtok(NULL, ",");
    f = atof(token);
    if (f > 25.5) {
        hdop[0] = 0xFF;
    } else {
        hdop[0] = (unsigned char)(f*10.0);
    }
}

void encode_alt(char *buffer, unsigned short *alt) {
    // 14 bits long max 16383
    // to decode: alt = (val*2)/10
    float f;
    char *token = strtok(NULL, ",");
    f = atof(token);
    if (f > 3276) {
        alt[0] = 0x3FFF;
    } else {
        alt[0] = (unsigned short)(f*5);
    }
}

void gps_setup() {
  ss.begin(GPSBaud);
  Serial.println();
  ts = 0;
}

void gps_loop() {
  while (ss.available() > 0) {
        char c = ss.read();
        switch (c) {
            case '$':
                b = 0;
                memset(buffer, 0, 80);
                break;
            //case '\r':
            //case '\n':
            case '*':
                if (is_gga(buffer) == 1) {
                    // do not change the order
                    Serial.println(buffer);
                    encode_lat(buffer, lat);
                    //Serial.println(lat);
                    if (lat > 0) {
                        ts = millis();
                        encode_lon(buffer, lon);
                        encode_hdop(buffer, hdop);
                        encode_alt(buffer, alt);
                        //Serial.println(lon);
                        //Serial.println(hdop[0]);
                        //Serial.println(alt);
                    }
                }
                break;
            default:
                buffer[b] = c;
                b++;
        }    
  }
  /*if (runEvery_gps(5000))
  {
    Serial.print(F("looping... "));
  }*/
}

boolean gps_valid() 
{
  if (ts == 0) { return false; }
  if (millis() - ts > 100000) { return false; }

  return true;
}

/*
boolean runEvery_gps(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
*/
