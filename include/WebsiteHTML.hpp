#ifndef WEBSITE_HTML_HPP
#define WEBSITE_HTML_HPP

const char index_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
  <html>
    <head>
      <title>Piano Visualizer</title>
      <style>
        body { font-family: Arial, sans-serif; background-color: #111; color: #eee; text-align: center; }
        .slider-container { margin: 20px auto; width: 80%; }
        input[type=range] { width: 70%; }
        label { display: block; margin-top: 10px; font-size: 18px; }
        .color-preview {
          display: inline-block;
          width: 40px;
          height: 40px;
          border-radius: 50%;
          margin-left: 15px;
          border: 1px solid white;
        }
        button {
          padding: 10px 20px;
          font-size: 16px;
          margin-top: 20px;
          background-color: #444;
          color: white;
          border: none;
          border-radius: 5px;
          cursor: pointer;
        }
        button:hover {
          background-color: #666;
        }
      </style>
    </head>
    <body>
      <h1>Piano Visualizer Control</h1>
  
      <div class="slider-container">
        <label for="hueSlider">Hue</label>
        <input type="range" id="hueSlider" min="0" max="255" value="80" oninput="updatePreview()">
        <span class="color-preview" id="huePreview"></span>
  
        <label for="brightnessSlider">Brightness</label>
        <input type="range" id="brightnessSlider" min="0" max="255" value="100" oninput="updatePreview()">
        <span class="color-preview" id="brightnessPreview"></span>
      </div>
  
      <button onclick="submitChanges()">Submit</button>
  
      <script>
        function hsvToRgb(h, s, v) {
          let f = (n, k = (n + h / 60) % 6) =>
            v - v * s * Math.max(Math.min(k, 4 - k, 1), 0);
          let r = Math.round(f(5) * 255);
          let g = Math.round(f(3) * 255);
          let b = Math.round(f(1) * 255);
          return `rgb(${r}, ${g}, ${b})`;
        }
  
        function updatePreview() {
          let hue = parseInt(document.getElementById("hueSlider").value);
          let brightness = parseInt(document.getElementById("brightnessSlider").value);
  
          let color = hsvToRgb((hue / 255) * 360, 1, brightness / 255);
          document.getElementById("huePreview").style.backgroundColor = color;
          document.getElementById("brightnessPreview").style.backgroundColor = color;
        }
  
        function submitChanges() {
          var hue = document.getElementById("hueSlider").value;
          var brightness = document.getElementById("brightnessSlider").value;
  
          var url = `/send?hue=${hue}&bright=${brightness}`;
  
          fetch(url)
            .then(response => response.json())
            .then(data => console.log("Success:", data))
            .catch(error => console.error("Error:", error));
        }
  
        window.onload = updatePreview;
      </script>
    </body>
  </html>
  )rawliteral";
  
  

#endif
