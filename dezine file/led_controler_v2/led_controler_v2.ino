//SUPORT LIBS
#include "FS.h"
#include "time.h"        // esp32 native ntp manager (netwerk tijd protocol).
#include <SPI.h>
#include <WiFi.h>        // esp32 native wifi manager.
#include <ThreeWire.h>
#include <Preferences.h> // esp32 native nvs manager (non voletile storeg).
#include <ArduinoJson.h>

//MAIN LIBS
#include <SD.h>
#include <FastLED.h>     // library voor de leds die gebruikt worden.
#include <RtcDS1302.h>   // real time clok ds1304 lib.


// woorden
#define HET 0
#define HET_O 1
#define HET_L 3

#define IS 4
#define IS_O 1
#define IS_L 2

#define VIJF_ 7
#define VIJF__O 1
#define VIJF__L 4

#define TIEN_ 11
#define TIEN__O 1
#define TIEN__L 4

#define VOOR 18
#define VOOR_O 1
#define VOOR_L 4

#define OVER 22
#define OVER_O 1
#define OVER_L 4

#define KWART 28
#define KWART_O 1
#define KWART_L 5

#define HALF 33
#define HALF_O 1
#define HALF_L 4

#define OVER_ 40
#define OVER__O 1
#define OVER__L 4

#define VOOR_ 44
#define VOOR__O 1
#define VOOR__L 4

#define EEN 51
#define EEN_O 1
#define EEN_L 3

#define TWEE 55
#define TWEE_O 1
#define TWEE_L 4

#define DRIE 62
#define DRIE_O 1
#define DRIE_L 4

#define VIER 66
#define VIER_O 1
#define VIER_L 4

#define VIJF 70
#define VIJF_O 1
#define VIJF_L 4

#define ZES 74
#define ZES_O 1
#define ZES_L 3

#define ZEVEN 77
#define ZEVEN_O 1
#define ZEVEN_L 5

#define ACHT 88
#define ACHT_O 1
#define ACHT_L 4

#define NEGEN 83
#define NEGEN_O 1
#define NEGEN_L 5

#define TIEN 92
#define TIEN_O 1
#define TIEN_L 4

#define ELF 96
#define ELF_O 1
#define ELF_L 3

#define TWALIF 99
#define TWALIF_O 1
#define TWALIF_L 6

#define UUR 106
#define UUR_O 1
#define UUR_L 3

#define AM_PM 109
#define AM_PM_O 1
#define AM_PM_L 1

#define KLOK_1 3
#define KLOK_O 12
#define KLOK_L 4

#define LOSER 15
#define LOSER_O 11
#define LOSER_L 5

#define PRO 6
#define PRO_O 10
#define PRO_L 3

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

#define COLOR_ORDER GRB  // kleur volgorde van de WS2812B led type.
#define LED_SORT WS2812B // led type.
#define NUM_LEDS 140
#define VOLTS 5       // V.
#define MAX_AMPS 500 // mA.
#define UPDATES_PER_SECOND 30

#define DATA_PIN 1
#define LDR 5

#define BUTON_1 45
#define BUTON_2 
#define BUTON_3 
#define BUTON_4 
#define BUTON_5 
#define BUTON_6 
#define BUTON_7 
#define BUTON_8 
#define BUTON_9 

#define SD_MOSI  11
#define SD_MISO  13
#define SD_SCLK  12
#define SD_CS    10

#define RST_PIN 10
#define DAT_PIN 11
#define CLK_PIN 12

ThreeWire myWire(DAT_PIN, CLK_PIN, RST_PIN); // DAT, CLK, RST pinnen
RtcDS1302<ThreeWire> rtc(myWire);

CRGB leds[NUM_LEDS];

int gmtOffset_sec = 3600;
#define daylightOffset_sec 3600


