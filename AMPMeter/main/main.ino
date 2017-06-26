//http://tyk-systems.com/PowerMeasure/PowerProgram.html
//クランプ式電流センサーを用いたArduino消費電力測定のプログラム(スケッチ)
// XBee(またはUSB)経由のシリアル通信でデータ送信

//LED点灯追加
//送信機能はETHERNET…？

int aveN=600; //average time 600で 1 scan/15秒程度
int sumN=100; // average time, 100回の平均

float sensV0 = 0;  // variable to store the value coming from the sensor
float sensV1 = 0;  // variable to store the value coming from the sensor
float sensV0ave = 0;  // variable to store the value coming from the sensor
float sensV1ave = 0;  // variable to store the value coming from the sensor
float SumW=0; //total Voltage
float Read0=0; //読み取った値0
float Read1=0; //読み取った値1
float TotalW[168]={
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //1week=168時間分のデータを記録する。
byte TimeNow=0; //0-167h
byte TimeBefore=167;//0-167時間のうち、どの時間を計測しているか。

long MsecNow=0; //Arduino起動から経過した時間(ミリ秒)
long SecNow=0; //Arduino起動から経過した時間(秒)
long MsecBefore=0; //1回前の測定のミリ秒。消費電力量を求めるため、一回の測定に要した時間を計算する。
float dwell=0; //積分時間。

float Coeff=42.96875; //センサーが出力する電流量(A,アンペア)から電力(W,ワット)を求める係数。
  //電流計は150mV=30A=3000W設定。電圧計測1.1V=1024

void setup() {
  Serial.begin(9600); //シリアル通信を開始。
  analogReference(INTERNAL);//電圧計測の基準として、内部電圧の1.1Vを使用する。
}

//これより下を繰り返して測定およびデータ送信する。
void loop() {
    sensV0ave=0; //600回平均を100回平均するための値の初期化
    sensV1ave=0;
  for (int j=0;j < aveN; j++){
  sensV0=0; //600回の平均を取る値の初期化
  sensV1=0; //
  //1000回の平均を取る。
  for (int i=0; i < sumN; i++){
    Read0=(float) analogRead(0); //Analog INの0番端子の電圧を読む。
    Read1=(float) analogRead(3); //Analog INの3番端子の電圧を読む。
    sensV0=sensV0+Read0*Read0/sumN; //二乗の和をとる。電力は電流の二乗に比例。
    sensV1=sensV1+Read1*Read1/sumN;
   }
  sensV0ave=sensV0ave + Coeff*sqrt(sensV0)/aveN; //600回計測して平均値を求める。
  sensV1ave=sensV1ave + Coeff*sqrt(sensV1)/aveN;
  }
  SumW=sensV0ave+sensV1ave; //200Vでは2本の端子の和で総電力量となる。
  Total(); //関数Totalを呼んで、メモリに消費電力量を記入。
  //現在の消費電力を出力。
   Serial.print("sec=");  Serial.print(SecNow); //現在の時間(秒)
   Serial.print(",Pin0=");  Serial.print(sensV0ave); //Pin0の電力
   Serial.print(",Pin1="); Serial.print(sensV1ave); //Pin1の電力
   Serial.print(",Total="); Serial.println(SumW);  //総電力
}

void Total(){
  MsecNow=millis(); //ミリ秒を求める。
   dwell=float (MsecNow-MsecBefore)/1000; //1回の測定に要した時間(sec)
  MsecBefore=millis(); //次の測定の時に呼び出して、積算時間を算出するための「前回の測定時間」
   SecNow=MsecNow/1000; //秒
   TimeNow=((SecNow/3600) % 168) ; //少数は切り捨てられる。
   if (TimeNow > TimeBefore){TotalW[TimeNow]=0;} //1時間をまたいだ時に、TimeNow(整数)が1増えるため、がTotalW(消費電力量)を0に戻す。これにより、１時間の消費電力が求まる。
   TimeBefore=TimeNow; //1回前に測定した時間。
   TotalW[TimeNow]=TotalW[TimeNow]+SumW*dwell/3600; //現在の時間の和を計算。1秒の時、1/3600だけ１時間の和に加える。
  for (int i=167; i< =0; i--){ ///過去24時間について１時間ごとの平均を出力。
       if (i==TimeNow){Serial.print("Now");} //現在和を計算している時間を示す。これにより、0-167hのうち、現在測定している時間が示される。
       Serial.print("H");
       Serial.print(i);
       Serial.print("=");
       Serial.println(TotalW[i]); //消費電力量を表示する。
       delay(10); //シリアル通信の遅れに対応するためにdelayで少し処理を遅くする。
      }

}
