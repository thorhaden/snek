#include "./src/Tiny4kOLED.h"

#define GAME_AREA_WIDTH 94
#define GAME_AREA_HEIGHT 62

#define HEAD_START_X 47
#define HEAD_START_Y 31


#define TAIL_ARRAY_LENGTH 100

static const unsigned char PROGMEM sidegraphics_bmp[] = {
0x00, 0x7f, 0xfc, 0x00, 0x07, 0xc2, 0x07, 0x00, 0x0c, 0x02, 0x01, 0x80, 0x18, 0x02, 0x00, 0xc0, 
0x30, 0x01, 0x00, 0x40, 0x60, 0x01, 0x00, 0x60, 0x46, 0x01, 0x00, 0x20, 0xc2, 0x00, 0x02, 0x20, 
0x80, 0x00, 0x04, 0x30, 0x80, 0xc0, 0x08, 0x10, 0x80, 0x40, 0x30, 0x10, 0x80, 0x07, 0xc0, 0x10, 
0x80, 0x0c, 0x80, 0x10, 0x80, 0x18, 0x80, 0x30, 0x80, 0x30, 0x80, 0x60, 0xc0, 0x21, 0x9f, 0xc0, 
0x40, 0x61, 0x00, 0xc0, 0x61, 0xc3, 0x00, 0x80, 0x3f, 0x06, 0x01, 0x80, 0x10, 0x04, 0x01, 0x00, 
0x10, 0x0c, 0x01, 0x00, 0x10, 0x0f, 0x03, 0x00, 0x10, 0x18, 0xe2, 0x00, 0x10, 0x10, 0x12, 0x00, 
0x28, 0x10, 0x02, 0x00, 0x00, 0x10, 0x02, 0x00, 0x00, 0x10, 0x02, 0x00, 0x00, 0x30, 0x02, 0x00, 
0x00, 0x20, 0x02, 0x00, 0x00, 0x20, 0x1e, 0x00, 0x00, 0x20, 0x62, 0x00, 0x00, 0x21, 0x83, 0x00, 
0x00, 0x20, 0x01, 0x00, 0x00, 0x20, 0x01, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x10, 0x01, 0x80, 
0x00, 0x10, 0x00, 0x80, 0x00, 0x10, 0x00, 0x80, 0x00, 0x10, 0x00, 0xc0, 0x00, 0x18, 0x38, 0x40, 
0x00, 0x09, 0xc0, 0x20, 0x00, 0x0e, 0x00, 0x30, 0x00, 0x04, 0x00, 0x18, 0x00, 0x06, 0x00, 0x0c, 
0x00, 0x03, 0x00, 0x3c, 0x00, 0x01, 0x00, 0xc0, 0x00, 0x01, 0x81, 0x00
};

const unsigned char pixel[] PROGMEM = {
  B11000000,
  B11000000
};

const unsigned char antipixel[] PROGMEM = {
  B00111111,
  B00111111
};


byte score;
byte food[2]; // {x,y} coordinates

byte snake_head[2]; // {x,y} coordinates
char snake_direction;

byte tail_length = 3;
byte snake_tail[TAIL_ARRAY_LENGTH][2];

bool alive = true;

void setup() {
  randomSeed(analogRead(0));
  Serial.begin(9600);
  Serial.print("Serial.begin");
 
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), change_dir_ld, FALLING);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), change_dir_ru, FALLING);

  oled.begin(128, 64, sizeof(tiny4koled_init_128x64r), tiny4koled_init_128x64r);
  
  oled.clear();

  oled.on();


  welcomescreen();
  
  // Set up game board
  oled.bitmap(32,64, 32, 64,sidegraphics_bmp);
  // display.drawRect(0, 0, 96, 64, SSD1306_WHITE);


  oled.setFont(FONT6X8);
  oled.setCursor(102,1);
  oled.print("snek");

  // Set up score print parameters
  oled.setFont(FONT6X8);
  oled.setCursor(97,10);
  oled.print(0);
  oled.setCursor(97,10);

  // display.display();

  // Initialize game data
  snake_head[0] = HEAD_START_X; // Close to center
  snake_head[1] = HEAD_START_Y; // Close to center
  score = 0;

  char start_directions[] = "LRU";
  snake_direction = start_directions[random(0,2)];
  
  newfood();

  while(alive == true){

    // Serial.println("Loooop!");
    
    move_snake();
    
    if(snake_head[0] < 1){
      alive = false;
      }
    if(snake_head[1] < 1){
      alive = false;
      }
    if(snake_head[0] > GAME_AREA_WIDTH-2){
      alive = false;
      }
    if(snake_head[1] > GAME_AREA_HEIGHT-2){
      alive = false;
      }

    for(int i = 0; i < TAIL_ARRAY_LENGTH; i++){
      // Serial.println("Checking tail position " + String(snake_tail[i][0]) + ", " + String(snake_tail[i][1]));
      if(snake_tail[i][0] == snake_head[0] && snake_tail[i][1] == snake_head[1]){
        Serial.println("Death by collision at " + String(snake_head[0]) + ", "  + String(snake_head[1]));
        alive = false;
      }
    }
    
    if(snake_head[0] == food[0] && snake_head[1] == food[1]){
      eatfood();
      newfood();
      for(int i = 0; i < TAIL_ARRAY_LENGTH; i++){
        if(snake_tail[i][0] == food[0] && snake_tail[i][1] == food[1]){
          newfood();
        }
      }
    }
      
  delay(1000);
  }
  
  gameover(); 
  // End of program execution
}
  
