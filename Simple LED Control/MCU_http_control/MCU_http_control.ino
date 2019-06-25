#include <ESP8266WiFi.h>

const char* ssid = "NETWORK_NAME_HERE";
const char* password = "NETWORK_PASSWORD_HERE";

int ledPin = D5;
int state = LOW;
bool strobe = false;
int cycle = 1;
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wifi connected");

  server.begin();
  Serial.println(WiFi.localIP());
}

void loop() {
  if (strobe) {
    Serial.println(++cycle);
    if (cycle % 100 == 0) {
      state = state == HIGH ? LOW : HIGH;
      digitalWrite(ledPin, state);
    }
  }
  WiFiClient client = server.available();
  if (!client) return;

  Serial.println("New client");
  while(!client.available()) delay(1);

  // Read the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(ledPin, HIGH);
    state = HIGH;
    strobe = false;
  } else if (request.indexOf("/LED=OFF") != -1) {
    digitalWrite(ledPin, LOW);
    state = LOW;
    strobe = false;
  } else if (request.indexOf("/LED=STROBE") != -1) {
    strobe = true;
    state = HIGH;
  }

  // Send a response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<style>*{font-size:xx-large;}</style>");
  client.print("Led is now ");
  if (!strobe) {
    client.println(state == LOW ? "off" : "on");
  } else {
    client.println("STROBING!!!11!");
  }
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON\"><button>Turn On</button></a>");
  client.println("<a href=\"/LED=OFF\"><button>Turn Off</button></a>");
  client.println("<a href=\"/LED=STROBE\"><button>STR0B3</button></a>");
  client.println("</html>");

  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
}
 
