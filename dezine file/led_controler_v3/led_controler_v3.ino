/*
Naam:    klok_controler
Date:    9-9-2024
Maaker:  Ype Brinkman
Klas:    21eca
school:  roc van twente
*/

/*
programa Uploaden.

stap 1
arduno ide opennen 
ga dan naar "File , prefrence" of klik "ctrl + ,"
Additional boards maneger URLs. "https://dl.espressif.com/dl/package_esp32_index.json"
klik "ok"
wacht tot douwnload klaar is.

stap 2
open library maneger en zoek
"ArduinoJson" by Benoit Blanchon,
"FastLED" by Daniel Garcia,
"Rtc by Makuna" by Michael C. Miller.
de andere libraries zijn gedouwnload met de vorige stap of zijn arduino standaart

Stap 3
conect de klok met data cable aan pc
ga naar       "tools , board , esp32 , 4D Systems gen4-ESP32 16MB Modules"
ga dan naar   "tools , port , ComX (ESP32 Family Device)"

Stap 4 
"ctrl + U" wacht tot die zecht "upload voltoid" dan ben je klaar
alls je een error krijgt probeer stap 3 opniew of ga naar stap 2 en verander de versies naar
"ArduinoJson" by Benoit Blanchon      7.2.1 ,
"FastLED" by Daniel Garcia            3.9.4 ,
"Rtc by Makuna" by Michael C. Miller  2.5.0 .

*/
//json lib
#include <ArduinoJson.h>  // https://arduinojson.org/?utm_source=meta&utm_medium=library.properties

//wifi LIBS
#include <WiFi.h>  // esp32 native wifi manager.
#include "FS.h"    // nodig voor wifi.
#include "time.h"  // esp32 native ntp manager (netwerk tijd protocol).

//LED LIBS
#include <FastLED.h>  // library voor de leds die gebruikt worden.  https://github.com/FastLED/FastLED

//sd lib
#include <SPI.h>          // SPI library
#include <SD.h>           // sdcard library
#include <ThreeWire.h>    // miso mosi library

//RTC lib
#include <RtcDS1302.h>  // real time clok ds1304 lib.  https://github.com/Makuna/Rtc/wiki


struct LedConfig {
  uint8_t pin;
  uint8_t offset;
  uint8_t length;
};

// woorden
// Initialiseer alle LED-configuraties
LedConfig HET = { 2, 1, 3 };      //
LedConfig IS = { 0, 1, 2 };       //
LedConfig VIJF_ = { 5, 1, 4 };    //
LedConfig TIEN_ = { 10, 1, 4 };   //
LedConfig VOOR = { 23, 1, 4 };    //
LedConfig OVER = { 19, 1, 4 };    //
LedConfig KWART = { 14, 1, 5 };   //
LedConfig HALF_ = { 27, 1, 4 };   //
LedConfig OVER_ = { 31, 1, 4 };   //
LedConfig VOOR_ = { 35, 1, 4 };   //
LedConfig EEN = { 39, 1, 3 };     //
LedConfig TWEE = { 53, 1, 4 };    //
LedConfig DRIE = { 49, 1, 4 };    //
LedConfig VIER = { 43, 1, 4 };    //
LedConfig VIJF = { 59, 1, 4 };    //
LedConfig ZES = { 64, 1, 3 };     //
LedConfig ZEVEN = { 68, 1, 5 };   //
LedConfig ACHT = { 82, 1, 4 };    //
LedConfig NEGEN = { 77, 1, 5 };   //
LedConfig TIEN = { 72, 1, 4 };    //
LedConfig ELF = { 85, 1, 3 };     //
LedConfig TWALIF = { 88, 1, 6 };  //
LedConfig UUR = { 96, 1, 3 };     //
LedConfig AM_PM = { 109, 1, 1 };  //

LedConfig KLOK_1 = { 3, 12, 4 };
LedConfig LOSER = { 15, 11, 5 };
LedConfig PRO = { 6, 10, 3 };


// tijden
#define TIMEDIF 4
#define VIJF_1 5
#define TIEN_1 10
#define KWART_1 15
#define TIEN_2 20
#define VIJF_2 25
#define HALF 30
#define VIJF_3 35
#define TIEN_3 40
#define KWART_2 45
#define TIEN_4 50
#define VIJF_4 55

