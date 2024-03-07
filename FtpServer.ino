// Includes For This Project
#include "esp_camera.h"        
#include "Arduino.h"
#include "FS.h"                
#include "SD_MMC.h"           
#include "SPI.h"
#include "soc/soc.h"           
#include "soc/rtc_cntl_reg.h"  
#include "driver/rtc_io.h"
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"
// inits()
int Number=1;

#include <WiFi.h>
#include <HTTPClient.h>

#define WIFI_SSID "Sadx"
#define WIFI_PASSWORD "Sadx12345"
#define SERVER_URL "http://192.168.70.239:5000/upload"

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound())
  {
    //Frame Size
    config.frame_size = FRAMESIZE_VGA; 
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } 
  else 
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  //Checking for Initialization
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) 
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  //Starting SD card
  Serial.println("Starting SD Card");
  delay(250);
  pinMode(13, INPUT_PULLUP);
  Serial.println("Trying Accessing SDcard");
  if(!SD_MMC.begin("/sdcard", true))
  {
    Serial.println("SD Card Mount Failed");
    return;
  }
  pinMode(13, INPUT_PULLDOWN);
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE)
  {
    Serial.println("No SD Card attached");
    return;
  }
}
void sendFileToServer(uint8_t *buffer, size_t fileSize) 
{
  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "image/jpeg");

  int httpResponseCode = http.POST(buffer, fileSize);
  if (httpResponseCode > 0) {
    Serial.printf("HTTP POST request sent, response code: %d\n", httpResponseCode);
  } else {
    Serial.printf("HTTP POST request failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
  }

  http.end();
}
void loop() 
{
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();  

  if(!fb) 
  {
    Serial.println("Camera capture failed");
    return;
  }

  Serial.println("Sending file to server...");
  sendFileToServer(fb->buf, fb->len);

  esp_camera_fb_return(fb);
  delay(2000);
}
