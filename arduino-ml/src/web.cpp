//
// Created by ZONT_ on 31.03.2022.
//

#include "web.hpp"

#include <iostream>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#include "wifi_conf.hpp"
#include "ml.hpp"


WebServer web::server(80);
bool web::ledState = false;


void web::setup() {
    using namespace std;

    pinMode(LED_BUILTIN, OUTPUT);

    setLedState(true);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_AP_SSID, WIFI_AP_PASSWORD);

    cout << "Connecting to WiFi..";
    while (WiFi.status() != WL_CONNECTED) {
        delay(300);
        setLedState(!ledState);
        cout << ".";
    }
    cout << endl;
    cout << "Connected. IP address: ";
    cout << WiFi.localIP().toString().c_str() << endl;
    setLedState(true);

    IPAddress result;
    if (WiFi.hostByName("www.google.com", result))
        cout << "hostByName www.google.com: " << result.toString().c_str() << endl;
    else
        cout << "hostByName failed" << endl;

    ml::setup();

    server.onNotFound(notFound);
    server.on("/predict", HTTP_GET, ctxPredict);
    server.begin();

    setLedState(false);
}

void web::ctxPredict() {
    setLedState(true);
    try {
        auto* x = new float[24];
        for (int i = 0; i < 24; i++) {
            auto xIdx = String("x");
            xIdx.concat(i + 1);

            if (!server.hasArg(xIdx)) {
                server.send(400, "text/plain", "Insufficient x parameter(s)");
                setLedState(false);
                return;
            }

            const String xStr = server.arg(xIdx);
            x[i] = xStr.toFloat();
        }
        const float* y = ml::inference(x);

        // Binary classification
        // const auto result = String(y[0]);

        // Multi-label classification
        auto result = String("");
        bool first = true;
        for (int i = 0; i < 4; i++) {
            if (first)
                first = false;
            else
                result.concat(" ");
            result.concat(y[i]);
        }

        server.send(200, "text/plain", result);
    } catch (const std::exception& ex) {
        server.send(500, "text/plain", ex.what());
    }
    setLedState(false);
}

void web::setLedState(const bool state) {
    digitalWrite(LED_BUILTIN, state ? LED_OUTPUT_ON : LED_OUTPUT_OFF);
    ledState = state;
}

void web::notFound() {
    String message = "404 Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (int i = 0; i < server.args(); i++)
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    server.send(404, "text/plain", message);
}

void web::loop() {
    server.handleClient();
}
