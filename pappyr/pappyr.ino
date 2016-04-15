 /* PINS for nodemcu 0.9
static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t D9   = 3;
static const uint8_t D10  = 1;
 */
 
#include <ESP8266WiFi.h>


#include <Servo.h> 

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal

const char* ssid     = "p";
const char* password = "a";
 
const char* host = "www.yr.no";
String url = "/sted/Norge/Oslo/Oslo/Oslo/varsel_time_for_time.xml"; // Bytt ut med din lokasjon

// finding values
boolean startRead = false;
int stringPos;
int stage;
String tempValue;
String charValue;
String dataValue;
String temperature1;
String weather1;
String temperature2;
String weather2;
String timefrom;

int loopCounter=0;
int displayView=1;

Servo myservo;

// Rutine for å skrive ut en melding til skjerm 

void displayInfo(String info){
    Serial.println(info); // skriv ut linjen til konsoll
    delay(300);
}

// Rutine for å skrive ut en kort melding til skjerm 
 
void displayInfoShort(String info){
    Serial.println(info); 
    delay(30);
}

// Definisjon fra yr på værtekst

int getWeatherDegree(String weatherNumber){
if(weatherNumber=="1"){return  1;}
if(weatherNumber=="2"){return  1;}
if(weatherNumber=="3"){return  2;}
if(weatherNumber=="4"){return  3;}
if(weatherNumber=="40"){return 4;}
if(weatherNumber=="5"){return  4;}
if(weatherNumber=="41"){return 4;}
if(weatherNumber=="24"){return 6;}
if(weatherNumber=="6"){return  6;}
if(weatherNumber=="25"){return 6;}
if(weatherNumber=="42"){return 8;}
if(weatherNumber=="7"){return  8;}
if(weatherNumber=="43"){return 8;}
if(weatherNumber=="26"){return 8;}
if(weatherNumber=="20"){return 8;}
if(weatherNumber=="27"){return 8;}
if(weatherNumber=="44"){return 7;}
if(weatherNumber=="8"){return  7;}
if(weatherNumber=="45"){return 7;}
if(weatherNumber=="28"){return 7;}
if(weatherNumber=="21"){return 7;}
if(weatherNumber=="29"){return 7;}
if(weatherNumber=="46"){return 5;}
if(weatherNumber=="9"){return  5;}
if(weatherNumber=="10"){return 5;}
if(weatherNumber=="30"){return 6;}
if(weatherNumber=="22"){return 6;}
if(weatherNumber=="11"){return 6;}
if(weatherNumber=="47"){return 8;}
if(weatherNumber=="12"){return 8;}
if(weatherNumber=="48"){return 8;}
if(weatherNumber=="31"){return 8;}
if(weatherNumber=="23"){return 8;}
if(weatherNumber=="32"){return 8;}
if(weatherNumber=="49"){return 7;}
if(weatherNumber=="13"){return 7;}
if(weatherNumber=="50"){return 7;}
if(weatherNumber=="33"){return 7;}
if(weatherNumber=="14"){return 7;}
if(weatherNumber=="34"){return 7;}
if(weatherNumber=="15"){return 9;}
return 1;
  }

