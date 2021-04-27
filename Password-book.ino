#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <ESP8266WebServer.h>

#include "Flash.h"

#define VERSION 0
//0-->opaque shell application version with external button & light
//1-->transparent shell application version with one external button
//2-->mini application version without any external equipment
#if VERSION == 0
#define BUTTON  14
#define LIGHT_1  12 //power indicator light
#define LIGHT_2  13 //info indicator light
#elif VERSION == 1
#define BUTTON  0
#endif

SYS_INFO sys_info;
PWD_INFO pwd_info;

const int led = LED_BUILTIN;
ESP8266WebServer server(80);

const String html_head = "<!DOCTYPE ><html ><head><title>密码库</title><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" /><meta name=\"viewport\" content=\"width=device-width, minimum-scale=1.0, maximum-scale=1.0\">";
const String html_css_1 = "<style>#myInput {background-image: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABUAAAAVCAYAAACpF6WWAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAAEnQAABJ0Ad5mH3gAAAACYktHRAD/h4/MvwAAAAl2cEFnAAABKgAAASkAUBZlMQAAACV0RVh0ZGF0ZTpjcmVhdGUAMjAxMy0wNC0xMFQwNjo1OTowNy0wNzowMI5BiVEAAAAldEVYdGRhdGU6bW9kaWZ5ADIwMTMtMDQtMTBUMDY6NTk6MDctMDc6MDD/HDHtAAAAGXRFWHRTb2Z0d2FyZQB3d3cuaW5rc2NhcGUub3Jnm+48GgAAABF0RVh0VGl0bGUAc2VhcmNoLWljb27Cg+x9AAACKklEQVQ4T6WUSavqQBCFK+2sII7gShFXLpUsBBHFf+1KcAQFwaWiolsnnBDn++4p0iHRqPDuByFJd/Wp6qrqVn5+IQP3+52m0ymtVis6Ho885na7KRgMUiKR4O9vmEQHgwGNx2NyOp0khCBFUXgcJo/Hg67XK8ViMcpkMjz+Dl200+nQZrMhh8PBE4gYQgDidrudvzEOm2KxyP9WsCginM1mHKEUS6VSFA6HOWI4G41GPAfx2+1GgUCAVFXVZMwovwY/lUqFPB4PiyFn+XxemzbT6/VovV6z8Ol0olwux+LPCBQFEQKIvhME2WyWbWGHFCD/VghUGVvE1rDlb6TTabbFmuVyqY2aEWgbFALeI5GINvyeUCjEtlgju+IZoRWfkS30CURoxFJUNjMEt9stf38CNjJKIFvNiMBJgTebzcZt843hcMhCELWqPBDxeJwulwtvC/3X7/e1qVfgFD0rC5tMJrUZM8Lr9VI0GmVBRDCfz6nZbHI/Sna7HXW7XZpMJtxSiBIP1lmhH9NqtaqfGKQDTmQREBnSgwfmMqfYYblc1o+2xHShtNttLgSiee4EmMEp3hDBPJzikimVSuRyuTTLJ1GwWCz4pCB3UhiL/X4/Hw50C5zjLSM+n898weCogxdRIzAGxigAdtNqtV6EC4UC+Xy+z6Kf2O/31Gg0TMK4ZBDxf4uCw+FA9XpdF0aaUOg/iQLcHbVaTb/p0Cl/FgXIJ/oYnaCqKv0DC6dltH6Ks84AAAAASUVORK5CYII=\");background-position: 10px 12px; background-repeat: no-repeat; width: 100%;font-size: 16px;padding: 12px 20px 12px 40px;border: 1px solid #ddd;margin-bottom: 12px; }h2 {text-align: center;}";
const String html_css_2 = "table {table-layout: fixed;word-wrap:break-word;border-collapse: collapse; width: 100%; border: 1px solid #ddd;font-size: 18px; }th, td {text-align: left;padding: 12px;}tr {border-bottom: 1px solid #ddd; }tr.header, tr:hover {background-color: #f1f1f1;}.section {margin: 7px auto;box-shadow: 0 2px 3px rgba( 0, 0, 0, .4 );overflow: hidden;-webkit-transition: .35s;transition: .35s;}button.accordion {background: #00C37E;color: #228;  cursor: pointer;padding: 18px;width: 100%;border: none;text-align: left;outline: none;font-size: 15px;transition: 0.4s;}button.accordion.active, button.accordion:hover {background-color: #f19790;}button.accordion:after {content: \"\\002B\";color: #777;font-weight: bold;float: right;margin-left: 5px;}button.accordion.active:after {content: \"\\2212\";}div.panel {padding: 2 6px;background-color: rgba(182, 216, 209, 0.74);max-height: 4px;overflow: hidden;transition: max-height 0.2s ease-out;}";
const String html_css_3 = ".basic-grey {margin-left: auto;margin-right:auto;max-width: 500px;background: #F7F7F7;padding: 25px 15px 25px 10px;font: 12px Georgia, \"Times New Roman\", Times, serif;color: #888;text-shadow: 1px 1px 1px #FFF;border:1px solid #E4E4E4;}.basic-grey h2 {font-size: 25px;padding: 0px 0px 10px 40px;display: block;border-bottom:1px solid #E4E4E4;margin: -10px -15px 30px -10px;;color: #888;}.basic-grey h2>span {display: block;font-size: 8px;}.basic-grey label {display: block;margin: 0px;}.basic-grey label>span {float: left;width: 20%;text-align: right;padding-right: 10px;margin-top: 10px;color: #888;}.basic-grey input[type=\"text\"], .basic-grey input[type=\"email\"],.basic-grey input[type=\"password\"], .basic-grey textarea {border: 1px solid #DADADA;color: #888;height: 30px;margin-bottom: 16px;margin-right: 6px;margin-top: 2px;outline: 0 none;padding: 3px 3px 3px 5px;width: 70%;font-size: 12px;line-height:15px;box-shadow: inset 0px 1px 4px #ECECEC;-moz-box-shadow: inset 0px 1px 4px #ECECEC;-webkit-box-shadow: inset 0px 1px 4px #ECECEC;}.basic-grey textarea{padding: 5px 3px 3px 5px;height:100px;}.basic-grey .button {background: #E27575;border: none;padding: 10px 25px 10px 25px;color: #FFF;box-shadow: 1px 1px 5px #B6B6B6;border-radius: 3px;text-shadow: 1px 1px 1px #9E3F3F;cursor: pointer;}</style>";
const String html_js = "<script type=\"text/javascript\">function fold() {var acc = document.getElementsByClassName(\"accordion\");var i;for (i = 0; i < acc.length; i++) {acc[i].onclick = function() {this.classList.toggle(\"active\");var panel = this.nextElementSibling;if (panel.style.maxHeight){panel.style.maxHeight = null;} else {panel.style.maxHeight = panel.scrollHeight + \"px\";} }}}</script><script type=\"text/javascript\">function search() {var input, filter, table, tr, td, i;input = document.getElementById(\"myInput\");filter = input.value.toUpperCase();table = document.getElementById(\"myTable\");tr = table.getElementsByTagName(\"div\");for (i = 0; i < tr.length; i++) {td = tr[i].getElementsByTagName(\"button\")[0];if (td) {if (td.innerHTML.toUpperCase().indexOf(filter) > -1) {tr[i].style.display = \"\";} else {tr[i].style.display = \"none\";}} }}</script>";
const String html_top = "<body onload=\"fold()\"><h2>密码本</h2><input type=\"text\" id=\"myInput\" onkeyup=\"search()\" placeholder=\"搜索站点...\"><div id=\"myTable\">";
const String html_new = "<div class = \"section\"><button class=\"accordion\"> ●&nbsp;新增密码信息</button><div class=\"panel\"><form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/\" class=\"basic-grey\"><h2>Password Form<span>Please fill all the texts in the fields.</span></h2><label><span>网址站点*:</span><input type=\"hidden\"  name=\"command\" value=\"1\"><input type=\"text\" name=\"SITE\" placeholder=\"Bilibili\" required=\"\"></label><label><span>用户姓名*:</span><input type=\"text\" name=\"ID\" placeholder=\"爱吃花椒的喵酱\" required=\"\"></label><label> <span>账号密码*:</span><input type=\"password\" name=\"PWD\" placeholder=\"wbb1997bilibli\" required=\"\"></label><label><span>注册电话:</span><input type=\"text\" name=\"PHONE\" placeholder=\"13000000000\" ></label><label><span>注册邮箱:</span><input type=\"email\" name=\"EMAIL\" placeholder=\"100000@qq.com\" ></label><label><span>关联信息:</span><input type=\"text\" name=\"LINK\" placeholder=\"绑定微信\" ></label><label><span>备注信息:</span><textarea name=\"TIPS\" placeholder=\"这是大号\" ></textarea></label><label><span>&nbsp;</span><input type=\"submit\" class=\"button\" value=\"Submit\"></label></form>  </div></div>";
const String html_confirm_1 = "<div class = \"section\"><button class=\"accordion\"> ●&nbsp;修改系统消息</button><div class=\"panel\"><form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/\" class=\"basic-grey\"><h2>Set System Info<span>Please fill your original password in the fields.</span></h2><label><span>登陆密码:</span><input type=\"hidden\"  name=\"command\" value=\"4\"><input type=\"password\" name=\"WWW_PWD\" placeholder=\"你的登录密码\" required=\"\"></label><label><span>&nbsp;</span><input type=\"submit\" class=\"button\" value=\"Submit\"></label></form> ";
const String html_forget_1 = "<form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/\" class=\"basic-grey\"><h2>Great!!!<span>You, lost password again! </span><br><span>点击找回密码后五秒钟内按物理按键</span><br><span>提示：长按按键后再通电开机可重置登陆用户密码，长点记性</span><br></h2><label><input type=\"hidden\"  name=\"command\" value=\"5\"></label><label><span>&nbsp;</span><input type=\"submit\" class=\"button\" value=\"忘记登陆密码\"></label></form>";
const String html_forget_2 = "<form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/\" class=\"basic-grey\"><h2>Great!!!<span>You, lost password again! </span><br><span>没有办法咯，不断重复试一试你的常用的6位数字密码</span><br><span>实在不行只能找密码本制作人重置密码了</span><br></h2></form>";
const String html_confirm_2 = "</div></div>";

