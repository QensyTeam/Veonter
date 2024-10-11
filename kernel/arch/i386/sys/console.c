#include "kernel/sys/console.h"
#include "kernel/drv/keyboard_buffer.h"
#include "kernel/sys/gui/shell.h"
#include <kernel/kernel.h>
#include <kernel/mini_programs/mini_programs.h>
#include <stdint.h>
#include <stdio.h>  // Для printf и других стандартных функций
#include <string.h> // Для strcmp и других строковых функций
#include <stdlib.h> // Для strtol
#include <kernel/drv/ps2_mouse.h>
#include <kernel/drv/serial_port.h>
#include <kernel/vfs.h>

extern rgb_color_t fg_color;
extern rgb_color_t bg_color;
extern rgb_color_t main_color;

char console_current_disk = 1;

#define HISTORY_SIZE 10  // Максимальное количество команд в истории
#define SCREEN_WIDTH 80

// Структура для истории команд
typedef struct {
    uint16_t commands[HISTORY_SIZE][COMMAND_BUFFER_SIZE];
    int current_index;
    int total_commands;
    int history_index;  // Индекс для навигации по истории
} CommandHistory;

static CommandHistory history = { .current_index = 0, .total_commands = 0, .history_index = -1 };

static uint16_t raw_buffer[COMMAND_BUFFER_SIZE * sizeof(uint16_t)];
static char command_buffer[COMMAND_BUFFER_SIZE];
static size_t command_length = 0;
static size_t command_position = 0;

void add_command_to_history(const uint16_t* command, size_t length) {
    if (history.total_commands < HISTORY_SIZE) {
        memcpy(history.commands[history.total_commands++], command, length * sizeof(uint16_t));
    } else {
        for (int i = 1; i < HISTORY_SIZE; i++) {
            memcpy(history.commands[i - 1], history.commands[i], COMMAND_BUFFER_SIZE);
        }
        memcpy(history.commands[HISTORY_SIZE - 1], command, COMMAND_BUFFER_SIZE);
    }
    history.current_index = history.total_commands;
    history.history_index = -1;
}

const uint16_t* get_previous_command() {
    if (history.total_commands == 0 || history.history_index == 0) {
        return NULL;
    }
    if (history.history_index == -1) {
        history.history_index = history.total_commands - 1;
    } else {
        history.history_index--;
    }
    return history.commands[history.history_index];
}

const uint16_t* get_next_command() {
    if (history.history_index == -1 || history.history_index == history.total_commands - 1) {
        return NULL;
    }
    history.history_index++;
    return history.commands[history.history_index];
}

void console_initialize() {
    vbe_clear_screen(bg_color);

    printf("[%d] %s", console_current_disk, PROMPT_STRING);
}

const char* console_help_content[] = {
        "help - Displays help about all possible commands.",
        "clear - Clear the screen.",
        "mm_test - Runs a Memory test program.",
        "window - Displays a test window.",
        "calc - Calculator.",
        "logo - Displays logo.",
        "off - Shutdown PC.",
        "reboot - Rebooting PC.",
        "cpu - Shows CPU information.",
        "colors - Allows you to color your terminal in the colors you want.",
        "echo <text> - Displays the text you enter.",
        "beep <frequency> - Allows you to hear a beep with the specified frequency.",
        "vbe_test - Checking the operation of VBE graphics mode.",
        "disks - Show disk list available in system.",
        "dhv - Disk Hex View (first 1024 bytes).",
        "meminfo - Show memory info.",
        "mousetest - Show button flags and coordinates.",
        "ls - List files.",
        "cat - View file.",
        "wr - Write file.",
        "heapdmp - Prints heap chain into COM1.",
        "rand - Displays 10 random numbers.",
        0
};

void show_help_menu() {
    int content_lines = 0;

    const char** real_content = console_help_content;
    char** backup_pointer = (char**)real_content;

    while(*backup_pointer) {
        content_lines++;
        backup_pointer++;
    }

    printf("\tHELP MENU\n\n");
    
    for (int i = 0; i < content_lines; ++i) {
        printf("\t\t%s\n", real_content[i]);
    }
}

