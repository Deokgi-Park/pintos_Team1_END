#include <stdio.h>
int main(){
    char test[7]  = "123456";
    printf("size : %d \n", sizeof(test)*5);
    printf("%s \n", test);
    printf("%x \n", test);
    printf("%p \n", test);
    char test2[5]  = "ABCD";
    printf("size : %d \n", sizeof(test2)*8);
    printf("%s \n", test2);
    printf("%x \n", test2);
    printf("%p \n", test2);
    memcpy(test, test2, 8);
    printf("%p : %p \n", test, test2);
    printf("%p \n", test - test2);
    printf("%s \n", test);
    printf("%s \n", test2);

    printf("%d \n", 0x47480000);
    printf("%p \n", 0x47480000);
    printf("%d \n", 0xc0000000);
    printf("%p \n", 0xc0000000);
}