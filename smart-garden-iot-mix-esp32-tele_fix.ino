#include <Wire.h>
#include "WiFi.h"
#include <AsyncTelegram2.h>

// Timezone definition
#define MYTZ "CET-1CEST,M3.5.0,M10.5.0/3"
struct tm sysTime;

#include <WiFiClientSecure.h>
WiFiClientSecure client;

Session   session;
X509List  certificate(telegram_cert);
#endif

AsyncTelegram2 myBot(client);

const char* ssid  =  "***********";     // SSID WiFi network
const char* pass  =  "***********";     // Password  WiFi network
const char* token =  "*****************************";  // Telegram token

// Check the userid with the help of bot @JsonDumpBot or @getidsbot (work also with groups)
// https://t.me/JsonDumpBot  or  https://t.me/getidsbot
int64_t userid = 1407526783;


#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 4 line display

//Pin Out sensorKelembaban
const int sensorKelembabanPin1 = 36;
const int sensorKelembabanPin2 = 39;
const int sensorKelembabanPin3 = 34;
const int sensorKelembabanPin4 = 35;


int pompa1 = 18;
int pompa2 = 17;

// init valve pin
int selenoidValvePin1 = 15;
int selenoidValvePin2 = 2;
int selenoidValvePin3 = 4;
int selenoidValvePin4 = 16;

int tangkiAirPin = 23;
int Buzzer = 19; 

int sensorKelembabanVal1;
int sensorKelembabanVal2;
int sensorKelembabanVal3;
int sensorKelembabanVal4;
int tangkiAirVal;

void setup()
{
  pinMode(pompa1, OUTPUT);
  pinMode(pompa2, OUTPUT);

  pinMode(selenoidValvePin1, OUTPUT);
  pinMode(selenoidValvePin2, OUTPUT);
  pinMode(selenoidValvePin3, OUTPUT);
  pinMode(selenoidValvePin4, OUTPUT);

  pinMode(sensorKelembabanPin1, INPUT);
  pinMode(sensorKelembabanPin2, INPUT);
  pinMode(sensorKelembabanPin3, INPUT);
  pinMode(sensorKelembabanPin4, INPUT);

  digitalWrite(pompa1, HIGH);
  digitalWrite(pompa2, HIGH);

  digitalWrite(selenoidValvePin1, HIGH);
  digitalWrite(selenoidValvePin2, HIGH);
  digitalWrite(selenoidValvePin3, HIGH);
  digitalWrite(selenoidValvePin4, HIGH);

  pinMode(tangkiAirPin, INPUT_PULLUP);
  pinMode(Buzzer, OUTPUT);
  digitalWrite(Buzzer, LOW);
  Wire.begin(); //SDA, SCL
  lcd.init();                      // initialize the lcd
  // Print a message to the LCD.
  lcd_print("  SMART GARDEN  ", "     by Me     ");
  delay(1500);
  lcd_print("Please", "Wait...");

  // Set console baud rate
  Serial.begin(9600);
  delay(10);
  initWifi(); //Inisialisai WiFi
  lcd_print("Internet", "Connected :)");
  delay(1000);
  initTelegram(); //Inisialisai Telegram
}

int output;
//int avg_output;

String flagTangkiAir = "";

void getSensorData()
{


  int sensor1 = analogRead(sensorKelembabanPin1);
  int sensor2 = analogRead(sensorKelembabanPin2);
  int sensor3 = analogRead(sensorKelembabanPin3);
  int sensor4 = analogRead(sensorKelembabanPin4);


  sensorKelembabanVal1 = convert(sensor1);
  sensorKelembabanVal2 = convert(sensor2);
  sensorKelembabanVal3 = convert(sensor3);
  sensorKelembabanVal4 = convert(sensor4);

  tangkiAirVal = digitalRead(tangkiAirPin);
  //int tangkiAirVal = HIGH; //for Debug //HIGH = Air penuh

}

String flagKontrol = "manual";

void Pelampung () {

  if (tangkiAirVal == HIGH) {
    Serial.println("Tangki Air terisi...");
    flagTangkiAir = "terisi.";
    digitalWrite(Buzzer, LOW);
  }
  else {
    Serial.println("Tangki Air kosong...");
    flagTangkiAir = "kosong";
    digitalWrite(Buzzer, HIGH);
    digitalWrite(pompa1, HIGH);
    digitalWrite(pompa2, HIGH);
  }
}