const String html_setting_1 = "<div class = \"section\"><button class=\"accordion\">修改系统消息</button><div class=\"panel\"><form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/\" class=\"basic-grey\"><h2>Modify System Setting Form<span>Please fill all the texts in the fields.</span></h2><label><span>WIFI名称*:</span><input type=\"hidden\"  name=\"command\" value=\"0\"><input type=\"text\" name=\"WIFI_ID\" placeholder=\"";
const String html_setting_1_init = "<div class = \"section\"><button class=\"accordion\">嗨~别来无恙</button><div class=\"panel\"><form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/\" class=\"basic-grey\"><h2>Hello~ Welcome to Password Book<span>Please initialize all of the following information</span></h2><label><span>WIFI名称*:</span><input type=\"hidden\"  name=\"command\" value=\"0\"><input type=\"text\" name=\"WIFI_ID\" placeholder=\"";
const String html_setting_2 = "\" required=\"\"></label><label><span>WIFI密码*:</span><input type=\"password\" name=\"WIFI_PWD\" placeholder=\"";
const String html_setting_3 = "\" required=\"\"></label><label><span>登陆名称*:</span><input type=\"text\" name=\"WWW_ID\" placeholder=\"";
const String html_setting_4 = "\" required=\"\"></label><label><span>登陆密码*:</span><input type=\"password\" name=\"WWW_PWD\" placeholder=\"";
const String html_setting_5 = "\" required=\"\"></label><label><span>&nbsp;</span><input type=\"submit\" class=\"button\" value=\"Submit\"></label></form>  </div></div>";
const String html_recyc = "<div class = \"section\"> <form action=\"/\" method=\"post\"><div><input type=\"hidden\"  name=\"command\" value=\"3\"><button class=\"accordion\"> ●&nbsp;密码回收站</button></div></form></div>";
const String html_rt = "<div class = \"section\"><a href=\"/\"><button class=\"accordion\"> ●&nbsp;返回密码列表</button><a></div> ";
const String html_section_1 = "<div class = \"section\"><button class=\"accordion\">";
const String html_section_2 = "</button><div class=\"panel\"><table border = \"1\"><tr ><th width=\"30%\" bgcolor=\"#ccccd6\">站点</th><th width=\"70%\" bgcolor=\"#c3d7df\">";
const String html_section_3 = "</th></tr><tr><td bgcolor=\"#ccccd6\">用户</td><td bgcolor=\"#c3d7df\">";
const String html_section_4 = "</td></tr><tr><td bgcolor=\"#ccccd6\">密码</td><td bgcolor=\"#c3d7df\">";
const String html_section_5 = "</td></tr><tr><td bgcolor=\"#ccccd6\">手机</td><td bgcolor=\"#c3d7df\">";
const String html_section_6 = "</td></tr><tr><td bgcolor=\"#ccccd6\">邮箱</td><td bgcolor=\"#c3d7df\">";
const String html_section_7 = "</td></tr><tr><td bgcolor=\"#ccccd6\">关联</td><td bgcolor=\"#c3d7df\">";
const String html_section_8 = "</td></tr><tr><td bgcolor=\"#ccccd6\">备注</td><td bgcolor=\"#c3d7df\">";  
const String html_section_9 = "</td></tr></table>";
const String html_del_1 = "<form action=\"/\" method=\"post\"><div><input type=\"hidden\"  name=\"command\" value=\"2\"><input type=\"hidden\"  name=\"id\" value=\"";
const String html_del_2 = "\"><input type=\"submit\" class=\"button\" value=\"删除本条密码信息\"></div></form>";
const String html_section_end = "</div></div>";

