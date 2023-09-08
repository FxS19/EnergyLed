#include <sys/pgmspace.h>
#pragma once

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <meta charset="UTF-8">
    <title>ESP Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
    <style>
    html {
      font-family: Arial, Helvetica, sans-serif;
      text-align: center;
    }
    h1 {
      font-size: 1.8rem;
      color: white;
    }
    h2{
      font-size: 1.5rem;
      font-weight: bold;
      color: #143642;
    }
    .topnav {
      overflow: hidden;
      background-color: #143642;
    }
    body {
      margin: 0;
    }
    .content {
      padding: 30px;
      max-width: 600px;
      margin: 0 auto;
    }
    .card {
      background-color: #F8F7F9;;
      box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
      padding-top:10px;
      padding-bottom:20px;
    }
    button {
      padding: 15px 50px;
      font-size: 24px;
      text-align: center;
      outline: none;
      color: #fff;
      background-color: #0f8b8d;
      border: none;
      border-radius: 5px;
      -webkit-touch-callout: none;
      -webkit-user-select: none;
      -khtml-user-select: none;
      -moz-user-select: none;
      -ms-user-select: none;
      user-select: none;
      -webkit-tap-highlight-color: rgba(0,0,0,0);
    }
    /*.button:hover {background-color: #0f8b8d}*/
    button:active {
      background-color: #0f8b8d;
      box-shadow: 2 2px #CDCDCD;
      transform: translateY(2px);
    }
    .state {
      font-size: 1.5rem;
      color:#8c8c8c;
      font-weight: bold;
    }
    </style>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  </head>
  <body>
    <div class="topnav">
      <h1>Power</h1>
    </div>
    <div class="content">
      <div class="card">
        <h2>Aktueller Überschuss</h2>
        <p class="state"><span id="power">???</span> W</p>
        <button onclick="location.href='/settings'" type="button">Settings</button>
      </div>
    </div>
    <script>
      var gateway = `ws://${window.location.hostname}/ws`;
      var websocket;
      window.addEventListener('load', onLoad);
      function initWebSocket() {
        console.log('Trying to open a WebSocket connection...');
        websocket = new WebSocket(gateway);
        websocket.onopen    = onOpen;
        websocket.onclose   = onClose;
        websocket.onmessage = onMessage; // <-- add this line
      }
      function onOpen(event) {
        console.log('Connection opened');
      }
      function onClose(event) {
        console.log('Connection closed');
        setTimeout(initWebSocket, 2000);
      }
      function onMessage(event) {
        document.getElementById('power').innerHTML = event.data;
      }
      function onLoad(event) {
        initWebSocket();
      }
    </script>
  </body>
</html>
)rawliteral";

const char kiosk_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <meta charset="UTF-8">
    <title>Power Gallery</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
      body {
        margin: 0;
        padding: 0;
        display: flex;
        justify-content: center;
        align-items: center;
        height: 100vh;
        overflow: hidden;
        background-color: #000;
      }
      
      .text-container {
        display: flex;
        justify-content: center;
        align-items: center;
        overflow: hidden;
      }
      
      .text {
        font-size: 24vw; /* 30% of viewport width */
        white-space: nowrap;
        color: #fff;
      }
    </style>
  </head>
  <body>
    <div class="text-container">
      <p id="power" class="text">??? W</p>
    </div>
    <script>
      var gateway = `ws://${window.location.hostname}/ws`;
      var websocket;
      var reconnectInterval = 0;
      var sendInterval = 0;
      window.addEventListener('load', onLoad);
      function initWebSocket() {
        console.log('Trying to open a WebSocket connection...');
        websocket = new WebSocket(gateway);
        websocket.onopen    = onOpen;
        websocket.onclose   = onClose;
        websocket.onmessage = onMessage;
        websocket.onerror   = onError;
      }
      function onOpen(event) {
        console.log('Connection opened');
        clearInterval(reconnectInterval);
        sendInterval = setInterval(()=>websocket.send("Hi"), 5000);
      }
      function onClose(event) {
        console.log('Connection closed');
        document.getElementById('power').innerHTML = "ERROR";
        clearInterval(sendInterval);
        clearInterval(reconnectInterval);
        reconnectInterval = setInterval(initWebSocket, 10000);
      }
      function onError(event) {
        console.log('Error!!!');
      }
      function onMessage(event) {
        document.getElementById('power').innerHTML = event.data + " W";
      }
      function onLoad(event) {
        initWebSocket();
      }
    </script>
  </body>
</html>
)rawliteral";

const char settings_html[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8">
    <title>Einstellungen</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
  <style>
    html {
      font-family: Arial, Helvetica, sans-serif;
      text-align: center;
    }
    h1 {
      font-size: 1.8rem;
      color: white;
    }
    h2{
      font-size: 1.5rem;
      font-weight: bold;
      color: #143642;
    }
    .topnav {
      overflow: hidden;
      background-color: #143642;
    }
    body {
      margin: 0;
    }
    .content {
      padding: 30px;
      max-width: 600px;
      margin: 0 auto;
    }
    .card {
      background-color: #F8F7F9;;
      box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
      padding-top:10px;
      padding-bottom:20px;
    }
    button {
      padding: 15px 50px;
      font-size: 24px;
      text-align: center;
      outline: none;
      color: #fff;
      background-color: #0f8b8d;
      border: none;
      border-radius: 5px;
      -webkit-touch-callout: none;
      -webkit-user-select: none;
      -khtml-user-select: none;
      -moz-user-select: none;
      -ms-user-select: none;
      user-select: none;
      -webkit-tap-highlight-color: rgba(0,0,0,0);
    }
    /*.button:hover {background-color: #0f8b8d}*/
    button:active {
      background-color: #0f8b8d;
      box-shadow: 2 2px #CDCDCD;
      transform: translateY(2px);
    }
    .state {
      font-size: 1.5rem;
      color:#8c8c8c;
      font-weight: bold;
    }
    </style>
    <script>
      function confirmAndSubmit() {
        var confirmed = confirm("Durch das Speichern der Einstellungen werden diese sofort übernommen und das Gerät neu gestartet. Falls keine WLAN-Verbindung hergestellt werden kann, werden die Werkseinstellungen geladen.\n\nMöchtest du fortfahren?");
        if (confirmed) {
          const fields = document.querySelectorAll('.input-field');
          fields.forEach((field) => {
            const key = field.getAttribute('data-key');
            const value = field.value;
            if (value != ""){
              const xhr = new XMLHttpRequest();
              xhr.open('POST', '/api/settings?key=' + key + '&value=' + encodeURIComponent(value), true);
              xhr.send();
            }
          });
          setTimeout(()=>{
            const xhr = new XMLHttpRequest();
            xhr.open('GET', '/api/reboot', true);
            xhr.send();
          }, 4000);
          alert("Gerät startet neu und ist unter den neuen Einstellungen erreichbar. Falls ein Fehler auftritt wird ein WLAN <EnergyLED> geöffnet (IP 192.168.4.1)");
        }
      }
    </script>
  </head>
  <body>
    <div class="topnav">
      <h1>Settings</h1>
    </div>
    <div class="content">
      <div class="card">
        <form id="settingsForm" action="/api/settings" method="post">
          [[SETTINGSFORM]]
          <br>
          <button class="submit-button" type="button" onclick="confirmAndSubmit()">Senden</button>
        </form>
        <br>
        <button onclick="location.href='/'" type="button">Home</button>
      </div>
    </div>
  </body>
</html>
)HTML";