struct date
{
  uint8_t sec;
  uint8_t min;
  uint8_t uur;
  uint8_t day;
  uint8_t mont;
  uint16_t jaar;
  uint8_t W_day;
  uint16_t Y_day;
  bool DSTF;
};

struct DEEL
{
  uint8_t een;
  uint8_t twee;
};

// Instellingen variabelen
char ssid[32] = "Galaxy A34 esp";
char password[32] = "jodijomensen";
uint8_t R = 255;
uint8_t G = 0;
uint8_t B = 55;
uint8_t H = 100;
bool LDR_A = false;
bool WiFi_A = false;
char ntpServer[16] = "pool.ntp.org";
uint16_t timeout = 25000;

uint8_t hours, minutes, x = 0;

date curent;
Preferences preferences; // NVS opslag object
File myFile;

void setup();
void loop();
bool loadConfig();
bool saveConfig();
void update_time();
void led_set(uint8_t startLED, uint8_t ofset, uint8_t numLED);
void printtime();
void start_wifintp_server();
void disconnect_wifintp_server();
void get_net_time();
void wifi_login();
DEEL split_char(char tijdelijk, char splits);
void control();

void setup() {
  Serial.begin(115200);
  delay(1000);

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

  // Initialiseer de SPI-bus met de juiste pinnen
  SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);

  // Start de SD-kaart verbinding
  if (!SD.begin(SD_CS, SPI, 4000000)) {  // 4 MHz SPI snelheid
    Serial.println("SD Card MOUNT FAIL");
    return;
  }
  Serial.println("SD Card MOUNT SUCCESS");

  // Laad instellingen van de SD-kaart
  if (!loadConfig()) {
    Serial.println("Failed to load configuration, using defaults");
  }

  // SD kaartgrootte controleren
  uint32_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.print("SD Card Size: ");
  Serial.print(cardSize);
  Serial.println(" MB");



  if(WiFi_A == 0) { // wifi
    preferences.begin("wifiCreds", false);

    // Haal opgeslagen SSID en wachtwoord op in de buffers
    preferences.getString("ssid", ssid, sizeof(ssid));
    preferences.getString("password", password, sizeof(password));

    if (strlen(ssid) == 0 || strlen(password) == 0)
    {
      Serial.println("No values saved for ssid or password");
    }
    else
    {
      start_wifintp_server();
    }
  }

  if(WiFi_A == 1) { // blutoot 
  }
}

void loop() {
}

// Functie om instellingen van SD-kaart te laden
bool loadConfig() {
  File file = SD.open("/config.json", FILE_READ);
  if (!file) {
    Serial.println("Failed to open config file");
    return false;
  }

  // Buffer voor JSON-bestand
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println("Failed to parse config file");
    file.close();
    return false;
  }

  // Lees de waarden uit het JSON-object
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

  file.close();
  Serial.println("Config loaded successfully");
  return true;
}

// Functie om instellingen op de SD-kaart op te slaan
bool saveConfig() {
  File file = SD.open("/config.json", FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open config file for writing");
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
    Serial.println("Failed to write config file");
    file.close();
    return false;
  }

  file.close();
  Serial.println("Config saved successfully");
  return true;
}

