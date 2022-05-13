/*
 * Code for the Briefcase weighing scale designed for Waggies by Maggies and Friends
 * By: Sophia Smith
 * Univeristy of Deleware
 * Date: May 13th, 2022
 * License: This code is public domain. 
 */


#include "HX711.h"

// pins 
const int BUTTON = 12; //dog treat type button
const int TARE= 13; //tare button
#define LOADCELL_DOUT_PIN  10
#define LOADCELL_SCK_PIN  11

//variables
#define calibration_factor -774873.31 //This value is obtained using the SparkFun_HX711_Calibration sketch
int state=1; //controls which dog treat type is currently selected.
float weight=0.00;
float previousWeight=20;
int unlockDraw=1; //controls when the graphics are redrawn on the screen

//LCD
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>

#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN   0x07FF
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

MCUFRIEND_kbv tft;

HX711 scale;

void setup() {
  //Setup scale readings
  Serial.begin(9600);
  Serial.println("HX711 scale");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

  //Setup LCD
  // Reading TFT ID:
  uint16_t ID=tft.readID();
  Serial.begin(9600);
  Serial.println(ID); 
  //Initializing TFT display:
  tft.begin(ID);
  tft.fillScreen(BLUE);
  tft.setRotation(1);
  tft.setCursor(80,95);
  tft.setTextSize(4);
  tft.setTextColor(WHITE);
  tft.print("Start Weighing!");
  tft.setRotation(0);
 
}

