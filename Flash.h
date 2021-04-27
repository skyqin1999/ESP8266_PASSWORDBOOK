#include <EEPROM.h>

// 使用的ESP8266官方示例文件中的EEPROM库

#define SYS_INFO_LEN 1272
#define PWD_INFO_LEN 400

#define SYS_SECTOR 254
#define PWD_SECTOR_START 255
#define PWD_SECTOR_END 375
#define PWD_SECTOR_SIZE 4004

//#define FLASH_DEBUG
#define SYS_INFO_DEFAULT  {0, 1, "PasswordBook", "","admin", "123456",{1}}
#define PWD_INFO_DEFAULT  {0,"【示例】BiliBili","吃花椒的喵酱","wbb1997bilibli","13000000000","wbb1997@outlook.com","绑定了微信","这是我的哔哩哔哩大号"}

typedef struct Sys_info{
    uint8_t sys_state;
    uint16_t pwd_num;
    char wifi_ssid[29]; //^[\u4e00-\u9fa5]{1,14}$|^[\dA-Za-z_]{1,28}$
    char wifi_pwd[16];  //^\w{8,15}$
    char www_id[16];    //^\w{8,15}$
    char www_pwd[8];    //^[0-9]{6}$
    uint8_t show_flag[1200];//operate 1B is easier than 1bit
}SYS_INFO;


typedef struct Pwd_info{
    uint16_t num;
    char site[40];
    char id[50];
    char pwd[30];
    char phone[40];
    char email[40];
    char link[60];
    char tips[138];

}PWD_INFO;

void flash_init(SYS_INFO& sys_info, PWD_INFO& pwd_info);
void save_pwd(int pwd_num, PWD_INFO& pwd_info);
void load_pwd(int pwd_num, PWD_INFO& pwd_info);
void load_sys(SYS_INFO& sys_info);
void save_sys(SYS_INFO& sys_info);
