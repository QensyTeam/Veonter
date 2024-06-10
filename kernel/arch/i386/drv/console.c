#include <kernel/kernel.h>
#include <kernel/mini_programs/mini_programs.h>
#include <stdio.h>  // Для printf и других стандартных функций
#include <string.h> // Для strcmp и других строковых функций

#define COMMAND_BUFFER_SIZE 256
#define PROMPT_STRING "csl> "
#define PROMPT_LENGTH 4  // Длина строки приглашения с учётом '\0'

static char command_buffer[COMMAND_BUFFER_SIZE];
static size_t command_length = 0;

void console_initialize() {
    keyboard_init();
    terminal_clearscreen();
    terminal_writestring(PROMPT_STRING);
}

void print_help_line(const char* line, int width, int frame_width) {
    int len = strlen(line);

    if (frame_width <= 78) {
        printf("  ");
        printf(" %s", line);

        for (int i = len + 1; i < width - 1; ++i) {
            putchar(' ');
        }
        putchar(179); // Правая граница
        printf("\n");
    }
    
}


void print_chars(char c, int count) {
    for (int i = 0; i < count; ++i) {
        putchar(c);
    }
}

void print_frame(const char* title, const char* content[], int content_lines, int frame_width) {
    // Верхняя граница
    if (frame_width <= 78) {
        print_chars(196, frame_width);
        putchar(191); // Правая граница
        printf("\n");
    }

    // Заголовок
    int title_len = strlen(title);
    int padding = (frame_width - title_len) / 2;
    if (frame_width <= 78) {
        print_chars(' ', padding);
        printf("%s", title);
        print_chars(' ', frame_width - title_len - padding);
        putchar(179); // Правая граница
        printf("\n");
    }

    // Разделительная линия
    if (frame_width <= 78) {
        print_chars(196, frame_width);
        putchar(180); // Т-образная линия справа
        printf("\n");

        // Содержание
        for (int i = 0; i < content_lines; ++i) {
            print_help_line(content[i], frame_width - 1, frame_width);
        }
    }

    // Нижняя граница
    if (frame_width <= 78) {
        print_chars(196, frame_width);
        putchar(217); // Правая граница
        printf("\n");
    }
}

void show_help_menu() {
    const char* content[] = {
        " ",
        "help - Displays help about all possible commands",
        "clear - Clear the screen.",
        "mm_test - Runs a Memory test program.",
        "calc - Calculator.",
        "logo - Displays logo.",
        "off - Shutdown PC.",
        " "
    };
    int content_lines = sizeof(content) / sizeof(content[0]);
    print_frame("HELP MENU", content, content_lines, 54);
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
        //test_scanf();
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
        terminal_writestring("Unknown command: ");
        terminal_writestring(command);
        terminal_putchar('\n');
    }
    terminal_writestring(PROMPT_STRING);
}

void console_input_loop() {
    char c;
    while (1) {
        c = keyboard_get_char();

        if (c == '\n') {
            if (command_length == 0) {
                terminal_writestring(PROMPT_STRING);
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
        } else {
            if (command_length < COMMAND_BUFFER_SIZE - 1) {
                command_buffer[command_length++] = c;
            }
        }
    }
}
