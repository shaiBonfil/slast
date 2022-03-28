#include <stdio.h>
#include <utmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


#define VALID_NUM_OF_ARGS 2

void show_info(struct utmp *utbufp) {

    long my_time;

    printf("%s\t ", utbufp->ut_user);
    if ((utbufp->ut_type) == BOOT_TIME) {
        printf("system boot \t ");
        printf("%s\t", utbufp->ut_host);
    }
    else {
        printf("%s\t\t", utbufp->ut_line);
        printf(" %s\t\t\t", utbufp->ut_host);
    }    
    my_time = utbufp->ut_tv.tv_sec;
    printf("%.20s\t", ctime(&my_time));
    printf("\n");
}

int main(int argc, char *argv[]) {

    int numOfElemToShow = atoi(argv[1]);
    if (argc != VALID_NUM_OF_ARGS || numOfElemToShow <= 0) {
        printf("invalid argument: run the command in the following format: ./slast n (n is integer number greater than zero)\n");
        exit(1);
    }

    struct utmp current_record;         // read info here
    int utmpfd;                         // read from this descriptor
    int reclen = sizeof(struct utmp);
    off_t offset;

    if ((utmpfd = open(WTMP_FILE, O_RDONLY)) == -1) {
        perror(WTMP_FILE);
        exit(1);
    }

    offset = lseek(utmpfd, -reclen, SEEK_END);

    while (read(utmpfd, &current_record, reclen) == reclen) {
        if (current_record.ut_type != DEAD_PROCESS && current_record.ut_type != RUN_LVL) {
            show_info(&current_record);
            numOfElemToShow--;
        }

        offset = lseek(utmpfd, -2*reclen, SEEK_CUR);

        if (numOfElemToShow == 0 || offset < 0) {
            break;
        }
    }

    close(utmpfd);

    return 0;
}