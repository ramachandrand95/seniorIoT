#define turbidity A0
void setup() {
  Serial.begin(9600); //Baud rate: 9600
  pinMode(turbidity, INPUT);
}
void loop() {
Serial.print("Turbidity 1 mess: ");
  int sensorValue = analogRead(turbidity); 
  Serial.println(sensorValue);
  float voltage = sensorValue * (5.0/1024.0);
  //4.20025 equation = 0
  Serial.print(voltage);Serial.println(" V");
  double NTU = -1120.4*(voltage*voltage)+5742.3*(voltage)-4352.9;
  Serial.print(NTU); Serial.println(" NTU");
  delay(500);
  Serial.print("Turbidity 2 new: ");
  sensorValue = analogRead(A1);
  Serial.println(sensorValue);
  voltage = sensorValue * (5.0/1024.0);
  Serial.print(voltage);Serial.println(" V");
  NTU = -1120.4*(voltage*voltage)+5742.3*(voltage)-4352.9;
  Serial.print(NTU); Serial.println(" NTU");
  delay(2000);
  Serial.println("");
  Serial.println("Sampling 10 times and averaging...");
  Serial.print(getTurbidity()); Serial.println("Average NTU");
  
}

float getTurbidity(){
  float turbidity1[10];
  for(int i = 0; i< 10; i++){
     int sensorValue = analogRead(turbidity);
     float voltage = sensorValue * (5.0/1024.0);
     float NTU = -1120.4*(voltage*voltage)+5742.3*(voltage)-4352.9;
     turbidity1[i] = NTU;
  }
  float average = averageArray(turbidity1);
  return average;
}
float averageArray(float *array ){
   float sum, avg;
   int loop;
   sum = avg = 0;  
   for(loop = 0; loop < 10; loop++) {
      sum = sum + array[loop];
   }
   avg = sum / loop;   
  return avg;
}
