const int STOP_CONTROL = 2;
const int WALK_CONTROL = 4;
const int UNIVERSAL_ECHO = 3;
const int TRIG_A_HUMAN_A = 5;
const int TRIG_B_HUMAN_A = 6;
const int TRIG_A_HUMAN_B = 9;
const int TRIG_B_HUMAN_B = 10;
const int TRIG_A_CAR_A = 7;
const int TRIG_B_CAR_A = 8;
const int TRIG_A_CAR_B = 11;
const int TRIG_B_CAR_B = 12;
const int PIN_BUZZER = 13;
const int BUZZER_FREQUENCE = 2000;
const int HUMAN_DISTANCE_THRESHOLD = 50;
const int CAR_STOP_THRESHOLD = 100;
const int CAR_STOP_TIME = 2000; // 2 seconds
const unsigned long WALK_CYCLE_DURATION = 10000; // 10 seconds
const unsigned long CROSS_CYCLE_DELAY = 20000; // 20 seconds

unsigned long carAStopStartTime = 0;
unsigned long carBStopStartTime = 0;
bool carAStopped = false;
bool carBStopped = false;
unsigned long lastCrossCycleTime = 0;
bool firstRun = true; 

void setup() {
  pinMode(STOP_CONTROL, OUTPUT);
  pinMode(WALK_CONTROL, OUTPUT);
  pinMode(UNIVERSAL_ECHO, INPUT);
  pinMode(TRIG_A_HUMAN_A, OUTPUT);
  pinMode(TRIG_B_HUMAN_A, OUTPUT);
  pinMode(TRIG_A_HUMAN_B, OUTPUT);
  pinMode(TRIG_B_HUMAN_B, OUTPUT);
  pinMode(TRIG_A_CAR_A, OUTPUT);
  pinMode(TRIG_B_CAR_A, OUTPUT);
  pinMode(TRIG_A_CAR_B, OUTPUT);
  pinMode(TRIG_B_CAR_B, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  checkCarStatus();

  bool isCrossTime = firstRun || millis() - lastCrossCycleTime >= CROSS_CYCLE_DELAY;
  bool requestToCross = (carAStopped || carBStopped) && (humanAIsPresent() || humanBIsPresent());

  Serial.print("isCrossTime: ");
  Serial.println(isCrossTime ? "true" : "false");
  Serial.print("requestToCross: ");
  Serial.println(requestToCross ? "true" : "false");

  if (isCrossTime && requestToCross) {
    startWalkCycle();
    lastCrossCycleTime = millis();
    firstRun = false;
  } else {
    stopWalkCycle();
  }

  delay(1000);
}

int getDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH);
  // Calculate the distance in centimeters
  int distance = duration / 58.2;
  return distance;
}

bool humanAIsPresent() {
  int humanASensorADistance = getDistance(TRIG_A_HUMAN_A, UNIVERSAL_ECHO);
  int humanASensorBDistance = getDistance(TRIG_B_HUMAN_A, UNIVERSAL_ECHO);

  // Print distances for debugging
  Serial.print("Distance Human A Sensor A: ");
  Serial.println(humanASensorADistance);
  Serial.print("Distance Human A Sensor B: ");
  Serial.println(humanASensorBDistance);

  // Check if both sensors detect a presence
  return (humanASensorADistance < HUMAN_DISTANCE_THRESHOLD && humanASensorBDistance < HUMAN_DISTANCE_THRESHOLD);
}

bool humanBIsPresent() {
  int humanBSensorADistance = getDistance(TRIG_A_HUMAN_B, UNIVERSAL_ECHO);
  int humanBSensorBDistance = getDistance(TRIG_B_HUMAN_B, UNIVERSAL_ECHO);

  // Print distances for debugging
  Serial.print("Distance Human B Sensor A: ");
  Serial.println(humanBSensorADistance);
  Serial.print("Distance Human B Sensor B: ");
  Serial.println(humanBSensorBDistance);

  // Check if both sensors detect a presence
  return (humanBSensorADistance < HUMAN_DISTANCE_THRESHOLD && humanBSensorBDistance < HUMAN_DISTANCE_THRESHOLD);
}

void checkCarStatus() {
  int carASensorADistance = getDistance(TRIG_A_CAR_A, UNIVERSAL_ECHO);
  int carASensorBDistance = getDistance(TRIG_B_CAR_A, UNIVERSAL_ECHO);
  int carBSensorADistance = getDistance(TRIG_A_CAR_B, UNIVERSAL_ECHO);
  int carBSensorBDistance = getDistance(TRIG_B_CAR_B, UNIVERSAL_ECHO);

  // Print distances for debugging
  Serial.print("Distance Car A Sensor A: ");
  Serial.println(carASensorADistance);
  Serial.print("Distance Car A Sensor B: ");
  Serial.println(carASensorBDistance);
  Serial.print("Distance Car B Sensor A: ");
  Serial.println(carBSensorADistance);
  Serial.print("Distance Car B Sensor B: ");
  Serial.println(carBSensorBDistance);

  // Check if car A is stopped
  if (carASensorADistance < CAR_STOP_THRESHOLD && carASensorBDistance < CAR_STOP_THRESHOLD) {
    if (!carAStopped) {
      carAStopStartTime = millis();
      carAStopped = true;
    } else if (millis() - carAStopStartTime >= CAR_STOP_TIME) {
      carAStopped = true;
    }
  } else {
    carAStopped = false;
  }

  // Check if car B is stopped
  if (carBSensorADistance < CAR_STOP_THRESHOLD && carBSensorBDistance < CAR_STOP_THRESHOLD) {
    if (!carBStopped) {
      carBStopStartTime = millis();
      carBStopped = true;
    } else if (millis() - carBStopStartTime >= CAR_STOP_TIME) {
      carBStopped = true;
    }
  } else {
    carBStopped = false;
  }
}

void startWalkCycle() {
  digitalWrite(WALK_CONTROL, HIGH);
  digitalWrite(STOP_CONTROL, LOW);
  tone(PIN_BUZZER, BUZZER_FREQUENCE);
  delay(WALK_CYCLE_DURATION);
  stopWalkCycle();
}

void stopWalkCycle() {
  digitalWrite(WALK_CONTROL, LOW);
  digitalWrite(STOP_CONTROL, HIGH);
  noTone(PIN_BUZZER);
}
