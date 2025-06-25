//Tower
#define dirTower 26
#define stepTower 27

//Plate
#define dirPlate 13
#define stepPlate 14

int Delay = 700;
int stepsPlate = 0;
int stepsTower = 0;
unsigned long previous1 = 0;
unsigned long previous2 = 0;
unsigned long previous3 = 0;
unsigned long previous4 = 0;

bool platedone = false;

void setup() {
  pinMode(stepPlate, OUTPUT);
  pinMode(dirPlate, OUTPUT);
  pinMode(stepTower, OUTPUT);
  pinMode(dirTower, OUTPUT);
  digitalWrite(dirPlate, HIGH);
  digitalWrite(dirTower, HIGH);
}

void loop() {
  
}

void firstTry() {//  it's necesary to change the delay function to finite state machine
  digitalWrite(dirPlate, HIGH);
  digitalWrite(dirTower, HIGH);
  // making 3200 steps per rotation
  for(int i = 0; i < 3200; i++){
    digitalWrite(stepPlate, HIGH);
    delayMicroseconds(700);
    digitalWrite(stepPlate, LOW);
    delayMicroseconds(700);
    
  }
  //  making 800 steps for a 1/4 rotation
  for(int j = 0; j < 800; j++){
    digitalWrite(stepTower, HIGH);
    delayMicroseconds(700);
    digitalWrite(stepTower, LOW);
    delayMicroseconds(700);
  }
}

void secondTry(){
  if(!platedone){
    digitalWrite(stepPlate, HIGH);
    stepsPlate ++;
    unsigned long current1 = micros();
    if(current1 - previous1 >= Delay){
      previous1 = current1;
      digitalWrite(stepPlate, LOW);
    }
    unsigned long current2 = micros();
    if(current2 - previous2 >= Delay){
      previous2 = current2;
    }
    if(stepsPlate >= 3200){
      stepsPlate = 0;
      platedone = true;
    }
  }
  if(platedone == true){
    digitalWrite(stepTower, HIGH);
    stepsTower ++;
    unsigned long current3 = micros();
    if(current3 - previous3 >= Delay){
      previous3 = current3;
      digitalWrite(stepTower, LOW);
    }
    unsigned long current4 = micros();
    if(current4 - previous4 >= Delay){
      previous4 = current4;
    }
    if(stepsTower >= 800){
      stepsTower = 0;
      platedone = false;
    }
  }
}



































                                             
