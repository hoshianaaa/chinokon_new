
//2019/01/25 :　VELOCITY_MMは正しいものが出て来るようにした

//i2cで送りたいのはVELOCITY_MMの値(最大100の桁まで行くので1000くらいまで送れれば大丈夫そう)




#include <Wire.h>
#include <FlexiTimer2.h>

const int DT = 100;      //微小時間、タイマー割込みの周期ms
const int D = 51;      //タイヤの直径mm

volatile long ENCODER_PULSE = 0;    //エンコーダのパルス
volatile uint8_t prev = 0;          
double ANGULAR = 2*PI*ENCODER_PULSE/400/4.4/5;    //タイヤの回転角度rad
double ANGULAR_VELOCITY_MM = 0;                   //タイヤの角速度rad/s --> flash()
double VELOCITY_MM = ANGULAR_VELOCITY_MM * D;     //車輪が生成する速度 mm/s
double PREV_ANGULAR;   //1時刻前の角度rad --> flash()
static double deb_angular;                      //ANGULAR_VELOCITY_MMを足していったもの(ANGUAR_VELOCITY_MMが正しいか確かめるため)
static double deb_distance;                     //VELOCITY_MMが正しいか確認用

void setup() 
{
  
  //i2c
  Wire.begin(1);
  Wire.onRequest(requestEvent);


  
  pinMode(2, INPUT); 
  pinMode(3, INPUT);
  
  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  
  Serial.begin(9600);

  //timer
  FlexiTimer2::set(DT, flash); //タイマー割込みの設定
  FlexiTimer2::start();
  
}



void loop()
{
  
   ANGULAR = 2 * PI * ENCODER_PULSE / 400 / 4.4 / 5;
   VELOCITY_MM = ANGULAR_VELOCITY_MM * D / 2;   
/*

   Serial.print(int(VELOCITY_MM));//i2cで送信する値
   Serial.print("   ang: ");
   Serial.print(deb_angular);
   Serial.print("  ang2: ");
   Serial.print(ANGULAR);
   Serial.print("   enc: ");
   Serial.print(ENCODER_PULSE);
   Serial.print("  ");

  Serial.print("  dis: ");
  Serial.println(deb_distance);
 
 */
   
}


void flash() {
  
 
  
  ANGULAR_VELOCITY_MM = 1000 * (ANGULAR - PREV_ANGULAR) / DT;             
  PREV_ANGULAR = ANGULAR;

  
                                                 
  
  deb_angular += ANGULAR_VELOCITY_MM*DT/1000;//ANGULAR_VELOCITYの値が正しいのか確かめるためのもの、回転角度rad
  deb_distance += VELOCITY_MM * DT /1000;
  
  
}

//i2c要求が来たら実行される関数
void requestEvent() {

  int vel = (int)VELOCITY_MM;
  Wire.write(highByte(vel)); 
  Wire.write(lowByte(vel));
  
}

//外部割込み関数、ENCODER_PULSEの値が変化する
void updateEncoder()
{
  uint8_t a = digitalRead(2);
  uint8_t b = digitalRead(3);
 
  uint8_t ab = (a << 1) | b;
  uint8_t encoded  = (prev << 2) | ab;

  if(encoded == 0b1101 || encoded == 0b0100 || encoded == 0b0010 || encoded == 0b1011){
    ENCODER_PULSE ++;
  } else if(encoded == 0b1110 || encoded == 0b0111 || encoded == 0b0001 || encoded == 0b1000) {
    ENCODER_PULSE --;
  }

  prev = ab;
}
