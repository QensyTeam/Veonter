#include <kernel/kernel.h>
#include <kernel/mini_programs/mini_programs.h>
#include <stdio.h>  // Для printf и других стандартных функций
#include <string.h> // Для strcmp и других строковых функций

#define SCREEN_WIDTH 80
#define HISTORY_SIZE 10  // Максимальное количество команд в истории

#define BORDER_TOP_LEFT 218
#define BORDER_TOP_RIGHT 191
#define BORDER_BOTTOM_LEFT 192
#define BORDER_BOTTOM_RIGHT 217
#define BORDER_HORIZONTAL 196
#define BORDER_VERTICAL 179
#define BORDER_HORIZONTAL_TITLE 205
#define BORDER_VERTICAL_LEFT 181
#define BORDER_VERTICAL_RIGHT 198

// Структура для истории команд
typedef struct {
    char commands[HISTORY_SIZE][COMMAND_BUFFER_SIZE];
    int current_index;
    int total_commands;
    int history_index;  // Индекс для навигации по истории
} CommandHistory;

static CommandHistory history = { .current_index = 0, .total_commands = 0, .history_index = -1 };

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
    terminal_clearscreen();
    printf(PROMPT_STRING);
}

void print_chars(char c, int count) {
    for (int i = 0; i < count; ++i) {
        putchar(c);
    }
}

void print_help_line(const char* line, int width, int frame_width) {
    int len = strlen(line);
    int left_padding = (SCREEN_WIDTH - frame_width) / 2;
    if (frame_width <= SCREEN_WIDTH) {
        print_chars(' ', left_padding);
        putchar(BORDER_VERTICAL); // Левая граница
        printf("  ");
        printf(" %s", line);

        for (int i = len + 1; i < width - 1; ++i) {
            putchar(' ');
        }
        putchar(BORDER_VERTICAL); // Правая граница
        printf("\n");
    }
}

void print_frame(const char* title, const char* content[], int content_lines, int frame_width) {
    // Вычислите отступ слева для центрирования рамки
    int left_padding = (SCREEN_WIDTH - frame_width) / 2;

    // Верхняя граница
    if (frame_width <= SCREEN_WIDTH) {
        print_chars(' ', left_padding);
        putchar(BORDER_TOP_LEFT); // Левый вверхний угол
        print_chars(BORDER_HORIZONTAL, frame_width - 2);
        putchar(BORDER_TOP_RIGHT); // Правый вверхний угол
        printf("\n");
    }

    // Заголовок
    int title_len = strlen(title);
    int title_padding = (frame_width - title_len) / 2;
    if (frame_width <= SCREEN_WIDTH) {
        print_chars(' ', left_padding);
        putchar(BORDER_VERTICAL); // Левая граница
        print_chars(' ', title_padding);
        printf("%s", title);
        print_chars(' ', frame_width - title_len - title_padding - 2);
        putchar(BORDER_VERTICAL); // Правая граница
        printf("\n");
    }

    // Разделительная линия
    if (frame_width <= SCREEN_WIDTH) {
        print_chars(' ', left_padding);
        putchar(BORDER_VERTICAL_RIGHT); // Т-образная линия 
        print_chars(BORDER_HORIZONTAL_TITLE, frame_width - 2);
        putchar(BORDER_VERTICAL_LEFT); // Т-образная линия 
        printf("\n");

        // Содержание
        for (int i = 0; i < content_lines; ++i) {
            print_help_line(content[i], frame_width - 3, frame_width);
        }
    }

    // Нижняя граница
    if (frame_width <= SCREEN_WIDTH) {
        print_chars(' ', left_padding);
        putchar(BORDER_BOTTOM_LEFT); // Левый нижний угол
        print_chars(BORDER_HORIZONTAL, frame_width - 2);
        putchar(BORDER_BOTTOM_RIGHT); // Правый нижний угол
        printf("\n");
    }
}

void show_help_menu() {
    const char* content[] = {
        " ",
        "help - Displays help about all possible commands",
        "clear - Clear the screen.",
        //"mm_test - Runs a Memory test program.",
        "calc - Calculator.",
        "logo - Displays logo.",
        "off - Shutdown PC.",
        " "
    };
    int content_lines = sizeof(content) / sizeof(content[0]);
    print_frame("HELP MENU", content, content_lines, 56);
}

void console_process_command(const char* command) {
    if (strcmp(command, "clear") == 0) {
        terminal_clearscreen();
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
        terminal_clearscreen();
        terminal_startscreen();
        printf("\n");
    } else if (strcmp(command, "off") == 0) {
        printf("Shutting down...\n");
        sleep(1000);
        outw(0x604, 0x2000);
    } else {
        printf("Unknown command: ");
        printf(command);
        terminal_putchar('\n');
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
                if (terminal_getcolumn() > PROMPT_LENGTH) {
                    command_length--;
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
                            putchar('\b');
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
                            putchar('\b');
                            command_length--;
                        }
                        printf("%s", next_command);
                        strcpy(command_buffer, next_command);
                        command_length = strlen(next_command);
                    } else {
                        while (command_length > 0) {
                            putchar('\b');
                            command_length--;
                        }
                    }
                }
            }
        } else {
            if (command_length < COMMAND_BUFFER_SIZE - 1) {
                command_buffer[command_length++] = c;
            }
        }
    }
}
