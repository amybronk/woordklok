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