#define COLOR_ORDER GRB   // kleur volgorde van de WS2812B led type.
#define LED_SORT WS2812B  // led type.
#define NUM_LEDS 140      //hoeveelhijd ledjes (met extra voor de bassis plaat dat je het programaniet hoeft te veranderen met grotere ledarrays)
#define VOLTS 5       // max volts 
#define MAX_AMPS 500  // mA.
#define UPDATES_PER_SECOND 30

#define DATA_PIN 5
#define LDR 1

#define BUTON_1 45
#define BUTON_2 2
#define BUTON_3 3
#define BUTON_4 4
#define BUTON_5 5
#define BUTON_6 6
#define BUTON_7 7
#define BUTON_8 8
#define BUTON_9 9

#define SD_MOSI 13
#define SD_MISO 46
#define SD_SCLK 21
#define SD_CS 9

#define RST_PIN 10
#define DAT_PIN 11
#define CLK_PIN 12

ThreeWire myWire(DAT_PIN, CLK_PIN, RST_PIN);  // DAT, CLK, RST pinnen
RtcDS1302<ThreeWire> rtc(myWire);

CRGB leds[NUM_LEDS];

int gmtOffset_sec = 3600;
int daylightOffset_sec = 3600;

LedConfig words[30];  // Array om max. 30 woorden op te slaan
int wordCount = 0;    // Werkelijke aantal woorden dat geladen is

// Instellingen variabelen
char ssid[32] = "Galaxy A34 esp";
char password[32] = "jodijomensen";
uint8_t R = 255;
uint8_t G = 0;
uint8_t B = 0;
uint8_t H = 50;
bool LDR_A = false;
bool WiFi_A = false;
char ntpServer[16] = "pool.ntp.org";
uint16_t timeout = 25000;
uint8_t vorm = 0;
bool test_mode = 1;

uint8_t hours, minutes, x = 0;

File myFile;


void setup();
void loop();
bool loadConfig();
bool saveConfig();
bool loadWordsConfig();
void update_time();
void led_set(LedConfig config);
void printtime();
void start_wifintp_server();
void disconnect_wifintp_server();
void get_net_time();
void wifi_login();  // testmode only

void setup() {
  if (test_mode == 1) {
    Serial.begin(115200);
    delay(2000); // Wacht even om de seriële monitor te initialiseren
    Serial.println("Serial klaar");
  }

  // Configureer knoppen als input met interne pull-up weerstanden
  pinMode(BUTON_1, INPUT_PULLUP);
  pinMode(BUTON_2, INPUT_PULLUP);
  pinMode(BUTON_3, INPUT_PULLUP);
  pinMode(BUTON_4, INPUT_PULLUP);
  pinMode(BUTON_5, INPUT_PULLUP);
  pinMode(BUTON_6, INPUT_PULLUP);
  pinMode(BUTON_7, INPUT_PULLUP);
  pinMode(BUTON_8, INPUT_PULLUP);
  pinMode(BUTON_9, INPUT_PULLUP);
  pinMode(DATA_PIN, INPUT_PULLUP);
  pinMode(LDR, INPUT_PULLUP);

  // Initialiseer de LED-strip
  FastLED.addLeds<LED_SORT, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(H);

  // Initialiseer de SPI-bus en de SD-kaart
  SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS, SPI, 4000000)) { // 4 MHz SPI snelheid
    if (test_mode == 1) {
      Serial.println("SD Card MOUNT FAIL");
    }
    return; // Stop als de SD-kaart niet werkt
  } else if (test_mode == 1) {
    Serial.println("SD Card MOUNT SUCCESS");
  }

  // Controleer het type SD-kaart
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card detected");
    return;
  }
  if (test_mode == 1) {
    Serial.print("SD Card Type: ");
    switch (cardType) {
      case CARD_MMC: Serial.println("MMC"); break;
      case CARD_SD: Serial.println("SDSC"); break;
      case CARD_SDHC: Serial.println("SDHC"); break;
      default: Serial.println("UNKNOWN"); break;
    }
  }

  // Laad configuratie vanaf de SD-kaart
  if (!loadConfig()) {
    if (test_mode == 1) {
      Serial.println("Failed to load configuration, using defaults");
    }
  }

  // Laad woordconfiguratie vanaf de SD-kaart
  if (!loadWordsConfig()) {
    if (test_mode == 1) {
      Serial.println("Using default word configuration");
    }
  }

  // Controleer de grootte van de SD-kaart
  uint32_t cardSize = SD.cardSize() / (1024 * 1024);
  if (test_mode == 1) {
    Serial.print("SD Card Size: ");
    Serial.print(cardSize);
    Serial.println(" MB");
  }

  // Wi-Fi-instellingen initialiseren als WiFi_A is uitgeschakeld
  if (!WiFi_A) {
    if (strlen(ssid) == 0 || strlen(password) == 0) {
      if (test_mode == 1) {
        Serial.println("No values saved for ssid or password");
      }
    } else {
      start_wifintp_server();
      delay(100);
      get_net_time();
      delay(100);
      update_time();
      delay(100);
      disconnect_wifintp_server();
      delay(100);
    }
  }
}

