#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <stdint.h>
typedef intptr_t ssize_t;
typedef int socklen_t;
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <sys/types.h>

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

void process_request(char *request, char *response, uint16_t address) {
    // Разбор запроса
    uint16_t client_id = *(uint16_t *)request;
    // uint16_t address = *(uint16_t *)(request + 2);  // удалена эта строка
    int16_t value = *(int16_t *)(request + 5);
    uint16_t high_byte = *(uint16_t *)(request + 7);
    uint8_t significant_digits = *(uint8_t *)(request + 9);
    uint16_t received_crc = *(uint16_t *)(request + 10);

    // Проверка контрольной суммы
    uint16_t calculated_crc = calculate_crc16(request, BUFFER_SIZE - 2);
    if (received_crc != calculated_crc) {
        snprintf(response, BUFFER_SIZE, "invalid request");
        return;
    }

    // Логика обработки запроса
    // В данном примере просто копируем адрес слова в ответ
    memcpy(response, &address, sizeof(address));
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char request_buffer[BUFFER_SIZE];
    char response_buffer[BUFFER_SIZE];
    // Объявление переменной address
    uint16_t address;

    // Создание сокета
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Привязка сокета к адресу и порту
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Ожидание подключения клиента
    listen(server_socket, 5);
    printf("Server listening on port %d\n", PORT);

    // Принятие запросов от клиентов
    while (1) {
        // Получение и вывод данных от клиента
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        // Чтение запроса от клиента
        // ssize_t bytes_received = recv(client_socket, request_buffer, BUFFER_SIZE, 0); // ошибка Unexpected number of bytes received: 2 
        ssize_t bytes_received = recv(client_socket, request_buffer, sizeof(request_buffer), 0);
        if (bytes_received == -1) {
            perror("Error receiving data");
            close(client_socket);
            continue;
        }

        // Инициализация переменной address
        address = *(uint16_t *)(request_buffer + 2);

        // Обработка запроса
        process_request(request_buffer, response_buffer, address);

        // Вывод данных от клиента
        printf("Received data from client: ");
        for (ssize_t i = 0; i < bytes_received; ++i) {
            printf("%02X ", (unsigned char)request_buffer[i]);
        }
        printf("\n");

        // Отправка ответа клиенту
        // send(client_socket, response_buffer, sizeof(address), 0);
        send(client_socket, response_buffer, BUFFER_SIZE, 0);


        // Закрытие соединения с клиентом
        close(client_socket);
    }

    // Закрытие серверного сокета
    close(server_socket);

    return 0;
}
