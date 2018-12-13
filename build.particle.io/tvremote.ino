#include <stdio.h>
#include <string.h>
#include "IRremote.h"

bool started = false;
bool ended = false;

char inData[80];
byte BYTE_INDEX;

int RECV_PIN = 4;

IRrecv irrecv(RECV_PIN);
IRsend irsend;
decode_results results;
String decodeTypeString = "Decode Type";
String decodeType;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
    if (irrecv.decode(&results)) {
        if (results.decode_type == NEC) {
          decodeType = "NEC";
        } else if (results.decode_type == SONY) {
          decodeType = "SONY";
        } else if (results.decode_type == RC5) {
          decodeType = "RC5";
        } else if (results.decode_type == RC6) {
          decodeType = "RC6";
        } else if (results.decode_type == UNKNOWN) {
          decodeType = "UNKNOWN";
        }
        
        publish("IR DATA", "irsend.send" + decodeType + "(0x" + String(results.value, HEX) + ", " + String(results.bits, DEC) + ");");

        dumpRaw(&results);
        
        irrecv.resume(); // Receive the next value
    }
}

void dumpRaw(decode_results *results) {
  for (int i = 1; i < results->rawlen; i++) {
    if (i & 1) {
      Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
    }
    else {
      Serial.write('-');
      Serial.print((unsigned long) results->rawbuf[i]*USECPERTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println();
}

bool sendCommandSuccess = Particle.function("command", sendCommand);
int sendCommand(String params) {
    char *command;
    char *value;

    char testChar[65];
    params.toCharArray(testChar, 65);

    char *token = strtok(testChar, ",");
    command = token;
    token = strtok(NULL, ","); 
    value = token;
    
    if (strcmp(command, "tv") == 0 || strcmp(command, "television") == 0) { // tv,[on|off]
        power(value);
    } 
    else if (strcmp(command, "input") == 0 || strcmp(command, "source") == 0) { // input,[xbox|nintendo|tv]
        source(value);
    } 
    else if (strcmp(command, "channel") == 0) { // channel,[0|1|2|3|4|5|6|7|8|9]
        channel(value);
    } 
    else if (strcmp(command, "volumeup") == 0 || strcmp(command, "volumedown") == 0) {
        volume(command, value);
    } 
    else if (strcmp(command, "info") == 0) {
        sendCode("info");
    } 
    else if (strcmp(command, "input") == 0) {
        sendCode("input");
    }
    
    return 1;
}

void volume(char *command, char* value) {
  for (int i = 0; i < atoi(value); i++) {
    if (strcmp(command, "volumeup") == 0) {
      sendCode("volumeUp");
    } 
    else if (strcmp(command, "volumedown") == 0) {
      sendCode("volumeDown");
    }
    delay(300);
  }
}

void power(char* device) {
  if (strcmp(device, "on") == 0 || strcmp(device, "off") == 0) {
    sendCode("onOffReceived");
  } 
}

void channel(char* numbers) {
  int i = 0;

  for (i = 0; i < 2; i++) {
    if (numbers[i] == '0') {
      sendCode("0");
    } 
    else if (numbers[i] == '1') {
      sendCode("1");
    } 
    else if (numbers[i] == '2') {
      sendCode("2");
    } 
    else if (numbers[i] == '3') {
      sendCode("3");
    } 
    else if (numbers[i] == '4') {
      sendCode("4");
    } 
    else if (numbers[i] == '5') {
      sendCode("5");
    } 
    else if (numbers[i] == '6') {
      sendCode("6");
    } 
    else if (numbers[i] == '7') {
      sendCode("7");
    } 
    else if (numbers[i] == '8') {
      sendCode("8");
    } 
    else if (numbers[i] == '9') {
      sendCode("9");
    }
  }
}

void source(char* source) {
  if (strcmp(source, "tv") == 0) {
    sendCode("input");
    delay(1000);
    sendCode("arrowDown");
    delay(500);
    sendCode("ok");
    delay(500);
    sendCode("exit");
    delay(500);
  } 
  else if (strcmp(source, "nintendo") == 0) {
    sendCode("input");
    delay(1000);
    sendCode("arrowDown");
    delay(500);
    sendCode("arrowDown");
    delay(500);
    sendCode("arrowDown");
    delay(500);
    sendCode("arrowDown");
    delay(500);
    sendCode("ok");
    delay(500);
    sendCode("exit");
    delay(500);
  } 
  else if (strcmp(source, "xbox") == 0) {
    sendCode("input");
    delay(1000);
    sendCode("arrowDown");
    delay(500);
    sendCode("arrowDown");
    delay(500);
    sendCode("ok");
    delay(500);
    sendCode("exit");
    delay(500);
  } 
  else if (strcmp(source, "mute") == 0) {
    sendCode("mute");
  }
}

void publish(String name, String data) {
    Particle.publish(name, data);
}

int khz = 38; // 38kHz carrier frequency for the NEC protocol
unsigned int rawSignal1[] = { 450, 350, 550, 300, 550, 300, 550, 300, 550, 300, 550 };

bool sendCodeSuccess = Particle.function("code", sendCode);
int sendCode(String code) {
    long unsigned int irCode;

    publish("Remote Code Transmitted", code);
    
    if (code == "onOffReceived") {
      irCode = 0x20DF10EF;
    }
    if (code == "energyRcv") {
      irCode = 0x20DFA956;
    }
    if (code == "avMode") {
      irCode = 0x20DF0CF3;
    }
    if (code == "input") {
      irCode = 0x20DFD02F;
    }
    if (code == "tvRad") {
      irCode = 0x20DF0FF0;
    }
    if (code == "list") {
      irCode = 0x20DFCA35;
    }
    if (code == "quickView") {
      irCode = 0x20DF58A7;
    }
    if (code == "volumeUp") {
      irCode = 0x20DF40BF;
    }
    if (code == "volumeDown") {
      irCode = 0x20DFC03F;
    }
    if (code == "programUP") {
      irCode = 0x20DF00FF;
    }
    if (code == "programDown") {
      irCode = 0x20DF807F;
      }
    if (code == "guide") {
      irCode = 0x20DFD52A;
    }
    if (code == "homeBtn") {
      irCode = 0x20DFC23D;
    }
    if (code == "fav") {
      irCode = 0x20DF7887;
    }
    if (code == "ratio") {
      irCode = 0x20DF9E61;
    }
    if (code == "mute") {
      irCode = 0x20DF906F;
    }
    if (code == "arrowUp") {
      irCode = 0x20DF02FD;
    }
    if (code == "arrowDown") {
      irCode = 0x20DF827D;
    }
    if (code == "arrowLeft") {
      irCode = 0x20DFE01F;
    }
    if (code == "arrowRight") {
      irCode = 0x20DF609F;
    }
    if (code == "ok") {
      irCode = 0x20DF22DD;
    }
    if (code == "back") {
      irCode = 0x20DF14EB;
    }
    if (code == "info") {
      irCode = 0x20DF55AA;
    }
    if (code == "exit") {
      irCode = 0x20DFDA25;
    }
    if (code == "redBttn") {
      irCode = 0x20DF4EB1;
    }
    if (code == "greenBttn") {
      irCode = 0x20DF8E71;
    }      
    if (code == "yellowBttn") {
      irCode = 0x20DFC639;
    }
    if (code == "blueBttn") {
      irCode = 0x20DF8679;
    }
    if (code == "text") {
      irCode = 0x20DF04FB;
    }
    if (code == "tOpt") {
      irCode = 0x20DF847B;
    }
    if (code == "subtitle") {
      irCode = 0x20DF9C63;
    }
    if (code == "stopBttn") {
      irCode = 0x20DF8D72;
    }
    if (code == "playBttn") {
      irCode = 0x20DF0DF2;
    }
    if (code == "pause") {
      irCode = 0x20DF5DA2;
    }
    if (code == "rew") {
      irCode = 0x20DFF10E;
    }
    if (code == "forward") {
      irCode = 0x20DF718E;
    }
    if (code == "ad") {
      irCode = 0x20DF8976;
    }
    if (code == "0") {
      irCode = 0x20DF08F7;
    }
    if (code == "1") {
      irCode = 0x20DF8877;
    }
    if (code == "2") {
      irCode = 0x20DF48B7;
    }
    if (code == "3") {
      irCode = 0x20DFC837;
    }
    if (code == "4") {
      irCode = 0x20DF28D7;
    }
    if (code == "5") {
      irCode = 0x20DFA857;
    }
    if (code == "6") {
      irCode = 0x20DF6897;
    }
    if (code == "7") {
      irCode = 0x20DFE817;
    }
    if (code == "8") {
      irCode = 0x20DF18E7;
    }
    if (code == "9") {
      irCode = 0x20DF9867;
    }
    
    delay(400);
    irsend.sendNEC(irCode, 32);
    delay(40);
    
    return 1;
}