void Autocontroll() {

  if (flagKontrol == "auto") {
    if (tangkiAirVal == HIGH) {
      if (sensorKelembabanVal1 < 50) { //jika sensor1 atau sensor2 kurang dari 50
        digitalWrite(pompa1, LOW); //menyalakan pompa 1
        Serial.println("Pompa 1 ON...");
        digitalWrite(selenoidValvePin1, LOW); //buka valve 1
        Serial.println("Selenoid Valve 1 On...");

        delay(3000);
        // When the timer completes, any code here will be executed
        Serial.println("Selenoid Valve 1 Off...");
        digitalWrite(selenoidValvePin1, HIGH);
      }

      else if (sensorKelembabanVal2 < 50) {
        digitalWrite(pompa1, LOW); //menyalakan pompa 1
        Serial.println("Pompa 1 ON...");
        digitalWrite(selenoidValvePin2, LOW);
        Serial.println("Selenoid Valve 2 On...");

        delay(3000);
        // When the timer completes, any code here will be executed
        Serial.println("Selenoid Valve 2 Off...");
        digitalWrite(selenoidValvePin2, HIGH);
      }

      else if (sensorKelembabanVal3 < 50) {
        digitalWrite(pompa2, LOW);
        Serial.println("Pompa 2 ON...");
        digitalWrite(selenoidValvePin3, LOW);
        Serial.println("Selenoid Valve 3 On...");

        delay(3000);
        // When the timer completes, any code here will be executed
        digitalWrite(selenoidValvePin3, HIGH);
        Serial.println("Selenoid Valve 3 Off...");
      }

      else if (sensorKelembabanVal4 < 50) {
        digitalWrite(pompa2, LOW);
        Serial.println("Pompa 2 ON...");
        digitalWrite(selenoidValvePin4, LOW);
        Serial.println("Selenoid Valve 4 On...");

        delay(3000);
        // When the timer completes, any code here will be executed
        Serial.println("Selenoid Valve 4 Off...");
        digitalWrite(selenoidValvePin4, HIGH);
      }

      else {
        digitalWrite(pompa1, HIGH); //pompa mati
        digitalWrite(selenoidValvePin1, HIGH); //valve1 off
        digitalWrite(selenoidValvePin2, HIGH); //valve2 off

        digitalWrite(pompa2, HIGH);
        digitalWrite(selenoidValvePin3, HIGH);
        digitalWrite(selenoidValvePin4, HIGH);
      }
    }
  }
}

void showSensorDataToLcd() {
  getSensorData();
  lcd_print_kelembaban("P1 " + String(sensorKelembabanVal1) + " %", "P2 " + String(sensorKelembabanVal2)
                       + " %", "P3 " + String(sensorKelembabanVal3) + " %", "P4 " + String(sensorKelembabanVal4) + " %");
}


int convert(int sensor) {
  output = map(sensor, 1900, 600, 0, 100);
  return output;
}

void loop()
{
  showSensorDataToLcd();
  delay(500);
  getTelegramMsg();
  Autocontroll();
  Pelampung();

}

