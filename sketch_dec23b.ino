int analogPin[3] = {0,1,2};     // potentiometer wiper (middle terminal) connected to analog pin 3
                        // outside leads to ground and +5V
int val[3] = {0,0,0};           // variable to store the value read
int val_old[3] = {0,0,0};
void setup()
{
  Serial.begin(9600);          //  setup serial
}


void loop()
{
  for(int j=0;j<3;j++){
  val[j] = analogRead(analogPin[j]);
  if((val_old[j]+7 < val[j])||(val_old[j]-7 > val[j]))
  {
      val_old[j] = val[j];
      switch(j){
        case 0 :  Serial.print("s_vol=");
                  break;
        case 1 :  Serial.print("s_bas=");
                  break;
        case 2 :  Serial.print("s_tre=");
                  break;         
                  

 
      }
      if(val[j]<10){
        Serial.print("00");
        Serial.println(val[j]);
      }
      else if(val[j] < 100){
        Serial.print("0");
        Serial.println(val[j]);
      }
      else if(val[j] <= 999){ 
      Serial.println(val[j]);
      }else if(val[j] >= 1000){
        Serial.println("999");
      }
  }
              // debug value
  }
}
