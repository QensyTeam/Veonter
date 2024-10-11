#include <stdio.h>
#include <kernel/drv/keyboard_buffer.h>
#include <string.h>
#include <math.h> // Для работы со степенями

void calculator(void) {
    char operator;
    double num1, num2;
    double result;

    printf("\t1. Basic operations: 3 + 4\n");
    printf("\t2. Exponentiation: 2 ^ 3\n\n");
    printf("Enter an expression: ");
    // Read the input expression
    int items = scanf("%lf %c %lf", &num1, &operator, &num2);
    
    // Check if the input was successfully read
    if (items != 3) {
        printf("\nInvalid input. Please enter in the format: number operator number\n");
        return;// 1;
    }

    // Выполнение вычисления на основе оператора
    switch (operator) {
        case '+':
            result = num1 + num2;
            break;
        case '-':
            result = num1 - num2;
            break;
        case '*':
            result = num1 * num2;
            break;
        case '/':
            if (num2 != 0) {
                result = num1 / num2;
            } else {
                printf("Error: Division by zero.\n");
                return; // 1;
            }
            break;
        case '^': // Добавляем поддержку степени
            result = pow(num1, num2);
            break;
        default:
            printf("Invalid operator. Please use +, -, *, /, or ^.\n");
            return; // 1;
    }

    // Печать результата
    printf("Result: %.2f\n", result);
}
