#include <LiquidCrystal.h>
#include <IRremote.h>

int process = 0;
int clap;
unsigned long delayStart = 0;
unsigned long count = 0;
bool delayRunning = false;
bool setupButton, status1, status2, status3;
int button;
int recvPin = 9;
IRrecv irrecv(recvPin);
decode_results volume_up, volume_down, channel_up;
IRsend irsend;

LiquidCrystal lcd(8, 7, 6, 5, 4, 2);

void setup() {
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(11, INPUT);
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  delayStart = millis();   // start delay
  delayRunning = false;
  
  setupButton = true;
  status1 = false;
  status2 = false;
  status3 = false;
}

void loop() {
  clap = analogRead(A0);
  button = digitalRead(11);

  //Listen mode
  if (!setupButton) {
    if (count == 0) {
      lcd.print("Listening");
      delay(100);
    }
    if (button && !setupButton) {
      setupButton = true;
    }

    
    Serial.print("Clap: ");
    Serial.println(clap);

    
    if (clap > 500 && !delayRunning) {
      delayRunning = true;
      delayStart = millis();
    }

    if (delayRunning && ((millis() - delayStart) <= 5000)) {
      if (clap > 500) {
        digitalWrite(13,HIGH);
        delay(300);
        digitalWrite(13,LOW);
        count++;
      }
      lcd.print(count);
      delay(100);
    }

    if (count > 0 && ((millis() - delayStart) > 5000)) {
      process = count;
      if (count > 3) {
        process = 3;
      }

      if (process == 1) {
        lcd.clear();
        lcd.print("Volume Up");
        delay(300);
        for (int i = 0; i < 4; i++) {
          sender(&volume_up);
          delay(300);
        }
      } else if (process == 2) {
        lcd.clear();
        lcd.print("Volume Down");
        delay(300);
        for (int i = 0; i < 4; i++) {
          sender(&volume_down);
          delay(300);
        }

      } else if (process == 3) {
        lcd.clear();
        lcd.print("Channel Up");
        delay(300);
        sender(&channel_up);
        delay(300);


      }

    }

    if (delayRunning && ((millis() - delayStart) > 5000)) {
      delayRunning = false;
      count = 0;
    }
    lcd.clear();



    //Setup mode
  } else {
    irrecv.enableIRIn();
    lcd.print("Setup mode");
    delay(300);

    if (status1 == false && status2 == false && status3 == false) {
      lcd.clear();
      lcd.print("Volume up button");
      delay(500);

      if (irrecv.decode(&volume_up)) {
        lcd.clear();
        lcd.print(volume_up.value, HEX);
        delay(500);
        status1 = true;
        irrecv.resume();
      }

    }
    else if (status2 == false && status1 == true && status3 == false) {
      lcd.clear();
      lcd.print("Volume down button");
      delay(500);

      if (irrecv.decode(&volume_down)) {
        lcd.clear();
        lcd.print(volume_down.value, HEX);
        delay(500);
        status2 = true;
        irrecv.resume();
      }
    }
    else if (status3 == false && status2 == true && status1 == true) {
      lcd.clear();
      lcd.print("Channel button");
      delay(500);

      if (irrecv.decode(&channel_up)) {
        lcd.clear();
        lcd.print(channel_up.value, HEX);
        status3 = true;
        irrecv.resume();
      }
    }
    else if (status1 == true && status2 == true && status3 == true) {
      lcd.clear();
      lcd.print("Setup completed");
      delay(2000);
      setupButton = false;
      status1 = false;
      status2 = false;
      status3 = false;
    }
    lcd.clear();
  }
}

void sender(decode_results *results) {
  if (results->decode_type == UNKNOWN) {
    lcd.print("Unknown device!");
    delay(2000);
    lcd.clear();
  }
  else if (results->decode_type == NEC) {
    irsend.sendNEC(results->value, results->bits);
  }
  else if (results->decode_type == SONY) {
    irsend.sendSony(results->value, results->bits);
  }
  else if (results->decode_type == RC5) {
    irsend.sendRC5(results->value, results->bits);
  }
  else if (results->decode_type == RC6) {
    irsend.sendRC6(results->value, results->bits);
  }
  else if (results->decode_type == LG) {
    irsend.sendLG(results->value, results->bits);
  }
  else if (results->decode_type == JVC) {
    irsend.sendJVC(results->value, results->bits, 0);
  }
  else if (results->decode_type == SAMSUNG) {
    irsend.sendSAMSUNG(results->value, results->bits);
  }



}
