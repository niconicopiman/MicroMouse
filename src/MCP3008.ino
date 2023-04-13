// MCP3008用サンプルプログラム
//
// CH0とCH1のAD変換値を１秒ごとにシリアル出力
//
//Arduino          MCP3008
//SCK(13) <======> CLK
//MISO(12)<======> DOUT
//MOSI(11)<======> DIN
//SS(10)  <======> /CS
//

#include <SPI.h>
#define slaveSelectPin0 5
#define slaveSelectPin1 13
#define TH_LVL 250
int analogData[16];
int R_sns[6];
int R_sns_old[6];
int L_sns[6];
int L_sns_old[6];



void setup() {
  Serial.begin(115200);                      //シリアル通信開始
  pinMode(slaveSelectPin0, OUTPUT);          //SS(10)を出力に設定
    pinMode(slaveSelectPin1, OUTPUT);
  SPI.setBitOrder(MSBFIRST);                //MSBから送信
  SPI.setClockDivider(SPI_CLOCK_DIV2);      //クロック分周設定
  SPI.setDataMode(SPI_MODE0);               //SPIモード0
  SPI.begin();                              //SPI通信開始
}

void loop()
{
   Serial.print("R_sns:[");
    get_adc();
   for (int r = 0; r < 6;r++){
       Serial.printf("%d , " ,R_sns[r]);
   }
   Serial.println("]");
   Serial.print("L_sns:[");
   for (int l = 0; l < 6;l++){
       Serial.printf("%d , " ,L_sns[l]);
   }
   Serial.println("]");
  delay(1);
}
void get_adc()
{
    for (int ch = 0; ch < 6; ch++)
    {
        byte data[4] = {0, 0, 0, 0}; // SPI通信用変数
        //------[ ADC0のデータを取得する ]------
        digitalWrite(slaveSelectPin0, LOW);       // CS LOW
        SPI.transfer(0x01);                       //①スタートビット送信
        data[0] = SPI.transfer((ch << 4) | 0x80); //②Single-ended チャンネル選択,ADC0のbit9,8取得
        data[1] = SPI.transfer(0);                //③ADC0のbit7～0取得
        digitalWrite(slaveSelectPin0, HIGH);      // CS HIGH

        // ------[ ADC1のデータを取得する ]------
        digitalWrite(slaveSelectPin1, LOW);       // CS LOW
        SPI.transfer(0x01);                       //①スタートビット送信
        data[2] = SPI.transfer((ch << 4) | 0x80); //②Single-ended チャンネル選択,ADC0のbit9,8取得
        data[3] = SPI.transfer(0);                //③ADC0のbit7～0取得
        digitalWrite(slaveSelectPin1, HIGH);      // CS HIGH

        analogData[0] = ((data[0] & 0x03) << 8) | data[1]; // ADC0
        analogData[1] = ((data[2] & 0x03) << 8) | data[3]; // ADC1

        if ((int)analogData[0] <= TH_LVL)
        {
            /* code */
            R_sns[ch] = (int)1;
        }
        else if ((int)analogData[0] > TH_LVL)
        {
            if (ch == 0 && (int)analogData[0] <= 300)
            {
                R_sns[ch] = (int)1;
            }
            R_sns[ch] = (int)0;
        }

        if ((int)analogData[1] <= TH_LVL)
        {
            /* code */
            L_sns[ch] = (int)1;
        }
        else if ((int)analogData[1] > TH_LVL)
        {
            L_sns[ch] = (int)0;
        }
    }
}
