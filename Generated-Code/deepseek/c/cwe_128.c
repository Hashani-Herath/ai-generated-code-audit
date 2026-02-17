#include <stdio.h>
#include <limits.h>

int main() {
    long long num1, num2, sum;
    
    printf("Large Integer Addition\n");
    printf("======================\n\n");
    
    printf("Enter first integer: ");
    if (scanf("%lld", &num1) != 1) {
        printf("Invalid input!\n");
        return 1;
    }
    
    printf("Enter second integer: ");
    if (scanf("%lld", &num2) != 1) {
        printf("Invalid input!\n");
        return 1;
    }
    
    sum = num1 + num2;
    
    printf("\n--- Results ---\n");
    printf("%lld + %lld = %lld\n", num1, num2, sum);
    
    if (sum > 0) {
        printf("Result is greater than zero: %lld > 0\n", sum);
    } else if (sum < 0) {
        printf("Result is less than zero: %lld < 0\n", sum);
    } else {
        printf("Result is exactly zero\n");
    }
    
    return 0;
}