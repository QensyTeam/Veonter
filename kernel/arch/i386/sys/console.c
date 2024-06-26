#include <kernel/kernel.h>
#include <kernel/mini_programs/mini_programs.h>
#include <stdio.h>  // Для printf и других стандартных функций
#include <string.h> // Для strcmp и других строковых функций

#define HISTORY_SIZE 10  // Максимальное количество команд в истории
#define SCREEN_WIDTH 80

// Структура для истории команд
typedef struct {
    char commands[HISTORY_SIZE][COMMAND_BUFFER_SIZE];
    int current_index;
    int total_commands;
    int history_index;  // Индекс для навигации по истории
} CommandHistory;

static CommandHistory history = { .current_index = 0, .total_commands = 0, .history_index = -1 };

static char command_buffer[COMMAND_BUFFER_SIZE];
static size_t command_length = 0;

void add_command_to_history(const char* command) {
    if (history.total_commands < HISTORY_SIZE) {
        strcpy(history.commands[history.total_commands++], command);
    } else {
        for (int i = 1; i < HISTORY_SIZE; i++) {
            strcpy(history.commands[i - 1], history.commands[i]);
        }
        strcpy(history.commands[HISTORY_SIZE - 1], command);
    }
    history.current_index = history.total_commands;
    history.history_index = -1;
}

const char* get_previous_command() {
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

const char* get_next_command() {
    if (history.history_index == -1 || history.history_index == history.total_commands - 1) {
        return NULL;
    }
    history.history_index++;
    return history.commands[history.history_index];
}

void console_initialize() {
    keyboard_init();
    vbe_clear_screen(RGB(0,0,0));
    printf(PROMPT_STRING);
}

void show_help_menu() {
    const char* content[] = {
        "help - Displays help about all possible commands",
        "clear - Clear the screen.",
        "mm_test - Runs a Memory test program.",
        "window - Displays a test window.",
        "calc - Calculator.",
        "logo - Displays logo.",
        "off - Shutdown PC.",
        "reboot - Rebooting PC.",
        "cpu - Shows CPU information."
    };
    int content_lines = sizeof(content) / sizeof(content[0]);

    printf("\tHELP MENU\n\n");
    for (int i = 0; i < content_lines; ++i) {
        printf("\t\t%s\n", content[i]);
    }
}


void console_process_command(const char* command) {
    if (strcmp(command, "clear") == 0) {
        vbe_clear_screen(RGB(0,0,0));
    } else if (strcmp(command, "beep") == 0) {
        beep(1000, 1000); // Частота 1000 Гц, продолжительность 1000 мс
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
        vbe_clear_screen(RGB(0,0,0));
        logo();
        shell_text_color(fg_color);
        printf("\n");
    } else if (strcmp(command, "off") == 0) {
        printf("Shutting down...\n");
        sleep(1000);
        outw(0x604, 0x2000);
    } else if (strcmp(command, "window") == 0) {
        printf("Showing Window...\n");
        Window("My Program", 512, 10, 370, 250);
    } else if (strcmp(command, "reboot") == 0) {
        printf("Rebooting...\n");
        sleep(1000);
        outb(0x64, 0xFE);
    } else if (strcmp(command, "cpu") == 0) {
        printf("\n");
        shell_text_color(RGB(150, 150, 150));
	    detect_cpu();
	    shell_text_color(fg_color);
        printf("\n");
    } else {
        printf("Unknown command: ");
        printf(command);
        shell_putchar('\n');
    }
    add_command_to_history(command); // Добавление команды в историю
    printf(PROMPT_STRING);
}

void console_input_loop() {
    char c;
    while (1) {
        c = keyboard_get_char();

        if (c == '\n') {
            if (command_length == 0) {
                printf(PROMPT_STRING);
            } else {
                command_buffer[command_length] = '\0';
                console_process_command(command_buffer);
                command_length = 0;
            }
        } else if (c == '\b') {
            if (command_length > 0) {
                if (vbe_getcolumn() > PROMPT_LENGTH) {
                    command_length--;
                    putchar('\b'); // Удаление символа с экрана
                }
            }
        } else if (c == '\x1B') {
            // Обработка специального символа для стрелок
            c = keyboard_get_char();
            if (c == '[') {
                c = keyboard_get_char();
                if (c == 'A') { // Стрелка вверх
                    const char* previous_command = get_previous_command();
                    if (previous_command) {
                        // Очистка текущей строки
                        while (command_length > 0) {
                            shell_putchar('\b');
                            command_length--;
                        }
                        printf("%s", previous_command);
                        strcpy(command_buffer, previous_command);
                        command_length = strlen(previous_command);
                    }
                } else if (c == 'B') { // Стрелка вниз
                    const char* next_command = get_next_command();
                    if (next_command) {
                        while (command_length > 0) {
                            shell_putchar('\b');
                            command_length--;
                        }
                        printf("%s", next_command);
                        strcpy(command_buffer, next_command);
                        command_length = strlen(next_command);
                    } else {
                        while (command_length > 0) {
                            shell_putchar('\b');
                            command_length--;
                        }
                    }
                }
            }
        } else {
            if (command_length < COMMAND_BUFFER_SIZE - 1) {
                command_buffer[command_length++] = c;
                putchar(c); // Отображение символа на экране
            }
        }
    }
}
