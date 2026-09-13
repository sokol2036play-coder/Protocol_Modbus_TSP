#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <clocale>
#include <modbus.h>

// ================== НАСТРОЙКИ ПОДКЛЮЧЕНИЯ ==================
const char* IP_ADDRESS = "192.168.1.99"; // IP МКОН-230
const int   PORT = 502;
const int   DEFAULT_SLAVE_ID = 16;         // Slave ID по умолчанию
// ==========================================================

modbus_t* ctx = nullptr;

// ---------- Вспомогательная функция вывода ошибок ----------
void print_error(const char* operation) {
	fprintf(stderr, "[ОШИБКА] %s: %s\n", operation, modbus_strerror(errno));
}

// ---------- 01 (0x01): Чтение Coils ----------
void read_coils(int slave_id, int addr, int count) {
	if (modbus_set_slave(ctx, slave_id) == -1) {
		print_error("Set Slave ID");
		return;
	}
	uint8_t bits[64];
	if (count > 64) count = 64;
	int rc = modbus_read_bits(ctx, addr, count, bits);
	if (rc == -1) { print_error("Read Coils (0x01)"); return; }
	printf("\n[01] Slave %d: прочитано %d Coils с адреса %d:\n", slave_id, rc, addr);
	for (int i = 0; i < rc; i++)
		printf("  Coil[%d] = %s\n", addr + i, bits[i] ? "ON" : "OFF");
}

// ---------- 02 (0x02): Чтение Discrete Inputs ----------
void read_discrete_inputs(int slave_id, int addr, int count) {
	if (modbus_set_slave(ctx, slave_id) == -1) {
		print_error("Set Slave ID");
		return;
	}
	uint8_t bits[64];
	if (count > 64) count = 64;
	int rc = modbus_read_input_bits(ctx, addr, count, bits);
	if (rc == -1) { print_error("Read Discrete Inputs (0x02)"); return; }
	printf("\n[02] Slave %d: прочитано %d Discrete Inputs с адреса %d:\n", slave_id, rc, addr);
	for (int i = 0; i < rc; i++)
		printf("  Input[%d] = %s\n", addr + i, bits[i] ? "ON" : "OFF");
}

// ---------- 03 (0x03): Чтение Holding Registers ----------
void read_holding_registers(int slave_id, int addr, int count) {
	if (modbus_set_slave(ctx, slave_id) == -1) {
		print_error("Set Slave ID");
		return;
	}
	uint16_t regs[64];
	if (count > 64) count = 64;
	int rc = modbus_read_registers(ctx, addr, count, regs);
	if (rc == -1) { print_error("Read Holding Registers (0x03)"); return; }
	printf("\n[03] Slave %d: прочитано %d Holding Registers с адреса %d:\n", slave_id, rc, addr);
	for (int i = 0; i < rc; i++)
		printf("  Reg[%d] = %u (0x%04X)\n", addr + i, regs[i], regs[i]);
}

// ---------- 04 (0x04): Чтение Input Registers ----------
void read_input_registers(int slave_id, int addr, int count) {
	if (modbus_set_slave(ctx, slave_id) == -1) {
		print_error("Set Slave ID");
		return;
	}
	uint16_t regs[64];
	if (count > 64) count = 64;
	int rc = modbus_read_input_registers(ctx, addr, count, regs);
	if (rc == -1) { print_error("Read Input Registers (0x04)"); return; }
	printf("\n[04] Slave %d: прочитано %d Input Registers с адреса %d:\n", slave_id, rc, addr);
	for (int i = 0; i < rc; i++)
		printf("  InputReg[%d] = %u (0x%04X)\n", addr + i, regs[i], regs[i]);
}

// ---------- 05 (0x05): Запись одного Coil ----------
void write_single_coil(int slave_id, int addr, bool value) {
	if (modbus_set_slave(ctx, slave_id) == -1) {
		print_error("Set Slave ID");
		return;
	}
	int rc = modbus_write_bit(ctx, addr, value ? TRUE : FALSE);
	if (rc == -1) { print_error("Write Single Coil (0x05)"); return; }
	printf("\n[05] Slave %d: Coil[%d] установлен в %s.\n", slave_id, addr, value ? "ON" : "OFF");
}