void loop() {
  
}

void newfood() {
  food[0] = random(0, (GAME_AREA_WIDTH / 2)) * 2 - 1 ;  // random even number inside the game board
  food[1] = random(0, (GAME_AREA_HEIGHT / 2)) * 2 - 1;
  Serial.print(food[0]);
  Serial.print(", ");
  Serial.println(food[1]);
  
  oled.bitmap(food[0], food[1], 2, 2,pixel );
  // display.display();
}


void eatfood() {
  Serial.print("Food eaten at ");
  Serial.print(food[0]);
  Serial.print(", ");
  Serial.println(food[1]);
  
  score++;
  tail_length += 3;
  Serial.print("Food score ");
  Serial.println(score);
  
  oled.print(score);
  // display.display();
  oled.setCursor(97,10);
}

 void move_snake() {

    Serial.print(snake_head[0]);
    Serial.print(" head ");
    Serial.println(snake_head[1]);
    
     //move all tail elements back one step
    for(int i = TAIL_ARRAY_LENGTH - 1; i >= 0; i--){
//      Serial.print(i);
//      Serial.print(": ");
//      Serial.print(snake_tail[i][0]);
//      Serial.print(", ");
//      Serial.println(snake_tail[i][1]);
      snake_tail[i][0] = snake_tail[i-1][0];
      snake_tail[i][1] = snake_tail[i-1][1];
      if(i >= tail_length && snake_tail[i][0] > 0 && snake_tail[i][1] > 0){
        oled.bitmap(snake_tail[i][0], snake_tail[i][1], 2, 2,antipixel );
        snake_tail[i][0] = 0;
        snake_tail[i][1] = 0;
      }
      else if(snake_tail[i][0] > 0 && snake_tail[i][1] > 0){
        oled.bitmap(snake_tail[i][0], snake_tail[i][1], 2, 2,pixel );
      }
      
    }
    
       
    // make the head position part of the tail
    snake_tail[0][0] = snake_head[0];
    snake_tail[0][1] = snake_head[1];

    oled.bitmap(snake_tail[0][0], snake_tail[0][1], 2, 2,pixel );

    // move the head
    byte xpos = snake_head[0]; 
    byte ypos = snake_head[1];
    
    if(snake_direction == 'L'){
        xpos -= 2;
    }
    if(snake_direction == 'R'){
        xpos += 2;
    }
    if(snake_direction == 'U'){
        ypos -= 2;
    }
    if(snake_direction == 'D'){
        ypos += 2;
    }
    
    snake_head[0] = xpos;
    snake_head[1] = ypos;

    oled.bitmap(snake_head[0], snake_head[1], 2, 2,pixel );
    
    // display.display();
}

void draw_snake() {

    Serial.print(snake_head[0]);
    Serial.print(", ");
    Serial.println(snake_head[1]);




    // display.fillRect(snake_head[0], snake_head[1], 2, 2, SSD1306_WHITE);
    oled.bitmap(snake_head[0], snake_head[1], 2, 2,pixel );
    
//    for(byte i = 0; i < TAIL_ARRAY_LENGTH; i++) {
//      if(snake_tail[i][0] != 0 && snake_tail[i][1] != 0){
//      display.fillRect(snake_tail[i][0], snake_tail[i][1], 2, 2, SSD1306_WHITE);  
//      }
//    }
    // display.display();
}

void change_dir_ld()
{
  // Software debounce
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 50)
  {
    switch(snake_direction)
    {
      case 'L':
        snake_direction = 'D';
        break;
      case 'D':
        snake_direction = 'L';
        break;
      case 'U':
        snake_direction = 'L';
        break;
      case 'R':
        snake_direction = 'D';
        break;
    }
    last_interrupt_time = interrupt_time;
  }
}

void change_dir_ru()
{
  // Software debounce
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 50)
  {
    Serial.println("R U Button");
    if (snake_direction == 'R'){
      snake_direction = 'U';
      }
    else if (snake_direction == 'U'){
      snake_direction = 'R';
    }
    else if (snake_direction == 'L'){
      snake_direction = 'U';
    }
    else if (snake_direction == 'D'){
      snake_direction = 'R';
    }
  }
  last_interrupt_time = interrupt_time;
}


void welcomescreen(){

  // Set up text mode
  oled.clear();
  //display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  oled.setFont(FONT8X16);
  oled.setCursor(20,4);

  // Display splash screen
  oled.print("SNEK!");
  //display.display();
  delay(1000);

  // Clear display
  oled.clear();
  //display.display();
}


void gameover() {

  // Set up text mode
  byte cursorposition = 64;
  oled.clear();
  oled.setFont(FONT8X16);
  //display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  oled.setCursor(20,cursorposition/8);

  //Animate GAME OVER
  for(byte i = 0; i < 46; i++) {
    oled.print("GAME OVER");
    //display.display();
    delay(20);
    oled.setCursor(30,8);
    cursorposition--;
    }

  // Set up text mode
  oled.setFont(FONT6X8);
  //display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  oled.setCursor(40,6);

  // Display final score
  oled.print(score);
  oled.print(" points!");
  //display.display();

  // End of program execution
}