void update_time() {
  int uurcalc;
  // De tijd en datum uitlezen
  RtcDateTime now = rtc.GetDateTime();
  uurcalc = now.Hour();
  Serial.print(analogRead(ROODPIN));
  Serial.print(analogRead(GROENPIN));
  Serial.print(analogRead(BLOUWPIN));
  FastLED.clear();
  Serial.print("het is ");
  led_set(HET, HET_O, HET_L);
  led_set(IS, IS_O, IS_L);

  if ((now.Minute() >= VIJF_1 && now.Minute() <= (VIJF_1 + TIMEDIF)) || (now.Minute() >= VIJF_3 && now.Minute() <= (VIJF_3 + TIMEDIF)))
  {
    Serial.print("vijv over ");
    led_set(VIJF_, VIJF__O, VIJF__L);
    led_set(OVER, OVER_O, OVER_L);
  }
  else if ((now.Minute() >= VIJF_2 && now.Minute() <= (VIJF_2 + TIMEDIF)) || (now.Minute() >= VIJF_4 && now.Minute() <= (VIJF_4 + TIMEDIF)))
  {
    Serial.print("vijv voor ");
    led_set(VIJF_, VIJF__O, VIJF__L);
    led_set(VOOR, VOOR_O, VOOR_L);
  }
  else if ((now.Minute() >= TIEN_1 && now.Minute() <= (TIEN_1 + TIMEDIF)) || (now.Minute() >= TIEN_3 && now.Minute() <= (TIEN_3 + TIMEDIF)))
  {
    Serial.print("tien over ");
    led_set(TIEN_, TIEN__O, TIEN__L);
    led_set(OVER, OVER_O, OVER_L);
  }
  else if ((now.Minute() >= TIEN_2 && now.Minute() <= (TIEN_2 + TIMEDIF)) || (now.Minute() >= TIEN_4 && now.Minute() <= (TIEN_4 + TIMEDIF)))
  {
    Serial.print("tien voor ");
    led_set(TIEN_, TIEN__O, TIEN__L);
    led_set(VOOR, VOOR_O, VOOR_L);
  }
  else if (now.Minute() >= KWART_1 && now.Minute() <= (KWART_1 + TIMEDIF))
  {
    Serial.print("kwart over ");
    led_set(KWART, KWART_O, KWART_L);
    led_set(OVER_, OVER__O, OVER__L);
  }
  else if (now.Minute() >= KWART_2 && now.Minute() <= (KWART_2 + TIMEDIF))
  {
    Serial.print("kwart voor ");
    led_set(KWART, KWART_O, KWART_L);
    led_set(VOOR_, VOOR__O, VOOR__L);
  }
  else
  {
    Serial.print("    ");
  }

  if (now.Minute() >= (HALF - 10) && now.Minute() <= (HALF + 14))
  {
    Serial.print("half ");
    led_set(HALF, HALF_O, HALF_L);
  }

  if (now.Hour() >= 13 && now.Hour() <= 24)
  {
    Serial.print(" PM ");
    uurcalc = uurcalc - 12;
  }

  if ((uurcalc == 12 && now.Minute() >= 20) || (uurcalc == 1 && now.Minute() <= 20))
  {
    Serial.print("een");
    led_set(EEN, EEN_O, EEN_L);
  }
  else if ((uurcalc == 1 && now.Minute() >= 20) || (uurcalc == 2 && now.Minute() <= 20))
  {
    Serial.print("twee");
    led_set(TWEE, TWEE_O, TWEE_L);
  }
  else if ((uurcalc == 2 && now.Minute() >= 20) || (uurcalc == 3 && now.Minute() <= 20))
  {
    Serial.print("drie");
    led_set(DRIE, DRIE_O, DRIE_L);
  }
  else if ((uurcalc == 3 && now.Minute() >= 20) || (uurcalc == 4 && now.Minute() <= 20))
  {
    Serial.print("vier");
    led_set(VIER, VIER_O, VIER_L);
  }
  else if ((uurcalc == 4 && now.Minute() >= 20) || (uurcalc == 5 && now.Minute() <= 20))
  {
    Serial.print("vijf");
    led_set(VIJF, VIJF_O, VIJF_L);
  }
  else if ((uurcalc == 5 && now.Minute() >= 20) || (uurcalc == 6 && now.Minute() <= 20))
  {
    Serial.print("zes");
    led_set(ZES, ZES_O, ZES_L);
  }
  else if ((uurcalc == 6 && now.Minute() >= 20) || (uurcalc == 7 && now.Minute() <= 20))
  {
    Serial.print("zeven");
    led_set(ZEVEN, ZEVEN_O, ZEVEN_L);
  }
  else if ((uurcalc == 7 && now.Minute() >= 20) || (uurcalc == 8 && now.Minute() <= 20))
  {
    Serial.print("acht");
    led_set(ACHT, ACHT_O, ACHT_L);
  }
  else if ((uurcalc == 8 && now.Minute() >= 20) || (uurcalc == 9 && now.Minute() <= 20))
  {
    Serial.print("negen");
    led_set(NEGEN, NEGEN_O, NEGEN_L);
  }
  else if ((uurcalc == 9 && now.Minute() >= 20) || (uurcalc == 10 && now.Minute() <= 20))
  {
    Serial.print("tien");
    led_set(TIEN, TIEN_O, TIEN_L);
  }
  else if ((uurcalc == 10 && now.Minute() >= 20) || (uurcalc == 11 && now.Minute() <= 20))
  {
    Serial.print("elf");
    led_set(ELF, ELF_O, ELF_L);
  }
  else if ((uurcalc == 11 && now.Minute() >= 20) || (uurcalc == 12 && now.Minute() <= 20))
  {
    Serial.print("twalif");
    led_set(TWALIF, TWALIF_O, TWALIF_L);
  }
  FastLED.show();
  Serial.print("    ");
  Serial.println(now.Hour());
  delay(1000);

  // easter eggs
  if (now.Hour() == 1 && now.Minute() == 1)
  {
  }
  else if (now.Hour() == 0 && now.Minute() == 0)
  {
    FastLED.clear();
  }
  else if (now.Hour() == 5 && now.Minute() == 59)
  {
  }
  else if (now.Hour() == 22 && now.Minute() == 0)
  {
  }
  else if (now.Hour() == 1 && now.Minute() == 1)
  {
  }
  else if (now.Hour() == 1 && now.Minute() == 1)
  {
  }
}

