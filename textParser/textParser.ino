#include "ctype.h"

String p = "1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;0,0,0;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;0,0,0;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;0,0,0;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;0,0,0;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;0,0,0;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;";

const int NUMPIXELS = 10;

int pixels[4][NUMPIXELS][NUMPIXELS][3];

String pixelTempStr = "";

int colorIndex = 0;

int savedLedCount = 0;
int quadIndex = 0;
int rowIndex = 0;
int colIndex = 0;

int pixelTempInt[3];
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
    for(int i = 0; i < p.length(); i++){
      if(isDigit(p[i])){
        pixelTempStr += p[i];
      } 
      else if(p[i] == ','){
        pixelTempInt[colorIndex] = pixelTempStr.toInt();
        colorIndex++;
        pixelTempStr = "";
      }
      else if(p[i] == ';'){

        //get quadrant
        if(savedLedCount < 100){
          int quadIndex = savedLedCount/10 % 2;
        }
        else{
          int quadIndex = savedLedCount/100 % 2;
        }
        if(savedLedCount > NUMPIXELS * 2 * NUMPIXELS){
          quadIndex += 2;
        }
        
        //get row
        rowIndex = savedLedCount / NUMPIXELS % NUMPIXELS;

        //get column
        if(savedLedCount < NUMPIXELS){
          colIndex = savedLedCount;
        }
        else{
          colIndex = savedLedCount % NUMPIXELS;
        }

        
        Serial.println("quadIndex: " + String(quadIndex));
        Serial.println("rowIndex: " + String(rowIndex));
        Serial.println("colIndex: " + String(colIndex));
        Serial.read();
        
        // set color for current Pixel
        for(int j = 0; j < 3; j++){
          pixels[quadIndex][rowIndex][colIndex][j] = pixelTempInt[j];
        }

        savedLedCount ++;
        pixelTempStr = "";
        colorIndex = 0;
      }
    }

    /*for(int i = 0; i < 4; i++){
      Serial.println("Quad " + String(i + 1));
      for(int j = 0; j < NUMPIXELS; j++){
        for(int k = 0; k < NUMPIXELS; k++){
          String r = String(pixels[i][j][k][0]);
          String g = String(pixels[i][j][k][1]);
          String b = String(pixels[i][j][k][2]);
          Serial.print("{" + r + "," + g + "," + b + "} ");
        }

        Serial.println("");
      }
    }*/
    delay(3000);
}
