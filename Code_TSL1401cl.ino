/*
TSL1401test --- Taos TSL1401 image sensor chip 2010-07-24
datasheet: http://www.ams.com/eng/content/download/250163/975677/file/TSL1401CL.pdf

trace: http://ap.urpi.fei.stuba.sk/sensorwiki/index.php/TSL1401_Line_Sensor
other inos:
  - http://forums.parallax.com/showthread.php/125594-TSL1401-and-Arduino
  - https://github.com/ap-tech/Getting-started-with-the-TSL1401CL-linescan-camera-with-arduino-and-processing.-/blob/master/TSL1401CL%20linescan%20camera%20code./Linescane_camera_code/Linescane_camera_code.ino
  

*/


//code permettant d'utiliser un TSL1401cl avec un Arduino Nano un nombre de 
//fois défini et d'envoyer ces données vers un ordinateur pour processing

                 
#define AOpin  6     // Analog output - orange
#define SIpin  4     // Start Integration - blanc
#define CLKpin 5     // Clock - rouge
                   // Vcc - jaune
                   // GND - noir et brun

#define TRIGINT 8 
#define TRIGREAD 7


#define NPIXELS 128  // Nombre de pixels/nombre d'informations à récupérer

byte Pixel[NPIXELS]; // tableau contenant les données de l'analyse


#define FASTADC 1   
// defines for setting and clearing register bits    //
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))  // Accélère le CAN d'un facteur 16
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))   //



void setup(void)
{
 pinMode(SIpin, OUTPUT);
 pinMode(CLKpin, OUTPUT);
 pinMode(TRIGINT, OUTPUT);
 pinMode(TRIGREAD, OUTPUT);
 //pinMode (AOpin, INPUT);

 digitalWrite(SIpin, LOW);   // IDLE state
 digitalWrite(CLKpin, LOW);  // IDLE state
 digitalWrite(TRIGINT, LOW);
 digitalWrite(TRIGREAD, LOW);
//////////////////////////
#if FASTADC             //
// set prescale to 16   //
sbi(ADCSRA,ADPS2);      //
cbi(ADCSRA,ADPS1);      //  Suite de l'accélération du CAN
cbi(ADCSRA,ADPS0);      //
#endif                  //
//////////////////////////
 Serial.begin (115200);
}
///////////////////////////////////////////////////////////////////////
//read an unsigned integer value already present on the serial port  //
unsigned long readserial()                                           //
{                                                                    //
  int incomingByte=Serial.read();                                    //
  unsigned long val=0;                                               //
  while (incomingByte!=-1)                                           //
  {                                                                  //
      if (incomingByte>='0' && incomingByte<='9')                    //    Fonction qui permet d'envoyer par voie-série
           val=val*10+incomingByte-'0';                              //    un temps d'intégration (sinon t_int par défaut)
       delay(10);                                                    //
       incomingByte=Serial.read();                                   //
  }                                                                  //
  return val;                                                        //
}                                                                    //
///////////////////////////////////////////////////////////////////////






unsigned long t_int=50000; //temps d'intégration en microsecondes







void loop (void)
{
   int i;
   int j;   
   unsigned long tstart,tstop;     
   unsigned long val=readserial();
  
  //////////////////////////////
  int x=Serial.available();   //
  while(x==0){                //
    delay(100);               //
    x=Serial.available();     // Attends un Input pour lancer les mesures
  }                           //
  for (i=0; i<30;i++){        //
    char phrase=Serial.read();//
  }/////////////////////////////

  
for(j=0; j<550;j++){      // Boucle déclenchant x mesures
   if (val) t_int=val;
   //delayMicroseconds (1);    
   digitalWrite (CLKpin, LOW);
   //delayMicroseconds (1); 
   digitalWrite (SIpin, HIGH);   
   digitalWrite (TRIGINT, HIGH);            
   //delayMicroseconds (1); 
   digitalWrite (CLKpin, HIGH);   
   digitalWrite (TRIGINT, LOW);            
   //delayMicroseconds (1); 
   digitalWrite (SIpin, LOW);  
                    

   for (i = 0; i<17; i++) {
    delayMicroseconds(1);
    digitalWrite (CLKpin, LOW);
    delayMicroseconds(1);
    digitalWrite (CLKpin, HIGH);    
   }
   tstart=micros();         
   for (; i < NPIXELS; i++) {
    delayMicroseconds(1);
    digitalWrite (CLKpin, LOW);
    delayMicroseconds(1);
    digitalWrite (CLKpin, HIGH);    
   }

   tstop=tstart+t_int;
   while (micros()<tstop) { //clock pour attendre un peu plus longtemps le temps d'intégration
      delayMicroseconds(1);
      digitalWrite (CLKpin, LOW);
      delayMicroseconds(1);
      digitalWrite (CLKpin, HIGH);    
   }  


   digitalWrite (CLKpin, LOW);
   //delayMicroseconds (1); 
   digitalWrite (SIpin, HIGH);   
   digitalWrite (TRIGREAD, HIGH);            
   //delayMicroseconds (1); 
   digitalWrite (CLKpin, HIGH);   
   digitalWrite (TRIGREAD, LOW);            
   //delayMicroseconds (1); 
   digitalWrite (SIpin, LOW);                                 
   for (i = 0; i < NPIXELS; i++) {
    //analogRead(AOpin);    
    Pixel[i] =(byte) (analogRead(AOpin)>>2); // 8-bit est assez (perte de précision 
    digitalWrite (CLKpin, LOW);              //sur les derniers bits à cause de l'accéleration du CAN)
    delayMicroseconds (1);
    digitalWrite (CLKpin, HIGH);
   }
   
////////////////////////////////////
 for (i = 0; i < NPIXELS; i++) {  //Envoi des données vers l'ordinateur pour procesing
     Serial.print(Pixel[i]);      //Une ligne contiendra les 128 données avec un espace entre chaque donnée
     Serial.print(' ');           //
  }                               //
  Serial.println("");             //On imprime la prochaine suite de données sur la ligne suivante
}//////////////////////////////////
}
