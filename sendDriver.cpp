#include "sendDriver.h"

sendDriver::sendDriver(std::string device)
    :
         canfd_on(1),
        ss(0),
          ret(0),
         max_devname_len(0)
{
    if ((sock = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("senddriver init: socket initialization failed");
        exit(1);
    }
    char argv[]=device.c_str();

    strncpy(ifr.ifr_name, argv, IFNAMSIZ - 1);//IFNAMSIZ是预定义的16，是因为ifr.ifr.name的空间就是这么大
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
        ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);//把char[]转换为int
    if (!ifr.ifr_ifindex) {
        perror("senddriver init: if_nametoindex error");
        exit(1);
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;//将转换好的index赋值给sockaddr_can
     setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("senddriver init: bind error");
        exit(1);
    }
}

sendDriver::~sendDriver()
{
    ::close(sock);
}

QString sendDriver::sendByte(const int& canID, unsigned char canMsg[])
{
    /* parse CAN frame */
    required_mtu = parse_canframe(canID,canMsg, &frame);
    if (!required_mtu)
        return("error with required mtu or parse");

    if (required_mtu > CAN_MTU) {
        /* check if the frame fits into the CAN netdevice */
        if (ioctl(sock, SIOCGIFMTU, &ifr) < 0)
            return("SIOCGIFMTU");

        mtu = ifr.ifr_mtu;
        if (mtu != CANFD_MTU)
            return("CAN interface ist not CAN FD capable - sorry");

        /* interface is ok - try to switch the socket into CAN FD mode */
        if (setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FD_FRAMES,
                   &enable_canfd, sizeof(enable_canfd)))
            return("error when enabling CAN FD support");

        /* ensure discrete CAN FD length values 0..8, 12, 16, 20, 24, 32, 64 */
        frame.len = can_dlc2len(can_len2dlc(frame.len));
    }
    if (write(sock, &frame, required_mtu) != required_mtu)
        return("write error");

    return "Driver: no error code";
}

unsigned char sendDriver::can_dlc2len(unsigned char can_dlc)
{
    return dlc2len[can_dlc & 0x0F];
}

unsigned char sendDriver::can_len2dlc(unsigned char len)
{
    if (len > 64)
        return 0xF;

    return len2dlc[len];
}

int sendDriver::parse_canframe(int canID,unsigned char canMsg[], struct canfd_frame *cf) {

    int i, idx, dlen, len;
    idx=4;
    int maxdlen = CAN_MAX_DLEN;
    int ret = CAN_MTU;
    unsigned char tmp;
    memset(cf, 0, sizeof(*cf)); /* init CAN FD frame, e.g. LEN = 0 */

    cf->can_id=canID;
    for(int i=0;i<8;i++)
        cf->data[i] = canMsg[i];

    cf->len =8;// dlen;

    return ret;
}

unsigned char sendDriver:: asc2nibble(char c) {

    if ((c >= '0') && (c <= '9'))
        return c - '0';

    if ((c >= 'A') && (c <= 'F'))
        return c - 'A' + 10;

    if ((c >= 'a') && (c <= 'f'))
        return c - 'a' + 10;

    return 16; /* error */
}

int sendDriver::idx2dindex(int ifidx, int socket) {

    int i;
    struct ifreq ifr;

    for (i=0; i < MAXIFNAMES; i++) {
        if (dindex[i] == ifidx)
            return i;
    }
    /* remove index cache zombies first */
    for (i=0; i < MAXIFNAMES; i++) {
        if (dindex[i]) {
            ifr.ifr_ifindex = dindex[i];
            if (ioctl(socket, SIOCGIFNAME, &ifr) < 0)
                dindex[i] = 0;
        }
    }

    for (i=0; i < MAXIFNAMES; i++)
        if (!dindex[i]) /* free entry */
            break;

    if (i == MAXIFNAMES) {
        fprintf(stderr, "Interface index cache only supports %d interfaces.\n",
               MAXIFNAMES);
        exit(1);
    }

    dindex[i] = ifidx;

    ifr.ifr_ifindex = ifidx;
    if (ioctl(socket, SIOCGIFNAME, &ifr) < 0)
        perror("SIOCGIFNAME");

    if (max_devname_len < strlen(ifr.ifr_name))
        max_devname_len = strlen(ifr.ifr_name);

    strcpy(devname[i], ifr.ifr_name);

#ifdef DEBUG
    printf("new index %d (%s)\n", i, devname[i]);
#endif

    return i;
}




