#include "common.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdint.h>


static int openSocket(const char *sock_file)
{
    struct sockaddr_un server;
    if (strlen(sock_file) >= sizeof(server.sun_path)) {
        fprintf(stderr, "Warning: Path of socketfile exceeds UNIX_PATH_MAX.\n");
        return -1;
    }

    /*
     * create the socket stream
     */
    int s = socket(PF_UNIX, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Warning: socket() failed: ");
        return -1;
    }

    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, sock_file);
    socklen_t socklen = sizeof(server);
    if (connect(s, (struct sockaddr *)&server, socklen) == -1) {
        fprintf(stderr, "kdeinit5_wrapper: Warning: connect(%s) failed:", sock_file);
        perror(" ");
        close(s);
        return -1;
    }
    return s;
}

int main(int argc, char *argv[])
{
    char *socketPath = generatePath(".socket");
    printf("%s\n", socketPath);
    //checkExisting(socketPath);
    int conn = openSocket(socketPath);
    free(socketPath);

    if (conn < 0) {
        puts("Failed to connect to socket");
        return -1;
    }

    struct Command command;
    command.commandId = COMMAND_LAUNCH;
    command.argCount = argc - 1;

    write(conn, &command, sizeof(command));

    for (int i=1; i<argc; i++) {
        const uint8_t stringLength = strlen(argv[i]);
        assert(stringLength == strlen(argv[i]));

        write(conn, &stringLength, 1);
        write(conn, argv[i], stringLength);
    }

    close(conn);

    return 0;
}