const String html_about = "<div class = \"section\"><button class=\"accordion\">●&nbsp;About关于</button><div class=\"panel\"><h3>关于本站点：</h3><p>程序版本：v1.3<br>博客地址：<a href=\"https://www.write-down.tech/junior/esp8266-passwordbook.html\">www.write-down.tech<a><br>给踏啊步：<a href=\"https://github.com/skyqin1999/ESP8266_PASSWORDBOOK\">ESP8266_PASSWORDBOOK<a><br></p></div></div>";
const String html_tail = "</div></body></html>";

void load_info(){
  flash_init(sys_info, pwd_info);
  
  Serial.println("----------- SYS_INFO -----------");
  Serial.println(sys_info.sys_state);
  Serial.println(sys_info.pwd_num);
  Serial.println(sys_info.wifi_ssid);
  Serial.println(sys_info.wifi_pwd);
  Serial.println(sys_info.www_id);
  Serial.println(sys_info.www_pwd);
  Serial.println();

  Serial.println("----------- PWD_INFO -----------");
  Serial.println(pwd_info.site);
  Serial.println(pwd_info.id);
  Serial.println(pwd_info.pwd);
  Serial.println(pwd_info.phone);
  Serial.println(pwd_info.email);
  Serial.println(pwd_info.link);
  Serial.println(pwd_info.tips);
}

