#include <Firebase.h>
#include <FirebaseArduino.h>
#include <NtpClientLib.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include "Memo.h"
#include "MAX30105.h"
#include "heartRate.h"
#include <MPU6050.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "WorkScheduler.h"
#include "Timer.h"
#include "FS.h"
String uid;
String dayStamp;
String timeStamp;
int SCL_PIN = D1;
int SDA_PIN = D2;
int batnhac;
int16_t ax;
int16_t ay;
int16_t az;
unsigned long milli;
unsigned long milli2;
unsigned long milli4;
unsigned long milli5;
int nguTrua = 0;
int ToneMode;
int Avg = 55;
int tg = 450;
String tgn;
String avgn;
int ledc = D7;
int ledx = D8;
int Stop;
float bpm;
bool Started = false;
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;
int counts;
int8_t timeZone = 7;
int8_t minutesTimeZone = 0;
const PROGMEM char *ntpServer = "pool.ntp.org";
bool wifiFirstConnected = false;
bool Start = false;
const char index_html[] PROGMEM = {"\n\n<!DOCTYPE html>\n<meta charset=\"utf-8\">\n<html>\n<head>\n\t<title>Trung tâm điều khiển thiết bị</title>\n   <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n   <link rel=\"icon\" type=\"image/png\" href=\"https://lh6.googleusercontent.com/H5j1nkrA-y_UV1-AVuaLIP3K3NIicd1wupsy22-I-9qPlS1ADOqdujQ0F0i3UXHPTe4Dp8RU-ud-NPGrt-Dj=w1326-h627-rw\">\n  <link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/materialize/1.0.0/css/materialize.min.css\">\n  <link href=\"https://fonts.googleapis.com/icon?family=Material+Icons\" rel=\"stylesheet\">\n    <link href=\"https://fonts.googleapis.com/css?family=Nunito:400,600,700\" rel=\"stylesheet\">\n  <style>\n    .center {\n      width: 800px;\n      margin: auto;\n      padding: 10px;\n    }\n    .popup {\n  position: relative;\n  display: inline-block;\n  cursor: pointer;\n}\n/* The actual popup (appears on top) */\n.popup .popuptext {\n  visibility: hidden;\n  width: 160px;\n  background-color: #555;\n  color: #fff;\n  text-align: center;\n  border-radius: 6px;\n  padding: 8px 0;\n  position: absolute;\n  z-index: 1;\n  bottom: 125%;\n  left: 50%;\n  margin-left: -80px;\n}\n\n/* Popup arrow */\n.popup .popuptext::after {\n  content: \"\";\n  position: absolute;\n  top: 100%;\n  left: 50%;\n  margin-left: -5px;\n  border-width: 5px;\n  border-style: solid;\n  border-color: #555 transparent transparent transparent;\n}\n\n/* Toggle this class when clicking on the popup container (hide and show the popup) */\n.popup .show {\n  visibility: visible;\n  -webkit-animation: fadeIn 1s;\n  animation: fadeIn 1s\n}\n\n/* Add animation (fade in the popup) */\n@-webkit-keyframes fadeIn {\n  from {opacity: 0;}\n  to {opacity: 1;}\n}\n\n@keyframes fadeIn {\n  from {opacity: 0;}\n  to {opacity:1 ;}\n}\n  </style>\n</head>\n<div class=\"card-panel teal lighten-2 center white-text\">Trung tâm điều khiển thiết bị</div>\n<br>\n<body class=\"center\">\n  <!-- Chế độ báo thức -->\n  <div class=\"center\">\n    <div class=\"col s12 m6\">\n      <div class=\"card blue-grey darken-1\">\n        <div class=\"card-content white-text\">\n          <span class=\"card-title\">Chế độ báo thức</span>\n        </div>\n        <div class=\"card-action\">\n         <button class=\"waves-effect waves-light btn\" onclick=\"ToneMode1()\">Thông thường\n          <i class=\"material-icons\">power_settings_new</i></button>\n          <button class=\"waves-effect waves-light btn\" onclick=\"ToneMode2()\">Âm nhạc\n           <i class=\"material-icons\">power_settings_new</i></button>\n         </div>\n       </div>\n     </div>\n   </div>  \n   <!--Chế độ ngủ trưa-->\n   <div class=\"center\">\n    <div class=\"col s12 m6\">\n      <div class=\"card blue-grey darken-1\">\n        <div class=\"card-content white-text\">\n          <span class=\"card-title\">Chế độ ngủ trưa</span>\n        </div>\n        <div class=\"card-action\">\n         <button class=\"waves-effect waves-light btn\" onclick=\"Tat()\"> Tắt\n           <i class=\"material-icons\">power_settings_new</i></button>\n           <button class=\"waves-effect waves-light btn\" onclick=\"mode1()\"> MODE 1\n             <i class=\"material-icons\">power_settings_new</i></button>\n             <button class=\"waves-effect waves-light btn\" onclick=\"mode2()\"> MODE 2\n               <i class=\"material-icons\">power_settings_new</i></button>\n             </div>\n           </div>\n         </div>\n       </div>\t\n       <br>\n       <!-- Điều chỉnh thời gian ngủ -->\n       <div class=\"center\">\n        <div class=\"col s12 m6\">\n          <div class=\"card blue-grey darken-1\">\n            <div class=\"card-content white-text\">\n              <span class=\"card-title\">Điều chỉnh thời gian ngủ</span>\n            </div>\n            <div class=\"card-action\">\n             <button class=\"waves-effect waves-light btn\" onclick=\"timeUp()\">Tăng\n              <i class=\"material-icons\">arrow_upward</i></button>\n              <button class=\"waves-effect waves-light btn\" onclick=\"timeDown()\">Giảm\n                <i class=\"material-icons\">arrow_downward</i>\n              </button>\n            </div>\n          </div>\n        </div>\n      </div>\t\n      <br>\n      <!-- Điều chỉnh nhịp tim báo thức -->\n      <div class=\"center\">\n        <div class=\"col s12 m6\">\n          <div class=\"card blue-grey darken-1\">\n            <div class=\"card-content white-text\">\n              <span class=\"card-title\">Điều chỉnh nhịp tim báo thức</span>\n            </div>\n            <div class=\"card-action\">\n             <button class=\"waves-effect waves-light btn\" onclick=\"avgUp()\">Tăng\n               <i class=\"material-icons\">arrow_upward</i>\n             </button>\n             <button class=\"waves-effect waves-light btn\" onclick=\"avgDown()\">Giảm\n               <i class=\"material-icons\">arrow_downward</i>\n             </button>\n           </div>\n         </div>\n       </div>\n     </div>\t\n     <br>\n     <!-- Chế độ nhạc -->\n     <div class=\"center\">\n      <div class=\"col s12 m6\">\n        <div class=\"card blue-grey darken-1\">\n          <div class=\"card-content white-text\">\n            <span class=\"card-title\">Chế độ nhạc</span>\n          </div>\n          <div class=\"card-action\">\n           <button class=\"waves-effect waves-light btn\" onclick=\"turnOn()\">Bật\n            <i class=\"material-icons\">power_settings_new</i></button>\n            <button class=\"waves-effect waves-light btn\" onclick=\"turnOff()\">Tắt\n             <i class=\"material-icons\">power_settings_new</i></button>\n           </div>\n         </div>\n       </div>\n     </div>\t\n     <br>\n     <form action=\"http://baothuctudong.com\">\n      <button class=\"btn waves-effect waves-light\" name=\"action\">Quay trở lại trang chính\n        <i class=\"material-icons home\">home</i>\n      </button>\n    </form>\n    <script>\n     function Tat() {\n       var xhttp = new XMLHttpRequest();\n       xhttp.open('GET', '/Tat.html', true);\n       xhttp.send();\n     }\n     function timeUp() {\n       var xhttp = new XMLHttpRequest();\n       xhttp.open('GET', '/timeUp.html', true);\n       xhttp.send();\n     }\n     function timeDown() {\n       var xhttp = new XMLHttpRequest();\n       xhttp.open('GET', '/timeDown.html', true);\n       xhttp.send();\n     }\n     function avgUp() {\n       var xhttp = new XMLHttpRequest();\n       xhttp.open('GET', '/hrUp.html', true);\n       xhttp.send();\n     }\n     function avgDown() {\n       var xhttp = new XMLHttpRequest();\n       xhttp.open('GET', '/hrDown.html', true);\n       xhttp.send();\n     }\n     function turnOn() {\n       var xhttp = new XMLHttpRequest();\n       xhttp.open('GET', '/batNhac.html', true);\n       xhttp.send();\n     }\n     function turnOff() {\n       var xhttp = new XMLHttpRequest();\n       xhttp.open('GET', '/tatNhac.html', true);\n       xhttp.send();\n     }\n     function ToneMode1() {\n      var xhttp = new XMLHttpRequest();\n      xhttp.open('GET', '/tone1.html', true);\n      xhttp.send();\n    }\n    function ToneMode2() {\n      var xhttp = new XMLHttpRequest();\n      xhttp.open('GET', '/tone2.html', true);\n      xhttp.send();\n    }\n    function mode1() {\n      var xhttp = new XMLHttpRequest();\n      xhttp.open('GET', '/bat1.html', true);\n      xhttp.send();\n    }\n    function mode2() {\n      var xhttp = new XMLHttpRequest();\n      xhttp.open('GET', '/bat2.html', true);\n      xhttp.send();\n    }\n    function Form() {\n    }\n  </script>\n</body>\n</html>"};
#define REPORTING_PERIOD_MS     1000
#define LED_PIN 13
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#include <SoftwareSerial.h>
#define ARDUINO_RX D5
#define ARDUINO_TX D6

