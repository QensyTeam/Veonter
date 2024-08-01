#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <kernel/kernel.h>

void Window(const char* name, int x, int y, int x1, int y1)
{
    int xt = x / 8;
    int yt = y / 16;

    draw_Rectangle(x, y, x1, y1, RGB(195, 195, 195)); // Главное окно
    draw_Rectangle(x + 2, y + 2, x1 - 4, 25, RGB(123, 62, 173)); // Заголовочный отдел
    fb_puts(name, xt + 1, yt + 1, RGB(255, 255, 255), RGB(123, 62, 173)); // Заголовочный текст
    draw_Rectangle(x + 2, y + 29, x1 - 4, y1 - 31, RGB(127, 127, 127)); // Содержание

    // Кнопка закрыть окно
    draw_circle(x + x1 - 15, y + 14, 8, RGB(255, 0, 0));
    // Кнопка раскрыть окно
    draw_circle(x + x1 - 37, y + 14, 8, RGB(129, 129, 129));

}






