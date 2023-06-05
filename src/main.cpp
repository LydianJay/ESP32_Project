#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


WebServer server(80);                                 // the server uses port 80 (standard port for websites
WebSocketsServer webSocket = WebSocketsServer(81);    // the websocket uses port 81 (standard port for websockets
LiquidCrystal_I2C lcd(0x27, 16, 2);

constexpr unsigned char RELAY_PIN_1 = 25, RELAY_PIN_2 = 26;


bool relayState1 = false, relayState2 = false;
const char* SSID = "ALMA";
const char* PASSWORD = "(EVIL)(ALMA)(1322)";
const char* webpage = "<html><head> <title>ESP Relay Control</title></head><body> <center> <p class='title'>Relay Status</p> <div class='toggle-wrapper'> <label class='toggle'> <span class='text1'>Relay 1</span> <input type='checkbox' id = 'r1'> <span class='slider'></span> </label> </div> <div class='toggle-wrapper'> <label class='toggle'> <span class='text1'>Relay 2</span> <input type='checkbox' id = 'r2'> <span class='slider'></span> </label> </div> <div class='toggle-wrapper'> <label class='toggle'> <span class='text1'>LCD <br>OFF ON</span> <input type='checkbox' id = 'lcd'> <span class='slider'></span> </label> </div> </center> <style> body { background-color: darkgray; } .text1 { position: relative; color: black; font-size: 30px; } .title { color: red; font-size: 52px; } .toggle-wrapper { display: flex; justify-content: center; margin-top: 20px; } .toggle { position: relative; display: inline-block; block-size: 32px; color: black; cursor: pointer; width: 120px; height: 80px; background-color: rgb(102, 76, 145); border: 1px solid black; } .toggle input[type='checkbox'] { display: none; } .toggle .slider::before { position: absolute; content: ''; height: 40px; width: 60px; left: 0px; bottom: 0px; background-color: green; transition: .4s; } .toggle input[type='checkbox']:checked + .slider::before { transform: translateX(60px); background-color: red; } </style> <script> let Socket; const toggle1 = document.getElementById('r1'); const toggle2 = document.getElementById('r2'); const lcdToggle = document.getElementById('lcd'); toggle1.addEventListener('change', function(){ let sendVal = 0; if(toggle1.checked){ sendVal = 0b00000001; } else{ sendVal = 0b00000000; } Socket.send(sendVal); } ); toggle2.addEventListener('change', function(){ let sendVal = 0; if(toggle2.checked){ sendVal = 0b00000011; } else{ sendVal = 0b00000010; } Socket.send(sendVal); } ); lcdToggle.addEventListener('change', function(){ let sendVal = 0; if(lcdToggle.checked){ sendVal = 0b00001000; } else{ sendVal = 0b00000100; } Socket.send(sendVal); } ); window.onload = function(event) { Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); } </script></body></html>";




void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length);
void setup() {
  Serial.begin(115200);
  lcd.init();
  
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Connecting");
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  digitalWrite(RELAY_PIN_1, HIGH);
  digitalWrite(RELAY_PIN_2, HIGH);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    delay(300);
  }

  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());

  server.on("/", []() {                               // define here wat the webserver needs to do
    server.send(200, "text/html", webpage);           //    -> it needs to send out the HTML string "webpage" to the client
  });
  server.begin();


  webSocket.begin();                                  // start websocket
  webSocket.onEvent(webSocketEvent);



  
}

void loop() {
  server.handleClient();
  webSocket.loop();
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  lcd.setCursor(0,0);
  lcd.print("R1:");
  lcd.print(relayState2 ? "ON" : "OFF");
  lcd.print(" R2:");
  lcd.print(relayState1 ? "ON" : "OFF");
}

void setRelay(uint8_t data){
  if(data & 0b00000010){ 
    if(data & 0b00000001){
      relayState1 = true;
    }
    else{
      relayState1 = false;
    }
  }
  else{
    if(data & 0b00000001){
      relayState2 = true;
    }
    else{
      relayState2 = false;
    }
  }

  
  digitalWrite(RELAY_PIN_1, !relayState1);
  digitalWrite(RELAY_PIN_2, !relayState2);
  updateLCD();
}


void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length) {      // the parameters of this callback function are always the same -> num: id of the client who send the event, type: type of message, payload: actual data sent and length: length of payload
  
  switch (type) {                                     // switch on the type of information sent
    
    case WStype_TEXT:                                 // if a client has sent data, then type == WStype_TEXT
      uint8_t data = payload[0];
      if(data & 0b00000100){
        lcd.display();
        lcd.backlight();
      }
      else if (data & 0b00001000){
        lcd.noDisplay();
        lcd.noBacklight();
        
      }
      else{
        setRelay(data);
      }
      break;
  }
}