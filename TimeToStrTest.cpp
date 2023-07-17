#include <time.h>
#include <iostream>

int main()
{
    char outstr[200];
    time_t t;
    struct tm *tmp;
    const char* fmt = "%a, %d %b %y %T %z";

    t = time(NULL);
    tmp = gmtime(&t);
    if (tmp == NULL) {
        perror("gmtime error");
        exit(EXIT_FAILURE);
    }

    if (strftime(outstr, sizeof(outstr), fmt, tmp) == 0) { 
        fprintf(stderr, "strftime returned 0");
        exit(EXIT_FAILURE);
    } 
    printf("%s\n", outstr);
    exit(EXIT_SUCCESS); 
}