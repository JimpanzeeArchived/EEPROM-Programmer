//Define seven segment output//
#define zero   0b00111111
#define one    0b00000110
#define two    0b01011011
#define three  0b01001111
#define four   0b01100110
#define five   0b01101101
#define six    0b01111101 
#define seven  0b00000111
#define eight  0b01111111
#define nine   0b01101111

//OP CODE//
#define BEGIN       0b0001
#define LOAD_A      0b0110
#define LOAD_ADD    0b0111
#define LOAD_OUTPUT 0b1000
#define CLEAR       0b0110
#define JUMP        0b1010
#define ADD         0b1001

//Instruction Pin Output//*/
/* | PIN | DESCRIPTION   |
 * |  8  | SELECTOR      |
 * |  7  | COUNTER LOAD  |
 * |  6  | OUT_REG LOAD  |
 * |  5  | REG_A LOAD    |
 * |  4  | REG_SAVE LOAD |
 * |  3  | RESET         |
 * |  2  | MAIN CLOCK    |
 * |  1  | LOAD CLOCK    |
 */

//data shift register
int ser_595_1 = 9; //data out for HC595
int outEnable_595_1 = 8; //output enable for HC595
int rclk_595_1 = 7;
int srclk_595_1 = 6; //clock control HC595

//address shift register
int ser_595_2 = 5; //data out for HC595
int outEnable_595_2 = 4; //output enable for HC595
int rclk_595_2 = 3;
int srclk_595_2 = 2; //clock control HC595

//serial-out register
int clk_165 = 10;   //postive edge to shift
int shld_165 = 11; //parallel load, load into the output register, low = load, high = do nothing
int ser_read_165 = 12; //reading pin for hc165
int clk_inh_165 = 13;  //keep low to enable shifting

//EEPROM Control
int EEPROM_WE = 14; //A0, yellow
int EEPROM_OE = 15; //A1, white
int EEPROM_CE = 16; //A2, black

void setup() {
  pinAssign();
  delay(1000);
 
//  writeEEPROM(0b00000000, (CLEAR*16) + 0);
//  readEEPROM(0b00000000);
//  writeEEPROM(0b00000001, (ADD*16) + 3);
//  readEEPROM(0b00000001);
//  writeEEPROM(0b00000010, (ADD*16) + 4);
//  readEEPROM(0b00000010);
//  writeEEPROM(0b00000011, (JUMP*16) + 0);
//  readEEPROM(0b00000011);
  int opCode[] = {CLEAR, ADD, ADD, JUMP};
  int value[] = {0, 3, 4, 0};
  //LOAD_MAIN_EEPROM(opCode, value);
  LOAD_INSTRUCTION_EEPROM();
}

void loop() {}
 //this is for uploading the code 

void LOAD_MAIN_EEPROM(int opcode[], int value[]){
  //to load main EEPROM
  //load the instruction onto the main eeprom
  //find length of array.
  int len = sizeof(opcode)/sizeof(opcode[0]);
  for(int i = 0; i < 4; i++){
    writeEEPROM(i, (opcode[i]*16) + value[i]);
    readEEPROM(i);}
}

void LOAD_INSTRUCTION_EEPROM(){
  //to load instruction EEPROM
  BEGIN_INSTRUCT();
  LOAD_A_INSTRUCT();
  ADD_INSTRUCT();
  CLEAR_INSTRUCT();
  JUMP_INSTRUCT();
}

void LOAD_SEVEN_SEG_DISPLAY_EEPROM(){
  //to load seven segment display EEPROM
  sevenSegmentDisplay();
}





void BEGIN_INSTRUCT(){
  //the first instruction in the EEPROM
  //does nothing
  int shift_left_4 = 16;
  for(int j = 0; j < 15; j++){
      writeEEPROM((BEGIN*shift_left_4) + j, 0b00000001);}  //automate neutral state
  writeEEPROM((BEGIN*shift_left_4)+ 15, 0b00000011); //increment the clock
}

