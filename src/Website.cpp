#include "Website.hpp"
#include "LEDHandler.hpp"


// Replace with your network credentials HERE ->
const char* SSID = "DJ";
const char* PASSword = "daejung123";

const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE HTML><html>
  <head>
    <title>Piano Visualizer Controller</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body {
        font-family: Arial, sans-serif;
        background-color: #121212;
        color: #ffffff;
        text-align: center;
        padding: 20px;
      }
      h2 {
        font-size: 2.5rem;
        margin-bottom: 20px;
      }
      .slider-container {
        margin: 30px 0;
      }
      label {
        font-size: 1.2rem;
      }
      input[type=range] {
        width: 80%;
        margin-top: 10px;
        height: 20px;
        -webkit-appearance: none;
        background: transparent;
      }
      input[type=range]::-webkit-slider-thumb {
        -webkit-appearance: none;
        height: 25px;
        width: 25px;
        border-radius: 50%;
        background: #ffffff;
        cursor: pointer;
      }
      input[type=range]#hue::-webkit-slider-runnable-track {
        background: linear-gradient(to right, red, yellow, lime, cyan, blue, magenta, red);
        height: 10px;
        border-radius: 5px;
      }
      input[type=range]#saturation::-webkit-slider-runnable-track {
        background: linear-gradient(to right, gray, #00ffff);
        height: 10px;
        border-radius: 5px;
      }
      input[type=range]#brightness::-webkit-slider-runnable-track {
        background: linear-gradient(to right, black, white);
        height: 10px;
        border-radius: 5px;
      }
      select {
        background-color: #1e1e1e;
        color: #ffffff;
        font-size: 1rem;
        padding: 5px;
        border: 1px solid #ffffff;
        border-radius: 5px;
      }
      #colorPreview {
        width: 100px;
        height: 100px;
        margin: 30px auto;
        border-radius: 50%;
        background-color: hsl(0, 100%, 50%);
        border: 2px solid #fff;
      }
    </style>
  </head>
  <body>
    <h2>Piano Visualizer Controller</h2>
  
    <div class="slider-container">
      <label for="hue">Hue</label><br>
      <input type="range" id="hue" min="0" max="360" value="0" oninput="updateColor()">
    </div>
  
    <div class="slider-container">
      <label for="saturation">Saturation</label><br>
      <input type="range" id="saturation" min="0" max="100" value="100" oninput="updateColor()">
    </div>
  
    <div class="slider-container">
      <label for="brightness">Brightness</label><br>
      <input type="range" id="brightness" min="0" max="100" value="50" oninput="updateColor()">
    </div>
  
    <div class="slider-container">
      <label for="mode">LED Mode</label><br>
      <select id="mode" onchange="sendUpdate()">
        <option value="octave">Octave Colors</option>
        <option value="single">Single Color</option>
        <option value="rainbow">Rainbow Mode</option>
      </select>
    </div>
  
    <div id="colorPreview"></div>
  
    <script>
      function updateColor() {
        const hue = document.getElementById('hue').value;
        const sat = document.getElementById('saturation').value;
        const bright = document.getElementById('brightness').value;
  
        const preview = document.getElementById('colorPreview');
        preview.style.backgroundColor = `hsl(${hue}, ${sat}%, ${bright}%)`;
  
        sendUpdate();
      }
  
      function sendUpdate() {
        const hue = document.getElementById('hue').value;
        const sat = document.getElementById('saturation').value;
        const bright = document.getElementById('brightness').value;
        const mode = document.getElementById('mode').value;
  
        const xhr = new XMLHttpRequest();
        xhr.open("GET", `/led?hue=${hue}&sat=${sat}&bright=${bright}&mode=${mode}`, true);
        xhr.send();
      }
  
      // Set preview on page load
      window.onload = updateColor;
    </script>
  </body>
  </html>
  )rawliteral";
  

// Replaces placeholder with button section in your web page
String processor(const String& var){
    //Serial.println(var);
    if(var == "BUTTONPLACEHOLDER"){
      String buttons = "";
      buttons += "<h4>Turn On LEDs</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"2\" " + outputState(2) + "><span class=\"slider\"></span></label>";
      buttons += "<h4>Turn on Bluetooth</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"4\" " + outputState(4) + "><span class=\"slider\"></span></label>";
      buttons += "<h4>Connect to Piano(s)</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"33\" " + outputState(33) + "><span class=\"slider\"></span></label>";
      return buttons;
    }
    return String();
  }

  String outputState(int output){
    if(digitalRead(output)){
      return "checked";
    }
    else {
      return "";
    }
  }
  
void WebsiteSetup() {
  
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  pinMode(33, OUTPUT);
  digitalWrite(33, LOW);

  // Connect to Wi-Fi
  WiFi.begin(SSID, PASSword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send_P(200, "text/html", index_html, processor);
    request->send(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1;
    String inputMessage2;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());
      
      int pin = inputMessage1.toInt();
      int state = inputMessage2.toInt();

      if (pin == 2) {  // If "Turn On LEDs" button was toggled
          if (state == 1) {
              // Turn on LEDs (example: turn on middle C note, velocity max)
              lightUpLED(60, 127);
          } else {
              // Turn off LED
              turnOffLED(60);
          }
      } else {
          digitalWrite(pin, state);
      }
    }
    else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
    }
    Serial.print("GPIO: ");
    Serial.print(inputMessage1);
    Serial.print(" - Set to: ");
    Serial.println(inputMessage2);
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();
}

