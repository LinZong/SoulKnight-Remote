#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <syslog.h>
#include <stdlib.h>
int main(int argc, char **argv)
{
    //define some flag variables.
    struct sockaddr_in saSRC, saRCV,saDST0,saDST1,saDST2;
    struct sockaddr_in saDSTArr[3] = {saDST0,saDST1,saDST2};
    int sSRC, sDST[3];
    char cBuf0[1 << 16];
    char cBuf1[1 << 16];
    char cBuf2[1 << 16];
    char *cBufArr[3] = {cBuf0, cBuf1, cBuf2};

    struct hostent *ph0, *ph1, *ph2;
    struct hostent *phArr[3] = {ph0, ph1, ph2};

    int PortNumber = 0;
    int PlayerNumber = 0;
    int nLen;
    int bDebug = 0;
    int on = 1;

    pid_t pid;
    int nAS;
    unsigned long aRCV, aDST[3];
    char *pszApp = *argv++;
    int failcnt = 0;

    printf("Welcome to Soul Knight remote multiplayer host UDP relay helper by Nemesiss.Lin.\n");

    if (argc > 1 && !strcmp(*argv, "-d"))
    {
        printf("Debug mode.\n");
        bDebug = 1;
        argc--;
        argv++;
    }

    if (argc < 4 || argc > 6)
    {
        printf("Usage: %s [-d] num-of-player port nth-player-addr\n", pszApp);
        exit(1);
    }

    // get player num.
    PlayerNumber = atoi(argv[0]);
    printf("Will allow %d players, and you will be the host.\n", PlayerNumber);
    // get port num.
    PortNumber = atoi(argv[1]);
    if(PlayerNumber > 3)
    {
        printf("No allow so many players\n");
        exit(1);
    }
    printf("Port Number : %d\n",PortNumber);
    //create resources for players.

    for (size_t i = 0; i < PlayerNumber; i++)
    {
        printf("Add player %s\n", argv[i + 2]);
        phArr[i] = gethostbyname(argv[i + 2]);
        if (phArr[i] == NULL)
        {
            printf("Invalid address\n");
            exit(1);
        }
        saDSTArr[i].sin_family = AF_INET;
        saDSTArr[i].sin_port = htons(atoi(argv[1]));
        saDSTArr[i].sin_addr.s_addr = *((unsigned long *)phArr[i]->h_addr);
    }

    printf("Setting host info...\n");
    saSRC.sin_family = AF_INET;
    saSRC.sin_port = saDSTArr[0].sin_port;
    saSRC.sin_addr.s_addr = 0;

    sSRC = socket(AF_INET, SOCK_DGRAM, 0);

    for (size_t i = 0; i < PlayerNumber; i++)
    {
        sDST[i] = socket(AF_INET, SOCK_DGRAM, 0);
    }

    if (bind(sSRC, (struct sockaddr *)&saSRC, sizeof(saSRC)))
    {
        printf("Unable to bind to socket\n");
        exit(1);
    }
    for (size_t i = 0; i < PlayerNumber; i++)
    {
        setsockopt(sDST[i], SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR | SO_REUSEPORT, &(on), sizeof(on));
    }

    if (!bDebug)
    {
        close(0);
        close(1);
        close(2);
        pid = fork();
        if (pid < 0)
            syslog(LOG_ERR, "Could not go into background.");
        if (pid > 0)
            exit(0);
    }
    for (size_t i = 0; i < PlayerNumber; i++)
    {
        aDST[i] = htonl(saDSTArr[i].sin_addr.s_addr);
    }

    //enter loop
    while (1)
    {
        nAS = sizeof(saRCV);
        for (size_t k = 0; k < PlayerNumber; k++)
        {
            nLen = recvfrom(sSRC, cBufArr[k], sizeof(cBuf0), 0, (struct sockaddr *)&saRCV, &nAS);
            aRCV = htonl(saRCV.sin_addr.s_addr);
            for (size_t i = 0; i < 32; i++)
            {
                if (!(aDST[k] & (1 << i)))
                    break;
                aRCV |= (1 << i);
            }

            if (nLen > 0 && aRCV != aDST[k])
            {
                if (bDebug)
                    printf("Player %d -- Relaying a packet of length %d from %d.%d.%d.%d.\n",
                           k,
                           nLen,
                           ((unsigned char *)&saRCV.sin_addr.s_addr)[0],
                           ((unsigned char *)&saRCV.sin_addr.s_addr)[1],
                           ((unsigned char *)&saRCV.sin_addr.s_addr)[2],
                           ((unsigned char *)&saRCV.sin_addr.s_addr)[3]);
                sendto(sDST[k], cBufArr[k], nLen, 0, (struct sockaddr *)&saDSTArr[k], sizeof(saDST0));
            }
        }
    }

    return 0;
}