void getTelegramMsg() {
  // a variable to store telegram message data
  TBMessage msg;

  // if there is an incoming message...
  if (myBot.getNewMessage(msg)) {
    MessageType msgType = msg.messageType;

    // Received a text message
    if (msgType == MessageText) {
      String msgText = msg.text;
      Serial.print("Text message received: ");
      Serial.println(msgText);

      if (msgText == ("/status")) {
        String statusNow = "P1 " + String(sensorKelembabanVal1) + " %. P2 " + String(sensorKelembabanVal2)
                           + " %. P3 " + String(sensorKelembabanVal3) + " %. P4 " + String(sensorKelembabanVal4) + " %. "
                           + "Tangki Air : " + flagTangkiAir;

        myBot.sendMessage(msg, statusNow);
      }
      else if (msgText == "/valve1on") {
        if (flagKontrol == "manual") {
          digitalWrite(selenoidValvePin1, LOW);
          digitalWrite(pompa1, LOW);
          myBot.sendMessage(msg, "Valve Selenoid 1 Sudah Dinyalakan");
        } else {
          myBot.sendMessage(msg, "Aktifkan Dahulu mode manual");
        }
      }

      else if (msgText == "/valve1off") {
        if (flagKontrol == "manual") {
          digitalWrite(selenoidValvePin1, HIGH);
          digitalWrite(pompa1, HIGH);
          myBot.sendMessage(msg, "Valve Selenoid 1 Sudah Dimatikan");
        } else {
          myBot.sendMessage(msg, "Aktifkan Dahulu mode manual");
        }
      }

      else if (msgText == "/valve2on") {
        if (flagKontrol == "manual") {
          digitalWrite(selenoidValvePin2, LOW);
          digitalWrite(pompa1, LOW);
          myBot.sendMessage(msg, "Valve Selenoid 2 Sudah Dinyalakan");
        } else {
          myBot.sendMessage(msg, "Aktifkan Dahulu mode manual");
        }
      }

      else if (msgText == "/valve2off") {
        if (flagKontrol == "manual") {
          digitalWrite(selenoidValvePin2, HIGH);
          digitalWrite(pompa1, HIGH);
          myBot.sendMessage(msg, "Valve Selenoid 2 Sudah Dimatikan");
        } else {
          myBot.sendMessage(msg, "Aktifkan Dahulu mode manual");
        }
      }

      else if (msgText == "/valve3on") {
        if (flagKontrol == "manual") {
          digitalWrite(selenoidValvePin3, LOW);
          digitalWrite(pompa2, LOW);
          myBot.sendMessage(msg, "Valve Selenoid 3 Sudah Dinyalakan");
        } else {
          myBot.sendMessage(msg, "Aktifkan Dahulu mode manual");
        }
      }

      else if (msgText == "/valve3off") {
        if (flagKontrol == "manual") {
          digitalWrite(selenoidValvePin3, HIGH);
          digitalWrite(pompa2, HIGH);
          myBot.sendMessage(msg, "Valve Selenoid 3 Sudah Dimatikan");
        } else {
          myBot.sendMessage(msg, "Aktifkan Dahulu mode manual");
        }
      }

      else if (msgText == "/valve4on") {
        if (flagKontrol == "manual") {
          digitalWrite(selenoidValvePin4, LOW);
          digitalWrite(pompa2, LOW);
          myBot.sendMessage(msg, "Valve Selenoid 4 Sudah Dinyalakan");
        } else {
          myBot.sendMessage(msg, "Aktifkan Dahulu mode manual");
        }
      }

      else if (msgText == "/valve4off") {
        if (flagKontrol == "manual") {
          digitalWrite(selenoidValvePin4, HIGH);
          digitalWrite(pompa2, HIGH);
          myBot.sendMessage(msg, "Valve Selenoid 4 Sudah Dimatikan");
        } else {
          myBot.sendMessage(msg, "Aktifkan Dahulu mode manual");
        }
      }

      else if (msgText == "/auto") {
        flagKontrol = "auto";
        myBot.sendMessage(msg, "Mode sudah diubah ke Auto");
      }

      else if (msgText == "/manual") {
        flagKontrol = "manual";
        myBot.sendMessage(msg, "Mode sudah diubah ke Manual");
      }
      else if (msgText == "/start") {
        String f = myBot.getBotName();
        String welcome = "Selamat Datang Di SmartGreen Control. @" + f + "\n";
        welcome += "Kontrol SmartGreen Anda Melalui Telegram\n";
        welcome += "Gunakan Kode Berikut Untuk Mengontrol dan mendapatkan Status sensor :\n\n";

        welcome += "# AUTO/MANUAL Tipe Kontrol :\n";
        welcome += "/manual";
        welcome += " - Mode Manual\n";
        welcome += "/auto";
        welcome += " - Mode Auto\n\n";

        welcome += "# STATUS Sensor dan Ketinggian Air :\n";
        welcome += "/status";
        welcome += " - Tekan STATUS\n\n";

        welcome += "# ON/OFF Valve Selenoid 1 :\n";
        welcome += "/valve1on";
        welcome += " - Tekan ON\n";
        welcome += "/valve1off";
        welcome += " - Tekan OFF\n\n";

        welcome += "# ON/OFF Valve Selenoid 2 :\n";
        welcome += "/valve2on";
        welcome += " - Tekan ON\n";
        welcome += "/valve2off";
        welcome += " - Tekan OFF\n\n";

        welcome += "# ON/OFF Valve Selenoid 3 :\n";
        welcome += "/valve3on";
        welcome += " - Tekan ON\n";
        welcome += "/valve3off";
        welcome += " - Tekan OFF\n\n";

        welcome += "# ON/OFF Valve Selenoid 4 :\n";
        welcome += "/valve4on";
        welcome += " - Tekan ON\n";
        welcome += "/valve4off";
        welcome += " - Tekan OFF\n\n";

        myBot.sendMessage(msg, welcome);
      }
    }
  }
}

void lcd_print(String msg0, String msg1) {
  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(msg0);
  lcd.setCursor(0, 1);
  lcd.print(msg1);
}

void lcd_print_kelembaban(String msg0, String msg1, String msg2, String msg3) {
  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(msg0);
  lcd.setCursor(9, 0);
  lcd.print(msg1);
  lcd.setCursor(0, 1);
  lcd.print(msg2);
  lcd.setCursor(9, 1);
  lcd.print(msg3);
}

void initWifi() {
  WiFi.setAutoConnect(true);
  WiFi.mode(WIFI_STA);

  // connects to access point
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());
}

void initTelegram() {
#elif defined(ESP32)
  // Sync time with NTP
  configTzTime(MYTZ, "time.google.com", "time.windows.com", "pool.ntp.org");
  client.setCACert(telegram_cert);
#endif

  // Set the Telegram bot properies
  myBot.setUpdateTime(1000);
  myBot.setTelegramToken(token);

  // Check if all things are ok
  Serial.print("\nTest Telegram connection... ");
  myBot.begin() ? Serial.println("OK") : Serial.println("NOK");

  // https://core.telegram.org/bots/api#formatting-options
  myBot.setFormattingStyle(AsyncTelegram2::FormatStyle::HTML /* MARKDOWN */);

  // Send a welcome message to user when ready
  char welcome_msg[128];
  snprintf(welcome_msg, sizeof(welcome_msg),
           "BOT @%s online.\n/start for command list.",
           myBot.getBotName());

  // Check the userid with the help of bot @JsonDumpBot or @getidsbot (work also with groups)
  // https://t.me/JsonDumpBot  or  https://t.me/getidsbot
  myBot.sendTo(userid, welcome_msg);
}