void loop() {
  update_time();
  FastLED.setBrightness(H);
  delay(1000);
}

// leest de setings van de sd-kaart
bool loadConfig() {
  if (!SD.begin(10)) {  // Controleer de SD-kaart, pas de CS-pin aan
    Serial.println("Failed to initialize SD card");
    return false;
  }

  File file = SD.open("/config.json", FILE_READ);
  if (!file) {
    Serial.println("Failed to open config file");
    return false;
  }

  // Vergroot buffer indien nodig
  StaticJsonDocument<1024> doc;

  // JSON parsing met foutdiagnostiek
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.print("JSON parse error: ");
    Serial.println(error.c_str());
    file.close();
    return false;
  }

  // JSON-waarden uitlezen met foutafhandeling
  strlcpy(ssid, doc["ssid"] | "default_ssid", sizeof(ssid));
  strlcpy(password, doc["password"] | "default_password", sizeof(password));
  R = doc["R"] | 255;
  G = doc["G"] | 0;
  B = doc["B"] | 55;
  H = doc["H"] | 100;
  LDR_A = doc["LDR_A"] | false;
  WiFi_A = doc["WiFi_A"] | false;
  strlcpy(ntpServer, doc["ntpServer"] | "pool.ntp.org", sizeof(ntpServer));
  timeout = doc["timeout"] | 25000;
  vorm = doc["vorm"] | 0;
  test_mode = doc["test_mode"] | 1;

  file.close();

  Serial.println("Config loaded successfully");
  return true;
}

// Functie om instellingen op de SD-kaart op te slaan
bool saveConfig() {
  File file = SD.open("/config.json", FILE_WRITE);
  if (!file) {
    if (test_mode == 1) {
      Serial.println("Failed to open config file for writing");
    }
    return false;
  }

  StaticJsonDocument<512> doc;
  doc["ssid"] = ssid;
  doc["password"] = password;
  doc["R"] = R;
  doc["G"] = G;
  doc["B"] = B;
  doc["H"] = H;
  doc["LDR_A"] = LDR_A;
  doc["WiFi_A"] = WiFi_A;
  doc["ntpServer"] = ntpServer;
  doc["timeout"] = timeout;

  if (serializeJson(doc, file) == 0) {
    if (test_mode == 1) {
      Serial.println("Failed to write config file");
    }
    file.close();
    return false;
  }

  file.close();
  if (test_mode == 1) {
    Serial.println("Config saved successfully");
  }
  return true;
}
// laad de vorm van de sd-kaart met failsave
bool loadWordsConfig() {
  // Declareer de file-variabele hier
  File file;

  // Kies het juiste bestand op basis van 'vorm'
  switch (vorm) {
    case 0:
      file = SD.open("/vierkant.json", FILE_READ);
      break;
    case 1:
      file = SD.open("/ovaal.json", FILE_READ);
      break;
    case 2:
      file = SD.open("/strip.json", FILE_READ);
      break;
    case 3:
      file = SD.open("/custom1.json", FILE_READ);
      break;
    case 4:
      file = SD.open("/custom2.json", FILE_READ);
      break;
    case 5:
      file = SD.open("/custom3.json", FILE_READ);
      break;
    default:
      file = SD.open("/vierkant.json", FILE_READ);
      break;
  }

  if (!file) {
    if (test_mode == 1) {
      Serial.println("Failed to open words configuration file");
    }
    return false;
  }

  StaticJsonDocument<2048> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    if (test_mode == 1) {
      Serial.println("Failed to parse words configuration");
    }
    file.close();
    return false;
  }

  wordCount = 0;
  for (JsonObject word : doc.as<JsonArray>()) {
    words[wordCount].pin = word["pin"];
    words[wordCount].offset = word["offset"];
    words[wordCount].length = word["length"];
    wordCount++;
  }

  file.close();
  if (test_mode == 1) {
    Serial.println("Words configuration loaded successfully");
  }
  return true;
}

