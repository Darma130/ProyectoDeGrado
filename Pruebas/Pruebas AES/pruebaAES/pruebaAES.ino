#include <AESLib.h>

void setup() {

  Serial.begin(115200);
  uint8_t key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  uint8_t data[] = {0,1,2}; //16 chars == 16 bytes
  aes128_enc_single(key, data);
  Serial.print("encrypted:");
  Serial.print(data[0]);
  Serial.print(data[1]);
  Serial.println(data[2]);
  aes128_dec_single(key, data);
  Serial.print("decrypted:");
  Serial.print(data[0]);
  Serial.print(data[1]);
  Serial.println(data[2]);

}

void loop() {
  // put your main code here, to run repeatedly:

}