void LOAD_A_INSTRUCT(){
  //this is the step for set up the instruction EEPROM for LOAD Reg A
  //instruction always increment
  int shift_left_4 = 16;
  writeEEPROM((LOAD_A*shift_left_4)+ 0, 0b10000001); 
  writeEEPROM((LOAD_A*shift_left_4)+ 1, 0b10000001); //load to REG_SAVE
  writeEEPROM((LOAD_A*shift_left_4)+ 2, 0b10001001); 
  writeEEPROM((LOAD_A*shift_left_4)+ 3, 0b10000001);
  writeEEPROM((LOAD_A*shift_left_4)+ 4, 0b10010001);
  writeEEPROM((LOAD_A*shift_left_4)+ 5, 0b10000001); 
  for(int j = 6; j < 15; j++){
      writeEEPROM((LOAD_A*shift_left_4) + j, 0b10000001);}  //automate neutral state
  writeEEPROM((LOAD_A*shift_left_4)+15, 0b10000011); //increment the clock
}

void ADD_INSTRUCT(){
  //add the value in REG_A and current output 
  //then output the result to seven segment display
  int shift_left_4 = 16;
  writeEEPROM((ADD*shift_left_4) + 0, 0b10000001);
  writeEEPROM((ADD*shift_left_4) + 1, 0b10001001); //load to REG_SAVE
  writeEEPROM((ADD*shift_left_4) + 2, 0b10000001); //return to neutral state
  writeEEPROM((ADD*shift_left_4) + 3, 0b10010001); //load to REG_A
  //from now on output the selector to choose output instead of zero
  writeEEPROM((ADD*shift_left_4) + 4, 0b10000001); //return to neutral state and output previous data
  writeEEPROM((ADD*shift_left_4) + 5, 0b11000001); //load result (REG_A + Current Output) to REG_ADD
  writeEEPROM((ADD*shift_left_4) + 6, 0b10000001); //return to neutral state
  writeEEPROM((ADD*shift_left_4) + 7, 0b10100001); //load to REG_OUTPUT
  writeEEPROM((ADD*shift_left_4) + 8, 0b10000001); //return to neutral state
  for(int j = 9; j < 15; j++){
      writeEEPROM((ADD*shift_left_4) + j, 0b10000001);}  //automate neutral state
  writeEEPROM((ADD*shift_left_4) + 15, 0b10000011);
}

void CLEAR_INSTRUCT(){
  //purpose is to clear the all the register to zero
  //input to clear_main must be zero
  //should be the first instruction
  int shift_left_4 = 16;
  writeEEPROM((CLEAR*shift_left_4) + 0, 0b00000001);
  writeEEPROM((CLEAR*shift_left_4) + 1, 0b00001001); //load zero to REG_SAVE
  writeEEPROM((CLEAR*shift_left_4) + 2, 0b00000001); //return to neutral state
  writeEEPROM((CLEAR*shift_left_4) + 3, 0b00010001); //load zero to REG_A
  writeEEPROM((CLEAR*shift_left_4) + 4, 0b00000001); //return to neutral state
  //Selector must be low to load zero to adder_reg and output_reg
  writeEEPROM((CLEAR*shift_left_4) + 5, 0b01000001); //load zero to REG_ADDER
  writeEEPROM((CLEAR*shift_left_4) + 6, 0b00000001); //return to neutral state
  writeEEPROM((CLEAR*shift_left_4) + 7, 0b00100001); //load zero to REG_ADDER
  writeEEPROM((CLEAR*shift_left_4) + 8, 0b00000001); //return to neutral state
  for(int j = 9; j < 15; j++){
      writeEEPROM((CLEAR*shift_left_4) + j, 0b00000001);}  //automate neutral state
  writeEEPROM((CLEAR*shift_left_4) + 15, 0b00000011); //increment the clock
}

void JUMP_INSTRUCT(){
  //This instruction is use to jump between instruction
  //This is performed by load preset value into main counter
  int shift_left_4 = 16;
  writeEEPROM((JUMP*shift_left_4) + 0, 0b10000001);      //neutral state
  writeEEPROM((JUMP*shift_left_4) + 1, 0b10001001);      //load value (new address) in REG_A
  writeEEPROM((JUMP*shift_left_4) + 2, 0b10000001);      //neutral state
  for(int j = 3; j < 14; j++){
      writeEEPROM((JUMP*shift_left_4) + j, 0b10000001);} //automate neutral state
  writeEEPROM((JUMP*shift_left_4) + 14, 0b10000000);     //load new address to main clock
  writeEEPROM((JUMP*shift_left_4) + 15, 0b10000011);     //increment the main clock
}


