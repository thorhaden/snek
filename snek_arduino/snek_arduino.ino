#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define GAME_AREA_WIDTH 94
#define GAME_AREA_HEIGHT 62

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

static const unsigned char PROGMEM sidegraphics_bmp[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x0e, 0x62, 0xf2, 0x20, 0x10, 0x62, 0x82, 0x40, 0x10, 0x52, 0x82, 0x80, 0x0e, 0x52, 0xf3, 0x00, 
0x01, 0x4a, 0x82, 0x80, 0x01, 0x4a, 0x82, 0x40, 0x01, 0x46, 0x82, 0x20, 0x1e, 0x46, 0xf2, 0x10, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0x00, 0x01, 0xf0, 0x81, 0xc0, 0x03, 0x00, 0x80, 0x60, 
0x06, 0x00, 0x80, 0x30, 0x0c, 0x00, 0x40, 0x10, 0x18, 0x00, 0x40, 0x18, 0x11, 0x80, 0x40, 0x08, 
0x30, 0x80, 0x00, 0x88, 0x20, 0x00, 0x01, 0x0c, 0x20, 0x30, 0x02, 0x04, 0x20, 0x10, 0x0c, 0x04, 
0x20, 0x01, 0xf0, 0x04, 0x20, 0x03, 0x20, 0x04, 0x20, 0x06, 0x20, 0x0c, 0x20, 0x0c, 0x20, 0x18, 
0x30, 0x08, 0x67, 0xf0, 0x10, 0x18, 0x40, 0x30, 0x18, 0x70, 0xc0, 0x20, 0x0f, 0xc1, 0x80, 0x60, 
0x04, 0x01, 0x00, 0x40, 0x04, 0x03, 0x00, 0x40, 0x04, 0x03, 0xc0, 0xc0, 0x04, 0x06, 0x38, 0x80, 
0x04, 0x04, 0x04, 0x80, 0x0a, 0x04, 0x00, 0x80, 0x00, 0x04, 0x00, 0x80, 0x00, 0x04, 0x00, 0x80, 
0x00, 0x0c, 0x00, 0x80, 0x00, 0x08, 0x00, 0x80, 0x00, 0x08, 0x07, 0x80, 0x00, 0x08, 0x18, 0x80, 
0x00, 0x08, 0x60, 0xc0, 0x00, 0x08, 0x00, 0x40, 0x00, 0x08, 0x00, 0x40, 0x00, 0x0c, 0x00, 0x40, 
0x00, 0x04, 0x00, 0x60, 0x00, 0x04, 0x00, 0x20, 0x00, 0x04, 0x00, 0x20, 0x00, 0x04, 0x00, 0x30, 
0x00, 0x06, 0x0e, 0x10, 0x00, 0x02, 0x70, 0x08, 0x00, 0x03, 0x80, 0x0c, 0x00, 0x01, 0x00, 0x06, 
0x00, 0x01, 0x80, 0x03, 0x00, 0x00, 0xc0, 0x0f, 0x00, 0x00, 0x40, 0x30, 0x00, 0x00, 0x60, 0x40
};

int food_score = 0;
int food[2];

int snake_head[2] = {45,31};
char snake_direction = {'D'};
int snake_length = 5;
int snake_tail[100][2] = {{44,32},{43,32}};

void setup() {
  
 Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    }

  welcomescreen();
  
  // Set up game graphics
  display.drawBitmap(97, 0, sidegraphics_bmp, 32, 64, 1);
  display.drawRect(0, 0, 96, 64, SSD1306_INVERSE);

  // Set up score print parameters
  display.setTextSize(1);
  display.setCursor(97,57);
  display.print(0);
  display.setCursor(97,57);
  display.display();
  
  snake_head[0] = 45;
  snake_head[1] = 31;
  food_score = 0;
}


void loop() {
  Serial.println("Loooop!");

//  newfood(food);
  draw_snake(snake_head);
  delay(500);
  move_snake(snake_direction, snake_head);
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
//  food_score = eatfood(food, food_score);
  delay(500);
  


}

void newfood(int food[]) {
    
    food[0] = random(1, (GAME_AREA_WIDTH / 2) * 2);
    food[1] = random(1, (GAME_AREA_HEIGHT / 2) * 2);
//    Serial.print(food[0]);
//    Serial.print(", ");
//    Serial.println(food[1]);
    
    display.fillRect(food[0], food[1], 2, 2, SSD1306_WHITE);
    display.display();
    
 }

 int eatfood(int food[], int food_score) {
    
    display.fillRect(food[0], food[1], 2, 2, SSD1306_BLACK);  //Not used when snake exists
    food_score++;
    display.print(food_score);
    display.display();
    display.setCursor(97,57);
    return food_score;

 }

 void move_snake(char snake_direction, int snake_head[]){
    
    //*snake_tail[snake_length] = *snake_head;
    int xpos = snake_head[0];
    int ypos = snake_head[1];
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
 }

void draw_snake(int snake_head[]) {

    Serial.print(snake_head[0]);
    Serial.print(", ");
    Serial.println(snake_head[1]);
  
//    if (sizeof(snake_tail) > snake_length){
//        snake_tail[snake_length] = {0,0}
//        display.fillRect(food[0], food[1], 2, 2, SSD1306_BLACK);
//    }

  display.fillRect(snake_head[0], snake_head[1], 2, 2, SSD1306_WHITE);
//  for(int i = 0; i < snake_length; i++) {
//    display.fillRect(snake_tail[i][0], snake_tail[i][1], 2, 2, SSD1306_WHITE);  
//  }
  display.display();
} 

void gameover(int food_score) {
  
  int cursorposition = 64;
  
  display.clearDisplay(); 
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setCursor(10,cursorposition);
  
  for(int i = 0; i < 46; i++) {
    display.println(F("GAME OVER"));
    display.display();      // Show initial text
    delay(20);
    display.setCursor(10,cursorposition);
    cursorposition--;
    }

   display.setTextSize(1); // Draw 2X-scale text
   display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
   display.setCursor(40,42);
   display.print(food_score);
   display.println(" points!");
   display.display();      // Show initial text
   delay(1000);
   
}

void welcomescreen(){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setTextSize(3);
  display.setCursor(20,10);
  display.print("SNEK!");
  display.display();
  delay(100);
  display.clearDisplay();
  display.display();
}
