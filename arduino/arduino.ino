#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define HOST "192.168.86.238"
#define PORT 3000
#define STASSID "zagrobelny"
#define STAPSK  "bridget1"

const int bufCapacity = 20000;
const int bufTarget = 60; // how much to buffer
int pos = 0;
int startPos = 0;
int bufSize = 0;
uint16_t values[bufCapacity] = {0};
unsigned long last = 0;
int interval = 1E6;
int testval = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();
  WiFi.begin(STASSID, STAPSK);
  pinMode(D0, OUTPUT);
  digitalWrite(D0, LOW);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print(F("Connected! IP address: "));
  Serial.println(WiFi.localIP());
}

void loop() {
  if (micros() - last > interval) {
    last = micros();
    
    values[pos] = readHumidity();

    // debug();
    
    if (bufSize == bufCapacity) {
      if (startPos < bufCapacity - 1) {
        startPos++;
      } else {
        startPos = 0;
      }
    }
    
    if (bufSize < bufCapacity) {
      bufSize++;
    }

    if (pos < (bufCapacity - 1)) {
      pos++;
    } else {
      pos=0;
    }

    if (bufSize >= bufTarget) {
      attemptUpdate();
    }

  }
}

void debug() {
  Serial.print("pos:");
  Serial.print(pos);
  Serial.print(" startPos:");
  Serial.print(startPos);
  Serial.println();
  for (int i = 0; i < bufCapacity; i++) {
    Serial.print(values[i]);
    if (i < bufCapacity - 1) {
      Serial.print(",");
    }
  }
  Serial.println();
}


void attemptUpdate() {
  WiFiClient client;
  if ((WiFi.status() == WL_CONNECTED) && (client.connect(HOST, PORT))) {
    client.println(F("POST / HTTP/1.1"));
    client.println(F("Host: http://" HOST "/"));
    client.println(F("Content-Type: text/plain"));
    //client.println("Transfer-Encoding: chunked");
    client.print(F("Content-Length: "));
    client.println(bufSize * 5 + 1);
    client.println();

    client.print("[");
    for (int i = 0; i < bufSize; i++) {
      int j = (i + startPos) % bufCapacity;
      uint16_t v = values[j];
      client.printf("%*d", 4, v);
      if (i < bufSize - 1) {
        client.print(",");
      }
    }
    client.println("]");
    client.println();
    client.stop();

    pos = 0;
    bufSize = 0;
    startPos = 0;
  } else {
    Serial.println(F("update attempt failed."));
  }
}

int readHumidity() {
  int value;
  digitalWrite(D0, HIGH);
  delay(500);
  value = analogRead(A0);
  digitalWrite(D0, LOW);

  return value;
}
