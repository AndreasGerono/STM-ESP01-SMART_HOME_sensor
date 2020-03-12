//
// esp01.h
// Created by Andreas Gerono on 10/10/2019.

#ifndef _esp01_h_
#define _esp01_h_

#include "stdint.h"
#include "usart.h"

#define ESP_UART hlpuart1

#define RESTART "AT+RESTART"
#define RESTORE "AT+RESTORE"
#define CONNECT_WIFI(ssid, password) "AT+CWJAP=\""#ssid"\",\""#password"\""
#define AUTOCONNECT_ON "AT+CWAUTOCONN=1"
#define AUTOCONNECT_OFF "AT+CWAUTOCONN=0"
#define STATION_MODE "AT+CWMODE=1"
#define START_WPS "AT+WPS=1"
#define CONNECT_TCP(ip, port) "AT+CIPSTART=\"TCP\",\""#ip"\""#port
#define DISCONNECT_TCP "AT+CIPCLOSE"
#define PASSTHROUGH_ON "AT+CIPMODE=1"
#define PASSTHROUGH_OFF "AT+CIPMODE=1"
#define PASSTHROUGH_STOP "+++"
#define SEND_MESSAGE "AT+CIPSEND"


void esp_send_data(const char*);
void esp_uart_callback();
void esp_initialize();
void esp_check_connection();
void esp_wps();
void esp_restore();

void esp_set_id(uint32_t id);
void esp_power_on();
_Bool esp_parse_value(uint32_t *value);
_Bool esp_ready();




#endif
