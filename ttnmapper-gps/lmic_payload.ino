//----------------------------------------//
//  lmic_payload.ino
//
//  created 03/06/2019
//  by Luiz Henrique Cassettari
//----------------------------------------//

char txBuffer[10];
unsigned char battery = encode_battery();

unsigned char encode_battery() {
  unsigned short sensorValue = analogRead(BATTERY_SENSE_PIN);
  float batV = sensorValue * 6.6/1024;
  if (batV > 4.4) return 255;
  if (batV < 2.8) return 0;
  
  return 255 - (unsigned char)(round((4.4-batV)*160));
}

void encode(unsigned char *buffer, unsigned char *lat, unsigned char *lon, unsigned short *alt, unsigned char *hdop, unsigned char door, unsigned char battery) {
    // the order is door (1 bit), alt (14 bits), lon (25 bits), lat (24 bits), hdop (8 bits) in 9 bytes, LSB is HDOP, MSbit is door state    
    //Serial.println(hdop[0]);
    buffer[0] = battery;
    buffer[1] = (door << 7) + (alt[0] >> 7);
    buffer[2] = ((alt[0] & 0xFF) << 1) + (lon[0] & 1);
    buffer[3] = lon[1];
    buffer[4] = lon[2];
    buffer[5] = lon[3];
    buffer[6] = lat[0];
    buffer[7] = lat[1];
    buffer[8] = lat[2];
    buffer[9] = hdop[0];    
}

void PayloadNow()
{
  boolean confirmed = false;

  if (button_count() == 1) confirmed = true;
  
  if (gps_valid()) {
    encode(txBuffer, lat, lon, alt, hdop, confirmed, encode_battery());

    LMIC_setTxData2(1, txBuffer, sizeof(txBuffer), 0);
    Serial.println(txBuffer);
  }
  else
  {
    txBuffer[0] = encode_battery();
    txBuffer[1] = confirmed;
    LMIC_setTxData2(1, txBuffer, 2, 0);
  }
}