// ---------- 06 (0x06): Запись одного Holding Register ----------
void write_single_register(int slave_id, int addr, uint16_t value) {
	if (modbus_set_slave(ctx, slave_id) == -1) {
		print_error("Set Slave ID");
		return;
	}
	int rc = modbus_write_register(ctx, addr, value);
	if (rc == -1) { print_error("Write Single Register (0x06)"); return; }
	printf("\n[06] Slave %d: Register[%d] = %u.\n", slave_id, addr, value);
}

// ---------- 15 (0x0F): Запись нескольких Coils ----------
void write_multiple_coils(int slave_id, int addr, int count, const uint8_t* values) {
	if (modbus_set_slave(ctx, slave_id) == -1) {
		print_error("Set Slave ID");
		return;
	}
	int rc = modbus_write_bits(ctx, addr, count, values);
	if (rc == -1) { print_error("Write Multiple Coils (0x0F)"); return; }
	printf("\n[15] Slave %d: записано %d Coils с адреса %d.\n", slave_id, rc, addr);
}

// ---------- 16 (0x10): Запись нескольких Holding Registers ----------
void write_multiple_registers(int slave_id, int addr, int count, const uint16_t* values) {
	if (modbus_set_slave(ctx, slave_id) == -1) {
		print_error("Set Slave ID");
		return;
	}
	int rc = modbus_write_registers(ctx, addr, count, values);
	if (rc == -1) { print_error("Write Multiple Registers (0x10)"); return; }
	printf("\n[16] Slave %d: записано %d Holding Registers с адреса %d.\n", slave_id, rc, addr);
}

// ---------- Меню ----------
void print_menu() {
	printf("\n========== МЕНЮ ==========\n");
	printf(" 1 - Читать Coils (0x01)\n");
	printf(" 2 - Читать Discrete Inputs (0x02)\n");
	printf(" 3 - Читать Holding Registers (0x03)\n");
	printf(" 4 - Читать Input Registers (0x04)\n");
	printf(" 5 - Записать один Coil (0x05)\n");
	printf(" 6 - Записать один Holding Register (0x06)\n");
	printf(" 7 - Записать несколько Coils (0x0F)\n");
	printf(" 8 - Записать несколько Holding Registers (0x10)\n");
	printf(" 0 - Выход\n");
	printf("Выбор: ");
}


bool is_port_open(const char* ip, int port) {
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	//InetPton(AF_INET, ip, &addr.sin_addr);
#pragma warning(push)
#pragma warning(disable: 4996)
	addr.sin_addr.s_addr = inet_addr(ip);
#pragma warning(pop)

	// Устанавливаем таймаут 3 секунды
	DWORD timeout = 3000;
	setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));

	int result = connect(s, (sockaddr*)&addr, sizeof(addr));
	int err = WSAGetLastError();
	closesocket(s);
	WSACleanup();

	if (result == SOCKET_ERROR) {
		printf("Ошибка сокета: %d\n", err);
		// 10060 = таймаут, 10061 = отказ, 10013 = доступ запрещён
		return false;
	}
	return true;
}

