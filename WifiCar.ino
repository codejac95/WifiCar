#include <ArduinoHttpClient.h>
#include <WiFiS3.h>
#include "settings.h"

char ssid[] = SECRET_SSID;
char password[] = SECRET_PASSWORD;

WiFiServer server(80);
IPAddress ip;

const int FL1 = 5;
const int FL2 = 4;
const int RL1 = 3;
const int RL2 = 2;
const int FR1 = 9;
const int FR2 = 8;
const int RR1 = 7;
const int RR2 = 6;
const int HONK = 12;

bool forward = false;
bool backward = false;
bool cLeft = false;
bool cRight = false;

void setup() {
  Serial.begin(9600);
  wifiSetup();

  pinMode(FL1, OUTPUT);
  pinMode(FL2, OUTPUT);
  pinMode(RL1, OUTPUT);
  pinMode(RL2, OUTPUT);
  pinMode(FR1, OUTPUT);
  pinMode(FR2, OUTPUT);
  pinMode(RR1, OUTPUT);
  pinMode(RR2, OUTPUT);
  pinMode(HONK, OUTPUT);

  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  ip = WiFi.localIP();
  Serial.print("IP-Adress: ");
  Serial.println(ip);
  server.begin();
}

void loop() {
   if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi frånkopplad, försöker ansluta igen...");
    wifiSetup(); 
  }
  WiFiClient client = server.available();
  
  if (client) { 
    String request = client.readStringUntil('\r'); 
    client.flush(); 

    if (request.indexOf("/move/forward") != -1) {
      forward = true;
      Serial.println("forward true");
    } else if (request.indexOf("/move/backward") != -1) {
      backward = true;
      Serial.println("backward true");
    } else if (request.indexOf("/move/cLeft") != -1) {
      cLeft = true;
      Serial.println("CLeft true");
    } else if (request.indexOf("/move/cRight") != -1) {
      cRight = true;
      Serial.println("CRight true");
    } else if (request.indexOf("/honk") != -1) {
      honk();
    } else if (request.indexOf("/forward/false") != -1) {
      forward = false;
      Serial.println("forward false");
    } else if (request.indexOf("/backward/false") != -1) {
      backward = false;
      Serial.println("backward false");
    } else if (request.indexOf("/cLeft/false") != -1) {  
      cLeft = false;
       Serial.println("CLefft false");
    } else if (request.indexOf("/cRight/false") != -1) { 
      cRight = false;
       Serial.println("CRight false");
    } 

    if(forward && cLeft) {
      moveLeftForward();
    } else if (forward && cRight) {
      moveRightForward();
    } else if (backward && cLeft) {
      moveLeftBackward();
    } else if(backward && cRight) {
      moveRightBackward();
    } else if(forward) {
      moveForward();
    } else if (backward) {
      moveBackward();
    } else if (cLeft) {
      moveCLeft();
    } else if (cRight) {
      moveCRight();
    } else {
      stopMotors();
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.println("<html><body>");
    client.println("<style>");
    client.println("  .control-container {");
    client.println("    display: flex;");
    client.println("    justify-content: center;");
    client.println("    align-items: center;");
    client.println("    height: 100vh;");
    client.println("    flex-direction: column;");
    client.println("  }");
    client.println("  .arrow-grid {");
    client.println("    display: grid;");
    client.println("    grid-template-columns: 1fr 1fr 1fr;");
    client.println("    grid-template-rows: 1fr 1fr 1fr;");
    client.println("    gap: 20px;");
    client.println("    justify-items: center;");
    client.println("    align-items: center;");
    client.println("  }");
    client.println("  .arrow-grid button {");
    client.println("    width: 220px;");  
    client.println("    height: 220px;");
    client.println("    font-size: 40px;"); 
    client.println("    cursor: pointer;");
    client.println("    padding: 20px;"); 
    client.println("    border-radius: 10px;"); 
    client.println("  }");
    client.println("  .honk-button {");
    client.println("    grid-column: 2;");
    client.println("    grid-row: 2;");
    client.println("    width: 240px;");
    client.println("    height: 240px;"); 
    client.println("    font-size: 42px;"); 
    client.println("  }");
    client.println("</style>");

    client.println("<div class='control-container'>");
    client.println("<div class='arrow-grid'>");
    client.println("  <div></div>");
    client.println("  <button ontouchstart=\"sendRequest('/move/forward')\"ontouchend=\"sendRequest('/forward/false')\">&#9650;</button>");
    client.println("  <div></div>"); 
    client.println("  <button ontouchstart=\"sendRequest('/move/cLeft')\"ontouchend=\"sendRequest('/cLeft/false')\">&#9664;</button>");
    client.println("  <button class='honk-button' ontouchstart=\"sendRequest('/honk')\">TUT!!</button>");
    client.println("  <button ontouchstart=\"sendRequest('/move/cRight')\"ontouchend=\"sendRequest('/cRight/false')\">&#9654;</button>"); 
    client.println("  <div></div>"); 
    client.println("  <button ontouchstart=\"sendRequest('/move/backward')\"ontouchend=\"sendRequest('/backward/false')\">&#9660;</button>");
    client.println("  <div></div>");
    client.println("</div>");
    client.println("</div>");

    client.println("<script>");
    client.println("function sendRequest(url) {");
    client.println("  var xhr = new XMLHttpRequest();");
    client.println("  xhr.open('GET', url, true);");
    client.println("  xhr.send();");
    client.println("}");
    client.println("document.addEventListener('touchstart', function(event) {");
    client.println("  event.preventDefault();");
    client.println("}, { passive: false });"); 
    client.println("</script>");
    client.println("</body></html>");
    client.stop();
  }
}

