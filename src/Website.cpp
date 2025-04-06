#include "Website.hpp"
#include "LEDHandler.hpp"
#include "ESP_NOW.hpp"


// Replace with your network credentials HERE ->
// const char* SSID = "IAUSA";
// const char* PASSword = "divinelight18";

IPAddress local_IP(192, 168, 5, 245); 
IPAddress gateway(192, 168, 255, 255);
IPAddress subnet(255, 255, 255, 0);

 uint8_t currentHue=29;
 uint8_t currentBrightness=20;



const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


void setupAP(){
  WiFiManager wm;
  WiFi.mode(WIFI_STA);
  bool res;
  res = wm.autoConnect("AutoConnectAP");
  
  if(!res) {
    Serial.println("Failed to connect");
    // ESP.restart();
} 
else {
    //if you get here you have connected to the WiFi    
    Serial.println("connected...yeey :)");
}

}

String scanNearbyDevices() {
  String result = "";
  int n = WiFi.scanNetworks(false, true); // async = false, show_hidden = true
  if (n == 0) {
    result = "No devices found";
  } else {
    for (int i = 0; i < n; ++i) {
      result += "SSID: " + WiFi.SSID(i);
      result += " | MAC: " + WiFi.BSSIDstr(i);
    
    }
  }
  return result;
}

const char index_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE HTML><html>
  <head>
    <title>Piano Visualizer Controller read</title>
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
  <div class="mac-address" id="macDisplay">MAC: Loading...</div>

    <div id="colorPreview"></div>
  <div style="margin-top: 40px;">
  <h3>Nearby ESP32 Devices</h3>
  <pre id="deviceList">Scanning...</pre>
  <button onclick="fetchNearbyDevices()">Rescan</button>
</div>
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

      function fetchNearbyDevices() {
       fetch("/scan")
    .then(response => response.text())
    .then(data => {
      document.getElementById("deviceList").innerText = data;
    })
    .catch(err => {
      console.error("Scan failed:", err);
    });
}
      function fetchMacAddress() {
       fetch("/mac")
      .then(response => response.text())
      .then(mac => {
       document.getElementById("macDisplay").innerText = `MAC: ${mac}`;
    })
       .catch(err => {
        console.error("Failed to load MAC address:", err);
    });
}
      // Set preview on page load
      // window.onload = updateColor;
      window.onload = () => {
     updateColor();
    fetchMacAddress();
};

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
  
  setupAP();
  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin();
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected!");
  Serial.println(WiFi.localIP());
  if (MDNS.begin("PianoVisualizer")) {
    Serial.println("Access your ESP32 at: http://PianoVisualizer.local");
  }
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send_P(200, "text/html", index_html, processor);
    request->send(200, "text/html", index_html);
    });

    //get Mac Address
  server.on("/mac", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", readMacAddress());
    });
    server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", scanNearbyDevices());
    });

    server.on("/send", HTTP_GET, [](AsyncWebServerRequest *request) {
      String params[] = {"background", "ledmode", "animmode", "yourhue", "bluehue", "bright", "bluetooth"};
      bool backgroundEnabled = false;
    
      // Local vars to safely handle updates before any LED activity
  
      String animMode = "";
      int newHue = currentHue;
      int newBrightness = currentBrightness;
      int BlueHue=1; 
      bool BluetoothBool; 
    
      for (int i = 0; i < sizeof(params) / sizeof(params[0]); i++) {
        if (!request->hasParam(params[i])) continue;
    
        String value = request->getParam(params[i])->value();
    
        if (params[i] == "background") {
          backgroundEnabled = (value == "1" || value == "true");
        }  else if (params[i] == "animmode") {
          animMode = value;
        } else if (params[i] == "yourhue") {
          newHue = constrain(value.toInt(), 0, 255);  // Ensure safe hue
        } else if (params[i] == "bluehue") {
          BlueHue = constrain(value.toInt(), 0, 255);  // Ensure safe brightness
        } else if (params[i] == "bright") {
          newBrightness = constrain(value.toInt(), 0, 255);  // Ensure safe brightness
        } else if (params[i] == "bluetooth") {
          BluetoothBool = (value == "1" || value == "true");
      }
    
      // Apply updates *after* parsing all params
      currentHue = newHue;
      currentBrightness = newBrightness;
    
      // Handle background LED logic
      for (int i = 0; i <= 86; i++) {
        if (backgroundEnabled) {
          lightUpLED(i, 127);
        } else {
          turnOffLED(i);
        }
      }
    }
    
      // Respond once, at the end
      String response = "Data received:\n";
      response += "Background: " + String(backgroundEnabled ? "ON" : "OFF") + "\n";
      response += "Animation Mode: " + animMode + "\n";
      response += "Your Hue: " + String(currentHue) + "\n";
      response += "Bluetooth Hue: " + String(BlueHue) + "\n";
      response += "Brightness: " + String(currentBrightness) + "\n";
      response += "Bluetooth On: " + String(BluetoothBool) + "\n";
    
      request->send(200, "text/plain", response);
      Serial.println(response);
      
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
              // lightUpLED(60, 127);
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
  Serial.println("Starting web server...");
  server.begin();
  Serial.println("Web server started!");
}

