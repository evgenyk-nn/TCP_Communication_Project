#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <stdint.h>
typedef intptr_t ssize_t;
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif


#define SERVER_IP "127.0.0.1"
#define PORT 8081
#define BUFFER_SIZE 64

// Функция для вычисления CRC16
uint16_t calculate_crc16(const char *data, size_t size) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < size; ++i) {
        crc ^= (uint16_t)data[i];
        for (size_t j = 0; j < 8; ++j) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char request_buffer[BUFFER_SIZE];
    char response_buffer[BUFFER_SIZE];

    // Создание сокета
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Подключение к серверу
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Создание и отправка запроса
    // Пример запроса: client_id=123, address=456, value=-789, high_byte=987, significant_digits=3
    uint16_t client_id = 123;
    uint16_t address = 456;
    int16_t value = -789;
    uint16_t high_byte = 987;
    uint8_t significant_digits = 3;

    // Формирование запроса
    memcpy(request_buffer, &client_id, sizeof(client_id));
    memcpy(request_buffer + 2, &address, sizeof(address));
    memcpy(request_buffer + 5, &value, sizeof(value));
    memcpy(request_buffer + 7, &high_byte, sizeof(high_byte));
    memcpy(request_buffer + 9, &significant_digits, sizeof(significant_digits));

    // Вычисление CRC16 для запроса
    uint16_t crc = calculate_crc16(request_buffer, BUFFER_SIZE - 2);
    memcpy(request_buffer + BUFFER_SIZE - 2, &crc, sizeof(crc));

    // Отправка запроса серверу
    send(client_socket, request_buffer, BUFFER_SIZE, 0);

    // Вывод отправленных данных
    printf("Sent data to server: ");
    for (size_t i = 0; i < BUFFER_SIZE; ++i) {
        printf("%02X ", (unsigned char)request_buffer[i]);
    }
    printf("\n");   


    // Получение ответа от сервера
    ssize_t bytes_received = recv(client_socket, response_buffer, BUFFER_SIZE, 0);
    if (bytes_received == -1) {
        perror("Error receiving data");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Обработка ответа от сервера
    // Реализуйте логику обработки ответа здесь
    // Обработка ответа от сервера
    if (bytes_received == BUFFER_SIZE) {
        // Печать полученных данных
        printf("Received response from server: ");
        for (ssize_t i = 0; i < bytes_received; ++i) {
            printf("%02X ", (unsigned char)response_buffer[i]);
        }
        printf("\n");

        // Распаковка данных из буфера ответа
        uint16_t server_response;
        memcpy(&server_response, response_buffer, sizeof(server_response));

        // Ваша логика обработки данных
        // Например, печать результата
        printf("Server response: %u\n", server_response);
    } else {
        fprintf(stderr, "Unexpected number of bytes received: %zd\n", bytes_received);
    }

    // Закрытие соединения с сервером
    close(client_socket);

    return 0;
}