void turn_on_light(){
  digitalWrite(led, 0);
  #if VERSION == 0
  digitalWrite(LIGHT_1, 0);
  digitalWrite(LIGHT_2, 1);
  #endif
}

void turn_off_light(){
  delay(300);
  digitalWrite(led, 1);
  #if VERSION == 0
  digitalWrite(LIGHT_1, 1);
  digitalWrite(LIGHT_2, 0);
  #endif
}

void handleRoot() {
    if(server.method() != HTTP_POST && sys_info.sys_state == 0){//init setting
      Serial.println("----------- 进入初始设置页面 -----------");
      
      turn_on_light();
      String html = html_head + html_css_1 + html_css_2 + html_css_3 + html_js + html_top ;
      html += html_setting_1_init + String(sys_info.wifi_ssid) + html_setting_2 + " (起码8位)"  + html_setting_3 + String(sys_info.www_id) + html_setting_4 + String(sys_info.www_pwd) + "(6位数,勿忘！)" + html_setting_5 + html_about + html_tail;
      server.send(200, "text/html", html);
      turn_off_light();
    }
    else {//normal mode
      Serial.println("----------- NORMAL MODE -----------");
      if(sys_info.sys_state > 0){
        if (!server.authenticate(sys_info.www_id, sys_info.www_pwd))
        return server.requestAuthentication();
      }else{
        sys_info.sys_state++;
        save_sys(sys_info);
      }
      

      if (server.method() != HTTP_POST) {
        Serial.println("----------- 正常显示 -----------");
        turn_on_light();
        String html = html_head + html_css_1 + html_css_2 + html_css_3 + html_js + html_top + html_confirm_1;
        #if VERSION == 2
          html += html_forget_2;
        #else
          html += html_forget_1;
        #endif

        html += html_confirm_2 + html_new + html_recyc;
        int index = 0;
        for(int i = 0; i < sys_info.pwd_num; i++){
          if(sys_info.show_flag[i]){
            index++;
            load_pwd(i, pwd_info); 
            html += html_section_1 + String(index) + ". " + String(pwd_info.site) + html_section_2 + String(pwd_info.site) + html_section_3 + String(pwd_info.id) + html_section_4 + String(pwd_info.pwd) + html_section_5 + String(pwd_info.phone) + html_section_6 + String(pwd_info.email) + html_section_7 + String(pwd_info.link)+ html_section_8 + String(pwd_info.tips) + html_section_9 + html_del_1;
            html += String(i) + html_del_2 + html_section_end;
          }
        }
        html += html_about + html_tail;
        server.send(200, "text/html", html);
        turn_off_light();
      }else{
        //对于第一个POST参数server.arg(0)
        //0. 修改系统消息 1. 新增 2. 删除  3. 进入回收站页面 4. 进入确认原始密码页面 5. 进入修改设置页面
        if(server.arg(0) == "0"){
          Serial.println("----------- 处理修改设置 -----------");
          turn_on_light();
          strcpy(sys_info.www_id , server.arg(3).c_str());
          strcpy(sys_info.www_pwd , server.arg(4).c_str());
          if(sys_info.sys_state == 0)
            sys_info.sys_state = 1;
          if(strcmp(sys_info.wifi_ssid, server.arg(1).c_str()) || strcmp(sys_info.wifi_pwd, server.arg(2).c_str())){
            strcpy(sys_info.wifi_ssid , server.arg(1).c_str());
            strcpy(sys_info.wifi_pwd , server.arg(2).c_str());
            save_sys(sys_info);
            String plain = String("WiFi information has been modified\nPlease connect to the new wifi\n\twifi_ssid: ")+ String(sys_info.wifi_ssid)+ String("\n\twifi_password: ") + String(sys_info.wifi_pwd)+ String("\n\tusr_name: ") + String(sys_info.www_id)+ String("\n\tusr_password: ") + String(sys_info.www_pwd);
            server.send(200, "text/plain", plain);
            delay(1000);
            ESP.reset();
          }else{
            save_sys(sys_info);
            server.send(200, "text/html", "<meta http-equiv=\"refresh\" content=\"0;url=/\">");
          }
          turn_off_light();
        }else if(server.arg(0) == "1"){
          Serial.println("----------- 处理新增信息 -----------");
          turn_on_light();
          strcpy(pwd_info.site , server.arg(1).c_str());
          strcpy(pwd_info.id , server.arg(2).c_str());
          strcpy(pwd_info.pwd , server.arg(3).c_str());
          strcpy(pwd_info.phone , server.arg(4).c_str());
          strcpy(pwd_info.email , server.arg(5).c_str());
          strcpy(pwd_info.link , server.arg(6).c_str());
          strcpy(pwd_info.tips , server.arg(7).c_str());

          save_pwd(sys_info.pwd_num, pwd_info);
          sys_info.show_flag[sys_info.pwd_num] = 1;
          sys_info.pwd_num++;
          
          save_sys(sys_info);

          turn_off_light();
          server.send(200, "text/html", "<meta http-equiv=\"refresh\" content=\"0;url=/\">");
          //server.sendHeader("Location","/");
        }else if(server.arg(0) == "2"){
          turn_on_light();
          Serial.println("----------- 处理删除信息 -----------");
          uint16_t n = atoi(server.arg(1).c_str());
          Serial.print("获取的编号：");
          Serial.println(n);
          sys_info.show_flag[n] = 0;
          save_sys(sys_info);
          server.send(200, "text/html", "<meta http-equiv=\"refresh\" content=\"0;url=/\">");
          turn_off_light();
        }else if(server.arg(0) == "3"){
          Serial.println("----------- 进回收站页面 -----------");
          turn_on_light();
          String html = html_head + html_css_1 + html_css_2 + html_css_3 + html_js + html_top + html_rt;
          int index = 0;
          Serial.println("标识情况：");
          Serial.print("    总密码数:");
          Serial.println(sys_info.pwd_num);
          for(int i = 0; i < sys_info.pwd_num; i++){
            Serial.print("    ");
            Serial.print(i);
            Serial.print(" : ");
            Serial.println(sys_info.show_flag[i]);
            if(sys_info.show_flag[i] == 0){
                Serial.print("即将展示：");
                Serial.println(i);
                index++;
                load_pwd(i, pwd_info); 
                html += html_section_1 + String(index) + ". " + String(pwd_info.site) + html_section_2 + String(pwd_info.site) + html_section_3 + String(pwd_info.id) + html_section_4 + String(pwd_info.pwd) + html_section_5 + String(pwd_info.phone) + html_section_6 + String(pwd_info.email) + html_section_7 + String(pwd_info.link)+ html_section_8 + String(pwd_info.tips) + html_section_9 + html_section_end;          
              }
          }
          html +=  html_tail;
          server.send(200, "text/html", html);
          turn_off_light();
        }else if(server.arg(0) == "4"){
            Serial.println("----------- 进入修改设置页面 -----------");
            if(server.arg(1) == sys_info.www_pwd){
              turn_on_light();
              String html = html_head + html_css_1 + html_css_2 + html_css_3 + html_js + html_top ;
              html += html_setting_1 + String(sys_info.wifi_ssid) + html_setting_2 ;
              html += String(sys_info.wifi_pwd) + String(" (起码8位)") + html_setting_3 + String(sys_info.www_id) + html_setting_4 + String(sys_info.www_pwd) + "(6位数,勿忘！)" + html_setting_5 + html_about + html_tail;
              server.send(200, "text/html", html);
              turn_off_light();
            }else{
              server.send(200, "text/html", "<meta http-equiv=\"refresh\" content=\"0;url=/\">");
            }
        }else if(server.arg(0) == "5"){
            Serial.println("---------- 处理重置登录密码 ---------");
            turn_on_light();
            
            unsigned long start = millis();
            while(millis()-start < 5000){
              if((millis()-start)%1000 == 0)
                Serial.println(digitalRead(BUTTON));
              if(digitalRead(BUTTON) == 0){//mine is default high
                Serial.println("按下了按键");
                strcpy(sys_info.www_pwd , "123456");
                save_sys(sys_info);
                
                break;
              }
              
            }
            server.send(200, "text/html", "<meta http-equiv=\"refresh\" content=\"0;url=/\">");
            turn_on_light();
        }

      }
    }
    

}

