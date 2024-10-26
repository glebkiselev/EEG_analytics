#pragma once

// ReSharper disable CppUnusedIncludeDirective
#include "Arduino.h"

#include <WebServer.h>
// ReSharper restore CppUnusedIncludeDirective

// On MH-ET ESP32 DevKIT LOW == Led ON, HIGH == Led OFF
#define LED_OUTPUT_ON LOW
#define LED_OUTPUT_OFF HIGH


namespace web {
    extern WebServer server;

    extern bool ledState;

    void setup();
    void loop();

    void ctxPredict();
    void notFound();
    void setLedState(bool state);
}
