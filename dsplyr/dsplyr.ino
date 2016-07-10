#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <avr/pgmspace.h> 

#include <SPI.h>
#include <SD.h>

#define CLK 11  // MUST be on PORTB! (Use pin 11 on Mega)
#define OE  9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);
File myFiles[100];
//String fileNames[100];
//int fileIndexes[1000];
File myFile;
File root;

int fileindex;
// change this to match your SD shield or module;
//     Arduino Ethernet shield: pin 4
//     Adafruit SD shields and modules: pin 10
//     Sparkfun SD shield: pin 8
const int chipSelect = 53;

void setup() {
  
  fileindex = 0;
  
  Serial.begin(9600);
  Serial.println("Starting program");
  matrix.begin();
  
  matrix.setCursor(0, 0);    // start at top left, with one pixel of spacing
  matrix.setTextSize(1);     // size 1 == 8 pixels high
  matrix.setTextWrap(true); 
  matrix.setTextColor(matrix.Color333(7,7,7));

  
  matrix.print("Initializing SD card...");
  delay(1000);
  
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
  pinMode(SS, OUTPUT);
   
  if (!SD.begin(chipSelect)) {
    matrix.setCursor(0, 0);
    matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
    matrix.print("SD card not initialized!");
    delay(10000);
    return;
  }
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
  matrix.setCursor(0, 0);
  matrix.println("initialization done.");
  
  delay(1000);
  
}