void update_time() {
  int uurcalc;
  // De tijd en datum uitlezen
  RtcDateTime now = rtc.GetDateTime();
  uurcalc = now.Hour();
  FastLED.clear();
  if (test_mode == 1) {
    Serial.print("het is ");
  }
  led_set(HET);
  led_set(IS);
  // vertaal de data naar woorden en addresen voor de LEDs
  if ((now.Minute() >= VIJF_1 && now.Minute() <= (VIJF_1 + TIMEDIF)) || (now.Minute() >= VIJF_3 && now.Minute() <= (VIJF_3 + TIMEDIF))) {
    if (test_mode == 1) {
      Serial.print("vijv over ");
    }
    led_set(VIJF_);
    led_set(OVER);
  } else if ((now.Minute() >= VIJF_2 && now.Minute() <= (VIJF_2 + TIMEDIF)) || (now.Minute() >= VIJF_4 && now.Minute() <= (VIJF_4 + TIMEDIF))) {
    if (test_mode == 1) {
      Serial.print("vijv voor ");
    }
    led_set(VIJF_);
    led_set(VOOR);
  } else if ((now.Minute() >= TIEN_1 && now.Minute() <= (TIEN_1 + TIMEDIF)) || (now.Minute() >= TIEN_3 && now.Minute() <= (TIEN_3 + TIMEDIF))) {
    if (test_mode == 1) {
      Serial.print("tien over ");
    }
    led_set(TIEN_);
    led_set(OVER);
  } else if ((now.Minute() >= TIEN_2 && now.Minute() <= (TIEN_2 + TIMEDIF)) || (now.Minute() >= TIEN_4 && now.Minute() <= (TIEN_4 + TIMEDIF))) {
    if (test_mode == 1) {
      Serial.print("tien voor ");
    }
    led_set(TIEN_);
    led_set(VOOR);
  } else if (now.Minute() >= KWART_1 && now.Minute() <= (KWART_1 + TIMEDIF)) {
    if (test_mode == 1) {
      Serial.print("kwart over ");
    }
    led_set(KWART);
    led_set(OVER_);
  } else if (now.Minute() >= KWART_2 && now.Minute() <= (KWART_2 + TIMEDIF)) {
    if (test_mode == 1) {
      Serial.print("kwart voor ");
    }
    led_set(KWART);
    led_set(VOOR_);
  } else {
    if (test_mode == 1) {
      Serial.print("    ");
    }
  }

  if (now.Minute() >= (HALF - 10) && now.Minute() <= (HALF + 14)) {
    if (test_mode == 1) {
      Serial.print("half ");
    }
    led_set(HALF_);
  }

  if (now.Hour() >= 13 && now.Hour() <= 24) {
    if (test_mode == 1) {
      Serial.print(" PM ");
    }
    uurcalc = uurcalc - 12;
  } 

  if ((uurcalc == 0 && now.Minute() >= 20) || (uurcalc == 12 && now.Minute() >= 20) || (uurcalc == 1 && now.Minute() <= 20)) {
    if (test_mode == 1) {
      Serial.print("een");
    }
    led_set(EEN);
  } else if ((uurcalc == 1 && now.Minute() >= 20) || (uurcalc == 2 && now.Minute() <= 20)) {
    if (test_mode == 1) {
      Serial.print("twee");
    }
    led_set(TWEE);
  } else if ((uurcalc == 2 && now.Minute() >= 20) || (uurcalc == 3 && now.Minute() <= 20)) {
    if (test_mode == 1) {
      Serial.print("drie");
    }
    led_set(DRIE);
  } else if ((uurcalc == 3 && now.Minute() >= 20) || (uurcalc == 4 && now.Minute() <= 20)) {
    if (test_mode == 1) {
      Serial.print("vier");
    }
    led_set(VIER);
  } else if ((uurcalc == 4 && now.Minute() >= 20) || (uurcalc == 5 && now.Minute() <= 20)) {
    if (test_mode == 1) {
      Serial.print("vijf");
    }
    led_set(VIJF);
  } else if ((uurcalc == 5 && now.Minute() >= 20) || (uurcalc == 6 && now.Minute() <= 20)) {
    if (test_mode == 1) {
      Serial.print("zes");
    }
    led_set(ZES);
  } else if ((uurcalc == 6 && now.Minute() >= 20) || (uurcalc == 7 && now.Minute() <= 20)) {
    if (test_mode == 1) {
      Serial.print("zeven");
    }
    led_set(ZEVEN);
  } else if ((uurcalc == 7 && now.Minute() >= 20) || (uurcalc == 8 && now.Minute() <= 20)) {
    if (test_mode == 1) {
      Serial.print("acht");
    }
    led_set(ACHT);
  } else if ((uurcalc == 8 && now.Minute() >= 20) || (uurcalc == 9 && now.Minute() <= 20)) {
    if (test_mode == 1) {
      Serial.print("negen");
    }
    led_set(NEGEN);
  } else if ((uurcalc == 9 && now.Minute() >= 20) || (uurcalc == 10 && now.Minute() <= 20)) {
    if (test_mode == 1) {
      Serial.print("tien");
    }
    led_set(TIEN);
  } else if ((uurcalc == 10 && now.Minute() >= 20) || (uurcalc == 11 && now.Minute() <= 20)) {
    if (test_mode == 1) {
      Serial.print("elf");
    }
    led_set(ELF);
  } else if ((uurcalc == 11 && now.Minute() >= 20) || (uurcalc == 12 && now.Minute() <= 20)) {
    if (test_mode == 1) {
      Serial.print("twalif");
    }
    led_set(TWALIF);
  }
  if (test_mode == 1) {
    Serial.print("    ");
    Serial.println(now.Hour());
  }
  delay(1000);

  // easter eggs
  if (now.Hour() == 1 && now.Minute() == 1) {
    if (test_mode == 1) {
    }
  } else if (now.Hour() == 0 && now.Minute() == 0) {
    if (test_mode == 1) {
    }
    FastLED.clear();
  } else if (now.Hour() == 5 && now.Minute() == 59) {
    if (test_mode == 1) {
    }
  } else if (now.Hour() == 22 && now.Minute() == 0) {
    if (test_mode == 1) {
    }
  } else if (now.Hour() == 1 && now.Minute() == 1) {
    if (test_mode == 1) {
    }
  } else if (now.Hour() == 1 && now.Minute() == 1) {
    if (test_mode == 1) {
    }
  }
  FastLED.show(); // verstuurd de led array naar de LEDs
}
// update de led array die naar de LEDs word gestuurd
void led_set(LedConfig config) {
  for (int i = config.pin; i <= ((config.pin + config.length) - 1); i = i + config.offset) {
    leds[i] = CRGB(R, G, B);
  }
  if (test_mode == 1) {
    Serial.print("  :");
    Serial.print(config.pin);
  }
}
// haalt de tijd op van de RTC (deze functie word niet aangeroepen maar is er nog voor debuging)
void printtime() {
  // De tijd en datum uitlezen
  RtcDateTime now = rtc.GetDateTime();

  // Controleer of de datum en tijd geldig zijn
  if (!rtc.IsDateTimeValid()) {
    if (test_mode == 1) {
      Serial.println("RTC-tijd is niet geldig!");
    }
    return;
  }



  // De datum en tijd in een string opslaan
  char datestring[20];
  snprintf(datestring,
           sizeof(datestring),
           "%02u/%02u/%04u %02u:%02u:%02u",
           now.Month(),
           now.Day(),
           now.Year(),
           now.Hour(),
           now.Minute(),
           now.Second());

  // De datum en tijd afdrukken naar de seriële monitor
  if (test_mode == 1) {
    Serial.println(datestring);
  }

  return;
}

