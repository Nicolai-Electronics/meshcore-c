#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s port baudrate\r\n", argv[0]);
        return 1;
    }
    printf("Meshcore compantion radio protocol client\r\n");

    char* port     = argv[1];
    int   baudrate = atoi(argv[2]);

    printf("Accessing server on port %s @ %d baud\r\n", port, baudrate);
}