void loop() {
  //matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
  root = SD.open("/");
  
  
  
  root.rewindDirectory();
  
  fileindex = 0;
  
  Serial.println("Begining directory scan");
  
  boolean endOfDirectory = false;
  
    while(!endOfDirectory) {
    Serial.print("Looking for file index: ");
    Serial.println(fileindex);
     myFile = root.openNextFile();
     if (myFile) {
       if (myFile.isDirectory()) {
        Serial.print(myFile.name());
        Serial.println(" is a directory");
        continue;
      }
      //myFiles[fileindex] = myFile;
      myFiles[fileindex] = myFile;
      Serial.print("Found file");
      Serial.println(myFile.name());
      //fileIndexes[fileindex] = fileindex;
      fileindex++;
      //myFile.close();
      
      
     } else {
        Serial.print("Error opening file: ");
        Serial.println(myFile.name());
        endOfDirectory = true;
     }
     
  }
  
  Serial.println("End of directory listing");
  Serial.println("Begin sorting");
  
  for(int i = 0; i < fileindex-1; i++) {
      for(int j = i+1; j < fileindex; j++) {
        String fileName = myFiles[i].name();
        if(fileName.compareTo(myFiles[j].name()) > 0) {
          File bigger = myFiles[i];
          File smaller = myFiles[j];
          myFiles[i] = smaller;
	  myFiles[j] = bigger;
        } 
      } 
  }
  
  for(int k=0; k<fileindex; k++){
    //Serial.print("Reading file: ");
    //Serial.println(myFiles[k].name());
    
    /*
    char __dataFileName[sizeof(fileNames[i])];
    fileNames[i].toCharArray(__dataFileName, sizeof(__dataFileName));
    */
    //myFile = SD.open(__dataFileName,FILE_READ);
    myFile = myFiles[k];
    
    //Serial.print("Opening file: ");
    //Serial.println(myFile.name());
    
    for(int i = 0; i < 32; i++) {
      
      for(int j = 0; j < 32; j++) {
        
            
        
        if(myFile.available()) {
          matrix.drawPixel(i,j, matrix.Color444(getVal(myFile.read()),getVal(myFile.read()),getVal(myFile.read())));
          /*if(j == 0) {
            Serial.println("Drawing row");
          }*/
          
        } else {
          
          matrix.setCursor(0, 0);
          matrix.print("Premature EOF");
          delay(1000);
        }
      }
    }
    
    if(myFile.available()) {
      byte timeDelay = getVal(myFile.read());
      if(timeDelay>10) {
        delay(timeDelay*1000);
      } else {
        delay(timeDelay*300);
      }
    } else {
      delay(10000);
    }
    // close the file:
    myFile.close(); 
  }
  
  /*
  while(!endOfDirectory) {
    Serial.print("Looking for file index: ");
    Serial.println(fileindex);
     myFile = root.openNextFile();
     if (myFile) {
       if (myFile.isDirectory()) {
        Serial.print(myFile.name());
        Serial.println(" is a directory");
        continue;
      }
      //myFiles[fileindex] = myFile;
      fileNames[fileindex] = myFile.name();
      Serial.print("Found file");
      Serial.println(myFile.name());
      //fileIndexes[fileindex] = fileindex;
      fileindex++;
      myFile.close();
      
      
     } else {
        Serial.print("Error opening file: ");
        Serial.println(myFile.name());
        endOfDirectory = true;
     }
     
  }
  
  Serial.println("End of directory listing");
  Serial.println("Begin sorting");
  
  for(int i = 0; i < fileindex-1; i++) {
      for(int j = i+1; j < fileindex; j++) {
        if(fileNames[i].compareTo(fileNames[j]) > 0) {
          String bigger = fileNames[i];
          String smaller = fileNames[j];
          fileNames[i] = smaller;
	  fileNames[j] = bigger;
        } 
      } 
  }
  
  for(int i=0; i<fileindex; i++){
    Serial.print("Reading file: ");
    Serial.println(fileNames[i]);
    
    char __dataFileName[sizeof(fileNames[i])];
    fileNames[i].toCharArray(__dataFileName, sizeof(__dataFileName));
    
    myFile = SD.open(__dataFileName,FILE_READ);
    
    if (myFile) {
      if (myFile.isDirectory()) {
        Serial.print(myFile.name());
        Serial.println(" is a directory");
        continue;
      }
      Serial.print("Opening file: ");
      Serial.println(myFile.name());
      for(int i = 0; i < 32; i++) {
        
        for(int j = 0; j < 32; j++) {
          
              
          
          if(myFile.available()) {
            matrix.drawPixel(i,j, matrix.Color444(getVal(myFile.read()),getVal(myFile.read()),getVal(myFile.read())));
            if(j == 0) {
              Serial.println("Drawing row");
            }
            
          } else {
            
            matrix.setCursor(0, 0);
            matrix.print("Premature EOF");
            delay(1000);
          }
        }
      }
      
      if(myFile.available()) {
        delay(getVal(myFile.read())*1000);
      } else {
        delay(10000);
      }
      // close the file:
      myFile.close();
    } else {
    	// if the file didn't open, print an error:
      //matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
      //matrix.setCursor(0, 0);
     // matrix.print("error opening file");//" + myFile.name());
      Serial.print("Error reading file: ");
      Serial.println(myFile.name());
      delay(1000);
      
    }
  }
  */
  delay(1000);
  /*
  while(true) { 
    
    //myFile = SD.open(fullname);
    myFile = root.openNextFile();
    
    
    if (myFile) {
      if (myFile.isDirectory()) {
        Serial.print(myFile.name());
        Serial.println(" is a directory");
        continue;
      }
      Serial.print("Opening file: ");
      Serial.println(myFile.name());
      for(int i = 0; i < 32; i++) {
        
        for(int j = 0; j < 32; j++) {
          
              
          
          if(myFile.available()) {
            matrix.drawPixel(i,j, matrix.Color444(getVal(myFile.read()),getVal(myFile.read()),getVal(myFile.read())));
            if(j == 0) {
              Serial.println("Drawing row");
            }
            
          } else {
            
            matrix.setCursor(0, 0);
            matrix.print("Premature EOF");
            delay(1000);
          }
        }
      }
      
      if(myFile.available()) {
        delay(getVal(myFile.read())*1000);
      } else {
        delay(10000);
      }
      // close the file:
      myFile.close();
    } else {
    	// if the file didn't open, print an error:
      matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
      matrix.setCursor(0, 0);
      matrix.print("error opening file");//" + myFile.name());

      delay(1000);
      break;
    }

  }*/
}

byte getVal(char c)
{
   if(c >= '0' && c <= '9')
     return (byte)(c - '0');
   else
     return (byte)(c-'a'+10);
}