void led_set(uint8_t startLED, uint8_t ofset, uint8_t numLED) {
  for (int i = startLED; i <= ((startLED + numLED) - 1); i = i + ofset)
  {
    leds[i] = CRGB(R, G, B);
  }
  Serial.print("  :");
  Serial.print(startLED);
}

void printtime() {
  // De tijd en datum uitlezen
  RtcDateTime now = rtc.GetDateTime();

  // Controleer of de datum en tijd geldig zijn
  if (!rtc.IsDateTimeValid())
  {
    Serial.println("RTC-tijd is niet geldig!");
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
  Serial.println(datestring);

  return;
}

void wifi_login() {
  char tssid[32];
  char tpassword[32];

  // Vraag om de SSID
  Serial.println("Voer uw WiFi naam (SSID) in:");
  Serial.print("WiFi name: ");

  // Wacht op invoer van SSID
  while (true)
  {
    // Zorg ervoor dat de seriële buffer wordt leeggemaakt
    while (Serial.available() > 0)
    {
      // Leeg de buffer om oude gegevens te verwijderen
      Serial.read();
    }

    // Wacht totdat er iets in de seriële buffer is
    while (Serial.available() == 0)
    {
      // niets doen, wacht op invoer
    }

    // Lees de SSID
    Serial.readBytesUntil('\n', tssid, sizeof(tssid));
    tssid[strcspn(tssid, "\r\n")] = 0; // Verwijder newline characters

    // Controleer of er daadwerkelijk iets is ingevoerd
    if (strlen(tssid) > 0)
    {
      break; // Breek de lus als er een SSID is ontvangen
    }
    else
    {
      Serial.println("Geen SSID ingevoerd. Probeer het opnieuw:");
      Serial.print("WiFi name: ");
    }
  }

  // Vraag om het wachtwoord
  Serial.println("Voer uw WiFi wachtwoord in:");
  Serial.print("WiFi password: ");

  // Wacht op invoer van het wachtwoord
  while (true)
  {
    // Zorg ervoor dat de seriële buffer wordt leeggemaakt
    while (Serial.available() > 0)
    {
      // Leeg de buffer om oude gegevens te verwijderen
      Serial.read();
    }

    // Wacht totdat er iets in de seriële buffer is
    while (Serial.available() == 0)
    {
      // niets doen, wacht op invoer
    }

    // Lees het wachtwoord
    Serial.readBytesUntil('\n', tpassword, sizeof(tpassword));
    tpassword[strcspn(tpassword, "\r\n")] = 0; // Verwijder newline characters

    // Controleer of er daadwerkelijk iets is ingevoerd
    if (strlen(tpassword) > 0)
    {
      break; // Breek de lus als er een wachtwoord is ontvangen
    }
    else
    {
      Serial.println("Geen wachtwoord ingevoerd. Probeer het opnieuw:");
      Serial.print("WiFi password: ");
    }
  }

  // Sla SSID en wachtwoord op in de NVS
  preferences.putString("ssid", tssid);
  preferences.putString("password", tpassword);
  Serial.println("Nieuwe WiFi-gegevens opgeslagen in NVS.");
}

void start_wifintp_server() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); // probeer met wifi te verbinden.

  unsigned long startAttemptTime = millis(); // Houd bij wanneer de poging begint.

  while (true)
  {
    Serial.print(".");
    delay(250);

    // Check of WiFi verbonden is
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("Verbonden!");
      break; // Verlaat de loop als WiFi verbonden is
    }

    // Check of de tijd is verlopen (time-out)
    if (millis() - startAttemptTime >= timeout)
    {
      Serial.println("Time-out bereikt, stop poging");
      break; // Verlaat de loop als het te lang duurt
    }
  }

  // Controleer of de verbinding uiteindelijk is gelukt
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Failed to connect, try again");
    return;
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); // verbint ntp server.
  delay(1000);
  return;
}