String getWeatherText(String weatherNumber){
if(weatherNumber=="1" ){return  "Sol";}
if(weatherNumber=="2"){return  "Lett-  skyet";}
if(weatherNumber=="3"){return  "Delvis skyet";}
if(weatherNumber=="4"){return  "Skyet";}
if(weatherNumber=="40"){return "Lette  regnbyger";}
if(weatherNumber=="5"){return  "Regn-  byger";}
if(weatherNumber=="41"){return "Kraftige regnbyger";}
if(weatherNumber=="24"){return "Lette  regnbyger og torden";}
if(weatherNumber=="6"){return  "Regnbyger og torden";}
if(weatherNumber=="25"){return "Kraftige regnbyger og torden";}
if(weatherNumber=="42"){return "Lette   sluddbyger";}
if(weatherNumber=="7"){return  "Sluddbyger";}
if(weatherNumber=="43"){return "Kraftige sluddbyger";}
if(weatherNumber=="26"){return "Lette   sluddbyger og torden";}
if(weatherNumber=="20"){return "Sluddbyger og torden";}
if(weatherNumber=="27"){return "Kraftige sluddbyger og torden";}
if(weatherNumber=="44"){return "Lette   snobyger";}
if(weatherNumber=="8"){return  "Snobyger";}
if(weatherNumber=="45"){return "Kraftige snobyger";}
if(weatherNumber=="28"){return "Lette   snobyger og torden";}
if(weatherNumber=="21"){return "Snøbyger og torden";}
if(weatherNumber=="29"){return "Kraftige snobyger og torden";}
if(weatherNumber=="46"){return "Lett    regn";}
if(weatherNumber=="9"){return  "Regn";}
if(weatherNumber=="10"){return "Kraftig regn";}
if(weatherNumber=="30"){return "Lett    regn og torden";}
if(weatherNumber=="22"){return "Regn og torden";}
if(weatherNumber=="11"){return "Kraftig regn og torden";}
if(weatherNumber=="47"){return "Lett    sludd";}
if(weatherNumber=="12"){return "Sludd";}
if(weatherNumber=="48"){return "Kraftig sludd";}
if(weatherNumber=="31"){return "Lett    sludd og torden";}
if(weatherNumber=="23"){return "Sludd og torden";}
if(weatherNumber=="32"){return "Kraftig sludd og torden";}
if(weatherNumber=="49"){return "Lett sno";}
if(weatherNumber=="13"){return "Sno";}
if(weatherNumber=="50"){return "Kraftig sno";}
if(weatherNumber=="33"){return "Lett sno og torden";}
if(weatherNumber=="14"){return "Sno og  torden";}
if(weatherNumber=="34"){return "Kraftig sno og torden";}
if(weatherNumber=="15"){return "Taake";}
return "Ikke funnet type "+weatherNumber;
  }
  

void setup() {
  Serial.begin(9600);

  
 

  delay(100);
 
   WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    displayInfo("Connecting...");
  }
 
  displayInfo("");
  displayInfo("WiFi connected");  
  Serial.println(WiFi.localIP());


delay(3000);
   myservo.attach(4);

}

