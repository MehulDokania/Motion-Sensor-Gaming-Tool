//-- Libraries Included --------------------------------------------------------------
  #include <ESP8266WiFi.h>
//------------------------------------------------------------------------------------
  // Define I/O Pins
  #define     LED0      2           // WIFI Module LED
  #define     LED1      D0          // Connectivity With Client #1
  #define     LED2      D2          // Connectivity With Client #2
  #define     BUTTON    D1          // Connectivity ReInitiate Button
//------------------------------------------------------------------------------------
  // Authentication Variables
  char*       TKDssid;              // SERVER WIFI NAME
  char*       TKDpassword;          // SERVER PASSWORD
//------------------------------------------------------------------------------------
  #define     MAXSC     2           // MAXIMUM NUMBER OF CLIENTS
  WiFiServer  TKDServer(9001);      // THE SERVER AND THE PORT NUMBER
  WiFiClient  TKDClient[MAXSC];     // THE SERVER CLIENTS
//====================================================================================

int leftView;
int rightView;
int jump;
int shoot;
int message;

//----------the Pins which will be lowed to trigger buttons in UnoJoy:-
int ViewPin = 13;// to A0
int jumpPin = 12;//to 5
int shootPin = 14;// to 4

//------Variables for input from the donator NODEMCu (For Mehul)
int inLeftView;
int inRightView;
int inJump;
int inShoot;

  void setup()
  {
    // Setting The Serial Port
    Serial.begin(115200);           // Computer Communication
    
    // Setting The Mode Of Pins
    pinMode(LED0, OUTPUT);          // WIFI OnBoard LED Light
    pinMode(LED1, OUTPUT);          // Indicator For Client #1 Connectivity
    pinMode(LED2, OUTPUT);          // Indicator For Client #2 Connectivity
    pinMode(BUTTON, INPUT_PULLUP);  // Initiate Connectivity
    pinMode(ViewPin,OUTPUT);
   // pinMode(rightViewPin,OUTPUT);
    pinMode(jumpPin,OUTPUT);
    pinMode(shootPin,OUTPUT);
    
    // Print Message Of I/O Setting Progress
    Serial.println();
    Serial.println("I/O Pins Modes Set .... Done");

    // Setting Up A Wifi Access Point
    SetWifi("TAKEONE", "");
  }

//====================================================================================
  
  void loop()
  {
    
    IsClients2();
    IsClients();

    //------Harsh code for unoJoy

    //Camera:

    if(inLeftView == 0 and inRightView == 0){
      analogWrite(ViewPin,128);
    }
    else
    if(inRightView == 1){
      analogWrite(ViewPin,255);
    }
    else
    if(inLeftView == 1){
      analogWrite(ViewPin,0);
    }

  //Jump:
  if(inJump == 1){
    digitalWrite(jumpPin,LOW);
  }
  else{
    digitalWrite(jumpPin,HIGH);
  }

  //Shoot:
  if(inShoot == 1){
    digitalWrite(shootPin,LOW);
  }
  else{
    digitalWrite(shootPin,HIGH);
  }
  } 

//====================================================================================
  
  void SetWifi(char* Name, char* Password)
  {
    // Stop Any Previous WIFI
    WiFi.disconnect();

    // Setting The Wifi Mode
    WiFi.mode(WIFI_AP_STA);
    Serial.println("WIFI Mode : AccessPoint Station");
    
    // Setting The Access Point
    TKDssid      = Name;
    TKDpassword  = Password;
    
    // Starting The Access Point
    WiFi.softAP(TKDssid, TKDpassword);
    Serial.println("WIFI < " + String(TKDssid) + " > ... Started");
    
    // Wait For Few Seconds
    delay(1000);
    
    // Getting Server IP
    IPAddress IP = WiFi.softAPIP();
    
    // Printing The Server IP Address
    Serial.print("AccessPoint IP : ");
    Serial.println(IP);

    // Starting Server
    TKDServer.begin();
    Serial.println("Server Started");
  }

//====================================================================================

  void IsClients()
  {
    if(TKDServer.hasClient())
    {
      WiFiClient TKDClient = TKDServer.available();
      
      if(digitalRead(LED0) == HIGH) digitalWrite(LED0, LOW);
      
      digitalWrite(LED1, HIGH);

      while(1)
      {

        //--[ Draft ] ---------------------------------------------------

        
              
        //---------------------------------------------------------------
        // If Clients Are Connected 
        //---------------------------------------------------------------
        
        if(TKDClient.available())
        {
          // Here We Read The Message
          String Message = TKDClient.readStringUntil('\r');
          // Here We Print The Message On The Screen
          Serial.println(Message);
          // Here We Reply To The Client With A Message
          TKDClient.print("\nWe Got Your Message");
          TKDClient.flush();
        }
        
        //---------------------------------------------------------------
        // If Clients Are Disconnected
        //---------------------------------------------------------------
        
        if(!TKDClient || !TKDClient.connected())
        {
          // Here We Turn Off The LED To Indicated The Its Disconnectted
          digitalWrite(LED1, LOW);
          // Here We Jump Out Of The While Loop
          break;
        }

        //----------------------------------------------------------------
        
      }
      
    }
    else
    {
      // This LED Blinks If No Clients Where Available
      digitalWrite(LED0, HIGH);
      delay(250);
      digitalWrite(LED0, LOW);
      delay(250);
    }
  }

//====================================================================================

  void IsClients2()
  {
    if (TKDServer.hasClient())
    {
      for(int i = 0; i < MAXSC; i++)
      {
        //find free/disconnected spot
        if (!TKDClient[i] || !TKDClient[i].connected())
        {
          if(TKDClient[i]) TKDClient[i].stop();
          TKDClient[i] = TKDServer.available();
          Serial.print("New Client : "); Serial.print(String(i+1) + " - ");
          continue;
        }
      }
      // no free / disconnected spot so reject
      digitalWrite(LED1, HIGH);
      WiFiClient TKDClient = TKDServer.available();
      TKDClient.stop();
    }

    //check clients for data -------------------------------------------------------
    
    for(int i = 0; i < MAXSC; i++)
    {
      if (TKDClient[i] && TKDClient[i].connected())
      {
        if(TKDClient[i].available())
        {
          // If Any Data Was Available We Read IT
          while(TKDClient[i].available()) 
          {
            // Read From Client
            message = TKDClient[i].readStringUntil('\n').toInt();
            
            Serial.println(message);
          
  //Serial.println(strcmp(message , "LEFT1"));
            if(message==11 ){
                inLeftView = 1;
                inRightView = 0;
               // Serial.println("GOT LEFT BABA");
              }
            
            if(message == 13){
                inLeftView = 0;
                inRightView = 1;
            }
            
            if( message==19 ){
              inLeftView = 0;
              inRightView = 0;
              //Serial.println("GOT MID BABA");
              
            }
            if(message == 15){
                inJump = 1;
              }
            if(message == 10){
                inJump = 0;
              }
              if(message == 17){
                inShoot = 1;
                Serial.println("Gun chlaa re bba");
              }
              if(message == 16){
                inShoot = 0;
                Serial.println("Gun chlaa re babababababa");
              }
             // Reply To Client
             TKDClient[i].println("<OK>");
          }
        }
      }
    }
  }

//====================================================================================
