#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "SimpleTimer.h"
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

/*
* TO DO:
* Only sending 14 circuits worth of data right now
*
*/

const char* ssid = "WWU-Aruba-HWauth"; //Wi-Fi Bro WWU-Aruba-HWauth
const char* password = "5C:CF:7F:13:AD:C4"; //5C:CF:7F:13:AD:C4

const char* host = "35.165.91.189";
const int port = 8000;

String url = "/datapoints/"  + WiFi.macAddress() + "/batch/";
//String url = "/datapoints/";

WiFiClient client;

int muxAddress = B0000;

int data[16][35] = {0};    // can be 180 or 34

String circuitNames[16] = {"V", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"};

int currentCircuit = 0;

SimpleTimer timer;

int timerID = 1;

// ***Add later***
// Tests each circuit on startup to see if it is connected. 
/*void testCircuits() {
    //muxAddress = B0000;
    //muxAddressUpdate();
    for (int i = 1; i < 16; i++) {  // iterates through each circuit
    // starts at 1 because 0000 is voltage
        muxAddress = i;
        muxAddressUpdate();
        bool high = false;
        bool low = false;
        for (int n = 0 ; n < 4; n++) {  // tests for change from optoisolater
            // if circuit is 0 then add it to the class (because it inverts)
            if (analogRead(A0) >= 750) {
                //String name = String(n);
                high = true;

                n = 4;  // exit if circuit is connected
            } else if (analogRead(A0) >= 750) {
                low = true;
            }

            if (high = true && low == true) {
                circuitNamesStr.push_back(String(i));

                Circuit i;   // list of circuits that are active
                circuitNames.push_back(i);
            } else {
                delay(4.15);
            }
        }
        // for debugging
        //Serial.print("Circuits#");
        //Serial.print(circuitNamesStr[1]);
        //Serial.println();
    }
}*/

// updates the mux address pins based on the muxAddress var
void muxAddressUpdate() {
    //muxAddress = B0000;
    //Serial.print(muxAddress);
    //Serial.println();
    if ((muxAddress & B0001) == 1) {
        digitalWrite(D1, HIGH);
        //Serial.print("D1High");
        //Serial.println();
    } else {
        digitalWrite(D1, LOW);
        //Serial.print("D1Low");
        //Serial.println();
    }
    if ((muxAddress & B0010) == 2) {
        digitalWrite(D2, HIGH);
        //Serial.print("D2High");
        //Serial.println();
    } else {
        digitalWrite(D2, LOW);
        //Serial.print("D2Low");
        //Serial.println();
    }
    if ((muxAddress & B0100) == 4) {
        digitalWrite(D3, HIGH);
        //Serial.print("D3High");
        //Serial.println();
    } else {
        digitalWrite(D3, LOW);
        //Serial.print("D3Low");
        //Serial.println();
    }
    if ((muxAddress & B1000) == 8) {
        digitalWrite(D4, HIGH);
        //Serial.print("D4High");
        //Serial.println();
    } else {
        digitalWrite(D4, LOW);
        //Serial.print("D4Low");
        //Serial.println();
    }
    //Serial.println();
}

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
    client.println("Post " + url + " HTTP/1.1");
    client.println("Host: " + (String)host);
    client.println("User-Agent: ESP8266/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.println(data);
    delay(100);  //10
    String response = client.readString();
    Serial.print(response);
    Serial.println("data sent");
    int bodypos =  response.indexOf("\r\n\r\n") + 4;
    return response.substring(bodypos);
  }
  else {
    return "ERROR";
  }
}

// serializes data into JSON format
String jsonSerialization() {
    DynamicJsonBuffer jsonBuffer;

    JsonObject& objectJson = jsonBuffer.createObject();

    for (int i = 0; i<15; i++) { // should be i<16
        JsonArray& measurements = objectJson.createNestedArray(circuitNames[i]);
        int dataArray[35];
        for (int j = 0; j<35; j++) {
            dataArray[j] = data[i][j];
            //Serial.print(data[i][j]);
            //Serial.println();
        }
        measurements.copyFrom(dataArray);
    }
    
    String objStr;
    objectJson.printTo(objStr);
    return objStr;
}

// Collects one period of data
void collectData() {
    //Serial.print("start");
    //Serial.print(millis());
    memset(data[currentCircuit], 0, sizeof(data[currentCircuit]));
    int n = 0;
    unsigned long startTime = millis();
    int analogVal;
    while (millis() - startTime < 16.7) {       // should be 16.7, 19.65
        analogVal = analogRead(A0);
        //data[currentCircuit][n] = analogVal;
        //Serial.print(analogVal);
        //Serial.println();
        if (n % 5 == 0) {
            data[currentCircuit][n/5] = analogVal;
            //Serial.print(analogVal);
            //Serial.print("Test!");
            //Serial.print(n/5);
            //Serial.println();
        }
        n = n + 1;
    }

    /*for (int j = 0; n<34 ; n++) {
        Serial.print(data[currentCircuit][j]);
        Serial.println();
    }*/
    //Serial.println();

    if (currentCircuit < 14) {   // should be 15
        currentCircuit += 1;
    } else {
        // send out data to Ryan
        timer.disable(timerID);
        String dataJSON = jsonSerialization();
        //Serial.println("pre-JSON Serial");
        //Serial.println(dataJSON);
        currentCircuit = 0;
        //Serial.println("post-JSON Serial");
        String send = httpPost(url, dataJSON);
        if (send == "ERROR") {
            while(send == "ERROR") {
                Serial.println("Failed to Connect");
                //send = httpPost(url, dataJSON);
            }
        }
        timer.enable(timerID);
    }
    muxAddress = currentCircuit;
    muxAddressUpdate();

    //Serial.print("end");
    //Serial.print(millis());
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(38400);

    //Serial.setDebugOutput(true);

    // Mux Address Pins
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
    pinMode(D3, OUTPUT);
    pinMode(D4, OUTPUT);

    muxAddressUpdate();

    pinMode(LED_BUILTIN, OUTPUT);

    timerID = timer.setInterval(100, collectData); // should be 50
}



void loop() {
    // put your main code here, to run repeatedly:

    timer.run();

}