void pinAssign(){
  pinMode(shld_165, OUTPUT);
  pinMode(clk_165, OUTPUT);
  pinMode(clk_inh_165, OUTPUT);
  pinMode(ser_read_165, INPUT);
  
  pinMode(ser_595_1, OUTPUT);
  pinMode(outEnable_595_1, OUTPUT);
  pinMode(rclk_595_1, OUTPUT);
  pinMode(srclk_595_1, OUTPUT);

  pinMode(ser_595_2, OUTPUT);
  pinMode(outEnable_595_2, OUTPUT);
  pinMode(rclk_595_2, OUTPUT);
  pinMode(srclk_595_2, OUTPUT);

  pinMode(EEPROM_WE, OUTPUT);
  pinMode(EEPROM_OE, OUTPUT);
  pinMode(EEPROM_CE, OUTPUT);

  Serial.begin(9600); //To read data
  Serial.println("Serial Reading Begin");
  }

void writeSerialOut_1(int dataOut){
  digitalWrite(srclk_595_1, LOW);
  delay(5);
  int temp = 0; //making temp variable
  for(int i = 0, bitmask = 1; i < 8; i++, bitmask = bitmask << 1){  

    temp = dataOut & bitmask;

    if(temp == 0){
      //if current bit is LOW, output LOW
      digitalWrite(srclk_595_1, LOW); delay(20);
      digitalWrite(ser_595_1, LOW); delay(20);
      digitalWrite(srclk_595_1, HIGH); delay(20);
      //Serial.print("0");
    }
    else{
      //output HIGH
      digitalWrite(srclk_595_1, LOW); delay(20); 
      digitalWrite(ser_595_1, HIGH); delay(20);
      digitalWrite(srclk_595_1, HIGH); delay(20);
      //Serial.print("1");

    }
  }    
    digitalWrite(rclk_595_1, LOW); //to save input at the output register
    delay(20);
    digitalWrite(rclk_595_1, HIGH); //Positive edge trigger
    delay(20);
} 

void writeSerialOut_2(int dataOut){
  digitalWrite(srclk_595_2, LOW);
  delay(5);
  int temp = 0; //making temp variable

  for(int i = 0, bitmask = 1; i < 8; i++, bitmask = bitmask << 1){  //iterate 8 times
    //AND bitmask with dataout to get output
    temp = dataOut & bitmask;
    //Serial.println("temp: ");
    //Serial.println(temp);
    if(temp == 0){
      //if current bit is LOW, output LOW
      digitalWrite(srclk_595_2, LOW); delay(20);
      digitalWrite(ser_595_2, LOW); delay(20);
      digitalWrite(srclk_595_2, HIGH); delay(20);
      //Serial.print("0");
    }
    else{
      //output HIGH
      digitalWrite(srclk_595_2, LOW); delay(20); 
      digitalWrite(ser_595_2, HIGH); delay(20);
      digitalWrite(srclk_595_2, HIGH); delay(20);
      //Serial.print("1");

    }
  }    
    digitalWrite(rclk_595_2, LOW); delay(20); //to show at output 
    digitalWrite(rclk_595_2, HIGH); delay(20);//Positive edge trigger
} 

int readSerialIn(){
  //read data from HC165
  int dataIn = 0; //return input
  digitalWrite(shld_165, HIGH); //set HIGH to enable shifting
  delay(20);
  digitalWrite(shld_165, LOW); //load into register
  delay(20);
  digitalWrite(shld_165, HIGH); //set HIGH to enable shifting
  delay(20);


  digitalWrite(clk_inh_165, LOW); //set low to enable shifting
  delay(20);
  digitalWrite(clk_165, LOW); //initate the edge
  delay(20);
  
  Serial.print(digitalRead(ser_read_165));
  dataIn = digitalRead(ser_read_165);
  
  for(int i = 0; i < 7; i++) { //loop for 8 times 
    dataIn = dataIn * 2;
    digitalWrite(clk_165, LOW);
    delay(20); 
    digitalWrite(clk_165, HIGH); //Imitate a clock cycle
    delay(20);
    
    dataIn = dataIn + digitalRead(ser_read_165);
    Serial.print(digitalRead(ser_read_165));
    }
    Serial.println("");
    Serial.println(dataIn);
  return dataIn;
}


