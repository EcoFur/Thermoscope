#include <Adafruit_GFX.h>     //显示屏库
#include <Adafruit_SSD1306.h> //显示屏库
#include <max6675.h>          //热电偶库

#define ButtonDig 0           //按钮引脚
#define AnalogDig 29          //滑动变阻器引脚

Adafruit_SSD1306 display(128, 64, &Wire, -1);//定义显示屏
MAX6675 thermocouple(2, 1, 4);//定义热电偶

struct Tmp{                   //定义结构体用来存储温度与时间
  short tmp;
  unsigned long time;
}tmp[1030],tmpStop[1030];     //tmp是正常的时候记录（1秒一次），tmpStop是按下暂停键时用来缓存的
int tmpDig=0;
short nowTmp,minTmp=2000,maxTmp=0;//记录温度
int sum=0,stopSum;            //记录温度数据组数

unsigned long buttonTime1,buttonTime2;//记录按钮按下时间
bool buttonBool1=false,buttonBool2=false;//记录按钮按下状态
int buttonNow;                //记录按钮当前状态（只有一个按钮）
short isDisplay=0;            //设定显示状态
bool isStop=false;            //设定停止状态
int lastAnalogReading=0;      //ADC波动消除

Tmp   readTmp(int dig);
void  writeTmp(short data);
int   button();               //返回1为短按，返回3为长按
int   analogReading();        //消抖读取滑动变阻器电压

void setup() {
  Serial.begin(9600);         //串口初始化
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    for(;;);                  //显示屏初始化
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Be Made by\nSong Yuanzhuo\nPowered by\nRP2040");
  display.display();
  delay(1000);
  display.clearDisplay();
}

void loop() {
  buttonNow=button();         //读取按钮状态
  if(int(millis()/1000) > int(readTmp(0).time/1000))//每秒读取一次（可以改成500或2000，但只有1024个记录点）
    writeTmp(short(thermocouple.readCelsius()));
  display.display();
  display.clearDisplay();
  if(!isStop)
  {
    int height=63;
    for(int i=1;i<=sum;i++)
      display.drawLine(       //display.drawLine(起点x,起点y,终点x,终点y)
      map(i,0,sum,127,0),     //把i的温度和i-1温度在屏幕上连起来
      map(readTmp(i).tmp,0,maxTmp>=64?maxTmp:64,height,0),//“maxTmp>=64?maxTmp:64”是用来动态响应的，如果温度大于屏幕高度就等比缩小高度
      map(i+1,0,sum,127,0),
      map(readTmp(i+1).tmp,0,maxTmp>=64?maxTmp:64,height,0),
      SSD1306_WHITE);
  }
  else
  {
    int height=63;
    display.setTextSize(1);   //显示“STOP”
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,8);
    display.print("STOP");
    for(int i=1;i<=sum;i++)   //和非停止状态一样
    {
      display.drawLine(
      map(i,0,stopSum,127,0),
      map(tmpStop[i].tmp,0,maxTmp>=64?maxTmp:64,height,0),
      map(i+1,0,stopSum,127,0),
      map(tmpStop[i+1].tmp,0,maxTmp>=64?maxTmp:64,height,0),
      SSD1306_WHITE);
    }
  }
  if(buttonNow==1)            //设置显示状态
    isDisplay=isDisplay+1>=4?0:isDisplay+1;
  if(buttonNow==3)            //按钮长按暂停或取消暂停
  {
    isStop=!isStop;           //复制tmp一份到stopTmp
    for(int i=0;i<=sum;i++)
      tmpStop[i]=readTmp(i);
    stopSum=sum;
  }
  display.setTextSize(1);
  display.setTextColor(SSD1306_INVERSE);
  if(isDisplay==0)            //状态0，显示当前、最低、最高温度
  {
    display.setCursor(0, 56);
    display.print("Now:");
    display.print(nowTmp) ;
    display.setCursor(43, 56);
    display.print("Min:");
    display.print(minTmp) ;
    display.setCursor(85, 56);
    display.print("Max:");
    display.print(maxTmp) ;
  }
  if(isDisplay!=0)            //状态1-3，显示指定时间温度，时间由滑动变阻器调节
  {
    int height;
    if(isDisplay==1)height=56;
    if(isDisplay==2)height=28;
    if(isDisplay==3)height=0;
    display.drawFastVLine(map(analogReading(),5,1020,127,0),0,64,SSD1306_INVERSE);
    display.setCursor(0, height);
    Tmp localTmpValue=isStop?tmpStop[map(analogReading(),5,1020,1,stopSum)]:readTmp(map(analogReading(),5,1020,1,sum));
    display.print("Time:");
    display.print(int(localTmpValue.time/1000));
    display.print("s Tmp:");
    display.print(localTmpValue.tmp);
    display.print("^C");
  }
}

Tmp readTmp(int dig)
{
  return tmp[tmpDig-dig<0?tmpDig-dig+1024:tmpDig-dig];
}
void writeTmp(short data)
{                             //滚动记录，如果记到了第1025个数据，就把第0个数据覆盖掉，把起点从0调节为1，保持最多1024个数据点
	tmpDig++;                   //当前记录点
  if(sum<1023)sum++;
	if(tmpDig>=1024)            //刷新记录点
	tmpDig=0;
	tmp[tmpDig].time=millis();
	tmp[tmpDig].tmp=data;
  nowTmp=data;
  minTmp=minTmp<data?minTmp:data;//最大最小温度记录
  maxTmp=maxTmp>data?maxTmp:data;
}
int button()                  //此部分依靠BUG运行，我自己也看不懂了，不能改，改了就可能出问题
{
  if(digitalRead(ButtonDig) && !buttonBool1 && millis()-buttonTime2>=500)
  {
    buttonTime1=millis();
    buttonBool1=true;
  }
  if(millis()-buttonTime1<250 && millis()-buttonTime1>=50 && buttonBool1)
  {
    if(digitalRead(ButtonDig))buttonBool2=true;
    else buttonBool2=false;
  }
  if(millis()-buttonTime1<500 && millis()-buttonTime1>=250 && buttonBool1)
  {
    if(digitalRead(ButtonDig) && !buttonBool2)
    {
      buttonBool1=false;
      buttonTime2=millis();
      return 2;
    }
    if(!digitalRead(ButtonDig))
    {
      buttonBool1=false;
      return 1;
    }
  }
  if(millis()-buttonTime1>=500 && buttonBool1)
  {
    if(!digitalRead(ButtonDig))
    {
      buttonBool1=false;
      return 1;
    }
    else
    {
      buttonBool1=false;
      buttonTime2=millis();
      return 3;
    }
  }
  return 0;
}

int analogReading()     //消抖读取滑动变阻器ADC值
{
  if(analogRead(AnalogDig)>lastAnalogReading+1||analogRead(AnalogDig)<lastAnalogReading-1)
    lastAnalogReading=analogRead(AnalogDig);
  return lastAnalogReading;
}