void console_process_command(const char* command) {
    qemu_log("-> %s", command);

    if (strncmp(command, "echo ", 5) == 0) {  // Проверяем команду echo
        printf("%s\n", command + 5);  // Выводим текст после echo
    } else if (strcmp(command, "clear") == 0) {
        vbe_clear_screen(bg_color);
    } else if (strncmp(command, "beep", 4) == 0) {
        const char* freq_str = command + 4;

        // Проверяем, указана ли частота после команды beep
        while (*freq_str == ' ') {
            freq_str++;  // Пропуск всех пробелов
        }

        if (*freq_str == '\0') {
            // Если после команды beep нет ничего, сообщаем об ошибке
            printf("Please specify a frequency after the beep command.\n");
        } else {
            char* endptr;
            unsigned int frequency = strtol(freq_str, &endptr, 10);
            if (*endptr == '\0') {
                beep(frequency, 100);  // Используем указанную частоту
            } else {
                printf("Invalid frequency: %s\n", freq_str);
            }
        }
    } else if (strcmp(command, "help") == 0) {
        printf("\n");
        show_help_menu();
        printf("\n");
    } else if (strcmp(command, "mm_test") == 0) {
        printf("\n");
        mm_test();
        printf("\n");
    } else if (strcmp(command, "calc") == 0) {
        printf("\n");
        calculator();
        printf("\n");
    } else if (strcmp(command, "logo") == 0) {
        printf("\n");
        vbe_clear_screen(bg_color);
        printf("\n");
        logo();
        shell_text_color(fg_color);
        printf("\n");
    } else if (strcmp(command, "off") == 0) {
        printf("Shutting down...\n");
        sleep(1000);
        outw(0x604, 0x2000);
    } else if (strcmp(command, "rand") == 0) {
        srand(12345); // Установить начальное значение генератора случайных чисел

        // Генерация и вывод 10 случайных чисел
        for (int i = 0; i < 10; i++) {
            printf("%d\n", rand());
        }
    } else if (strcmp(command, "window") == 0) {
        printf("Showing Window...\n");
        Window("My Program", 400, 10, 270, 250);
    } else if (strcmp(command, "reboot") == 0) {
        printf("Rebooting...\n");
        sleep(1000);
        outb(0x64, 0xFE);
    } else if (strcmp(command, "vbe_test") == 0) {
        vbe_clear_screen(bg_color);
        vbe_test();
        vbe_clear_screen(bg_color);
        // Сообщаем об окончании теста
        printf("VBE Test completed successfully!\n");

    } else if (strcmp(command, "colors") == 0) {
        colors_program();
    } else if (strcmp(command, "cpu") == 0) {
        main_color = fg_color;
        printf("\n");
        shell_text_color(RGB(150, 150, 150));
        detect_cpu();
        shell_text_color(main_color);
        printf("\n");
    } else if(strcmp(command, "disks") == 0) {
        disk_list();
    } else if(strcmp(command, "dhv") == 0) {
        dhv_program();
    } else if(strcmp(command, "meminfo") == 0) {
        meminfo_program();
    } else if(strcmp(command, "mousetest") == 0) {
        printf("\n  Press q to exit.\n");
        while(true) {
            uint16_t sym = keyboard_get_from_buffer();

            if(sym == 'q') {
                printf("\n\n");
                break;
            }
            printf("  Buttons: %x; X: %lu; Y: %lu; Wheel: %d   \r", mouse_get_buttons(), mouse_get_x(), mouse_get_y(), mouse_get_wheel());
        }
    } else if(strncmp(command, "ls", 2) == 0) {
        char fpath[256] = {0};
        itoa(console_current_disk, fpath, 10);
        strcat(fpath, ":/");

        strcat(fpath, command + 3);

        printf("\nListing path `%s`\n", fpath);

        direntry_t* ent = diropen(fpath);

        if(ent == NULL) {
            printf("Invalid path or filesystem error.\n");
            goto end;
        }

        direntry_t* orig = ent;
        
        do {
            printf("%s\n", ent->name);
            ent = ent->next;
        } while(ent);

        dirclose(orig);
        printf("\n");
    } else if(strncmp(command, "cat ", 4) == 0) {
        char fpath[256] = {0};
        itoa(console_current_disk, fpath, 10);
        strcat(fpath, ":/");

        strcat(fpath, command + 4);

        NFILE* fp = nfopen(fpath);

        if(!fp) {
            printf("Invalid path or filesystem error\n");
            goto end;
        }

        char* data = calloc(fp->size + 1, 1);

        nfread(data, 1, fp->size, fp);

        printf("\n%s\n", data);        

        free(data);

        nfclose(fp);
    } else if (strncmp(command, "wr ", 3) == 0) {
        char fpath[256] = {0};
        char text_to_write[COMMAND_BUFFER_SIZE] = {0};  // Buffer to store text to be written
        const char *args = command + 3;  // Skip the "wr " part of the command

        // Find the first space in args, which separates the file path from the text
        const char *space_pos = strchr(args, ' ');
        if (!space_pos) {
            // If there is no space, the command format is incorrect
            printf("Usage: wr <file_path> <text>\n");
            goto end;
        }

        // Copy the file path part into fpath
        size_t path_len = space_pos - args;
        strncpy(fpath, args, path_len);
        fpath[path_len] = '\0';  // Null-terminate the file path string

        // Combine with the current disk prefix
        char full_fpath[256] = {0};
        itoa(console_current_disk, full_fpath, 10);
        strcat(full_fpath, ":/");
        strcat(full_fpath, fpath);

        // The text to write starts after the space
        strcpy(text_to_write, space_pos + 1);

        // Open the file
        NFILE *fp = nfopen(full_fpath);
        if (!fp) {
            printf("Invalid path or filesystem error\n");
            goto end;
        }

        // Write text to the file
        nfwrite(text_to_write, 1, strlen(text_to_write), fp);

        // Close the file
        nfclose(fp);

        printf("OK\n");
    } else if(strcmp(command, "heapdmp") == 0) {
        void kheap_dump();

        kheap_dump();
    } else if(strncmp(command, "mkdir ", 6) == 0) {
        char fpath[256] = {0};
        itoa(console_current_disk, fpath, 10);
        strcat(fpath, ":/");

        strcat(fpath, command + 6);

        int result = mkdir(fpath);

        if(result == -1) {
            printf("Error! Make sure that path is valid and you have parent directories created\n");
        } else {
            printf("OK\n");
        }
    } else {
        printf("Unknown command: ");
        printf("%s", command);
        shell_putchar('\n');
    }

end:
    printf("[%d] %s", console_current_disk, PROMPT_STRING);
}

