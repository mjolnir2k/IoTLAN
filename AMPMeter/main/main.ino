//http://tyk-systems.com/PowerMeasure/PowerProgram.html
//�N�����v���d���Z���T�[��p����Arduino����d�͑���̃v���O����(�X�P�b�`)
// XBee(�܂���USB)�o�R�̃V���A���ʐM�Ńf�[�^���M

//LED�_���ǉ�
//���M�@�\��ETHERNET�c�H

int aveN=600; //average time 600�� 1 scan/15�b���x
int sumN=100; // average time, 100��̕���

float sensV0 = 0;  // variable to store the value coming from the sensor
float sensV1 = 0;  // variable to store the value coming from the sensor
float sensV0ave = 0;  // variable to store the value coming from the sensor
float sensV1ave = 0;  // variable to store the value coming from the sensor
float SumW=0; //total Voltage
float Read0=0; //�ǂݎ�����l0
float Read1=0; //�ǂݎ�����l1
float TotalW[168]={
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //1week=168���ԕ��̃f�[�^���L�^����B
byte TimeNow=0; //0-167h
byte TimeBefore=167;//0-167���Ԃ̂����A�ǂ̎��Ԃ��v�����Ă��邩�B

long MsecNow=0; //Arduino�N������o�߂�������(�~���b)
long SecNow=0; //Arduino�N������o�߂�������(�b)
long MsecBefore=0; //1��O�̑���̃~���b�B����d�͗ʂ����߂邽�߁A���̑���ɗv�������Ԃ��v�Z����B
float dwell=0; //�ϕ����ԁB

float Coeff=42.96875; //�Z���T�[���o�͂���d����(A,�A���y�A)����d��(W,���b�g)�����߂�W���B
  //�d���v��150mV=30A=3000W�ݒ�B�d���v��1.1V=1024

void setup() {
  Serial.begin(9600); //�V���A���ʐM���J�n�B
  analogReference(INTERNAL);//�d���v���̊�Ƃ��āA�����d����1.1V���g�p����B
}

//�����艺���J��Ԃ��đ��肨��уf�[�^���M����B
void loop() {
    sensV0ave=0; //600�񕽋ς�100�񕽋ς��邽�߂̒l�̏�����
    sensV1ave=0;
  for (int j=0;j < aveN; j++){
  sensV0=0; //600��̕��ς����l�̏�����
  sensV1=0; //
  //1000��̕��ς����B
  for (int i=0; i < sumN; i++){
    Read0=(float) analogRead(0); //Analog IN��0�Ԓ[�q�̓d����ǂށB
    Read1=(float) analogRead(3); //Analog IN��3�Ԓ[�q�̓d����ǂށB
    sensV0=sensV0+Read0*Read0/sumN; //���̘a���Ƃ�B�d�͓͂d���̓��ɔ��B
    sensV1=sensV1+Read1*Read1/sumN;
   }
  sensV0ave=sensV0ave + Coeff*sqrt(sensV0)/aveN; //600��v�����ĕ��ϒl�����߂�B
  sensV1ave=sensV1ave + Coeff*sqrt(sensV1)/aveN;
  }
  SumW=sensV0ave+sensV1ave; //200V�ł�2�{�̒[�q�̘a�ő��d�͗ʂƂȂ�B
  Total(); //�֐�Total���Ă�ŁA�������ɏ���d�͗ʂ��L���B
  //���݂̏���d�͂��o�́B
   Serial.print("sec=");  Serial.print(SecNow); //���݂̎���(�b)
   Serial.print(",Pin0=");  Serial.print(sensV0ave); //Pin0�̓d��
   Serial.print(",Pin1="); Serial.print(sensV1ave); //Pin1�̓d��
   Serial.print(",Total="); Serial.println(SumW);  //���d��
}

void Total(){
  MsecNow=millis(); //�~���b�����߂�B
   dwell=float (MsecNow-MsecBefore)/1000; //1��̑���ɗv��������(sec)
  MsecBefore=millis(); //���̑���̎��ɌĂяo���āA�ώZ���Ԃ��Z�o���邽�߂́u�O��̑��莞�ԁv
   SecNow=MsecNow/1000; //�b
   TimeNow=((SecNow/3600) % 168) ; //�����͐؂�̂Ă���B
   if (TimeNow > TimeBefore){TotalW[TimeNow]=0;} //1���Ԃ��܂��������ɁATimeNow(����)��1�����邽�߁A��TotalW(����d�͗�)��0�ɖ߂��B����ɂ��A�P���Ԃ̏���d�͂����܂�B
   TimeBefore=TimeNow; //1��O�ɑ��肵�����ԁB
   TotalW[TimeNow]=TotalW[TimeNow]+SumW*dwell/3600; //���݂̎��Ԃ̘a���v�Z�B1�b�̎��A1/3600�����P���Ԃ̘a�ɉ�����B
  for (int i=167; i< =0; i--){ ///�ߋ�24���Ԃɂ��ĂP���Ԃ��Ƃ̕��ς��o�́B
       if (i==TimeNow){Serial.print("Now");} //���ݘa���v�Z���Ă��鎞�Ԃ������B����ɂ��A0-167h�̂����A���ݑ��肵�Ă��鎞�Ԃ��������B
       Serial.print("H");
       Serial.print(i);
       Serial.print("=");
       Serial.println(TotalW[i]); //����d�͗ʂ�\������B
       delay(10); //�V���A���ʐM�̒x��ɑΉ����邽�߂�delay�ŏ���������x������B
      }

}
