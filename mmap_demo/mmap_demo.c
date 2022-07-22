#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>

// https://beej.us/guide/bgipc/html/multi/mmap.html
int main() {
    int fd, pagesize;
    char *data;

    fd = open("mmap_demo.txt", O_RDONLY);
    pagesize = getpagesize();
    data = mmap((caddr_t)0, pagesize, PROT_READ, MAP_PRIVATE, fd, 0);
    /* printf("Data[0]: %c", data[0]); */
    putchar(data[0]);
}