// Functie om WiFi verbinding en NTP los te koppelen
void disconnect_wifintp_server() {
  if (WiFi.status() == WL_CONNECTED)
  {
    WiFi.disconnect(); // Verbreek de WiFi-verbinding
    Serial.println("WiFi verbinding verbroken.");
  }
  else
  {
    Serial.println("WiFi was niet verbonden.");
  }
}

void get_net_time() {
  struct tm timeinfo;
  delay(100);

  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  curent = {timeinfo.tm_sec, timeinfo.tm_min, timeinfo.tm_hour, timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900, timeinfo.tm_wday, timeinfo.tm_yday, timeinfo.tm_isdst};
  Serial.print(curent.day);
  Serial.print("/");
  Serial.print(curent.mont);
  Serial.print("/");
  Serial.print(curent.jaar);
  Serial.print("    ");
  Serial.print(curent.uur);
  Serial.print(":");
  Serial.print(curent.min);
  Serial.print(":");
  Serial.print(curent.sec);
  // De tijd en datum instellen
  rtc.SetIsRunning(true);
  RtcDateTime setTime = RtcDateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec); // jaar, maand, dag, uur, min, sec
  rtc.SetDateTime(setTime);
  Serial.println("Datum en tijd ingesteld");
  Serial.println(curent.jaar);
  delay(100);
  return;
}

DEEL split_char(char tijdelijk[6], char splits[2]) {
  DEEL gedeelt;
  char *token = strtok(const_cast<char *>(tijdelijk), splits);
  if (token != nullptr)
  {
    gedeelt.een = atoi(token); // Zet de eerste token om naar uint8_t

    token = strtok(nullptr, splits);
    if (token != nullptr)
    {
      gedeelt.twee = atoi(token); // Zet de tweede token om naar uint8_t
    }
  }
  return gedeelt;
}