void loop() {
  //Reset unlockDraw variable each time 
  unlockDraw=0;
  
  //Scale readings. Open the serial monitor to view print out.
  Serial.print("Reading: ");
  Serial.print(scale.get_units()*16,3); //scale.get_units() returns a float
  Serial.println(" oz"); //You can change this to kg but you'll need to refactor the calibration_factor
 
  previousWeight=weight;
  weight= scale.get_units()*16; //multiply by 16 to convert to ounces

  //Check if scale reading has changed. Will not redraw if the weight change is less than 0.0192 oz
  if(abs(previousWeight-weight) >= 0.0012*16){
    unlockDraw=1;
  }
  else{
    unlockDraw=0;
  }

  
  //digitalRead(BUTTON) returns 1 if button pressed
  if(digitalRead(BUTTON)){
    if(state !=3){
      state+=1;
      unlockDraw=1;
    }
    else{
      state=1;
      unlockDraw=1;
    }
  }

  if(digitalRead(TARE)){
    scale.tare();
    Serial.println("Scale tared");
    weight=0.0;
    unlockDraw=1;
  }
 
    switch(state){
    //Upper thresholds have 0.004 added to account for rounding of the weight variable. If weight= 8.303, happy face will still show and 8.30 will be displayed on the screen.
      case 1:
        if(unlockDraw){
          if(weight > 7.995 && weight <= 8.304){
            //Draw happy face
            tft.fillScreen(BLACK);
            tft.fillCircle(180,240, 120, GREEN);  //face
            tft.fillCircle(210,190, 28, BLACK); //left eye
            tft.fillCircle(210,290, 28, BLACK);  // right eye
            
            tft.fillCircle(125, 240, 50, BLACK); // Mouth
            tft.fillRect(125, 190, 52, 105, GREEN);  //Rectangle to create half circle
           
            //Display weight
            tft.setRotation(1); //set Landscape mode
            tft.setCursor(10,295);
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.print("Weight: ");
            tft.print(abs(weight),2);
            tft.print(" oz");
            
            //Display treat type
             tft.setCursor(320, 275);
            tft.setTextColor(RED);
            tft.print("Chicken");
            tft.setTextColor(WHITE);
            tft.print(" or");
            tft.setCursor(255,285);
            tft.setTextColor(WHITE);
            tft.print("Type: ");
            tft.setCursor(320, 295);
            tft.setTextColor(GREEN);
            tft.print("Sweet Potato");
            tft.setRotation(0);
          }
          else if(weight >8.304){
            //Draw minus sign
            tft.fillScreen(BLACK);
            tft.fillRect(145, 120, 70, 240, RED);  //horitzontal 
            
            //Display weight
            tft.setRotation(1); //set Landscape mode
            tft.setCursor(10,295);
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.print("Weight: ");
            tft.print(abs(weight),2);
            tft.print(" oz");

            //Display treat type
             tft.setCursor(320, 275);
            tft.setTextColor(RED);
            tft.print("Chicken");
            tft.setTextColor(WHITE);
            tft.print(" or");
            tft.setCursor(255,285);
            tft.setTextColor(WHITE);
            tft.print("Type: ");
            tft.setCursor(320, 295);
            tft.setTextColor(GREEN);
            tft.print("Sweet Potato");
            tft.setRotation(0);
          }
          else{
             //Draw plus sign
            tft.fillScreen(BLACK);
            tft.fillRect(60, 205, 240, 70, YELLOW);  //vertical
            tft.fillRect(145, 120, 70, 240, YELLOW);  //horitzontal
            
            //Display weight
            tft.setRotation(1); //set Landscape mode
            tft.setCursor(10,295);
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.print("Weight: ");
            tft.print(abs(weight),2);
            tft.print(" oz");

            //Display treat type
            tft.setCursor(320, 275);
            tft.setTextColor(RED);
            tft.print("Chicken");
            tft.setTextColor(WHITE);
            tft.print(" or");
            tft.setCursor(255,285);
            tft.setTextColor(WHITE);
            tft.print("Type: ");
            tft.setCursor(320, 295);
            tft.setTextColor(GREEN);
            tft.print("Sweet Potato");
            tft.setRotation(0);
            
          }
        }
        break;
        
      case 2:
        if(unlockDraw){
          if(weight > 8.395 && weight <= 8.704){
            //Draw happy face
            tft.fillScreen(BLACK);
            tft.fillCircle(180,240, 120, GREEN);  //face
            tft.fillCircle(210,190, 28, BLACK); //left eye
            tft.fillCircle(210,290, 28, BLACK);  // right eye
            
            tft.fillCircle(125, 240, 50, BLACK); // Mouth
            tft.fillRect(125, 190, 52, 105, GREEN);  //Rectangle to create half circle
           
            //Display weight
            tft.setRotation(1); //set Landscape mode
            tft.setCursor(10,295);
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.print("Weight: ");
            tft.print(abs(weight),2);
            tft.print(" oz");

            //Display treat type
            tft.setCursor(250,295);
            tft.setTextColor(YELLOW);
            tft.print("Type: Peanut Butter");
            tft.setRotation(0);
          }
          else if(weight >8.704){
            //Draw minus sign
            tft.fillScreen(BLACK);
            tft.fillRect(145, 120, 70, 240, RED);  //horitzontal 
            
            //Display weight
            tft.setRotation(1); //set Landscape mode
            tft.setCursor(10,295);
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.print("Weight: ");
            tft.print(abs(weight),2);
            tft.print(" oz");

            //Display treat type
            tft.setCursor(250,295);
            tft.setTextColor(YELLOW);
            tft.print("Type: Peanut Butter");
            tft.setRotation(0);
          }
          else{
             //Draw plus sign
            tft.fillScreen(BLACK);
            tft.fillRect(60, 205, 240, 70, YELLOW);  //vertical
            tft.fillRect(145, 120, 70, 240, YELLOW);  //horitzontal
            
            //Display weight
            tft.setRotation(1); //set Landscape mode
            tft.setCursor(10,295);
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.print("Weight: ");
            tft.print(abs(weight),2);
            tft.print(" oz");

            //Display treat type
            tft.setCursor(250,295);
            tft.setTextColor(YELLOW);
            tft.print("Type: Peanut Butter");
            tft.setRotation(0);
            
          }
        }
        
        break;
        
      case 3:
      if(unlockDraw){
          if(weight >= 7.995 && weight <= 8.104){
            //Draw happy face
            tft.fillScreen(BLACK);
            tft.fillCircle(180,240, 120, GREEN);  //face
            tft.fillCircle(210,190, 28, BLACK); //left eye
            tft.fillCircle(210,290, 28, BLACK);  // right eye
            
            tft.fillCircle(125, 240, 50, BLACK); // Mouth
            tft.fillRect(125, 190, 52, 105, GREEN);  //Rectangle to create half circle
           
            //Display weight
            tft.setRotation(1); //set Landscape mode
            tft.setCursor(10,295);
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.print("Weight: ");
            tft.print(abs(weight),2);
            tft.print(" oz");

            //Display treat type
            tft.setCursor(250,295);
            tft.setTextColor(CYAN);
            tft.print("Type: Bits");
            tft.setRotation(0);
          }
          else if(weight >8.104){
            //Draw minus sign
            tft.fillScreen(BLACK);
            tft.fillRect(145, 120, 70, 240, RED);  //horitzontal 
            
            //Display weight
            tft.setRotation(1); //set Landscape mode
            tft.setCursor(10,295);
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.print("Weight: ");
            tft.print(abs(weight),2);
            tft.print(" oz");

            //Display treat type
            tft.setCursor(250,295);
            tft.setTextColor(CYAN);
            tft.print("Type: Bits");
            tft.setRotation(0);
          }
          else{
             //Draw plus sign
            tft.fillScreen(BLACK);
            tft.fillRect(60, 205, 240, 70, YELLOW);  //vertical
            tft.fillRect(145, 120, 70, 240, YELLOW);  //horitzontal
            
            //Display weight
            tft.setRotation(1); //set Landscape mode
            tft.setCursor(10,295);
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.print("Weight: ");
            tft.print(abs(weight),2);
            tft.print(" oz");

            //Display treat type
            tft.setTextColor(CYAN);
            tft.setCursor(250,295);
            tft.print("Type: Bits");
            tft.setRotation(0);
          }
        break;
     }
    }
  
}
