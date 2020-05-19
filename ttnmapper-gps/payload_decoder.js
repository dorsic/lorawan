function Decoder(bytes, port) {
  // Decode an uplink message from a buffer
  // (array) of bytes to an object of fields.
  var decoded = {};

  decoded.battery = 4.4 - (255-bytes[0])/160.0;
  decoded.switch = bytes[1] >>> 7;
  if (bytes.length > 2) {
    decoded.altitude =  ((bytes[1] & 127) << 7) + (bytes[2] >>> 1);
    decoded.altitude = decoded.altitude/5;
    decoded.longitude = (1-(2*(bytes[2] & 1))) * (100*(bytes[3] & 128) + ((bytes[3] & 127)) + (bytes[4] >>> 2)/60 + (((bytes[4]&3) << 8) + bytes[5])/60000);
    decoded.latitude = (1-(2*(bytes[6] >>> 7))) * (((bytes[6] & 127)) + (bytes[7] >>> 2)/60 + (((bytes[7]&3) << 8) + bytes[8])/60000);
    decoded.hdop = bytes[9]/10.0;
    decoded.location = {
        lat: decoded.latitude,
        lon: decoded.longitude,
    };
  }  

  return decoded;
}
