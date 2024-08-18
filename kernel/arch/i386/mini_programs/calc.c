#include <kernel/kernel.h>
#include <stdio.h>
#define COMMANDS_BUFFER_SIZE 256

void calculator() {
    printf("\t[CALCULATOR]\n\n");
    double a, b; // Изменили тип переменных на float для поддержки чисел с плавающей запятой
    char op;

    printf("Enter first number: ");
    scanf("%lf", &a); // Используем %f для считывания числа с плавающей запятой

    printf("Enter operator (+, -, *, /, %%): ");
    scanf(" %c", &op); // Обратите внимание на пробел перед %c, чтобы игнорировать пробелы

    printf("Enter second number: ");
    scanf("%lf", &b); // Используем %f для считывания числа с плавающей запятой

    double result; // Изменили тип результата на float
    switch (op) {
        case '+':
            result = a + b;
            printf("Result: %f + %f = %f\n", a, b, result); // Выводим результат с двумя знаками после запятой
            break;
        case '-':
            result = a - b;
            printf("Result: %f - %f = %f\n", a, b, result);
            break;
        case '*':
            result = a * b;
            printf("Result: %f * %f = %f\n", a, b, result);
            break;
        case '/':
            if (b != 0) {
                result = a / b;
                printf("Result: %f / %f = %f\n", a, b, result);
            } else {
                printf("Error: Division by zero!\n");
            }
            break;
        case '%':
            result = a * (b / 100.0); // Рассчитываем процент от числа a
            printf("Result: %f%% of %f = %f\n", b, a, result); // Выводим результат с двумя знаками после запятой
            break;
        default:
            printf("Unknown operator: %c\n", op);
            break;
    }

}


void test_scanf() {
    char char_val;
    short short_val;
    int int_val;
    long long_val;
    long long long_long_val;
    float float_val;
    double double_val;
    long double long_double_val;
    char str_val[COMMANDS_BUFFER_SIZE];

    printf("Enter a character: ");
    scanf("%c", &char_val);
    printf("You entered the character: %c\n", char_val);

    printf("Enter a short: ");
    scanf("%hd", &short_val);
    printf("You entered the short: %hd\n", short_val);

    printf("Enter an int: ");
    scanf("%d", &int_val);
    printf("You entered the int: %d\n", int_val);

    printf("Enter a long: ");
    scanf("%ld", &long_val);
    printf("You entered the long: %ld\n", long_val);

    printf("Enter a long long: ");
    scanf("%lld", &long_long_val);
    printf("You entered the long long: %lld\n", long_long_val);

    printf("Enter a float: ");
    scanf("%f", &float_val);
    printf("You entered the float: %f\n", float_val);

    printf("Enter a double: ");
    scanf("%lf", &double_val);
    printf("You entered the double: %lf\n", double_val);

    printf("Enter a long double: ");
    scanf("%Lf", &long_double_val);
    printf("You entered the long double: %Lf\n", long_double_val);

    printf("Enter a string: ");
    scanf("%s", str_val);
    printf("You entered the string: %s\n", str_val);
}
