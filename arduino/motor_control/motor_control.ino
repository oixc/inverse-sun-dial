#define outputA 22
#define outputB 23

//Motor A
const int motorPin1 = 20;
const int motorPin2 = 21;
//Motor B
const int motorPin3 = 18;
const int motorPin4 = 19;

void motor_setup() {
  //Set pins as outputs
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  motor_stop();
}

void motor_stop() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  delay(100);
}

void motor_forward_A() {
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
}

void motor_backward_A() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
}

void motor_forward_B() {
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
}

void motor_backward_B() {
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
}

int counter = 0;
int aState;
int aLastState;

void update_counter() {
  aState = digitalRead(outputA);  // Reads the "current" state of the outputA
  // If the previous and the current state of the outputA are different, that means a Pulse has occured
  if (aState != aLastState) {
    // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
    if (digitalRead(outputB) != aState) {
      counter++;
    } else {
      counter--;
    }
  }
  aLastState = aState;  // Updates the previous state of the outputA with the current state
}

void move_to_counter(int target){
  if (counter > target) {
    motor_forward_A();
  }
  if (counter < target) {
    motor_backward_A();
  }
  if (counter == target) {
    motor_stop();
  }
}

void move_degrees(float target){
// 3 pulse per revolution (ppr) --> 3 * 4 = 12 counts per revolution (cpr)
// gear ratio 298:1 --> 298 * 12 = 3576 counters per revolution
// experiments show 3576 / 2 = 1788 for one revolution --> cpr probably just 6 and not 12 in this setup
  move_to_counter(int(1788 * target / 360));
}

void setup() {
  pinMode(outputA, INPUT);
  pinMode(outputB, INPUT);

  motor_setup();

  Serial.begin(9600);
  // Reads the initial state of the outputA
  aLastState = digitalRead(outputA);
}

void loop() {
  update_counter();
  move_degrees(-90);
  Serial.println(counter);
}