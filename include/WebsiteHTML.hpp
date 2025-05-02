#ifndef WEBSITE_HTML_HPP
#define WEBSITE_HTML_HPP

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
          
         const mac = document.getElementById('macDisplay').innerText.replace("MAC: ", "");
  
       const xhr = new XMLHttpRequest();
       xhr.open("GET", `/led?hue=${hue}&sat=${sat}&bright=${bright}&mode=${mode}&mac=${mac}`, true);
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

#endif
