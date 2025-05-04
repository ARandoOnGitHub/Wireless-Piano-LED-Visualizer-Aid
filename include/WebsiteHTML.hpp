#ifndef WEBSITE_HTML_HPP
#define WEBSITE_HTML_HPP

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html>
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

    .slider-section {
      margin-bottom: 40px;
    }

    .color-preview {
      width: 100px;
      height: 100px;
      margin: 10px auto 10px;
      border-radius: 50%;
      border: 2px solid white;
    }

    label {
      font-size: 1.2rem;
    }

    input[type=range] {
      width: 80%;
      margin-top: 10px;
      height: 25px;
      -webkit-appearance: none;
      background: transparent;
    }

    input[type=range]::-webkit-slider-thumb {
      -webkit-appearance: none;
      height: 20px;
      width: 20px;
      border-radius: 50%;
      background: #ffffff;
      cursor: pointer;
    }

    #hue::-webkit-slider-runnable-track,
    #incominghue::-webkit-slider-runnable-track {
      background: linear-gradient(to right, red, yellow, lime, cyan, blue, magenta, red);
      height: 10px;
      border-radius: 5px;
    }
    #bluehue::-webkit-slider-runnable-track {
      background: linear-gradient(to right, red, yellow, lime, cyan, blue, magenta, red);
      height: 10px;
      border-radius: 5px;
    }
    #bright::-webkit-slider-runnable-track {
      background: linear-gradient(to right, black, white);
      height: 10px;
      border-radius: 5px;
    }

    button {
      margin-top: 20px;
      padding: 10px 20px;
      font-size: 1rem;
      background-color: #333;
      color: white;
      border: none;
      border-radius: 8px;
      cursor: pointer;
    }

    button:hover {
      background-color: #555;
    }
  </style>
</head>
<body>
  <h2>Piano Visualizer Controller</h2>

  <div class="slider-section">
    <div id="yourPreview" class="color-preview"></div>
    <label for="hue">Your Hue</label><br>
    <input type="range" id="hue" min="0" max="255" value="0" oninput="updateColor()">
  </div>

  <div class="slider-section">
    <div id="incomingPreview" class="color-preview"></div>
    <label for="incominghue">Incoming Hue</label><br>
    <input type="range" id="incominghue" min="0" max="255" value="0" oninput="updateColor()">
  </div>

  <div class="slider-section">
    <div id="bluetoothPreview" class="color-preview"></div>
    <label for="bluehue">Bluetooth Hue</label><br>
    <input type="range" id="bluehue" min="0" max="255" value="0" oninput="updateColor()">
  </div>

  <div class="slider-section">
    <label for="bright">Brightness</label><br>
    <input type="range" id="bright" min="0" max="255" value="150" oninput="updateColor()">
  </div>

  <button onclick="sendUpdate()">Submit</button>

  <div style="margin-top: 50px;">
    <h3>Current ESP32 Device</h3>
    <div class="mac-address" id="macDisplay">MAC: Loading...</div>
  </div>

  <script>
    function hsvToCssHsl(h, s, v) {
      let scaledHue = Math.round((h / 255) * 360);  // Fix for correct preview hue!

      s /= 255;
      v /= 255;

      let l = v * (1 - s / 2);
      let ss = (l === 0 || l === 1) ? 0 : (v - l) / Math.min(l, 1 - l);

      return `hsl(${scaledHue}, ${Math.round(ss * 100)}%, ${Math.round(l * 100)}%)`;
    }

    function updateColor() {
      const hue = parseInt(document.getElementById('hue').value);
      const incoming = parseInt(document.getElementById('incominghue').value);
      const bluetooth = parseInt(document.getElementById('bluehue').value);
      const bright = parseInt(document.getElementById('bright').value);

      document.getElementById("yourPreview").style.backgroundColor = hsvToCssHsl(hue, 255, bright);
      document.getElementById("incomingPreview").style.backgroundColor = hsvToCssHsl(incoming, 255, bright);
      document.getElementById("bluetoothPreview").style.backgroundColor = hsvToCssHsl(bluetooth, 255, bright);
    }



    function sendUpdate() {
      const hue = document.getElementById('hue').value;
      const incominghue = document.getElementById('incominghue').value;
      const bluehue = document.getElementById('bluehue').value;
      const bright = document.getElementById('bright').value;

      const xhr = new XMLHttpRequest();
      xhr.open("GET", `/send?hue=${hue}&incominghue=${incominghue}&bluehue=${bluehue}&bright=${bright}`, true);
      xhr.send();
    }

    function fetchMacAddress() {
      fetch("/mac")
        .then(response => response.text())
        .then(mac => {
          document.getElementById("macDisplay").innerText = `MAC: ${mac}`;
        });
    }

    function fetchNearbyDevices() {
      fetch("/scan")
        .then(response => response.text())
        .then(data => {
          document.getElementById("deviceList").innerText = data;
        });
    }

    window.onload = () => {
      updateColor();
      fetchMacAddress();
    };
  </script>
</body>
</html>
)rawliteral";

#endif
