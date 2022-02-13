#include <stdio.h>

int main() {
    union 
    {
        short value;
        char bytes[sizeof(short)];
    } test;
    
    test.value = 0x0102;
    if (test.bytes[0] == 0x01 && test.bytes[1] == 0x02) {
        printf("大端\n");
    } else if (test.bytes[0] == 0x02 && test.bytes[1] == 0x01) {
        printf("小端\n");
    }

    return 0;
}