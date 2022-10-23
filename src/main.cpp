#include <M5StickCplus.h>
//#include <M5Atom.h>
#include "MFRC522_I2C.h"
#include <SparkFun_I2C_Mux_Arduino_Library.h> // http://librarymanager/All#SparkFun_I2C_Mux https://github.com/sparkfun/SparkFun_I2C_Mux_Arduino_Library
QWIICMUX i2cMux;
MFRC522 mfrc522(0x28); 
uint8_t channel = 0;
static const uint8_t num_channel = 3;
String mode ="reader";
String ids[num_channel];
boolean id_exist = false;
boolean octopass_flg = false;

#define TASK_NAME_NFC "NFCTask"
#define TASK_SLEEP_NFC 200 //10ms delay
static void NFCLoop(void* arg);

/*
#define TASK_NAME_IDcheck "IDCheckTask"
#define TASK_SLEEP_IDcheck 2300 //10ms delay
static void IDCheckLoop(void* arg);
*/

portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;


#include <WiFi.h>
static const char WIFI_SSID[] = "BCW710J-93152-G";
static const char WIFI_PASSPHRASE[] = "c8533d5577f7a";
static const char SERVER[] = "192.168.2.184";

/*
#include "time.h"
#define JST 3600* 9
struct tm timeinfo;
*/

#include <HTTPClient.h>
boolean HTTP_DEBUG = false;
String payload;
void httpGet(String mode);
void httpPost();
String host = "https://1hl0lg.deta.dev/reads";
//#define TASK_DEFAULT_CORE_ID 1
#define TASK_STACK_DEPTH 4096UL
#define TASK_NAME_HTTP "HTTPTask"
#define TASK_SLEEP_HTTP 5000 //10ms delay
static void HttpLoop(void* arg);


#include <ArduinoJson.h>
const int capacity = JSON_OBJECT_SIZE(2);
StaticJsonDocument<capacity> json_request;
char buffer[255];



const char* root_ca= \
"-----BEGIN CERTIFICATE-----\n"
"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\n"
"-----END CERTIFICATE-----";


void setup()
{
  M5.begin();
  delay(100);
  //Wire.begin(25,21); 
  //Wire.begin(26,32); 
  Wire.begin(32,33); 
  //Wire1.begin(26,32);  //If use second I2C channel, must change MFRC522_I2C.cpp
  M5.Lcd.setRotation(3); 
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(2); 
  while (i2cMux.begin(0x70, Wire) == false)
  {
    if(Serial.available())Serial.println("Mux not detected. Freezing...");
    M5.Lcd.print("I2C 0x70 Con...");

  }
  if(Serial.available())Serial.println("Mux detected");
  M5.Lcd.println("I2C 0x70 Connected");
  if(Serial.available())Serial.println("Begin scanning for I2C devices");

  WiFi.begin(WIFI_SSID, WIFI_PASSPHRASE);
  delay(100); 
  while( WiFi.status() != WL_CONNECTED) {
    delay(500); 
    M5.Lcd.print("WiFi con...");
    if(Serial.available())Serial.println("."); 
  }
  delay(100); 
  M5.Lcd.print("WiFi connected");
  if(Serial.available())Serial.print("WiFi Connected");//シリアル出力しないと処理が進まない
  xTaskCreatePinnedToCore(HttpLoop, TASK_NAME_HTTP, TASK_STACK_DEPTH, NULL, 2, NULL, 1); //同じコアだとHTTPが高い必要がある。
  xTaskCreatePinnedToCore(NFCLoop, TASK_NAME_NFC, TASK_STACK_DEPTH, NULL, 1, NULL, 1);
  //configTime(JST, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");

}

boolean reader_read_check[num_channel];
void loop()
{
  vTaskDelay(1);

  /*
  //==Mode Change by pushing button==
  M5.update();
  
  //if(M5.Btn.wasPressed()){
  //  if(mode = 'reader')mode = 'octopass';
  //  else if (mode = 'octopass')mode = 'reader';
  //}
  

  //==Reading==
  for (uint8_t channel = 0; channel < num_channel; channel++) {
    i2cMux.setPort(channel);
    delay(50);
    Serial.printf("CH%d | ", channel);
    M5.Lcd.setCursor(0, 100);   
    M5.Lcd.printf("CH%d  ", channel);
    

    mfrc522.PCD_Init();//!!!!!!removed soft reset()!!!!!!!!!
    delay(50);
    if (!mfrc522.PICC_IsNewCardPresent() ||!mfrc522.PICC_ReadCardSerial()) {
    //if (!mfrc522.PICC_ReadCardSerial()) {
        delay(50);
        //eader_read_check[channel] = false;
        //return;
    }else{
      Serial.print(" // ");
      String s = "";
      for (byte i = 0; i < mfrc522.uid.size;i++) {  // Output the stored UID data.  将存储的UID数据输出
        const auto d = mfrc522.uid.uidByte[i];
        s += String(d < 0x10 ? "0" : "") + String(d, HEX);
      }
      Serial.print(s);
      Serial.print(" / ");
      ids[channel] = String(s);
      reader_read_check[channel] = true;
      id_exist = true;
      //httpGet("registManID",s);
      //httpPost();
    }
  }

  //同時に読めないので、別でloop回す
  
  //if(id_exist){
  //  httpGet("registManID");
  // delay(500);
  //}
  

  //if(mode=='octopass'){
  //chanel初期化とoctopass判定
  

  //}
  Serial.println();
  */
}