int main() {
	setlocale(LC_ALL, "Russian");

	// 1. Создание контекста
	ctx = modbus_new_tcp(IP_ADDRESS, PORT);
	if (ctx == nullptr) {
		fprintf(stderr, "Ошибка создания контекста Modbus TCP: %s\n", modbus_strerror(errno));
		return EXIT_FAILURE;
	}

	// Таймаут ответа (1 секунда)
	modbus_set_response_timeout(ctx, 10, 0);

	//проверка доступности порта
	bool port_ok = is_port_open(IP_ADDRESS, PORT);
	printf("Порт %d %s\n", PORT, port_ok ? "ОТКРЫТ" : "НЕДОСТУПЕН");

	//включение повторного подключения при обрыве связи
	//---- приводит к бесконечному циклу при обрыве соединения!!!!!
	//modbus_set_error_recovery(ctx, MODBUS_ERROR_RECOVERY_LINK);

	//включение отладочного вывода
	modbus_set_debug(ctx, TRUE);

	// 2. Подключение
	if (modbus_connect(ctx) == -1) {
		int werr = WSAGetLastError();
		const char* desc = "неизвестная ошибка";
		switch (werr) {
		case 10060: desc = "Таймаут соединения (WSAETIMEDOUT)"; break;
		case 10061: desc = "Соединение отклонено (WSAECONNREFUSED)"; break;
		case 10065: desc = "Хост недоступен (WSAEHOSTUNREACH)"; break;
		case 10049: desc = "Неверный адрес (WSAEADDRNOTAVAIL)"; break;
		case 10013: desc = "Доступ запрещён (WSAEACCES) — вероятно, блокировка брандмауэром"; break;
		case 10022: desc = "Неверный аргумент (WSAEINVAL)"; break;
		case 10093: desc = "Winsock не инициализирован (WSANOTINITIALISED)"; break;
		}
		fprintf(stderr, "Ошибка подключения к %s:%d\n", IP_ADDRESS, PORT);
		fprintf(stderr, "Код Windows: %d — %s\n", werr, desc);
		fprintf(stderr, "errno: %d (%s)\n", errno, modbus_strerror(errno));
		modbus_free(ctx);
		return EXIT_FAILURE;
	}
	printf("Успешное подключение к %s:%d.\n", IP_ADDRESS, PORT);

	// 3. Главный цикл меню
	int choice = -1;
	while (choice != 0) {
		print_menu();
		if (scanf_s("%d", &choice) != 1) {
			while (getchar() != '\n');
			continue;
		}

		// Запрос Slave ID для каждой операции
		int slave_id = DEFAULT_SLAVE_ID;
		if (choice >= 1 && choice <= 8) {
			printf("Введите Slave ID (по умолчанию %d): ", DEFAULT_SLAVE_ID);
			if (scanf_s("%d", &slave_id) != 1) {
				while (getchar() != '\n');
				slave_id = DEFAULT_SLAVE_ID;
			}
		}

		switch (choice) {
		case 1: {
			int addr, count;
			printf("Адрес начала (0..): "); scanf_s("%d", &addr);
			printf("Количество (1..64): "); scanf_s("%d", &count);
			read_coils(slave_id, addr, count);
			break;
		}
		case 2: {
			int addr, count;
			printf("Адрес начала (0..): "); scanf_s("%d", &addr);
			printf("Количество (1..64): "); scanf_s("%d", &count);
			read_discrete_inputs(slave_id, addr, count);
			break;
		}
		case 3: {
			int addr, count;
			printf("Адрес начала (0..): "); scanf_s("%d", &addr);
			printf("Количество (1..64): "); scanf_s("%d", &count);
			read_holding_registers(slave_id, addr, count);
			break;
		}
		case 4: {
			int addr, count;
			printf("Адрес начала (0..): "); scanf_s("%d", &addr);
			printf("Количество (1..64): "); scanf_s("%d", &count);
			read_input_registers(slave_id, addr, count);
			break;
		}
		case 5: {
			int addr, val;
			printf("Адрес Coil: "); scanf_s("%d", &addr);
			printf("Значение (0=OFF, 1=ON): "); scanf_s("%d", &val);
			write_single_coil(slave_id, addr, val != 0);
			break;
		}
		case 6: {
			int addr, val;
			printf("Адрес регистра: "); scanf_s("%d", &addr);
			printf("Значение (0..65535): "); scanf_s("%d", &val);
			write_single_register(slave_id, addr, static_cast<uint16_t>(val));
			break;
		}
		case 7: {
			int addr, count;
			printf("Адрес начала: "); scanf_s("%d", &addr);
			printf("Количество (1..64): "); scanf_s("%d", &count);
			if (count < 1 || count > 64) { printf("Неверное количество.\n"); break; }
			uint8_t values[64];
			for (int i = 0; i < count; i++) {
				int v;
				printf("  Coil[%d] (0/1): ", addr + i); scanf_s("%d", &v);
				values[i] = (v != 0) ? 1 : 0;
			}
			write_multiple_coils(slave_id, addr, count, values);
			break;
		}
		case 8: {
			int addr, count;
			printf("Адрес начала: "); scanf_s("%d", &addr);
			printf("Количество (1..64): "); scanf_s("%d", &count);
			if (count < 1 || count > 64) { printf("Неверное количество.\n"); break; }
			uint16_t values[64];
			for (int i = 0; i < count; i++) {
				int v;
				printf("  Reg[%d] (0..65535): ", addr + i); scanf_s("%d", &v);
				values[i] = static_cast<uint16_t>(v);
			}
			write_multiple_registers(slave_id, addr, count, values);
			break;
		}
		case 0:
			printf("Выход...\n");
			break;
		default:
			printf("Неверный пункт меню.\n");
		}
	}

	// 4. Закрытие соединения
	modbus_close(ctx);
	modbus_free(ctx);
	printf("Соединение закрыто.\n");
	return EXIT_SUCCESS;
}