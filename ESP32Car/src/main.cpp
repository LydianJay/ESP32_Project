#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Wire.h> 
#include "MotorDriver.h"


const char* SSID = "ALMA";
const char* PASSWORD = "(EVIL)(ALMA)(1322)";

const char* webPage = "<html> <head> <title>Car Controller</title> </head> <body> <div class = 'container'> <div class = 'arrow'> <button class = 'up'>↑</button> <button class = 'left'>←</button> <button class = 'rigth'>→</button> <button class = 'down'>↓</button> </div> <div class = 'slider'> <input type='range' min='0' max='255' value='50' class='slider' id = 'sliderVal'></input> <p>Speed:</p> <p id ='speedVal'>0<p> </div> <div class = 'metric'> <p> Current State </p> <p class = 'cState'>IDLE</p> </div> </div> </body> <style> body { display: flex; justify-content: center; align-items: center; height: 100vh; background-color: dimgray; } .container{ display: inline-block; background-color: blue; } .arrow{ color: red; background-color: aqua; height: 65vh; width: 65vh; display: flex; flex-direction: column; justify-content: space-between; } .up, .left, .rigth, .down { padding: 18px; font-size: 400%; width: 30%; height: 30%; } .up { align-self: center; } .left { align-self: center; transform: translate(-110%, 50%); } .rigth { align-self: center; transform: translate(110%, -50%); } .down { align-self: center; } .slider{ display: flex; font-size: 30px; flex-direction: row; justify-content:space-evenly; } .metric{ display: flex; background-color: aquamarine; flex-direction: row; color: green; justify-content:space-evenly; font-size: 30px; height: 10vh; } </style> <script> const slider = document.getElementById('sliderVal'); const speedVal = document.getElementById('speedVal'); let webSocket; slider.addEventListener('input', function() { speedVal.textContent = slider.value; webSocket.send(slider.value); }); const upButton = document.querySelector('.up'); const leftButton = document.querySelector('.left'); const rightButton = document.querySelector('.right'); const downButton = document.querySelector('.down'); upButton.addEventListener('click', function() { // Send the value for the up button webSocket.send(0); }); leftButton.addEventListener('click', function() { // Send the value for the left button webSocket.send(1); }); rightButton.addEventListener('click', function() { // Send the value for the right button webSocket.send(2); }); downButton.addEventListener('click', function() { // Send the value for the down button webSocket.send(3); }); window.onload = function(event) { webSocket = new WebSocket('ws://' + window.location.hostname + ':81/'); } </script></html>";



WebServer webServer(80);
WebSocketsServer webSocket = WebSocketsServer(81);
void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length);
MotorDriver motorDriver;

MotorPinInfo motorPins = {
  25, 26, // motor a
  27, 14, // motor b
  33, // pwm
  32, 34
};

void setup() {
  Serial.begin(115200);
  motorDriver.begin(motorPins, false, false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  Serial.println("Connecting...");
  while(WiFi.status() != WL_CONNECTED){
    delay(300);
  }

  Serial.println(WiFi.localIP());

  webServer.on("/", []() {                               // define here wat the webserver needs to do
    webServer.send(200, "text/html", webPage);           //    -> it needs to send out the HTML string "webpage" to the client
  });
  webServer.begin();


  webSocket.begin();                                  // start websocket
  webSocket.onEvent(webSocketEvent);

}

void loop() {
  webServer.handleClient();
  webSocket.loop();
}



void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length) {      
  
  
  switch (type) {                                     
    
    case WStype_TEXT:                                 
      uint8_t data = payload[0];
      Serial.println(data);
      if(data > 3){
        uint8_t speed = data;
        motorDriver.setSpeed(speed);
      }
      else{
        switch(data){
          case 0:
            motorDriver.move(MotorMove::FWRD);
          break;
          case 1:
            motorDriver.move(MotorMove::LFT);
          break;
            
          case 2:
            motorDriver.move(MotorMove::RGHT);
          break;
          case 3:
            motorDriver.move(MotorMove::BCKWRD);
          break;
          default:
            motorDriver.move(MotorMove::STOP);
          break;
        }
      }

      Serial.print("Data Size: "); Serial.println(length);
      for(int i = 0; i < length; i++){
        Serial.print("Index: "); Serial.print(i); Serial.print("  ");
        Serial.print("Data Value: "); Serial.println(payload[i]);
      }

      break;
  }
}