void handleNotFound() {
  turn_off_light();
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  delay(1000);
  server.send(200, "text/html", "<head><meta http-equiv=\"refresh\" content=\"0;url=\"/\"\"> </head>");
  turn_off_light();
}

void setup(void) {
  
  #if VERSION == 0
  pinMode(BUTTON, INPUT);
  pinMode(LIGHT_1, OUTPUT);
  digitalWrite(LIGHT_1, 1);
  pinMode(LIGHT_2, OUTPUT);
  #elif VERSION == 1
  pinMode(BUTTON, INPUT);
  #endif


  Serial.begin(115200);
  for (uint8_t t = 3; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  load_info();
  if(digitalRead(BUTTON) == 0){//mine is default high
    Serial.println("开机重置密码");
    strcpy(sys_info.www_pwd , "123456");
    save_sys(sys_info);
  }
  Serial.print("Setting soft-AP ... ");
  
  IPAddress softLocal(192,168,128,1);   
  IPAddress softGateway(192,168,128,1);
  IPAddress softSubnet(255,255,255,0);
  
  WiFi.softAPConfig(softLocal, softGateway, softSubnet);  
 
  WiFi.softAP(sys_info.wifi_ssid, sys_info.wifi_pwd, 2, 0, 1);
  
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  pinMode(led, OUTPUT);
  turn_off_light();
  
  

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