void writeEEPROM(int address, int dataOut){
  digitalWrite(EEPROM_OE, HIGH); delay(10);
  digitalWrite(EEPROM_CE, LOW); delay(10);
  //write data into addresses
  digitalWrite(outEnable_595_2, HIGH);  //disable output
  digitalWrite(outEnable_595_1, HIGH);  //diable output
  
  Serial.println("Address is being loaded!!!");
  writeSerialOut_2(address); //Load address to output;
  Serial.println("Address done loading"); delay(20);

  Serial.println("Data is being loaded!!!");
  writeSerialOut_1(dataOut); //Load data to output;
  Serial.println("Data done loading"); delay(20);

  digitalWrite(outEnable_595_2, LOW);  //Enable output
  digitalWrite(outEnable_595_1, LOW);  //Enable output

  //digitalWrite(EEPROM_OE, HIGH); delay(10);
  //digitalWrite(EEPROM_CE, LOW); delay(10);
  digitalWrite(EEPROM_WE, HIGH); delay(20);  //initate a low pulse
  digitalWrite(EEPROM_WE, LOW); delay(20);
  digitalWrite(EEPROM_WE, HIGH); delay(20);
  Serial.println("EEPROM done loading"); delay(20);
}

void readEEPROM(int address){
  //read data from EEPROM, use for testing mostly
  digitalWrite(outEnable_595_1, HIGH);  //disable data bus output
  digitalWrite(outEnable_595_2, HIGH);  //Enable address

 
  digitalWrite(EEPROM_WE, HIGH); delay(10);  //set up EEPROM
  digitalWrite(EEPROM_OE, LOW); delay(10);
  digitalWrite(EEPROM_CE, LOW); delay(10);   //done set up EEPROM
  delay(20);

  Serial.println("Address is being loaded!!!");
  writeSerialOut_2(address); //Load address to output;
  Serial.println("Address done loading"); delay(20);
  digitalWrite(outEnable_595_2, LOW);  //Enable output

  Serial.println("Output is: ");
  readSerialIn();
}


void sevenSegmentDisplay(){
  int temp = 0;   //use for math process
  //loop for 128 cycles 
  for(int i = 0; i < 128; i++){   //program up from 0 to 100
    temp = i%10;         //find the first digit, least significant bit
    segmentWriteDigit_1(temp, i);
    temp = (i-temp)/10;  //find the second digit, most significant bit
    if(temp > 9){
      temp = temp%10;
      segmentWriteDigit_2(temp, i);
    }
    else{
      segmentWriteDigit_2(temp, i);
    }
  }
}

void segmentWriteDigit_1(int value, int index){
  int temp = segmentTranslate(value); delay(5);
  writeEEPROM(index, temp);  // this is wrong
}
void segmentWriteDigit_2(int value, int index){
  int temp = segmentTranslate(value); delay(5);
  writeEEPROM(index+128, temp);
}

int segmentTranslate(int data){
  int definition = 0;
  switch(data)
  { case 0:
      definition = zero; delay(5); break;
    case 1:
      definition = one; delay(5); break;
    case 2:
      definition = two; delay(5); break;
    case 3:
      definition = three; delay(5); break;
    case 4:
      definition = four; delay(5); break;
    case 5:
      definition = five; delay(5); break;   
    case 6:
      definition = six; delay(5); break;     
    case 7:
      definition = seven; delay(5); break;   
    case 8:
      definition = eight; delay(5); break;    
    case 9:
      definition = nine; delay(5); break;       
    default:
      Serial.println("Error! No match");                    
  }
  return definition;
}
