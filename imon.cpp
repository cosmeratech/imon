// Config
const int TEMP_LIMIT = 65;           // °C
const int PROXIMITY_LIMIT = 50;      // cm
const int VIBRATION_LIMIT = 600;     // analog value

// Pins
const int buttonPin   = 8;
const int warningLED  = 13;
const int statusLED   = 12;
const int trigPin     = 6;
const int echoPin     = 9;
const int tempPin     = A0;
const int vibPin      = A1;

// Interrupt Variables
volatile bool buttonPressed = false;
volatile unsigned long echoStart = 0;
volatile unsigned long echoEnd = 0;
volatile bool echoDone = false;
volatile bool timerFlag = false;

// Setup
void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(warningLED, OUTPUT);
  pinMode(statusLED, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);

  // Pin Change Interrupt
  PCICR |= (1 << PCIE0);          // Enable PCINT for group 0
  PCMSK0 |= (1 << PCINT0);        // D8 (button)
  PCMSK0 |= (1 << PCINT1);        // D9 (echo)

  // Timer1 Setup (500 ms)
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 7812;                   // 500 ms @ 1024 prescaler
  TCCR1B |= (1 << WGM12);         // CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10); // prescaler 1024
  TIMSK1 |= (1 << OCIE1A);        // enable compare interrupt
  interrupts();
}

// Timer ISR
ISR(TIMER1_COMPA_vect) {
  timerFlag = true;
}

// Pin Change ISR
ISR(PCINT0_vect) {
  buttonPressed = digitalRead(buttonPin);

  if (digitalRead(echoPin) == HIGH)
    echoStart = micros();
  else {
    echoEnd = micros();
    echoDone = true;
  }
}

// Sensor Logic
int readTemperature() {
  int raw = analogRead(tempPin);
  float voltage = raw * (5.0 / 1023.0);
  return voltage * 100;           // LM35 → °C
}

int readVibration() {
  return analogRead(vibPin);
}

void triggerUltrasonic() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
}

long calculateDistance() {
  unsigned long duration = echoEnd - echoStart;
  echoDone = false;
  return (duration / 2) / 29.1;
}

// Safety Logic
void evaluateSystem(int temp, int vib, long dist) {

  if (temp > TEMP_LIMIT || vib > VIBRATION_LIMIT) {
    digitalWrite(warningLED, HIGH);
    digitalWrite(statusLED, LOW);
    Serial.println("CRITICAL: MACHINE FAULT");
  }
  else if (dist < PROXIMITY_LIMIT && !buttonPressed) {
    digitalWrite(warningLED, HIGH);
    digitalWrite(statusLED, LOW);
    Serial.println("WARNING: HUMAN TOO CLOSE");
  }
  else {
    digitalWrite(warningLED, LOW);
    digitalWrite(statusLED, HIGH);
    Serial.println("STATUS: NORMAL");
  }
}

// Loop
void loop() {

  triggerUltrasonic();

  if (echoDone) {
    long distance = calculateDistance();
    int temperature = readTemperature();
    int vibration = readVibration();

    evaluateSystem(temperature, vibration, distance);

    Serial.print("Temp: ");
    Serial.print(temperature);
    Serial.print(" | Vib: ");
    Serial.print(vibration);
    Serial.print(" | Dist: ");
    Serial.println(distance);
  }

  if (timerFlag) {
    timerFlag = false;
    Serial.println("TIMER: System check");
  }
}