// wifi login lesen uit de seriele monitor (deze functie word niet aangeroepen maar is er nog voor debuging)
void wifi_login() {
  char tssid[32];
  char tpassword[32];

  // Vraag om de SSID
  Serial.println("Voer uw WiFi naam (SSID) in:");
  Serial.print("WiFi name: ");

  // Wacht op invoer van SSID
  while (true) {
    // Zorg ervoor dat de seriële buffer wordt leeggemaakt
    while (Serial.available() > 0) {
      // Leeg de buffer om oude gegevens te verwijderen
      Serial.read();
    }

    // Wacht totdat er iets in de seriële buffer is
    while (Serial.available() == 0) {
      // niets doen, wacht op invoer
    }

    // Lees de SSID
    Serial.readBytesUntil('\n', tssid, sizeof(tssid));
    tssid[strcspn(tssid, "\r\n")] = 0;  // Verwijder newline characters

    // Controleer of er daadwerkelijk iets is ingevoerd
    if (strlen(tssid) > 0) {
      break;  // Breek de lus als er een SSID is ontvangen
    } else {
      Serial.println("Geen SSID ingevoerd. Probeer het opnieuw:");
      Serial.print("WiFi name: ");
    }
  }

  // Vraag om het wachtwoord
  Serial.println("Voer uw WiFi wachtwoord in:");
  Serial.print("WiFi password: ");

  // Wacht op invoer van het wachtwoord
  while (true) {
    // Zorg ervoor dat de seriële buffer wordt leeggemaakt
    while (Serial.available() > 0) {
      // Leeg de buffer om oude gegevens te verwijderen
      Serial.read();
    }

    // Wacht totdat er iets in de seriële buffer is
    while (Serial.available() == 0) {
      // niets doen, wacht op invoer
    }

    // Lees het wachtwoord
    Serial.readBytesUntil('\n', tpassword, sizeof(tpassword));
    tpassword[strcspn(tpassword, "\r\n")] = 0;  // Verwijder newline characters

    // Controleer of er daadwerkelijk iets is ingevoerd
    if (strlen(tpassword) > 0) {
      break;  // Breek de lus als er een wachtwoord is ontvangen
    } else {
      Serial.println("Geen wachtwoord ingevoerd. Probeer het opnieuw:");
      Serial.print("WiFi password: ");
    }
  }

  Serial.println("Nieuwe WiFi-gegevens opgeslagen in NVS.");
}

