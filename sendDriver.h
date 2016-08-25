#ifndef SENDDRIVER_H
#define SENDDRIVER_H

#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include<string>

#include <libgen.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include<QString>




class sendDriver
{

public:
    sendDriver(std::string device);
    ~sendDriver();
     QString sendByte(const int& canID, unsigned char canMsg[]);

    unsigned char can_dlc2len(unsigned char can_dlc);
    unsigned char can_len2dlc(unsigned char len);
    int parse_canframe(int canID, unsigned char canMsg[], struct canfd_frame *cf);
    unsigned char asc2nibble(char c);
    int idx2dindex(int ifidx, int socket);


private:

    int sock; /* can raw socket */
    int required_mtu;
    int mtu;
    int enable_canfd = 1;
    struct sockaddr_can addr;
    struct canfd_frame frame;
    struct ifreq ifr;


    const unsigned char dlc2len[17] = {0, 1, 2, 3, 4, 5, 6, 7,
                        8, 12, 16, 20, 24, 32, 48, 64};
    const unsigned char len2dlc[66] = {0, 1, 2, 3, 4, 5, 6, 7, 8,		/* 0 - 8 */
                        9, 9, 9, 9,				/* 9 - 12 */
                        10, 10, 10, 10,				/* 13 - 16 */
                        11, 11, 11, 11,				/* 17 - 20 */
                        12, 12, 12, 12,				/* 21 - 24 */
                        13, 13, 13, 13, 13, 13, 13, 13,		/* 25 - 32 */
                        14, 14, 14, 14, 14, 14, 14, 14,		/* 33 - 40 */
                        14, 14, 14, 14, 14, 14, 14, 14,		/* 41 - 48 */
                        15, 15, 15, 15, 15, 15, 15, 15,		/* 49 - 56 */
                        15, 15, 15, 15, 15, 15, 15, 15};	/* 57 - 64 */


    static const int MAXIFNAMES=30; /* size of receive name index to omit ioctls */

     __u32 dropcnt=0;
     __u32 last_dropcnt=0;
     char devname[MAXIFNAMES][IFNAMSIZ+1];
     int  dindex[MAXIFNAMES];
     int  max_devname_len; /* to prevent frazzled device name output */
    const int canfd_on;

    fd_set rdfs;
    int ss;

      int ret;
     struct sockaddr_can addrDump;
    char ctrlmsg[CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(sizeof(__u32))];
    struct iovec iov;
    struct msghdr msg;
    struct cmsghdr *cmsg;
    struct canfd_frame frameDump;
    int nbytes, i, maxdlen;
    struct ifreq ifrDump;
    struct timeval tv, last_tv;
    struct timeval timeout, timeout_config = { 0, 0 }, *timeout_current = NULL;


};

#endif //
