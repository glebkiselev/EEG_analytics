#include "Arduino.h"

#include <iostream>
#include "web.hpp"

using namespace std;

void setup() {
    try {
        cout << F("\n\nSTART " __FILE__ " date " __DATE__ "\n---------------------------------");
        cout << endl << endl;

        web::setup();
    } catch (const exception& ex) {
        cerr << "SETUP ERR: " << ex.what() << endl;
    } catch (const string& ex) {
        cerr << "SETUP ERR: " << ex << endl;
    } catch (...) {
        cerr << "SETUP ERR [UNKNOWN]" << endl;
    }
}

void loop() {
    web::loop();
}