// Functie om aan de WiFi teverbinding en NTP server openen.
void start_wifintp_server() {
  if (test_mode == 1) {
    Serial.print("Connecting to ");
    Serial.println(ssid);
  }
  WiFi.begin(ssid, password);  // probeer met wifi te verbinden.

  unsigned long startAttemptTime = millis();  // Houd bij wanneer de poging begint.

  while (true) {
    if (test_mode == 1) {
      Serial.print(".");
    }
    delay(250);

    // Check of WiFi verbonden is
    if (WiFi.status() == WL_CONNECTED) {
      if (test_mode == 1) {
        Serial.println("Verbonden!");
      }
      break;  // Verlaat de loop als WiFi verbonden is
    }

    // Check of de tijd is verlopen (time-out)
    if (millis() - startAttemptTime >= timeout) {
      if (test_mode == 1) {
        Serial.println("Time-out bereikt, stop poging");
      }
      break;  // Verlaat de loop als het te lang duurt
    }
  }

  // Controleer of de verbinding uiteindelijk is gelukt
  if (WiFi.status() != WL_CONNECTED) {
    if (test_mode == 1) {
      Serial.println("Failed to connect, try again");
    }
    return;
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);  // verbint ntp server.
  delay(1000);
  return;
}

// Functie om WiFi verbinding en NTP los te koppelen
void disconnect_wifintp_server() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.disconnect();  // Verbreek de WiFi-verbinding
    if (test_mode == 1) {
      Serial.println("WiFi verbinding verbroken.");
    }
  } else {
    if (test_mode == 1) {
      Serial.println("WiFi was niet verbonden.");
    }
  }
}
// vragt de huidige tijd en datum aan de verbonden NTP server.
void get_net_time() {
  struct tm timeinfo;
  delay(100);

  if (!getLocalTime(&timeinfo)) {
    if (test_mode == 1) {
      Serial.println("Failed to obtain time");
    }
    return;
  }
  if (test_mode == 1) {
    Serial.print(timeinfo.tm_mday);
    Serial.print("/");
    Serial.print(timeinfo.tm_mon + 1);
    Serial.print("/");
    Serial.print(timeinfo.tm_year + 1900);
    Serial.print("    ");
    Serial.print(timeinfo.tm_hour);
    Serial.print(":");
    Serial.print(timeinfo.tm_min);
    Serial.print(":");
    Serial.print(timeinfo.tm_sec);
    Serial.print("     ; daylite savings time ==>");
    Serial.print(timeinfo.tm_isdst);
  }
  // De tijd en datum instellen
  rtc.SetIsRunning(true);
  RtcDateTime setTime = RtcDateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);  // jaar, maand, dag, uur, min, sec
  rtc.SetDateTime(setTime);
  if (test_mode == 1) {
    Serial.println("Datum en tijd ingesteld");
    Serial.println(timeinfo.tm_year + 1900);
  }
  delay(100);
  return;
}
