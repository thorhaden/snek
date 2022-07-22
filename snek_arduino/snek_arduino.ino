#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define GAME_AREA_WIDTH 94
#define GAME_AREA_HEIGHT 62

#define TAIL_ARRAY_LENGTH 10

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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

byte food_score = 0;
byte food[2];

byte snake_head[2] = {45,31};
char snake_direction = 'L';
byte tail_length = 2;
byte snake_tail[TAIL_ARRAY_LENGTH][2];
char input = "X";

void setup() {
  
 Serial.begin(9600);
 
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), change_dir_ld, FALLING);
  Serial.print("Pin ");
  Serial.print(2);
  Serial.print(" , interrupt ");
  Serial.println(digitalPinToInterrupt(2));
  
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), change_dir_ru, FALLING);
  Serial.print("Pin ");
  Serial.print(3);
  Serial.print(" , interrupt ");
  Serial.println(digitalPinToInterrupt(3));
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    }

  welcomescreen();
  // Set up game graphics
  
  display.drawBitmap(99, 17, sidegraphics_bmp, 32, 64, 1);
  display.drawRect(0, 0, 96, 64, SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(102,5);
  display.print("snek");

  // Set up score print parameters
  display.setTextSize(1);
  display.setCursor(97,57);
  
  display.print(0);
  display.setCursor(97,57);
  display.display();
  
  snake_head[0] = 43;
  snake_head[1] = 33;
  food_score = 0;
  newfood();
}
  
void loop() {

  Serial.println("Loooop!");


  // draw_snake(snake_head, snake_tail, tail_length);
  delay(100);
  move_snake(snake_direction, snake_head, snake_tail, tail_length);
  if(snake_head[0] < 1){
    gameover(food_score);
    }
  if(snake_head[1] < 1){
    gameover(food_score);
    }
  if(snake_head[0] > GAME_AREA_WIDTH){
    gameover(food_score);
    }
  if(snake_head[1] > GAME_AREA_HEIGHT){
    gameover(food_score);
    }
  if(snake_head[0] == food[0] && snake_head[1] == food[1] )
  {
    food_score = eatfood(food, food_score);
    newfood();
    }

  delay(100);
  


}

void newfood() {
    
    food[0] = random(2, (GAME_AREA_WIDTH / 2)) * 2 + 1;
    food[1] = random(2, (GAME_AREA_HEIGHT / 2)) * 2 + 1;
    Serial.print(food[0]);
    Serial.print(", ");
    Serial.println(food[1]);
    
    display.fillRect(food[0], food[1], 2, 2, SSD1306_WHITE);
    display.display();
    
 }

 byte eatfood(byte food[], byte food_score) {
    Serial.print("Food eaten at ");
    Serial.print(food[0]);
    Serial.print(", ");
    Serial.println(food[1]);
    
    food_score++;
    Serial.print("Food score ");
    Serial.println(food_score);
    
    display.print(food_score);
    display.display();
    display.setCursor(97,57);
    return food_score;

 }

 void move_snake(char snake_direction, byte snake_head[], byte snake_tail[TAIL_ARRAY_LENGTH][2], byte tail_length) {

    display.fillRect(snake_head[0], snake_head[1], 2, 2, SSD1306_BLACK);
    Serial.print(snake_head[0]);
    Serial.print(" head ");
    Serial.println(snake_head[1]);
    // move all tail elements back one step
//    for(int i = TAIL_ARRAY_LENGTH; i >= 1; i--){
//      Serial.print(i);
//      Serial.print(": ");
//      Serial.print(snake_tail[i][0]);
//      Serial.print(", ");
//      Serial.println(snake_tail[i][1]);
//      snake_tail[i][0] = snake_tail[i-1][0];
//      snake_tail[i][1] = snake_tail[i-1][1];
//      display.fillRect(snake_tail[i][0], snake_tail[i][1], 2, 2, SSD1306_WHITE);
//      }

    
 
    // make the head position part of the tail
    snake_tail[0][0] = snake_head[0];
    snake_tail[0][1] = snake_head[1];


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

    display.fillRect(snake_head[0], snake_head[1], 2, 2, SSD1306_WHITE);
    
    display.display();
 }

void draw_snake(byte snake_head[], byte snake_tail[TAIL_ARRAY_LENGTH][2], byte tail_length) {

    Serial.print(snake_head[0]);
    Serial.print(", ");
    Serial.println(snake_head[1]);

    display.fillRect(snake_head[0], snake_head[1], 2, 2, SSD1306_WHITE);
    
//    for(byte i = 0; i < TAIL_ARRAY_LENGTH; i++) {
//      if(snake_tail[i][0] != 0 && snake_tail[i][1] != 0){
//      display.fillRect(snake_tail[i][0], snake_tail[i][1], 2, 2, SSD1306_WHITE);  
//      }
//    }
    display.display();
}

void change_dir_ld(){
  
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) 
  {
  Serial.println("L D Button");
  Serial.println(snake_direction);
    if (snake_direction == 'L'){
      snake_direction = 'D';
      }
    else if (snake_direction == 'D'){
      snake_direction = 'L';
    }
    else if (snake_direction == 'U'){
      snake_direction = 'L';
    }
    else if (snake_direction == 'R'){
      snake_direction = 'D';
    }
  }
  last_interrupt_time = interrupt_time;
  }

void change_dir_ru(){
  
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200)
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

void gameover(int food_score) {
  
  byte cursorposition = 64;
  
  display.clearDisplay(); 
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setCursor(10,cursorposition);
  
  for(byte i = 0; i < 46; i++) {
    display.println("GAME OVER");
    display.display();
    delay(10);
    display.setCursor(10,cursorposition);
    cursorposition--;
    }

   display.setTextSize(1); // Draw 2X-scale text
   display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
   display.setCursor(40,42);
   display.print(food_score);
   display.println(" points!");
   display.display();
   delay(1000);
   setup();
   
}

void welcomescreen(){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setTextSize(3);
  display.setCursor(20,10);
  display.print("SNEK!");
  display.display();
  delay(500);
  display.clearDisplay();
  display.display();
}
