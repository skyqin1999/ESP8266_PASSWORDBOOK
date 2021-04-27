#include "Flash.h"
#include "Arduino.h"

void save_pwd(int pwd_num, PWD_INFO& pwd_info){
    char* pwd_info_ptr = (char*)&pwd_info;
    EEPROMClass MYROM((pwd_num / 10) + PWD_SECTOR_START);
    MYROM.begin(PWD_SECTOR_SIZE);
    int offset = (pwd_num % 10) * PWD_INFO_LEN;
    #ifdef FLASH_DEBUG
    Serial.println("----START SAVE PWD_INFO TO EEPROM----");
    Serial.print("PWD_NUM:  ");
    Serial.println(pwd_num);
    Serial.print("FLASH_SECTOR:  ");
    Serial.println((pwd_num / 10) + PWD_SECTOR_START);
    Serial.print("SECTOR_OFFSET:  ");
    Serial.println(offset);
    #endif

    for(int i = 0; i < PWD_INFO_LEN; i++){
        MYROM.write(offset+i, pwd_info_ptr[i]);
    }
    bool k = MYROM.commit();
    #ifdef FLASH_DEBUG
    if (k) 
        Serial.println("EEPROM successfully committed");
    else 
        Serial.println("ERROR! EEPROM commit failed");
    Serial.println("-------Finished SAVE PWD_INFO--------");
    #endif
    MYROM.end();
}

void load_pwd(int pwd_num, PWD_INFO& pwd_info){
    pwd_info.num = pwd_num;
    char* pwd_info_ptr = (char*)&pwd_info;
    EEPROMClass MYROM((pwd_num / 10) + PWD_SECTOR_START);
    MYROM.begin(PWD_SECTOR_SIZE);
    int offset = (pwd_num % 10) * PWD_INFO_LEN;
    #ifdef FLASH_DEBUG
    Serial.println("---START LOAD PWD_INFO FROM EEPROM---");
    Serial.print("PWD_NUM:  ");
    Serial.println(pwd_num);
    Serial.print("FLASH_SECTOR:  ");
    Serial.println((pwd_num / 10) + PWD_SECTOR_START);
    Serial.print("SECTOR_OFFSET:  ");
    Serial.println(offset);
    #endif

    for(int i = 0; i < PWD_INFO_LEN; i++){
        pwd_info_ptr[i] = MYROM.read(offset + i);
    }

    #ifdef FLASH_DEBUG
    Serial.println("-------Finished LOAD PWD_INFO--------");
    #endif
    MYROM.end();
}
void load_sys(SYS_INFO& sys_info){
    char* sys_info_ptr = (char*)&sys_info;
    EEPROMClass MYROM(SYS_SECTOR);
    MYROM.begin(SYS_INFO_LEN);
    
    #ifdef FLASH_DEBUG
    Serial.println("---START LOAD SYS_INFO FROM EEPROM---");
    Serial.print("FLASH_SECTOR:  ");
    Serial.println(SYS_SECTOR);
    #endif

    for(int i = 0; i < SYS_INFO_LEN; i++){
        sys_info_ptr[i] = MYROM.read(i);
    }
    #ifdef FLASH_DEBUG
    Serial.println("-------Finished LOAD SYS_INFO--------");
    #endif
    MYROM.end();
}
void save_sys(SYS_INFO& sys_info){
    char* sys_info_ptr = (char*)&sys_info;
    EEPROMClass MYROM(SYS_SECTOR);
    MYROM.begin(SYS_INFO_LEN);

    #ifdef FLASH_DEBUG
    Serial.println("----START SAVE SYS_INFO TO EEPROM----");
    Serial.print("FLASH_SECTOR:  ");
    Serial.println(SYS_SECTOR);
    #endif

    for(int i = 0; i < SYS_INFO_LEN; i++){
        MYROM.write(i, sys_info_ptr[i]);
    }
    bool k = MYROM.commit();
    #ifdef FLASH_DEBUG
    if (k) 
        Serial.println("EEPROM successfully committed");
    else 
        Serial.println("ERROR! EEPROM commit failed");
    Serial.println("-------Finished SAVE SYS_INFO--------");
    #endif
    MYROM.end();
}

void flash_init(SYS_INFO& sys_info,PWD_INFO& pwd_info){
    #ifdef FLASH_DEBUG
    Serial.println("-----LOAD SYS_INFO FROM ROM-----");
    #endif
    load_sys(sys_info);
    //if(sys_info.pwd_num == 255 && sys_info.wifi_ssid[0] == 255 && sys_info.wifi_pwd[0] == 255 && sys_info.www_id[0] == 255 && sys_info.www_pwd[0] == 255){
    if(sys_info.sys_state == 255){
        #ifdef FLASH_DEBUG
        Serial.println();
        Serial.println("--!--NO SYS_INFO  IN  ROM--!--");
        #endif
        SYS_INFO sys_info_default = SYS_INFO_DEFAULT;
        PWD_INFO pwd_info_default = PWD_INFO_DEFAULT;
        save_sys(sys_info_default);
        save_pwd(0, pwd_info_default);
        load_sys(sys_info);
    }
    load_pwd(0, pwd_info);
}