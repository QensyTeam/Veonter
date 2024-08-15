#include <kernel/mini_programs/mini_programs.h>
#include <kernel/kernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern rgb_color_t fg_color;
extern rgb_color_t bg_color;
extern rgb_color_t main_color;

int total_tests = 0;
int passed_tests = 0;
int failed_tests = 0;
const char* failed_test_names[100];
int failed_test_count = 0;

void bzero_test(){
    int arr[5] = {1, 2, 3, 4, 5};
    size_t arr_size = sizeof(arr);

    // Выводим исходный массив
    printf("Original array:\n");
    for (size_t i = 0; i < arr_size / sizeof(int); ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    // Обнуляем содержимое массива с помощью bzero
    bzero(arr, arr_size);

    // Выводим обнуленный массив
    printf("Array after bzero:\n");
    for (size_t i = 0; i < arr_size / sizeof(int); ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n\n");
}

void test_kheap() {
    main_color = fg_color;
    printf("\tKHEAP TEST\n\n");

    void *a = kmalloc(8);
    void *b = kmalloc(16);
    void *c = kmalloc(32);

    printf("Allocated a: %p\n", a);
    printf("Allocated b: %p\n", b);
    printf("Allocated c: %p\n", c);

    kfree(a);
    kfree(b);
    kfree(c);

    printf("Freed a, b, c: %p, %p, %p\n", a, b, c);

    void *d = kmalloc(64);
    printf("Allocated d: %p\n", d);

    kfree(d);
    printf("Freed d\n");
    shell_text_color(RGB(0, 255, 0));
    printf("kheap test complete.\n\n");
    shell_text_color(main_color);
}


void test_paging() {
    main_color = fg_color;
    printf("\tPAGING TEST\n\n");

    // Создаем директорию страниц
    page_directory_t* pd = paging_create_page_directory();

    // Создаем несколько таблиц страниц
    page_table_t* pt1 = paging_create_page_table();
    page_table_t* pt2 = paging_create_page_table();

    // Задаем виртуальные адреса
    uint32_t virt_addr1 = 0x400000;
    uint32_t virt_addr2 = 0x800000;

    // Задаем физические адреса
    uint32_t phys_addr1 = 0x1000;
    uint32_t phys_addr2 = 0x2000;

    // Отображаем страницы на физические адреса
    paging_map_page(pd, virt_addr1, phys_addr1, false, true); // отображаем первую страницу
    paging_map_page(pd, virt_addr2, phys_addr2, false, true); // отображаем вторую страницу

    // Проверяем, что страницы правильно отображены
    uint32_t* ptr1 = (uint32_t*)virt_addr1;
    uint32_t* ptr2 = (uint32_t*)virt_addr2;

    *ptr1 = 0x1234567;
    *ptr2 = 0x8765432;

    // Печатаем значения, чтобы убедиться, что они записаны в правильные физические адреса
    printf("Value at virtual address 0x%lx: 0x%lx\n", virt_addr1, *ptr1);
    printf("Value at virtual address 0x%lx: 0x%lx\n", virt_addr2, *ptr2);
    shell_text_color(RGB(0, 255, 0));
    printf("paging test complete.\n");
    shell_text_color(main_color);

    // Очищаем директорию страниц (необязательно)
    kfree(pd);
    kfree(pt1);
    kfree(pt2);
}

// Функция для тестирования, выводит результат
void test_result(const char* func_name, int result) {
    main_color = fg_color;
    total_tests++;
    if (result) {
        passed_tests++;
        shell_text_color(RGB(0, 255, 0));
    } else {
        failed_tests++;
        shell_text_color(RGB(255, 0, 0));
        failed_test_names[failed_test_count++] = func_name; // Сохраняем имя проваленного теста
    }
    printf("%s: %s\n", func_name, result ? "PASSED" : "FAILED");
    shell_text_color(main_color);
}

// Тест для memcmp
void test_memcmp() {
    const char* str1 = "hello";
    const char* str2 = "world";
    int res = memcmp(str1, str2, 3); // Сравниваем только первые 3 символа
    printf("Expected result: -1, Actual result: %d\n", res);
    test_result("memcmp", res == -1); // Проверяем, что результат равен -1
}

void test_memcpy() {
    char dest[20];
    const char* src = "hello";
    memcpy(dest, src, strlen(src) + 1); // Копируем строку, включая завершающий нуль-символ
    printf("memcpy result: %s\n", dest);
    test_result("memcpy", strcmp(dest, "hello") == 0); // Сравниваем скопированную строку с оригиналом
}

void test_memset() {
    char str[10];
    memset(str, 'A', 5); // Заполняем первые 5 байт символом 'A'
    str[5] = '\0'; // Добавляем завершающий нуль-символ для корректного сравнения
    printf("memset result: %s\n", str);
    test_result("memset", strcmp(str, "AAAAA") == 0); // Сравниваем результат с ожидаемым
}

void test_strcat() {
    char dest[20] = "hello";
    const char* src = " world";
    strcat(dest, src); // Добавляем src к dest
    printf("strcat result: %s\n", dest);
    test_result("strcat", strcmp(dest, "hello world") == 0); // Сравниваем результат с ожидаемым
}

void test_strchr() {
    const char* str = "hello";
    const char* ptr = strchr(str, 'l'); // Ищем первое вхождение символа 'l'
    printf("strchr result: %p (expected %p)\n", (void*)ptr, (void*)(str + 2));
    test_result("strchr", ptr != NULL && ptr == str + 2); // Проверяем, что результат указывает на вторую 'l'
}

void test_strcpy() {
    char dest[20];
    const char* src = "hello";
    strcpy(dest, src); // Копируем src в dest
    printf("strcpy result: %s\n", dest);
    test_result("strcpy", strcmp(dest, "hello") == 0); // Сравниваем результат с ожидаемым
}

// Тест для strlen
void test_strlen() {
    const char* str = "hello";
    size_t len = strlen(str);
    printf("strlen result: %zu (expected 5)\n", len);
    test_result("strlen", len == 5);
}

// Тест для strncpy
void test_strncpy() {
    char dest[10];
    const char* src = "hello";
    strncpy(dest, src, 5);
    dest[5] = '\0'; // Вручную добавляем завершающий нуль-символ
    printf("strncpy result: %s\n", dest);
    test_result("strncpy", strcmp(dest, "hello") == 0);
}

// Тест для strtod
void test_strtod() {
    const char* str = "123.456";
    char* endptr;
    double val = strtod(str, &endptr);
    printf("strtod result: %f (expected 123.456)\n", val);
    test_result("strtod", val == 123.456 && *endptr == '\0');
}

// Тест для strtof
void test_strtof() {
    const char* str = "123.456";
    char* endptr;
    float val = strtof(str, &endptr);
    printf("strtof result: %f (expected 123.456)\n", val);
    test_result("strtof", val == 123.456f && *endptr == '\0');
}

// Тест для strtol
void test_strtol() {
    const char* str = "123456";
    char* endptr;
    long val = strtol(str, &endptr, 10);
    printf("strtol result: %ld (expected 123456)\n", val);
    test_result("strtol", val == 123456 && *endptr == '\0');
}

// Тест для strtold
void test_strtold() {
    const char* str = "123.456";
    char* endptr;
    long double val = strtold(str, &endptr);
    printf("strtold result: %Lf (expected 123.456)\n", val);
    test_result("strtold", val == 123.456L && *endptr == '\0');
}

// Тест для strtoll
void test_strtoll() {
    const char* str = "123456789012345";
    char* endptr;
    long long val = strtoll(str, &endptr, 10);
    printf("strtoll result: %lld (expected 123456789012345)\n", val);
    test_result("strtoll", val == 123456789012345LL && *endptr == '\0');
}

// Тест для substr
void test_substr() {
    const char* str = "hello world";
    char sub[6]; // Длина подстроки + 1 для нуль-терминатора
    substr(sub, str, 6, 5);
    printf("substr result: %s\n", sub);
    test_result("substr", strcmp(sub, "world") == 0);
}

// Тест для itoa
void test_itoa() {
    char buffer[20];
    itoa(12345, buffer, 10);
    printf("itoa result: %s\n", buffer);
    test_result("itoa", strcmp(buffer, "12345") == 0);
}

// Тесты для ltoa
void test_ltoa() {
    char buffer[64];

    ltoa(123456L, buffer, 10);
    printf("ltoa result: %s\n", buffer);
    test_result("ltoa(123456)", strcmp(buffer, "123456") == 0);

    ltoa(-123456L, buffer, 10);
    printf("ltoa result: %s\n", buffer);
    test_result("ltoa(-123456)", strcmp(buffer, "-123456") == 0);

    lltoa(0x7FFFFFFFFFFFFFFFLL, buffer, 16); // Заменено на lltoa
    printf("lltoa result: %s\n", buffer);
    test_result("lltoa(0x7FFFFFFFFFFFFFFF, 16)", strcmp(buffer, "7fffffffffffffff") == 0);
}

// Тесты для lltoa
void test_lltoa() {
    char buffer[64];

    lltoa(123456789012345LL, buffer, 10);
    printf("lltoa result: %s\n", buffer);
    test_result("lltoa(123456789012345)", strcmp(buffer, "123456789012345") == 0);

    lltoa(-123456789012345LL, buffer, 10);
    printf("lltoa result: %s\n", buffer);
    test_result("lltoa(-123456789012345)", strcmp(buffer, "-123456789012345") == 0);

    lltoa(0x7FFFFFFFFFFFFFFFLL, buffer, 16);
    printf("lltoa result: %s\n", buffer);
    test_result("lltoa(0x7FFFFFFFFFFFFFFF, 16)", strcmp(buffer, "7fffffffffffffff") == 0);
}

// Тесты для ftoa
void test_ftoa() {
    char buffer[64];

    ftoa(123.456, buffer, 3);
    printf("ftoa result: %s\n", buffer);
    test_result("ftoa(123.456, 3)", strcmp(buffer, "123.456") == 0);

    ftoa(-123.456, buffer, 2);
    printf("ftoa result: %s\n", buffer);
    test_result("ftoa(-123.456, 2)", strcmp(buffer, "-123.45") == 0);
}

// Тесты для lftoa
void test_lftoa() {
    char buffer[64];

    lftoa(123.456L, buffer, 3);
    printf("lftoa result: %s\n", buffer);
    test_result("lftoa(123.456L, 3)", strcmp(buffer, "123.456") == 0);

    lftoa(-123.456L, buffer, 2);
    printf("lftoa result: %s\n", buffer);
    test_result("lftoa(-123.456L, 2)", strcmp(buffer, "-123.45") == 0);
}

void mm_test() {
    main_color = fg_color;
    printf("\tMEMORY TEST PROGRAM\n\n");
    bzero_test();
    test_kheap();
    sleep(2000);
    test_paging();
    test_memcmp();
    test_memcpy();
    test_memset();
    test_strcat();
    sleep(2000);
    test_strchr();
    test_strcpy();
    test_strlen();
    test_strncpy();
    sleep(2000);
    test_strtod();
    test_strtof();
    test_strtol();
    test_strtold();
    sleep(2000);
    test_strtoll();
    test_substr();
    test_itoa();
    sleep(2000);
    // Добавляем новые тесты
    test_ltoa();
    test_lltoa();
    test_ftoa();
    test_lftoa();

    // Вывод итоговых результатов
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", failed_tests);

    if (failed_tests > 0) {
        shell_text_color(RGB(255, 0, 0));
        printf("Failed test details:\n");
        for (int i = 0; i < failed_test_count; i++) {
            printf(" - %s\n", failed_test_names[i]);
        }
        printf("[TEST FAILED]\n");
    } else {
        shell_text_color(RGB(0, 255, 0));
        printf("[TEST PASSED]\n");
    }

    shell_text_color(main_color);
}

