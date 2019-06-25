#include <ESP8266WiFi.h>

const char* ssid = "NETWORK_NAME_HERE";
const char* password = "NETWORK_PASSWORD_HERE";

int pinR = D8;
int pinG = D7;
int pinB = D5;

bool stateR = false;
bool stateG = false;
bool stateB = false;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);
  digitalWrite(pinR, LOW);
  digitalWrite(pinG, LOW);
  digitalWrite(pinB, LOW);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Wifi connected: ");
  Serial.println(WiFi.localIP());
  
  Serial.println(WiFi.macAddress());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  Serial.println("New client");
  while(!client.available()) delay(1);

  // Read the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  if (request.indexOf("/red") != -1) {
    stateR = !stateR;
  } else if (request.indexOf("/green") != -1) {
    stateG = !stateG;
  } else if (request.indexOf("/blue") != -1) {
    stateB = !stateB;
  }
  digitalWrite(pinR, stateR ? HIGH : LOW);
  digitalWrite(pinG, stateG ? HIGH : LOW);
  digitalWrite(pinB, stateB ? HIGH : LOW);

  // Send a response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<style>*{font-size:xx-large;}</style>");
  client.println("<a href=\"/red\"><button>Toggle Red</button></a>");
  client.println("<a href=\"/green\"><button>Toggle Green</button></a>");
  client.println("<a href=\"/blue\"><button>Toggle Blue</button></a>");
  client.println("</html>");

  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
}
 
