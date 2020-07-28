//------------------------------------------------------------------------------------
  #include <Wire.h>
  #include <ESP8266WiFi.h>
//------------------------------------------------------------------------------------
// Defining I/O Pins
//------------------------------------------------------------------------------------
  #define       LED0      2         // WIFI Module LED
  #define       LED1      D0        // Connectivity With Client #1
  #define       LED2      D2        // Connectivity With Client #2
  #define       BUTTON    D1        // Connectivity ReInitiate Button
  #define       TWI_FREQ  400000L   // I2C Frequency Setting To 400KHZ
  #define       gunLED    D8
  #define       gunLOW    D7
//------------------------------------------------------------------------------------
// BUTTON Variables
//------------------------------------------------------------------------------------
  int           ButtonState;
  int           LastButtonState   = LOW;
  int           LastDebounceTime  = 0;
  int           DebounceDelay     = 50;
  const String  ButtonColor       = "BLU";
//------------------------------------------------------------------------------------
// LED Delay Variables
//------------------------------------------------------------------------------------
  int           LEDState          = LOW;
  unsigned long CurrMillis        = 0;
  unsigned long PrevMillis        = 0;
  unsigned long Interval          = 1000;
//------------------------------------------------------------------------------------
// Authentication Variables
//------------------------------------------------------------------------------------
  char*         TKDssid;
  char*         TKDpassword;
  IPAddress     TKDServer(192,168,4,1);
  WiFiClient    TKDClient;
//====================================================================================

const int trigger = 12;

const int MPU_addr=0x68;
int16_t axis_X,axis_Y,axis_Z;
int minVal=265;
int maxVal=402;
double x;
double y;
double z;


  void setup() 
  {
    pinMode(trigger,INPUT);
    pinMode(gunLED,OUTPUT);
    pinMode(gunLOW,OUTPUT);
    pinMode(5,OUTPUT);
    digitalWrite(5,HIGH);
    digitalWrite(gunLOW,LOW);

  Wire.begin(0,4);//d76
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  
    /* ----------------------------------------------------------------------
     * Setting The I2C Pins SDA, SCL
     * Because We Didnt Specify Any Pins The Defult
     * SDA = D4 <GPIO2>, SCL = D5 <GPIO14> For ESP8266 Dev Kit Node MCU v3
     --------------------------------------------------------------------- */
   // Wire.begin();                   //Begginning The I2C
    
    // Setting Up The I2C Of The MPU9250 ------------------------------------
   // Wire.setClock(TWI_FREQ);        // Setting The Frequency MPU9250 Require
        
    // Setting The Serial Port ----------------------------------------------
    Serial.begin(115200);           // Computer Communication
    
    // Setting The Mode Of Pins ---------------------------------------------
    pinMode(LED0, OUTPUT);          // WIFI OnBoard LED Light
    pinMode(LED1, OUTPUT);          // Indicator For Client #1 Connectivity
    pinMode(LED2, OUTPUT);          // Indicator For Client #2 Connectivity
    pinMode(BUTTON, INPUT_PULLUP);  // Initiate Connectivity
    digitalWrite(LED0, !LOW);       // Turn WiFi LED Off
    
    // Print Message Of I/O Setting Progress --------------------------------
    Serial.println("\nI/O Pins Modes Set .... Done");

    // Starting To Connect --------------------------------------------------
    WiFi.mode(WIFI_STA);            // To Avoid Broadcasting An SSID
    WiFi.begin("TAKEONE");          // The SSID That We Want To Connect To

    // Printing Message For User That Connetion Is On Process ---------------
    Serial.println("!--- Connecting To " + WiFi.SSID() + " ---!");

    // WiFi Connectivity ----------------------------------------------------
    CheckConnectivity();            // Checking For Connection

    // Stop Blinking To Indicate Connected ----------------------------------
    digitalWrite(LED0, !HIGH);
    Serial.println("!-- Client Device Connected --!");

    // Printing IP Address --------------------------------------------------
    Serial.println("Connected To      : " + String(WiFi.SSID()));
    Serial.println("Signal Strenght   : " + String(WiFi.RSSI()) + " dBm");
    Serial.print  ("Server IP Address : ");
    Serial.println(TKDServer);
    Serial.print  ("Device IP Address : ");
    Serial.println(WiFi.localIP());
    
    // Conecting The Device As A Client -------------------------------------
    TKDRequest();
  }

//====================================================================================
  String gun;
  
  void loop()
  {
    digitalWrite(gunLOW,LOW);
     Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  axis_X=Wire.read()<<8|Wire.read();
  axis_Y=Wire.read()<<8|Wire.read();
  axis_Z=Wire.read()<<8|Wire.read();
  int xAng = map(axis_X,minVal,maxVal,-90,90);
  int yAng = map(axis_Y,minVal,maxVal,-90,90);
  int zAng = map(axis_Z,minVal,maxVal,-90,90);
   x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
   y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
   z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);
    x = map(x,0,360,-90,90);
    y = map(y,0,360,-90,90);
    z = map(z,0,360,-90,90);
    //Serial.print("Angle of inclination in X axis = ");
   Serial.print(x);Serial.print(" ");
   //Serial.print("Angle of inclination in Y axis= ");
   Serial.print(y);Serial.print(" ");
   //Serial.print("Angle of inclination in Z axis= ");
   Serial.println(z);
   //Serial.println("-------------------------------------------");
   
    digitalWrite(5,HIGH);
    gun = "GUN" + String(digitalRead(trigger));
    //ReadButton();
    //Serial.println(gun);
    if(digitalRead(trigger)==0){
      Serial.println("Shoooooot!");
      digitalWrite(gunLED,HIGH);
        TKDClient.println(17);
        TKDClient.flush();
      }
      else{
    digitalWrite(gunLED,LOW);
    TKDClient.println(16);
    TKDClient.flush();
     }
    
  }

//====================================================================================

  void ReadButton()
  {
    // Reading The Button
    int reading = digitalRead(BUTTON);

    // If It Doest Match The Previous State
    if(reading != LastButtonState)
    {
      LastDebounceTime = millis();
    }

    // To Iliminate Debounce
    if((millis() - LastDebounceTime) > DebounceDelay)
    {
      if(reading != ButtonState)
      {
        ButtonState = reading;
        
        if(ButtonState == LOW)
        {
          LEDState    = !digitalRead(LED1);
          digitalWrite(LED1, LEDState);
          Serial.println("<" + ButtonColor + "-SCORED>");
          TKDClient.println("<" + ButtonColor + "-SCORED>");
          TKDClient.flush();
        }
      }
    }

    // Last Button State Concidered
    LastButtonState = reading;
  }

//====================================================================================

  void CheckConnectivity()
  {
    while(WiFi.status() != WL_CONNECTED)
    {
      for(int i=0; i < 10; i++)
      {
        digitalWrite(LED0, !HIGH);
        delay(250);
        digitalWrite(LED0, !LOW);
        delay(250);
        Serial.print(".");
      }
      Serial.println("");
    }
  }

//====================================================================================

  void TKDRequest()
  {
    // First Make Sure You Got Disconnected
    TKDClient.stop();

    // If Sucessfully Connected Send Connection Message
    if(TKDClient.connect(TKDServer, 9001))
    {
      Serial.println    ("<" + ButtonColor + "-CONNECTED>");
      TKDClient.println ("<" + ButtonColor + "-CONNECTED>");
    }
  }

//====================================================================================
