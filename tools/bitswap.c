#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

uint8_t bitswap8(uint8_t v)
{
	return ((v * 0x0802LU & 0x22110LU) |
			(v * 0x8020LU & 0x88440LU)) *
			   0x10101LU >>
		   16;
}

int main(int argc, char **argv)
{
    unsigned char block[256];
    unsigned int csum;

    int fd_i, bc;
    FILE *fd_o;
    struct stat fileinfo;

    unsigned int i, j;

    if (argc != 4) {
        printf("usage: %s width binfile hexfile\n", argv[0]);
        return -1;
    }

    bc = atoi(argv[1]);
    if ((bc < 1) || (bc > 255)) {
        printf("byte count per line must be 1-255");
        return -1;
    }

    if ((fd_i = open(argv[2], O_RDONLY)) == -1 || fstat(fd_i, &fileinfo) == -1) {
        printf("Couldn't open file\n");
        return -1;
    }

    printf("size: %lu bytes\n", fileinfo.st_size);

    if ((fd_o = fopen(argv[3], "w")) == NULL) {
        printf("invalid outfile\n");
        return -1;
    }

    for (i=0; i<fileinfo.st_size; i += bc) {
        read(fd_i, (void*)block, bc);
        
        for (j=0; j<bc; j++)
			block[j] = bitswap8(block[j]);

		fwrite((void *)block, 1, bc, fd_o);
	}

    fclose(fd_o);
    close(fd_i);

    return 0;
}