void console_reset() {
    memset(raw_buffer, 0, 256);
    memset(command_buffer, 0, 256);

    command_length = 0;
    command_position = 0;
}

size_t console_rawbuf_actual_length() {
    size_t a = 0;

    while(raw_buffer[a++]) {}

    return a;
}

void console_raw_to_command() {
    for(size_t i = 0, j = 0; i < command_length; i++) {
        if(raw_buffer[i] > 0xff) {
            command_buffer[j++] = (char)(raw_buffer[i] & 0xff);
            command_buffer[j] = (char)(raw_buffer[i] >> 8);
        } else {
            command_buffer[j] = (char)raw_buffer[i];
        }

        j++;
    }
}

extern volatile bool cursor_update;
extern volatile bool cursor_visible;

void console_input_loop() {
	uint16_t c;

    printf("[%d] %s", console_current_disk, PROMPT_STRING);
	
    console_reset();

    while (1) {
        disable_cursor(); // скрываем курсор перед изменением экрана
        c = keyboard_get_char();
        //enable_cursor(); // показываем курсор в текущем положении
        disable_cursor(); // скрываем курсор перед изменением экрана
        if (c == '\n') {
            disable_cursor(); // скрываем курсор перед изменением экрана
            putchar('\n');
            if (command_length == 0) {
                printf("[%d] %s", console_current_disk, PROMPT_STRING);
            } else {
                raw_buffer[command_length] = 0;

                console_raw_to_command();
                
                add_command_to_history(raw_buffer, command_length); // Добавление команды в историю

                console_process_command((char*)command_buffer);

                console_reset();
            }
        } else if (c == '\b') {
            if (command_length > 0) {
                if (vbe_getcolumn() > PROMPT_LENGTH) {
                    //console_reset();
                    command_position--;
                    command_length--;

                    // Удаляем символ на текущей позиции
                    for (size_t i = command_position; i < command_length; i++) {
                        raw_buffer[i] = raw_buffer[i + 1]; // Сдвигаем символы влево
                    }

                    // Перерисовываем строку после удаления символа
                    cursor_x--;
                    for (size_t i = command_position; i < command_length; i++) {
                        shell_putchar(raw_buffer[i]); // Печатаем оставшиеся символы
                    }

                    // Очищаем последний символ, который остаётся в конце строки
                    putchar(' ');

                    // Возвращаем курсор в правильную позицию
                    for (size_t i = command_length; i > command_position; i--) {
                        cursor_x--;
                    }

                    // Обновляем экранное положение курсора
                    cursor_x = command_position + (strlen(PROMPT_STRING) * 2);
                }
            }
        } else if (c == '\x1B') {
            qemu_log("Command position: %u; Cursor X: %u", command_position, cursor_x);

            // Обработка специального символа для стрелок
            c = keyboard_get_char();
            if (c == '[') {
                c = keyboard_get_char();
                if (c == 'A') { // Стрелка вверх
                    const uint16_t* previous_command = get_previous_command();
                    if (previous_command) {
                        // Очистка текущей строки
                        while (command_length > 0) {
                            shell_putchar('\b');
                            command_length--;
                        }
                        
                        console_reset();

                        memcpy(raw_buffer, previous_command, 256 * sizeof(uint16_t));
                        
                        command_length = console_rawbuf_actual_length();

                        console_raw_to_command();

                        printf("%s", command_buffer);
                        
                        command_length = strlen(command_buffer);

                        command_position = command_length;
                    }
                } else if (c == 'B') { // Стрелка вниз
                    const uint16_t* next_command = get_next_command();
                    if (next_command) {
                        while (command_length > 0) {
                            shell_putchar('\b');
                            command_length--;
                        }
                        
                        console_reset();

                        memcpy(raw_buffer, next_command, 256 * sizeof(uint16_t));
                        
                        command_length = console_rawbuf_actual_length();

                        console_raw_to_command();

                        printf("%s", command_buffer);
                        
                        command_length = strlen(command_buffer);
                        command_position = command_length;
                    } else {
                        while (command_length > 0) {
                            shell_putchar('\b');
                            command_length--;
                        }
                    }
                } else if(c == 'D') {
                    disable_cursor();
                    if(command_position > 0) {
                        command_position--;
                        cursor_x--;
                        qemu_log("Vasen %d/%d", command_position, command_length);
                    }
                } else if(c == 'C') {
                    disable_cursor();
                    if(command_position < command_length) {
                        command_position++;
                        cursor_x++;
                        qemu_log("Oikea %d/%d", command_position, command_length);
                    }
                }
            }
        } else {
            if (command_length < COMMAND_BUFFER_SIZE - 1) {
                // Вставка символа и сдвиг символов вправо
                for (size_t i = command_length; i > command_position; i--) {
                    raw_buffer[i] = raw_buffer[i - 1]; // Сдвиг существующих символов вправо
                }
                raw_buffer[command_position++] = c; // Вставка нового символа
                command_length++;

                // Перерисовка строки с новой позиции
                for (size_t i = command_position - 1; i < command_length; i++) {
                    //cursor_x = i; // Обновляем позицию курсора
                    shell_putchar(raw_buffer[i]);
                }

                cursor_x = command_position + (strlen(PROMPT_STRING) * 2) - 1;
            }
        }
        enable_cursor(); // показываем курсор после изменения экрана
    }
}
