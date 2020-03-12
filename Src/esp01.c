//
// esp01.c
// Created by Andreas Gerono on 10/10/2019.

#include "esp01.h"
#include "string.h"
#include "tasker.h"
#include <stdio.h>
#include <stdlib.h>

#define RX_BUFFER_SIZE 150
#define TX_BUFFER_SIZE 50
#define PAIRING 9999

enum Status{
	No_Status,
	AP_Disconnected,
	AP_Connected,
	Server_Connected,
	Passthrough
} esp_status;

static uint8_t rx_buffer[RX_BUFFER_SIZE];
static uint8_t tx_buffer[TX_BUFFER_SIZE];
static uint32_t device_id = 0;



_Bool esp_ready(){
	if (esp_status == Passthrough) {
		return 1;
	}

	return 0;
}




static void clean_rx_buffer(){
	memset(rx_buffer, ' ', RX_BUFFER_SIZE);
}

static _Bool search_rx_buffer(const char *str){
	return NULL != strstr((const char*)rx_buffer, str);
}

static void wait_for_response(const char* response, uint16_t timeout_ms){
	Task time_out_task = task_make(timeout_ms, (void*)NVIC_SystemReset);
	task_start(time_out_task);
	while(!search_rx_buffer(response) && !task_state(time_out_task, NULL));
	task_destroy(time_out_task);
}

void esp_send_data(const char* data){
	strcpy((char*)tx_buffer, data);
	strcat((char*)tx_buffer, "\r\n");
	HAL_UART_Transmit(&ESP_UART, tx_buffer, strlen(data)+2, HAL_MAX_DELAY);
}

static void send_command(const char* data){
	strcpy((char*)tx_buffer, data);
	strcat((char*)tx_buffer, "\r\n");
	HAL_UART_Transmit(&ESP_UART, tx_buffer, strlen(data)+2, HAL_MAX_DELAY);
	HAL_UART_Abort(&ESP_UART);
	HAL_UART_Receive_DMA(&ESP_UART, rx_buffer, RX_BUFFER_SIZE);
	clean_rx_buffer();
}



static void disable_passthrough(){
	HAL_UART_Transmit(&ESP_UART, (uint8_t*) "+++", 3, HAL_MAX_DELAY);
	HAL_UART_Abort(&ESP_UART);
	HAL_UART_Receive_IT(&ESP_UART, rx_buffer, RX_BUFFER_SIZE);
	HAL_Delay(1100);
}

static void enable_passthrough(){
	send_command("AT+CIPMODE=1");
	wait_for_response("OK", 500);
	send_command("AT+CIPSEND");
	wait_for_response(">", 500);
	esp_status = Passthrough;
}

static void connect_to_server(){
	send_command("AT+CIPSTART=\"TCP\",\"192.168.0.143\",1337");
	wait_for_response("OK", 3000);
	esp_status = Server_Connected;
	HAL_Delay(100);
}



static void set_esp_status(){
	send_command("AT+CIPSTATUS");
	wait_for_response("OK", 1000);
	if (search_rx_buffer("STATUS:2") || search_rx_buffer("STATUS:4")){
		esp_status = AP_Connected;
		LD1_GPIO_Port->BRR = LD1_Pin;
	}
	else if (search_rx_buffer("STATUS:3"))
		esp_status = Server_Connected;
	else if (search_rx_buffer("STATUS:5")){
		esp_status = AP_Disconnected;
		LD1_GPIO_Port->BSRR = LD1_Pin;

	}

}

static void pair_device(){
	char buffer[20];
	itoa((device_id*10000 + PAIRING), buffer, 10);
	send_command(buffer);
}

void esp_set_id(uint32_t id){
	device_id = id;
}

void esp_power_on(){
	HAL_Delay(50);
	ESP_PWR_GPIO_Port->BSRR = ESP_PWR_Pin;
	HAL_Delay(5000);
}


void esp_restart(){
	ESP_PWR_GPIO_Port->BRR = ESP_PWR_Pin;
	esp_power_on();
}




void esp_initialize(){

	if (esp_status == No_Status){
		disable_passthrough();
		set_esp_status();
	}
	if (esp_status == AP_Connected)
		connect_to_server();
	if (esp_status == Server_Connected)
		enable_passthrough();
	if (esp_status == Passthrough)
		pair_device();
	if (esp_status == AP_Disconnected){

	}

}

void esp_wps(){
	LD2_GPIO_Port->BSRR = LD2_Pin;
	LD1_GPIO_Port->BSRR = LD1_Pin;
	disable_passthrough();
	send_command("AT+CWMODE=1");
	send_command("AT+WPS=1");
	wait_for_response("connecting", 30000);
	esp_status = AP_Connected;
	HAL_Delay(1000);	//???
	esp_initialize();
}

void esp_restore(){
	send_command("AT+RESTORE");
}

void esp_check_connection(){
	if (search_rx_buffer("Connected"))
		esp_initialize();
	else if(search_rx_buffer("ERROR")){
		esp_status = No_Status;
		esp_initialize();
	}
	else if (esp_status == Passthrough) {
		LD2_GPIO_Port->ODR ^= LD2_Pin;
		esp_send_data("OK");
	}
	else
		LD2_GPIO_Port->BRR = LD2_Pin;
}

_Bool esp_parse_value(uint32_t *value){
	static char command[] = "$";

	if (esp_status != Passthrough)
		return 0;


	char* start_string = strstr((const char*) rx_buffer, command);
	if (NULL != start_string) {
		char* end_string = strstr(start_string, "\r\n");
		if (NULL != end_string) {
			char *value_start = strchr(start_string, ' ');
			if (NULL != value_start) {
				if (NULL != value)
					*value = strtoul(value_start, NULL, 10);
				memset(start_string, '*', end_string-start_string+2);
				return 1;
			}
		}
	}
	return 0;
}


void esp_uart_callback(){	//put this in UART rx_callback in main
	HAL_UART_Receive_DMA(&ESP_UART, rx_buffer, RX_BUFFER_SIZE);
}





