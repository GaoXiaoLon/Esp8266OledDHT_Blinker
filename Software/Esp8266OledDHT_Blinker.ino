/*
  esp8266连接oled
  3.3V --- VCC
  G (GND) --- GND
  D1(GPIO5)--- SCL
  D2(GPIO4)--- SDA
  esp8266连接DHT11
  VCC ---- 3V3（+）
  GND --- -GND（-）
  输出---- D5
 */
 
#define BLINKER_WIFI
#include <Blinker.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#define SCREEN_WIDTH 128                   // OLED 显示宽度，以像素为单位
#define SCREEN_HEIGHT 64                   // OLED 显示高度，以像素为单位
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
#define DHTPIN 2                           // 连接DHT传感器的数字引脚
                                           // 取消注释使用的传感器类型
#define DHTTYPE DHT11               // 传感器类型
DHT dht(DHTPIN, DHTTYPE);


//blinker 提供的key
char auth[] = "67052f63a6c4";
//你的wifi名字
char ssid[] = "MS";
//你的wifi密码
char pswd[] = "123456789";
//发送到 blinker 的湿度数据key
BlinkerNumber HUMI("humi");
//发送到 blinker 的温度数据key
BlinkerNumber TEMP("temp");

uint32_t read_time = 0;

float humi_read, temp_read;

void dataRead(const String & data)   //读取数据函数
{
    BLINKER_LOG("Blinker readString: ", data);
    Blinker.vibrate();  
    uint32_t BlinkerTime = millis(); 
    Blinker.print("millis", BlinkerTime);
}

void heartbeat()     //心跳函数
{
    HUMI.print(humi_read);
    TEMP.print(temp_read);
}

void dataStorage()    //初始化函数
{
    //添加数据存储 以便于图标数据展示
    Blinker.dataStorage("humi", humi_read);
    //添加数据存储 以便于图标数据展示
    Blinker.dataStorage("temp", temp_read);
}

//显示天气
void weather(){
    if (read_time == 0 || (millis() - read_time) >= 2000)
    {
        read_time = millis();
        //读取湿度
        float h = dht.readHumidity();
        //读取温度
        float t = dht.readTemperature();        

        if (isnan(h) || isnan(t)) {
            BLINKER_LOG("Failed to read from DHT sensor!");
            return;
        }

        float hic = dht.computeHeatIndex(t, h, false);

        humi_read = h;
        temp_read = t;

        BLINKER_LOG("Humidity: ", h, " %");
        BLINKER_LOG("Temperature: ", t, " *C");
        BLINKER_LOG("Heat index: ", hic, " *C");
    }
}

//屏幕显示
void oled(){ 
    delay(5000);
     //read temperature and humidity
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    // clear display
    display.clearDisplay();
    // 显示温度
    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("Temperature: ");
    display.setTextSize(2);
    display.setCursor(0,17);
    display.print(t);
    display.print(" ");
    display.setTextSize(1);
    display.cp437(true);
    display.write(167);
    display.setTextSize(2);
    display.print("C");
    //显示湿度
    display.setTextSize(1);
    display.setCursor(0, 35);
    display.print("Humidity: ");
    display.setTextSize(2);
    display.setCursor(0, 45);
    display.print(h);
    display.print(" %");
    display.display();
  
  }
void led(){
  while(humi_read > 90 && temp_read > 15){
        pinMode(D6,HIGH); 
  }
}

void setup() 
{
    Serial.begin(115200);
    Blinker.run();  
    BLINKER_DEBUG.stream(Serial);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    Blinker.begin(auth, ssid, pswd);
    Blinker.attachData(dataRead);
    Blinker.attachHeartbeat(heartbeat);
    Blinker.attachDataStorage(dataStorage);
       
    dht.begin();
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;);
    }
    
    delay(2000);
    display.clearDisplay();
    display.setTextColor(WHITE);
}
void loop() {
    oled();
    weather();
    led();
}
