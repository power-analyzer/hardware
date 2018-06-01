#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "Wi-Fi Bro"; //Wi-Fi Bro WWU-Aruba-HWauth
const char* password = "fabshack"; //5C:CF:7F:53:CE:22

const char* host = "35.165.91.189";
const int port = 8000;

String macUrl = "/datapoints/batch/";    // /datapoints/MAC/batch*/

WiFiClient client;

// sends post to server
String httpPost(String url, String data) {
  // Connnecting to wifi
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Use WiFiClientSecure class to create TLS connection
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return "wifi connection failed";
  }

  if (client.connect(host, port)) {
    client.println("POST " + url + " HTTP/1.1");
    client.println("Host: " + (String)host);
    client.println("User-Agent: ESP8266/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.println(data);
    delay(10);
    String response = client.readString();
    Serial.print(response);
    int bodypos =  response.indexOf("\r\n\r\n") + 4;
    return response.substring(bodypos);
  }
  else {
    return "ERROR";
  }
}

void setup() {
    // put your setup code here, to run once:

    // setup pins
    //AC PIN
    //analogRead(A0);

    Serial.begin(9600);

    // register device
    //String url = "datapoints/register/" + WiFi.macAddress();   

    /*String send = httpPost(url, "");    // turn into a request
    if (send == "ERROR") {
        while(send == "Error") {
            send = httpPost(url, "");
        }
    }*/
    // opto-isolator - for p.f.
    //pinMode(D7, INPUT_PULLUP); // mux input
    //attachInterrupt(digitalPinToInterrupt(D7), getAmplitude, RISING);
    //noInterrupts();

}

void loop() {
    // put your main code here, to run repeatedly:

    String url = "/datapoints/"  + WiFi.macAddress() + "/batch/";

    Serial.print("mac");
    Serial.print(WiFi.macAddress());

    String send = httpPost(url, "test");
    if (send == "ERROR") {
        while(send == "Error") {
            send = httpPost(url, "test");
        }
    } else {
        //circuitNames.clear();   // wipes all data after it is sent
    }
}

// test for 180 out of phase