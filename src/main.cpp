#include <Arduino.h>
#include "LoRa_E220.h"
#include <LoRaE220Communication.h>

LoRaE220Communication lora(&Serial2, 15, 21, 19, DeviceFunction::base_station);

void setup(){
    Serial.begin(9600);
    delay(1000);

    lora.setup();
    lora.printParameters();
}

void loop(){
    if(lora.updatePacket() == msgType::SENSORS_DATA){
        lora.printSensorsData(lora.getSensorsData());
    }
}