#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Mouse.h>

Adafruit_MPU6050 mpu;

// --- CONFIGURATION ---
float sensibiliteX = -5.0;  
float sensibiliteY = -5.0;  
float zoneMorte = 0.15;     
// ---------------------

// Attribution des pins pour les boutons
const int pinClicGauche = 4;
const int pinClicDroit = 5;

void setup() {
  Wire.begin();
  Mouse.begin();
  Serial.begin(115200);
  
  // Configuration des boutons avec la résistance interne de l'Arduino
  pinMode(pinClicGauche, INPUT_PULLUP);
  pinMode(pinClicDroit, INPUT_PULLUP);
  
  if (!mpu.begin()) {
    while (1) { 
      Serial.print("\e[2J\e[H");
      Serial.println("ERREUR : Capteur MPU6050 introuvable.");
      delay(1000); 
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ); 
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float vx = g.gyro.z; 
  float vy = g.gyro.y;

  int moveX = 0;
  int moveY = 0;

  if (abs(vx) > zoneMorte) moveX = (int)(vx * sensibiliteX);
  if (abs(vy) > zoneMorte) moveY = (int)(vy * sensibiliteY);

  if (moveX != 0 || moveY != 0) {
    Mouse.move(moveX, moveY, 0);
  }

  // --- GESTION DES CLICS ---
  // En mode INPUT_PULLUP, le bouton renvoie "LOW" quand on appuie dessus
  
  // Clic Gauche (Pin 4)
  if (digitalRead(pinClicGauche) == LOW) {
    if (!Mouse.isPressed(MOUSE_LEFT)) Mouse.press(MOUSE_LEFT);
  } else {
    if (Mouse.isPressed(MOUSE_LEFT)) Mouse.release(MOUSE_LEFT);
  }

  // Clic Droit (Pin 5)
  if (digitalRead(pinClicDroit) == LOW) {
    if (!Mouse.isPressed(MOUSE_RIGHT)) Mouse.press(MOUSE_RIGHT);
  } else {
    if (Mouse.isPressed(MOUSE_RIGHT)) Mouse.release(MOUSE_RIGHT);
  }

  // --- MONITEUR SÉRIE ÉPURÉ ---
  Serial.print("\e[2J\e[H"); 
  Serial.println("=== MANETTE VR INITIALISÉE ===");
  Serial.print("Clic Gauche (P4) : "); Serial.println(digitalRead(pinClicGauche) == LOW ? "APPUYÉ" : "RELACHÉ");
  Serial.print("Clic Droit  (P5) : "); Serial.println(digitalRead(pinClicDroit) == LOW ? "APPUYÉ" : "RELACHÉ");
  delay(15); 
}
