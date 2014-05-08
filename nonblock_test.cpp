#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int res;
    res = fcntl(0, F_GETFL);
    if (-1 == res)
    {
        perror("fcntl error!");
        exit(1);
    }
#ifdef NONBLOCK
    res |= O_NONBLOCK;
    if (fcntl(0, F_SETFL, res) == -1)
    {
        perror("error");
        exit(1);
    }
#endif
    char buf[100];
    int nread;
    nread = read(0, buf, 100);
    if (-1 == nread)
    {
        perror("read error");
        exit(1);
    }
    else
    {
        printf("read %d characters\n", nread);
    }
    return 0;
    return 0;
}