void moveForward() {
  digitalWrite(FL1, HIGH);
  digitalWrite(FL2, LOW);
  digitalWrite(FR1, HIGH);
  digitalWrite(FR2, LOW);
  digitalWrite(RL1, HIGH);
  digitalWrite(RL2, LOW);
  digitalWrite(RR1, HIGH);
  digitalWrite(RR2, LOW);
}

void moveBackward() {
  digitalWrite(FL1, LOW);
  digitalWrite(FL2, HIGH);
  digitalWrite(FR1, LOW);
  digitalWrite(FR2, HIGH);
  digitalWrite(RL1, LOW);
  digitalWrite(RL2, HIGH);
  digitalWrite(RR1, LOW);
  digitalWrite(RR2, HIGH);
}

void moveCLeft() {
  digitalWrite(FL1, LOW);
  digitalWrite(FL2, HIGH);
  digitalWrite(FR1, HIGH);
  digitalWrite(FR2, LOW);
  digitalWrite(RL1, LOW);
  digitalWrite(RL2, HIGH);
  digitalWrite(RR1, HIGH);
  digitalWrite(RR2, LOW);
}

void moveCRight() {
  digitalWrite(FL1, HIGH);
  digitalWrite(FL2, LOW);
  digitalWrite(FR1, LOW);
  digitalWrite(FR2, HIGH);
  digitalWrite(RL1, HIGH);
  digitalWrite(RL2, LOW);
  digitalWrite(RR1, LOW);
  digitalWrite(RR2, HIGH);
}

void moveLeftForward() {
  digitalWrite(FL1, LOW);
  digitalWrite(FL2, LOW);
  digitalWrite(FR1, HIGH);
  digitalWrite(FR2, LOW);
  digitalWrite(RL1, LOW);
  digitalWrite(RL2, LOW);
  digitalWrite(RR1, HIGH);
  digitalWrite(RR2, LOW);
}

void moveRightForward() {
  digitalWrite(FL1, HIGH);
  digitalWrite(FL2, LOW);
  digitalWrite(FR1, LOW);
  digitalWrite(FR2, LOW);
  digitalWrite(RL1, HIGH);
  digitalWrite(RL2, LOW);
  digitalWrite(RR1, LOW);
  digitalWrite(RR2, LOW);
}

void moveLeftBackward() {
  digitalWrite(FL1, LOW);
  digitalWrite(FL2, LOW);
  digitalWrite(FR1, LOW);
  digitalWrite(FR2, HIGH);
  digitalWrite(RL1, LOW);
  digitalWrite(RL2, LOW);
  digitalWrite(RR1, LOW);
  digitalWrite(RR2, HIGH);
}

void moveRightBackward() {
  digitalWrite(FL1, LOW);
  digitalWrite(FL2, HIGH);
  digitalWrite(FR1, LOW);
  digitalWrite(FR2, LOW);
  digitalWrite(RL1, LOW);
  digitalWrite(RL2, HIGH);
  digitalWrite(RR1, LOW);
  digitalWrite(RR2, LOW);
}

void stopMotors() {
  digitalWrite(FL1, LOW);
  digitalWrite(FL2, LOW);
  digitalWrite(FR1, LOW);
  digitalWrite(FR2, LOW);
  digitalWrite(RL1, LOW);
  digitalWrite(RL2, LOW);
  digitalWrite(RR1, LOW);
  digitalWrite(RR2, LOW);
}

void honk() {
  tone(HONK, 500); 
  delay(500);       
  noTone(HONK);     
  delay(300);    

  tone(HONK, 500);  
  delay(800);      
  noTone(HONK);   
}

void wifiSetup() {
  if (WiFi.status() != WL_CONNECTED) {  
    Serial.println("Försöker ansluta till Wi-Fi...");
    WiFi.begin(ssid, password);
    
    unsigned long startAttemptTime = millis(); 
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {  
      delay(500);
      Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Ansluten till Wi-Fi!");
    } else {
      Serial.println("Kunde inte ansluta till Wi-Fi.");
    }
  }
}