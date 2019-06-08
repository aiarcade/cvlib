
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "detector.h"
#include "object.h"


int
main(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <width> <height> <image.y>\n", argv[0]);
        return 1;
    }

    int width, height;
    width = atoi(argv[1]);
    height = atoi(argv[2]);

    if ((width <= 0) || (height <= 0)) {
        fprintf(stderr, "width/height invalid\n");
        return 1;
    }

    Detector *det;
    det = new Detector(width, height, 24,"lbpcascade_frontalface.xml");
    if (!det) {
        fprintf(stderr, "init face detector error\n");
        return 1;
    }

    unsigned char *y;
    ssize_t s;
    int fd;

    y = (unsigned char *)malloc(width * height);
    fd = open(argv[3], O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Cannot open file: %s\n", argv[3]);
        return 1;
    }

    s = read(fd, y, width * height);
    if (s != width * height) {
        fprintf(stderr, "Error reading file: %s\n", argv[3]);
        return 1;
    }

    Object f[30];
    int num_faces = 30;
    int i;

    fprintf(stderr, "Okay");
    det->detect( y, f, &num_faces);

    for (i = 0 ;i < num_faces; i++) {
        printf("Face: %d %d %d %d\n", f[i].x, f[i].y, f[i].width, f[i].height);
    }

    printf("Total: %d\n", num_faces);


}
