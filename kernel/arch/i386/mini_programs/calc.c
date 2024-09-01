#include <stdio.h>

void calculator(void) {
    char operator;
    double num1, num2;
    double result;

    printf("Enter an expression (e.g., 3 + 4): ");
    
    // Read the input expression
    int items = scanf("%lf %c %lf", &num1, &operator, &num2);
    
    // Check if the input was successfully read
    if (items != 3) {
        printf("Invalid input. Please enter in the format: number operator number\n");
        return 1;
    }

    // Perform the calculation based on the operator
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
                return 1;
            }
            break;
        default:
            printf("Invalid operator. Please use +, -, *, or /.\n");
            return 1;
    }

    // Print the result
    printf("Result: %.2f\n", result);
}