void loop() {


// Om loopCounter er 0, hent data fra yr. Hvis ikke, vis data som er hentet ned - skjermbilde for skjermbilde.
if(loopCounter==0){

  displayView=0; // Startskjermbilde
  stringPos = 0; // teller for inkomne tegn i XML
  stage = 0; // Brukes for å holde orden på hvor langt vi er kommet i XMLen
  tempValue = ""; // Midlertidig verdi
  charValue = ""; // Verdi på tegnet som nå ligger i buffer fra XML
  dataValue = ""; // Verdien hentet ut på det vi ønsker oss.
  temperature1 = ""; 
  weather1 = "";
  temperature2 = "";
  weather2 = "";
  timefrom = "";
  

  displayInfo("connecting to ");
  displayInfo(host);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    displayInfo("Connecting...");
  }
 
  displayInfo("");
  displayInfo("WiFi connected");  
  Serial.println(WiFi.localIP());
 
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    displayInfo("connection failed");
    return;
  }

  
  Serial.print("Requesting URL: ");

  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(500);
  
  // Read all the lines of the reply from server and print them to Serial
 
  //memset( &inString, 0, 32 ); //clear inString memory

  while(client.available()){
    //String line = client.readStringUntil('\r');
    //Serial.print(line);
    

 
    char c = client.read();
    //displayInfo(c);


    
     if (c == '<' ) { // Sjekk om vi har starten på en xml node
        startRead = true; // i såfall, la oss lese inn
      }

      //Kort forklart - leser XML fra yr tegn for tegn. Om den finner tegnet < er det starten på en XML-node. Leser så inn XML 
      //noden inn i en streng. Sjekker så, for hvert tegn, om strengen innholder kombinasjonen av tegn vi ønsker oss. Om den 
      // finner kombinasjonen av tegn, begynner den å lese ut / lagre funnet data. Bruker så stage til å sjekke hvor vi er i 
      // xml og tilegner den utleste verdien til en variabel (vær, temperatur, eller tidspunkt.)

      // leser inn innhold til en streng
      if(startRead == true){
          charValue = String(c); // konverter til string
          tempValue = tempValue + charValue; // legg til string
          
          stringPos++;

      // les ut vær-kode: sjekk om XML-strengen inneholder symbol number "numberEx=" og sjekk om vi er forbi tegn 38
           if(tempValue.indexOf("numberEx=") > 0)
            {
               if (c == '"' && dataValue != "=" ) {
                displayInfo(tempValue);
                displayInfo("Fant ver");
                startRead = false; // slutt å lese
                displayInfo(dataValue); // skriv ut linjen til skjerm
                tempValue = ""; // nullstill streng
                stringPos = 0; // Nullstill teller+

                break;
                
                stage++;
                

               }else{

                 if (c == '"' && dataValue == "=" ){
                  dataValue="";
                }else{
               
                dataValue = dataValue + charValue;
               }
               }
            }
       
/*
      // les ut temperatur. sjekk om det inneholder temperatur, og sjekk om vi er forbi tegn 35
          if(tempValue.indexOf("temperature") > 0 && stringPos > 35)
            {
               if (c == '"' ) {
                displayInfo("Fant grader");
                startRead = false; // slutt å lese
                displayInfo("dataValue: "+dataValue); // skriv ut linjen til skjerm

                tempValue = ""; // nullstill streng
                stringPos = 0; // Nullstill teller+

                stage++;

              
                 }else{
                dataValue = dataValue + charValue;
               }
            }

      if(tempValue.indexOf("time from=") > 0 && stringPos > 23 && weather1!="" && timefrom=="")
            {
               if (stringPos == 26 ) {
                displayInfo("Fant tidsinterval");
                startRead = false; // slutt å lese
                displayInfo("dataValue: "+dataValue); // skriv ut linjen til skjerm

                tempValue = ""; // nullstill streng
                stringPos = 0; // Nullstill teller+

                stage++;

              
                 }else{
                dataValue = dataValue + charValue;
               }
            }   */
        
        if (c == '>' ) { // ferdig med en xml-node
          startRead = false; // slutt å lese
          displayInfoShort(tempValue); // skriv ut linjen til konsoll
          tempValue = ""; // nullstill streng
          stringPos = 0; // Nullstill teller
        }

        //Stages


     /* if(stage==1 && weather1==""){
         weather1=getWeatherText(dataValue);
         displayInfo(weather1);
         dataValue="";
        }else if(stage==2 && temperature1==""){
         temperature1=dataValue;
         dataValue="";
        }else if(stage==3 && timefrom==""){
         timefrom="Fra kl "+dataValue+":00";
         dataValue="";
        }else if(stage==4 && weather2==""){
         weather2=getWeatherText(dataValue);
         dataValue="";
        }else if(stage==5 && temperature2==""){
         temperature2=dataValue;
         dataValue="";
         displayInfo("All done!: "+stage);
         break;
        }
       */
        
      }
      
    // end
      

  }

  displayInfo("closing connection");

 } //end loopcounter

  
  
 loopCounter++;
 if(loopCounter==2){loopCounter=0;}

// 300 sekunder x 2 = 10 minutter. Oppdater med data fra yr hvert 10. minutt. 

  int pos;

  int posw = getWeatherDegree(dataValue);
  posw = posw *20;

  for(pos = 0; pos <=  posw; pos += 1) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
 /* for(pos = 180; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } */

  myservo.write(pos+1);

  //Oppdater skjerm hvert 3 sekund. 
  delay(300000);

  

}


