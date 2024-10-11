#ifndef CONSOLE_H
#define CONSOLE_H
#define COMMAND_BUFFER_SIZE 256
#define PROMPT_STRING "csl> "
#define PROMPT_LENGTH 5  // Длина строки приглашения с учётом '\0'

void console_initialize();
void console_input_loop();
void console_clear();
void console_process_command(const char* command);

#endif // CONSOLE_H
