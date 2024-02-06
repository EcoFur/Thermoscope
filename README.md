# Thermoscope 热电偶温度计  
Use Arduino IDE, Depend RP2040 &amp; Max6675 Type K thermocouples  
### Sorry here only Chinese  
## 背景 
这个项目本来是化学课上老师提了一嘴，我就做了出来，后来拍了个视频到Bilibili，有人评论找我要代码，我就干脆开源算了，这也是我的第一个开源项目，**请多多关照**  
***[视频](https://www.bilibili.com/video/BV1Qw411a72G/ "Bilibili的跳链")*** 
## 材料  
> **SSD1306** OLED显示屏  
> **MAX6675** K型热电偶模块  
> **RP2040** 树莓派主控(性能好)  
> **按钮**和**滑阻**
## 接线  
我用的是SEEED的XIAO RP2040，做完后我就用硅橡胶密封了，接线只能从我的程序里推测  
首先OLED接IIC协议口，默认**GPIO 4**和**GPIO 5**  
热电偶模块接SPI协议口，我设置的SCK**GPIO 2**, CS **GPIO 1**, SO **GPIO4**  
按钮是**GPIO 0**, 滑阻是**GPIO 29**  
## 补充  
至于CPP文件夹，是我做滚动存储时做测试的文件
