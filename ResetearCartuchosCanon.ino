#include <Wire.h>

// Dirección I2C del chip EEPROM (0x50-0x57 para 24LC16B)
#define EEPROM_ADDR 0x50

// Datos de reset (primeros 32 bytes típicos para resetear)
const uint8_t reset_data[32] = {
  0x58, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x4D, 0x2D, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  Serial.println("Iniciando reset del cartucho de mantenimiento...");
  
  // 1. Escribir datos de reset en los primeros 32 bytes
  Serial.println("Escribiendo datos de reset...");
  for (int i = 0; i < 32; i++) {
    writeEEPROM(EEPROM_ADDR, i, reset_data[i]);
    delay(10);
    Serial.print(".");
  }
  
  // 2. Rellenar el resto con 0x00 (opcional pero recomendado)
  Serial.println("\nLimpiando resto de la EEPROM...");
  for (int i = 32; i < 2048; i++) {
    writeEEPROM(EEPROM_ADDR, i, 0x00);
    if (i % 64 == 0) Serial.print(".");
    delay(5);
  }
  
  // 3. Verificar escritura
  Serial.println("\nVerificando escritura...");
  bool error = false;
  for (int i = 0; i < 32; i++) {
    uint8_t val = readEEPROM(EEPROM_ADDR, i);
    if (val != reset_data[i]) {
      Serial.print("Error en dirección ");
      Serial.print(i);
      Serial.print(" (Escrito: 0x");
      Serial.print(reset_data[i], HEX);
      Serial.print(", Leído: 0x");
      Serial.print(val, HEX);
      Serial.println(")");
      error = true;
    }
  }
  
  if (!error) {
    Serial.println("\nRESET EXITOSO! El cartucho debería funcionar ahora.");
    Serial.println("Retira el cartucho y vuelve a insertarlo en la impresora.");
  } else {
    Serial.println("\nHubo errores en la escritura. Verifica las conexiones.");
  }
}

void loop() {}

// Función para escribir un byte en la EEPROM (CORREGIDA)
void writeEEPROM(uint8_t devAddr, uint16_t addr, uint8_t data) {
  uint8_t actualAddr = devAddr | ((addr >> 8) & 0x07); // Convertir a 8-bit
  Wire.beginTransmission(actualAddr);
  Wire.write(addr & 0xFF);
  Wire.write(data);
  Wire.endTransmission();
}

// Función para leer un byte de la EEPROM (CORREGIDA)
uint8_t readEEPROM(uint8_t devAddr, uint16_t addr) {
  uint8_t actualAddr = devAddr | ((addr >> 8) & 0x07); // Convertir a 8-bit
  Wire.beginTransmission(actualAddr);
  Wire.write(addr & 0xFF);
  Wire.endTransmission();
  Wire.requestFrom(actualAddr, 1);
  return Wire.available() ? Wire.read() : 0xFF;
}