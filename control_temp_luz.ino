/*
  Control de temperatura e iluminacion
*/

#include "DHT.h"
#include <LiquidCrystal_I2C.h>

#define PIN_LUZ_1  5
#define PIN_LUZ_2  6
#define PIN_RELE_VENT  7
#define PIN_SEN_LUZ A0

#define UMBRAL_INF_TEMP  30 // Umbral de desactivacion de ventilador
#define UMBRAL_SUP_TEMP  31 // Umbral de activacion de ventilador
#define UMBRAL_INF_LUZ  5 // Umbral de activacion de luz
#define UMBRAL_SUP_LUZ  20 // Umbral de desactivacion de luz
#define CAL_LUZ_MIN 1024
#define CAL_LUZ_MAX 30

#define DHTPIN 4 //2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27,16,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

bool vent_on = false;
bool luz_on = false;

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println("****** Control de temperatura e Iluminacion ******");

  dht.begin();

  pinMode(PIN_LUZ_1, OUTPUT);
  pinMode(PIN_LUZ_2, OUTPUT);
  pinMode(PIN_RELE_VENT, OUTPUT);

  lcd.init(); // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Ctrl Temperatura");
  lcd.setCursor(2,1);
  lcd.print("e Iluminacion");

  delay(2000);
}

void loop() {
  // Wait a few seconds between measurements.
  

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float hum_rel = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temp = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  // float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(hum_rel) || isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  uint16_t lect_adc = analogRead(PIN_SEN_LUZ);
  uint16_t luz_amb = map(lect_adc, CAL_LUZ_MIN, CAL_LUZ_MAX, 0, 1000);

  Serial.println("-----------------------------------------------------------------");
  Serial.print(F("H. Rel: "));
  Serial.print(hum_rel);
  Serial.print(F("% | Temp: "));
  Serial.print(temp);
  Serial.print(F("°C | ADC: "));
  Serial.print(lect_adc);
  Serial.print(F(" | Luz: "));
  Serial.print(luz_amb);
  // Serial.print(F("u "));

  String temp_str = String(temp);
  String hum_rel_str = String(hum_rel);

  int   lon_arr_temp  = temp_str.length()+1;    //The +1 is for the 0x00h Terminator
  char  arr_temp[lon_arr_temp];
  temp_str.toCharArray(arr_temp, lon_arr_temp);

  int   lon_arr_hum_rel  = hum_rel_str.length()+1;    //The +1 is for the 0x00h Terminator
  char  arr_hum_rel[lon_arr_hum_rel];
  temp_str.toCharArray(arr_hum_rel, lon_arr_hum_rel);

  if(temp >= UMBRAL_SUP_TEMP) {
    vent_on = true;
  } else if(temp <= UMBRAL_INF_TEMP) {
    vent_on = false;
  }

  if(luz_amb <= UMBRAL_INF_LUZ) {
    luz_on = true;
  } else if(temp >= UMBRAL_SUP_LUZ) {
    luz_on = false;
  }
  
  lcd.clear();
  // Mostrar temperatura y humedad en display
  lcd.setCursor(0,0); lcd.print("T:");
  lcd.setCursor(2,0); lcd.print(temp_str);

  // lcd.setCursor(8,0); lcd.print("H:");
  // lcd.setCursor(10,0); lcd.print(hum_rel_str);

  lcd.setCursor(9,0); lcd.print("L:");
  lcd.setCursor(11,0); lcd.print(luz_amb);

  // Estado ventilador
  lcd.setCursor(0,1); lcd.print("V:");
  lcd.setCursor(2,1); 

  if(vent_on) {
    Serial.print(" | Vent ON");
    lcd.print("ON");
    digitalWrite(PIN_RELE_VENT, HIGH);
  } else {
    Serial.print(" | Vent OFF");
    lcd.print("OFF");
    digitalWrite(PIN_RELE_VENT, LOW);
  }

  // Estado iluminacion
  lcd.setCursor(9,1); lcd.print("I:");
  lcd.setCursor(11,1);

  if(luz_on) {
    Serial.println(" | Ilum ON");
    lcd.print("ON");
    digitalWrite(PIN_LUZ_1, HIGH);
    digitalWrite(PIN_LUZ_2, HIGH);
  } else {
    Serial.println(" | Ilum OFF");
    lcd.print("OFF");
    digitalWrite(PIN_LUZ_1, LOW);
    digitalWrite(PIN_LUZ_2, LOW);
  }
  
  Serial.println();

  delay(2000);
}