void control(){
  x = 0;
  while (true)
  {
    uint8_t condition;
    char tipe[2];
    Serial.println(xPortGetCoreID());
    Serial.println("verander lijst");
    Serial.println("1 = wifi naam en wachtwoord");
    Serial.println("2 = verander tijdzone");
    Serial.println("3 = alarm time / setings");
    Serial.println("4 = kleur / helderhijd  min / max setings");
    Serial.println("5 = klaar ");
    Serial.println("6 = reset ");
    Serial.println();

    while (Serial.available() == 0)
    {
    }
    Serial.readBytesUntil('\n', tipe, sizeof(tipe) - 1);
    tipe[sizeof(tipe) - 1] = '\0'; // Voeg null terminator toe
    condition = atoi(tipe);
    delay(500);

    if (condition == 1)
    {
      Serial.println("er word eerst gevraagt wat je wifi naam is en later je wactwoord.");
      Serial.println("dit word gedaan zodat een keer perdag verbint de clok met je wifi");
      Serial.println("om de tijd van de clok te vergelijken met die van het internet.");
      delay(500);
      wifi_login();
    }
    else if (condition == 2)
    {
      char tijdelijktijdzone[4]; // -12 tot 12 + null terminator
      Serial.println("Elke tijdzone heeft een getal dat loopt van -12 tot 12.");
      Serial.println("Zoek het getal op voor jouw tijdzone en stuur het.");
      Serial.println("Europa is +1.");

      while (Serial.available() == 0)
      {
      }
      Serial.readBytesUntil('\n', tijdelijktijdzone, sizeof(tijdelijktijdzone) - 1);
      tijdelijktijdzone[sizeof(tijdelijktijdzone) - 1] = '\0'; // Voeg null terminator toe

      int8_t tijdzone = atoi(tijdelijktijdzone);
      gmtOffset_sec = tijdzone * 3600;
      Serial.println("Tijdzone verandert.");
    }
    else if (condition == 3)
    {
      char tijdelijkalarmtijd[6]; // "00:00" + null terminator
      DEEL tijd;
      Serial.println("Vul in de tijd dat er een alarm af moet gaan in de vorm van 00:00 tot 23:59.");

      while (Serial.available() == 0)
      {
      }
      Serial.readBytesUntil('\n', tijdelijkalarmtijd, sizeof(tijdelijkalarmtijd) - 1);
      tijdelijkalarmtijd[sizeof(tijdelijkalarmtijd) - 1] = '\0'; // Voeg null terminator toe

      tijd = split_char(tijdelijkalarmtijd, ":");

      hours = tijd.een;
      minutes = tijd.twee;
    }
    else if (condition == 4)
    {
      char timecoloure[8];

      Serial.println("vull in de min en max met een warde die 0/255 is ");
      Serial.println("vull in alls 0/255 of min/max ");
      Serial.println("Rood");

      while (Serial.available() == 0)
      {
      }
      Serial.readBytesUntil('\n', timecoloure, sizeof(timecoloure) - 1);
      timecoloure[sizeof(timecoloure) - 1] = '\0'; // Voeg null terminator toe

      R = atoi(timecoloure);

      Serial.println("Groen");

      while (Serial.available() == 0)
      {
      }
      Serial.readBytesUntil('\n', timecoloure, sizeof(timecoloure) - 1);
      timecoloure[sizeof(timecoloure) - 1] = '\0'; // Voeg null terminator toe

      G = atoi(timecoloure);

      Serial.println("Blouw");

      while (Serial.available() == 0)
      {
      }
      Serial.readBytesUntil('\n', timecoloure, sizeof(timecoloure) - 1);
      timecoloure[sizeof(timecoloure) - 1] = '\0'; // Voeg null terminator toe

      B = atoi(timecoloure);

      Serial.println("Helderhijd");
      // lees helderhijd
    }
    else if (condition == 5)
    {
      break;
    }
    else if (condition == 6)
    {
      Serial.println("ben je zeker van het reseten zo ja klik reset op de achter kant van de klok");
      Serial.println("of trek de steker  voor 10 sec");
    }
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
  }
  return;
}