#define FIREBASE_HOST "thiet-bi-ho-tro-giac-ngu.firebaseio.com"
#define FIREBASE_AUTH "F5imijFRow1RXDAYuegsZb6Uh1QLAfotCwmWtKzP"
SoftwareSerial mp3(ARDUINO_RX, ARDUINO_TX);
static int8_t Send_buf[8] = {0};
static uint8_t ansbuf[10] = {0};
String mp3Answer;
String Time;
String Date;
#define QUOTE(...) #__VA_ARGS__
#define CMD_NEXT_SONG     0X01
#define CMD_PREV_SONG     0X02
#define CMD_PLAY_W_INDEX  0X03
#define CMD_VOLUME_UP     0X04
#define CMD_VOLUME_DOWN   0X05
#define CMD_SET_VOLUME    0X06
#define CMD_SNG_CYCL_PLAY 0X08
#define CMD_SEL_DEV       0X09
#define CMD_SLEEP_MODE    0X0A
#define CMD_WAKE_UP       0X0B
#define CMD_RESET         0X0C
#define CMD_PLAY          0X0D
#define CMD_PAUSE         0X0E
#define CMD_PLAY_FOLDER_FILE 0X0F
#define CMD_STOP_PLAY     0X16
#define CMD_FOLDER_CYCLE  0X17
#define CMD_SHUFFLE_PLAY  0x18
#define CMD_SET_SNGL_CYCL 0X19
#define CMD_SET_DAC 0X1A
#define DAC_ON  0X00
#define DAC_OFF 0X01
#define CMD_PLAY_W_VOL    0X22
#define CMD_PLAYING_N     0x4C
#define CMD_QUERY_STATUS      0x42
#define CMD_QUERY_VOLUME      0x43
#define CMD_QUERY_FLDR_TRACKS 0x4e
#define CMD_QUERY_TOT_TRACKS  0x48
#define CMD_QUERY_FLDR_COUNT  0x4f
#define DEV_TF            0X02
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
MAX30105 particleSensor;
ESP8266WebServer sv(80);
MPU6050 mpu;
WiFiUDP u;
WiFiUDP ntpUDP;
WorkScheduler *ReadHRScheduler;
//----------------phục hồi i2c--------------
int I2C_ClearBus() {
#if defined(TWCR) && defined(TWEN)
  TWCR &= ~(_BV(TWEN));
#endif
  pinMode(SDA_PIN, INPUT_PULLUP);
  pinMode(SCL_PIN, INPUT_PULLUP);
  delay(2500);
  boolean SCL_LOW = (digitalRead(SCL_PIN) == LOW);
  if (SCL_LOW) {
    return 1;
  }
  boolean SDA_LOW = (digitalRead(SCL_PIN) == LOW);
  int clockCount = SCL_PIN;
  while (SDA_LOW && (clockCount > 0)) {
    clockCount--;
    pinMode(SCL_PIN, INPUT);
    pinMode(SCL_PIN, OUTPUT);
    delayMicroseconds(10);
    pinMode(SCL_PIN, INPUT);
    pinMode(SCL_PIN, INPUT_PULLUP);
    delayMicroseconds(10);
    SCL_LOW = (digitalRead(SCL_PIN) == LOW);
    int counter = SCL_PIN;
    while (SCL_LOW && (counter > 0)) {
      counter--;
      delay(100);
      SCL_LOW = (digitalRead(SCL_PIN) == LOW);
    }
    if (SCL_LOW) {
      return 2;
    }
    SDA_LOW = (digitalRead(SDA_PIN) == LOW);
  }
  if (SDA_LOW) {
    return 3;
  }
  pinMode(SDA_PIN, INPUT);
  pinMode(SDA_PIN, OUTPUT);
  delayMicroseconds(10);
  pinMode(SDA_PIN, INPUT);
  pinMode(SDA_PIN, INPUT_PULLUP);
  delayMicroseconds(10);
  pinMode(SDA_PIN, INPUT);
  pinMode(SCL_PIN, INPUT);
  return 0;
} //----------------phục hồi i2c--------------
void i2cClear() {
  //------------phục hồi i2c-----------------
  int rtn = I2C_ClearBus();
  if (rtn != 0) {
    Serial.println(F("I2C bus error. Could not clear"));
    if (rtn == 1) {
      Serial.println(F("SCL clock line held low"));
    } else if (rtn == 2) {
      Serial.println(F("SCL clock line held low by slave clock stretch"));
    } else if (rtn == 3) {
      Serial.println(F("SDA data line held low"));
    }
  } else {
    Wire.begin();
  }
  Serial.println("setup finished");
  //------------phục hồi i2c-----------------
}
void setup()
{
  i2cClear();
  Serial.begin(115200);
  mp3.begin(9600);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  sendCommand(CMD_SEL_DEV, DEV_TF);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  //-----------kết nối wifi-------------
  {
    Serial.begin(115200);
    sv.on("/", [] {
      if (sv.hasArg("ssid") && sv.hasArg("sspass") || sv.hasArg("uid"))
      {
        String tmp = sv.arg("ssid") + "\n" + sv.arg("sspass");
        String uidSto = sv.arg("uid") + "\n";
        SPIFFS.begin();
        File f = SPIFFS.open("wifi", "w");
        if (f) {
          f.write((const uint8_t*)tmp.c_str(), tmp.length());
        }
        f.close();
        File fnew = SPIFFS.open("data", "w");
        if (fnew) {
          fnew.write((const uint8_t*)uidSto.c_str(), uidSto.length());
          }
        fnew.close();
        sv.send(200, "text/plain; charset=utf-8", "Đã thiết lập xong");
        ESP.reset();
      } else{
        sv.send(200, "text/html", QUOTE(

     <meta charset = 'utf-8'>
     <html>
     <head>
      <title>Setup</title>
      <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/materialize/1.0.0/css/materialize.min.css">
      <link href="https://fonts.googleapis.com/icon?family=Material+Icons" rel="stylesheet">
      <link rel="icon" type="image/pmg" href="https://lh6.googleusercontent.com/H5j1nkrA-y_UV1-AVuaLIP3K3NIicd1wupsy22-I-9qPlS1ADOqdujQ0F0i3UXHPTe4Dp8RU-ud-NPGrt-Dj=w1326-h627-rw">
      <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css">
      <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>
      <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/js/bootstrap.min.js"></script>
      <script>
        $(document).ready(function(){
          $('[data-toggle="popover"]').popover();   
        });
      </script>
      <style>
        .center {
          width: 800px;
          margin: auto;
          padding: 10px;
        }
      </style>
     </head>
     <body class="center">
      <div class="card-panel teal lighten-2 center white-text">Trung tâm điều khiển thiết bị</div>
      <br>
      <div class="card-title teal lighten-2 center white-text">Cài đặt Wi-Fi</div>
      <div class="center">
        <div class="col s12 m6">
          <div class="card blue-grey darken-1">
            <div class="card-content white-text">
              <span class="card-title">Chọn Wi-Fi</span>
            </div>

            <form action = './'>
              <label class="input-field active left white-text" for="wfname">Nhập SSID của Wi-Fi</label>
              <input 
              class="white-text" type = 'text' name = 'ssid' id = 'wfname' />
              <br><br>
              <label class="input-field active left white-text" for="wfname">Mật khẩu</label>
              <input
              class="white-text" type = 'password' name = 'sspass' />

              <br><br>
              <input class="waves-effect waves-light btn" type = 'submit' value = 'OK'>
            </form>
          </form>
        </div>
      </div>
     </div>
     <div class="center">
      <div class="col s12 m6">
        <div class="card blue-grey darken-1"> 
          <div class="card-content white-text">
            <span class="card-title">Danh sách Wifi:</span>
            <div id = 'listWifi'>
            </div>
          </div>
        </div>  
      </div>
      <div class="card-panel teal lighten-2">
        <a class="white-text left" data-toggle="popover" title="Mã UID là gì? Ở đâu?" data-content="Mã UID là mã định danh người dùng. Mã UID được đặt ở phần thông tin người dùng trong trang web baothuctudong.com">
          <p>Mã UID</p>
        </a>
        <i class="material-icons left white-text" data-toggle="popover" title="Mã UID là gì? Ở đâu?" data-content="Mã UID là mã định danh người dùng. Mã UID được đặt ở phần thông tin người dùng trong trang web baothuctudong.com">help
        </i>  
        <form action = './'>
        <input class="input-field active white-text" type='text' name='uid' id='uidd'>
      </input>
        <input type="submit" class="waves-effect waves-light btn" value="OK"></input>
      </form>
      </div>
     </body>
     <script>
      window.onload = getData;
      function getData() {

        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
          if (this.readyState == 4 && this.status == 200) {

            var resText = xhttp.responseText.split('|');
            var tmp = '';
            for (i = 0; i < resText.length; i++) {
              tmp = tmp + '<div id = ' + i + ' onclick="setUsr(this)">' + resText[i] +  '</div>' ;
            }
            document.getElementById('listWifi').innerHTML = tmp;
            getData();
          }
        };
        xhttp.open('GET', '/listWifi', true);
        xhttp.send();

      };
      function setUsr(divObj) {
        document.getElementById('wfname').value = divObj.innerHTML;
      }
     </script>
     </html>
        ));
      }

    });
    sv.on("/listWifi", [] {
      String list;
      int n = WiFi.scanNetworks();
      if (n == 0) {
        list = "no networks found";
      } else {
        for (int i = 0; i < n; ++i) {
          list += String(WiFi.SSID(i)) + "|";

        }
      }
      sv.send(200, "text/html", list);

    });
    sv.begin();
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("thietbibaothuctudong", "12345678");
    NTP.begin (ntpServer, timeZone, true, minutesTimeZone);
    String id = "";
    String pass = "";
    uid = "";
    SPIFFS.begin();
    File f = SPIFFS.open("wifi", "r");
    if (f) {
      id = f.readStringUntil('\n');
      pass = f.readString();
    }
    f.close();
      File fnew = SPIFFS.open("data", "r");
      if (fnew) {
        Serial.write(fnew.read());
        uid = fnew.readStringUntil('\n');
      }
      else {
        Serial.println("Khong the mo file!");
      }
      fnew.close();
     
    if ( id != "" && pass != "") {
      Serial.println(String("Đang kết nối: ") + id + " | " + pass);
      WiFi.begin(id, pass);
      while (WiFi.status() != WL_CONNECTED )
      {
        delay(100);
        Serial.print(".");
        sv.handleClient();
      }
      Serial.print(String("Kết nối thành công"));
    }

    Serial.print(String("Chưa có dữ liệu wifi"));
  }
  //------------------------------------
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  delay(1);
  Serial.println("Initialize MPU6050");
  while (!mpu.beginSoftwareI2C(SCL_PIN, SDA_PIN, MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
  checkSettings();
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  pinMode(D3, INPUT);
  Serial.println(WiFi.localIP());
  sv.begin();
  sv.on("/index", handleRoot);
  sv.on("/style.css", [] {
    sv.send(200, "text/html", readData("style.css"));
  });
  sv.on("/script.js", [] {
    sv.send(200, "text/html", readData("script.js"));
  });
  sv.on("/hrUp.html", [] {
    Avg += 1;
    sv.send(200, "text/html", " ");
  });
  sv.on("/hrDown.html", [] {
    Avg -= 1;
    sv.send(200, "text/html", " ");
  });
  sv.on("/timeDown.html", [] {
    tg -= 30;
    sv.send(200, "text/html", " ");
  });
  sv.on("/timeUp.html", [] {
    tg += 30;
    sv.send(200, "text/html", " ");
  });
  sv.on("/Bat1.html", [] {
    nguTrua = 1;
    sv.send(200, "text/html", " ");
  });
  sv.on("/Tat.html", [] {
    nguTrua = 0;
    sv.send(200, "text/html", " ");
  });
  sv.on("/Bat2.html", [] {
    nguTrua = 2;
    sv.send(200, "text/html", " ");
  });
  sv.on("/batNhac.html", [] {
    batnhac = 1;
    Stop = 0;
    sv.send(200, "text/html", " ");
  });
  sv.on("/tatNhac.html", [] {
    batnhac = 0;
    Stop = 0;
    sv.send(200, "text/html", " ");
  });
  sv.on("/tone1.html", [] {
    ToneMode = 1;
    sv.send(200, "text/html", " ");
  });
  sv.on("/tone2.html", [] {
    ToneMode = 2;
    sv.send(200, "text/html", " ");
  });
  sv.on("/upLoad", HTTP_ANY, [] {
    sv.send(200, "text/html",
    "<meta charset= 'utf-8'>"
    "<html>"
    "<title>"
    "Chọn file để gửi lên server"
    "</title>"
    "<br>"
    "<Form method='POST' action= '/upLoad' enctype= 'multipart/form-data'>"
    "<input type= 'file' name = 'chonFile'>"
    "<input type= 'submit' value = 'guiFile'>"
    "</Form>"
           );
  }, [] {
    HTTPUpload& file = sv.upload();

    if (file.status == UPLOAD_FILE_START)
    {
      //   clearData(file.filename.c_str());
    }
    else if (file.status == UPLOAD_FILE_WRITE) {
      saveData(file.filename.c_str(), (const char*)file.buf, file.currentSize);
    }
  });
  ToneMode = 0;
  display.clearDisplay();
}
void loop()
{
  display.clearDisplay();
  unsigned long milli3;
  readHR();
  sv.handleClient();
  String formattedDate = NTP.getTimeDateString();
  int splitT = formattedDate.indexOf(" ");
  dayStamp = formattedDate.substring(0, splitT);
  timeStamp = formattedDate.substring(splitT + 1, formattedDate.length());
  if (millis() - milli3 > 1000) {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0 );
    display.print(WiFi.localIP());
    if (batnhac == 1 && Stop == 0) {
      sendCommand(CMD_PLAY, 0);
    }
    else if (batnhac == 0 && Stop == 0) {
      sendCommand(CMD_PAUSE, 0);
    }
    Stop = 1;
    display.setCursor(30, 50 );
    display.print(dayStamp);
    display.setCursor(0, 10 );
    display.print("Nhip tim: ");
    display.print(beatAvg);
    display.print(" TG: ");
    display.println(tg);
    display.print("Mode: ");
    if (nguTrua == 1 || nguTrua == 2)
      display.print("Trua");
    else
      display.print("Toi");
    display.print(" AVG: ");
    display.println(Avg);
    display.display();
    milli3 = millis();
  }
  Vector rawAccel = mpu.readRawAccel();
  Vector normAccel = mpu.readNormalizeAccel();
  ax = normAccel.XAxis;
  ay = normAccel.YAxis;
  az = normAccel.ZAxis;
  Started = true;
  ledMode();
  if (millis() - milli2 > 60000 && Started == true) {
    tg -= 1;
    milli2 = millis();
  }
  if (tg < 60 && beatAvg > Avg)
    Tones();
  if (millis() - milli > 60000 && beatAvg > 20 && Started == true) {
    milli = millis();
    String Date_Log_Heartrate = "/" + uid + "/" + String(timeStamp) + "/" + "HeartRate" + "/" + String(dayStamp);
    Firebase.setFloat(Date_Log_Heartrate, beatAvg);
    String Date_Log_Ax = "/" + uid + "/" + String(timeStamp) + "/" + "Ax" + "/" + String(dayStamp);
    Firebase.setFloat(Date_Log_Ax, ax);
  }
}
void handleRoot() {
  sv.send_P(200, "text/html", index_html);
} 
void Tones() {
  if (ToneMode == 1 || ToneMode == 0) {
      int buzzerPin = D4;
    {
      tone(buzzerPin, 150);
      delay(500);
      noTone(buzzerPin);
      tone(buzzerPin, 250);
      delay(500);
      noTone(buzzerPin);
    }
    noTone(buzzerPin);
    delay(1000);
  }
  else if (ToneMode == 2) {
    sendCommand(CMD_PLAY, 0);
  }
}
void ledMode() {
  digitalWrite(ledc, LOW);
  digitalWrite(ledx, LOW);
  if (beatAvg >= 55) {
    digitalWrite(ledx, HIGH);
    digitalWrite(ledc, LOW);
  }
  else {
    digitalWrite(ledc, HIGH);
    digitalWrite(ledx, LOW) ;
  }
}
void checkSettings()
{
  Serial.println();
  Serial.print(" * Sleep Mode:            ");
  Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");
  Serial.print(" * Clock Source:          ");
  switch (mpu.getClockSource())
  {
    case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
    case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
    case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
    case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
  }
  Serial.print(" * Accelerometer:         ");
  switch (mpu.getRange())
  {
    case MPU6050_RANGE_16G:            Serial.println("+/- 16 g"); break;
    case MPU6050_RANGE_8G:             Serial.println("+/- 8 g"); break;
    case MPU6050_RANGE_4G:             Serial.println("+/- 4 g"); break;
    case MPU6050_RANGE_2G:             Serial.println("+/- 2 g"); break;
  }
  Serial.print(" * Accelerometer offsets: ");
  Serial.print(mpu.getAccelOffsetX());
  Serial.print(" / ");
  Serial.print(mpu.getAccelOffsetY());
  Serial.print(" / ");
  Serial.println(mpu.getAccelOffsetZ());
  Serial.println();
}
void readHR()
{
  long irValue = particleSensor.getIR();
  if (checkForBeat(irValue) == true)
  {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60 / (delta / 1000.0);
    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
  if (irValue < 5000)
    beatAvg = 0;
}
void ngutrua() {
  unsigned long milliT;
  int timeNguTrua;
  if (nguTrua == 1) {
    if (millis() - milliT > 60000) {
      milliT = millis();
      if (Started == true && beatAvg < 65) {
        timeNguTrua += 1;
      }
    }
    if (timeNguTrua > 30)
      Tones();
  }
  else if (nguTrua == 2) {
    if (tg > 90) {
      if (millis() - milliT > 60000) {
        tg -= 1;
        milliT = millis();
        if (Started == true && tg < 60 && beatAvg > Avg) {
          Tones();
        }
      }
      else if (tg == 30 || tg == 60) {
        if (millis() - milliT > 60000) {
          tg -= 1;
          milliT = millis();
          if (tg == 0)
            Tones();
        }
      }
    }
  }
}
  void sendCommand(int8_t command, int16_t dat)
  {
    delay(20);
    Send_buf[0] = 0x7e;
    Send_buf[1] = 0xff;
    Send_buf[2] = 0x06;
    Send_buf[3] = command;
    Send_buf[4] = 0x01;
    Send_buf[5] = (int8_t)(dat >> 8);
    Send_buf[6] = (int8_t)(dat);
    Send_buf[7] = 0xef;   //
    Serial.print("Sending: ");
    for (uint8_t i = 0; i < 8; i++)
    {
      mp3.write(Send_buf[i]) ;
      Serial.print(sbyte2hex(Send_buf[i]));
    }
    Serial.println();
  }
  String sbyte2hex(uint8_t b)
  {
    String shex;
    shex = "0X";
    if (b < 16) shex += "0";
    shex += String(b, HEX);
    shex += " ";
    return shex;
  }
