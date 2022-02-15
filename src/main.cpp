#include <Arduino.h>
#include "LoRa_E220.h"
#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// ---------- esp32 pins --------------
LoRa_E220 e220ttl(&Serial2, 15, 21, 19, UART_BPS_RATE_9600);

void config(){
    ResponseStructContainer c;
	c = e220ttl.getConfiguration();
    Configuration configuration = *(Configuration*) c.data;
    configuration.ADDL = 0x47;
	configuration.ADDH = 0x51;
   	configuration.CHAN = 42;
	configuration.SPED.uartBaudRate = UART_BPS_9600; // Serial baud rate
	configuration.SPED.airDataRate = AIR_DATA_RATE_010_24; // Air baud rate
	configuration.SPED.uartParity = MODE_00_8N1; // Parity bit
	configuration.OPTION.subPacketSetting = SPS_200_00; // Packet size
	configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED; // Need to send special command
	configuration.OPTION.transmissionPower = POWER_22; // Device power
   	configuration.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED; // Enable RSSI info
   	configuration.TRANSMISSION_MODE.fixedTransmission = FT_FIXED_TRANSMISSION; // Enable repeater mode
   	configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED; // Check interference
   	configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011; // WOR timing
    e220ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
}

struct Message {
    char type[5];
    char message[8];
    byte temperature[4];
};

void setup() {
  Serial.begin(9600);
  delay(500);

  // Startup all pins and UART
  e220ttl.begin();
  config();
}

typedef struct
{
  	uint16_t typeMsg : 16;
  	uint16_t id : 16;
  	uint16_t current : 16;
  	uint16_t tempCell : 16;
  	uint16_t tempExternal : 16;
  	uint16_t tempInternal : 16;
  	uint32_t date : 32;
} packetTypeSensorsData;

typedef union packetSensorsData {
  	unsigned char encoded[16];
  	packetTypeSensorsData decoded;
} packetSensorsData;

void loop() {
	// If something available
  if (e220ttl.available()>1) {
	  // read the String message
		ResponseStructContainer rsc = e220ttl.receiveMessage(16);

	// Is something goes wrong print error
	if (rsc.status.code!=1){
		Serial.println(rsc.status.getResponseDescription());
	}else{
		// Print the data received
		Serial.println(rsc.status.getResponseDescription());
		packetSensorsData message = *(packetSensorsData*) rsc.data;
		uint16_t typeMsg =static_cast<uint16_t>(message.decoded.typeMsg);
		uint16_t id =static_cast<uint16_t>(message.decoded.id);
		uint16_t corrente =static_cast<uint16_t>(message.decoded.current);
		uint16_t celula = static_cast<uint16_t>(message.decoded.tempCell);
		uint16_t externa = static_cast<uint16_t>(message.decoded.tempExternal);
		uint16_t interna = static_cast<uint16_t>(message.decoded.tempInternal);
		uint32_t data = static_cast<uint32_t>(message.decoded.date);
		Serial.print("TypeMSG: ");
		Serial.println(typeMsg);
		Serial.print("ID: ");
		Serial.println(id);
		Serial.print("Corrente: ");
		Serial.println(corrente);
		Serial.print("Temperatura da celula: ");
		Serial.println(celula);
		Serial.print("Temperatura da externa: ");
		Serial.println(externa);
		Serial.print("Temperatura da interna: ");
		Serial.println(interna);
		Serial.print("Data: ");
		Serial.println(data);
		
		DateTime now(data);
		Serial.print(now.year(), DEC);
    	Serial.print('/');
    	Serial.print(now.month(), DEC);
    	Serial.print('/');
    	Serial.print(now.day(), DEC);
    	Serial.print(" (");
    	Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    	Serial.print(") ");
    	Serial.print(now.hour(), DEC);
    	Serial.print(':');
    	Serial.print(now.minute(), DEC);
    	Serial.print(':');
    	Serial.print(now.second(), DEC);
    	Serial.println();

	}
  }
}