static void NFCLoop(void* arg){
  while (1) {
    uint32_t entryTime = millis();

    M5.update();
    
    //==Reading==
    for (uint8_t channel = 0; channel < num_channel; channel++) {
      i2cMux.setPort(channel);
      delay(10);
      if(Serial.available())Serial.printf("CH%d | ", channel);
      M5.Lcd.setCursor(0, 100);   
      M5.Lcd.printf("%d ", channel);
      

      mfrc522.PCD_Init();//!!!!!!removed soft reset()!!!!!!!!!
      delay(10);

      //==========何故か一回置きにしか読み取れない===========
      if (!mfrc522.PICC_IsNewCardPresent() ||!mfrc522.PICC_ReadCardSerial()) {
      //if (!mfrc522.PICC_ReadCardSerial()) {
          delay(10);
          //eader_read_check[channel] = false;
          //return;
      }else{
        if(Serial.available())Serial.print(" // ");
        String s = "";
        for (byte i = 0; i < mfrc522.uid.size;i++) {  // Output the stored UID data.  将存储的UID数据输出
          const auto d = mfrc522.uid.uidByte[i];
          s += String(d < 0x10 ? "0" : "") + String(d, HEX);
        }
        if(Serial.available())Serial.print(s);
        if(Serial.available())Serial.print(" / ");
        ids[channel] = String(s);
        reader_read_check[channel] = true;
        id_exist = true;
        //httpGet("registManID",s);
        //httpPost();
      }
    }
    if(Serial.available())Serial.println();
    uint32_t elapsed_time = millis() - entryTime; 
    int32_t sleep = TASK_SLEEP_NFC  - elapsed_time;
    vTaskDelay((sleep > 0) ? sleep : 0);
  }
}



static void HttpLoop(void* arg){
  while (1) {
    uint32_t entryTime = millis();

    if(WiFi.status() == WL_CONNECTED){

      if(id_exist){
        httpGet("registManID");
      }

      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(0, 0);
      
      for (uint8_t channel = 0; channel < num_channel; channel++){
        M5.Lcd.println("Ch"+String(channel)+":"+String(ids[channel])); // 時分を表示
        if(!reader_read_check[channel]){octopass_flg = false;}
        reader_read_check[channel] = false;
        ids[channel] = "-";
      }
      
      if(octopass_flg){
        //M5.dis.drawpix(0, dispColor(0, 0, 255)); //LED（指定色)
        #ifdef _M5STICKC_H_
        M5.Lcd.setCursor(0, 60);                 //x,y,font 7:48ピクセル7セグ風フォント
        M5.Lcd.print("octo-pass!!!"); // 時分を表示
        #endif
        //M5.dis.drawpix(0, dispColor(0, 0, 0)); //LED（指定色)
        if(Serial.available())Serial.print("octo-pass!!!");
      }else{
        #ifdef _M5STICKC_H_

        M5.Lcd.setCursor(0, 60);
        M5.Lcd.print("                "); // 時分を表示
        #endif
      }
      id_exist = false;
      octopass_flg = true;
      M5.update();
    }

    uint32_t elapsed_time = millis() - entryTime; 
    int32_t sleep = TASK_SLEEP_HTTP  - elapsed_time;
    vTaskDelay((sleep > 0) ? sleep : 0);
    
  }
}

void httpGet(String mode){
    long tmp_start_millis = millis();
    HTTPClient http;
    //http.begin("http://"+String(SERVER)+":8000/nfc/?mode="+mode+"&id="+String(id)); // starter
    //http.begin("https://1hl0lg.deta.dev/read_regist/?object_id="+String(id)+"&reader_mac_id="+String(id)+"&read_date="+String("2222"),root_ca); // starter
    String host_and_msg = "https://1hl0lg.deta.dev/read_regist/?object_id0="+String(ids[0])+"&object_id1="+String(ids[1])+"&object_id2="+String(ids[2])+"&reader_mac_id="+String(WiFi.macAddress())+"&read_date="+String("2222");
    http.begin(host_and_msg,root_ca); // starter
    if(Serial.available())Serial.println(host_and_msg);
    int httpCode = http.GET(); // Make the request

    if (httpCode > 0) { //Check for the returning code
        payload = http.getString();
        if(HTTP_DEBUG) Serial.println(httpCode);
        if(HTTP_DEBUG) Serial.println(payload);
      }
    else {
      if(Serial.available())Serial.println("Error on HTTP request");
      if(Serial.available())Serial.println(httpCode);
    }
    http.end(); //Free the resources
}


void httpPost(){
  json_request["object_id"] = "22222222";
  json_request["reader_mac_id"] = "11111111";
  serializeJson(json_request, buffer, sizeof(buffer));

  HTTPClient http;
  http.begin(host);
  http.addHeader("Content-Type", "application/json");
  int status_code = http.POST((uint8_t*)buffer, strlen(buffer));
  if( status_code == 200 ){
    Stream* resp = http.getStreamPtr();

    DynamicJsonDocument json_response(255);
    deserializeJson(json_response, *resp);

    serializeJson(json_response, Serial);
    Serial.println("");
  }
  http.end();
}