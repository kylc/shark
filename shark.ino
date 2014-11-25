#define MOTOR_LEFT_ENABLE_PIN 3
#define MOTOR_LEFT_PIN1 4
#define MOTOR_LEFT_PIN2 5
#define MOTOR_RIGHT_ENABLE_PIN 6
#define MOTOR_RIGHT_PIN1 7
#define MOTOR_RIGHT_PIN2 8

#define SONAR_TRIGGER_PIN A2
#define SONAR_ECHO_PIN A3

#define MSG_SYNC_STR ("abcd")

struct state_t {
  float sonar_dist;
  long sonar_read_deadline;
  long remote_read_deadline;
};

struct remote_msg_t {
  float speed; // 0..1
  float angle; // -90..90
};

void setup() {
  pinMode(MOTOR_LEFT_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_LEFT_PIN1, OUTPUT);
  pinMode(MOTOR_LEFT_PIN2, OUTPUT);
  pinMode(MOTOR_RIGHT_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_RIGHT_PIN1, OUTPUT);
  pinMode(MOTOR_RIGHT_PIN2, OUTPUT);

  pinMode(SONAR_TRIGGER_PIN, OUTPUT);
  pinMode(SONAR_ECHO_PIN, INPUT);

  digitalWrite(MOTOR_LEFT_ENABLE_PIN, 1);
  digitalWrite(MOTOR_RIGHT_ENABLE_PIN, 1);
  
  Serial.begin(57600);
}

/**
 * Read from the sonar sensor. Returns the distance in cm.
 */
float read_sonar_dist() {
  // Tell the sensor we want to take a measurement.
  digitalWrite(SONAR_TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(SONAR_TRIGGER_PIN, LOW);

  // Sensor outputs a high pulse proportional to the distance.
  long pulse_width = pulseIn(SONAR_ECHO_PIN, HIGH, 5000);
  float dist = pulse_width / 58.0; // to cm

  return dist;
}

bool read_remote_msg(struct remote_msg_t *msg) {
  // Make sure there is data available in the buffer.
  if(Serial.available() < sizeof(struct remote_msg_t)) {
    return false;
  }

  // Skip to just past the synchronizing string.
  Serial.find(MSG_SYNC_STR);

  // Try to read the message.
  uint8_t buffer[sizeof(struct remote_msg_t)];
  if(Serial.readBytes(buffer, sizeof(struct remote_msg_t)) == sizeof(struct remote_msg_t)) {
    // Read enough data.
    memcpy(msg, buffer, sizeof(struct remote_msg_t));

    return true;
  } else {
    // Failed to read enough data. Just ignore this frame and try to re-sync
    // later.
    return false;
  }
}

/**
 * Drive at the given speed and angle.
 *
 * Speeds are scaled from 0..1 with 1 being the fastest. Angles are from -90..90
 * with 0 being straight forward and 90 being to the left.
 */
void drive(int speed, float angle) {
  uint8_t right_power = 0; // TODO(kylc):
  uint8_t left_power = 0; // TODO(kylc):

  analogWrite(MOTOR_RIGHT_PIN1, constrain(right_power, -1, 0));
  analogWrite(MOTOR_RIGHT_PIN2, constrain(right_power, 0, 1));

  analogWrite(MOTOR_LEFT_PIN1, constrain(left_power, -1, 0));
  analogWrite(MOTOR_LEFT_PIN2, constrain(left_power, 0, 1));
}

void loop() {
  static state_t state;

  // The loop is executing at a fixed time interval to simplify velocity
  // integration, so record when the next loop should start.
  unsigned long deadline = micros() + 5000; // 5ms

  if(micros() > state.sonar_read_deadline) {
    state.sonar_dist = read_sonar_dist();
    state.sonar_read_deadline = micros() + 100000; // 100ms
  }

  struct remote_msg_t msg;
  if(read_remote_msg(&msg)) {
    state.remote_read_deadline = micros() + 100000; // 100ms
    drive(msg.speed, msg.angle);
  }

  // If the deadline has passed and we haven't read a message
  if(micros() > state.remote_read_deadline) {
    // Stop the motors
    drive(0, 0);
  }

  // Busy-wait until it is time to start the next loop.
  while(micros() < deadline) {
  }
}
