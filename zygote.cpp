extern "C" {
#include "common.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <sys/stat.h>

#include <fontconfig/fontconfig.h>
}
#include <vector>
#include <string>

void checkExisting(const char *socketPath)
{
    if (access(socketPath, W_OK) != 0) {
        puts("no running");
        return;
    }

    int conn = socket(PF_UNIX, SOCK_STREAM, 0);
    if (conn < 0) {
        puts("socket() failed");
        exit(255);
    }

    struct sockaddr_un server;
    server.sun_family = AF_UNIX;
    strncpy(server.sun_path, socketPath, sizeof(server.sun_path));

    socklen_t socklen = sizeof(server);

    if (connect(conn, (struct sockaddr *)&server, socklen) == 0) {
        puts("Shutting down existing");
        Command request_header;
        request_header.commandId = COMMAND_QUIT;
        request_header.argCount = 0;
        write(conn, &request_header, sizeof(request_header));
        sleep(1); // Give it some time
    }

    unlink(socketPath);

    close(conn);
}

int createSocket(const char *socketPath)
{
    // Create socket
    int wrapper = socket(PF_UNIX, SOCK_STREAM, 0);
    if (wrapper < 0) {
        puts("socket() failed");
        exit(255);
    }

    int options = fcntl(wrapper, F_GETFL);
    if (options == -1) {
        puts("Can not make socket non-blocking");
        close(wrapper);
        exit(255);
    }

    if (fcntl(wrapper, F_SETFL, options | O_NONBLOCK) == -1) {
        puts("Can not make socket non-blocking");
        close(wrapper);
        exit(255);
    }

    int maxTries = 10;
    while (1) {
        // bind it
        struct sockaddr_un sa;
        socklen_t socklen = sizeof(sa);
        memset(&sa, 0, socklen);
        sa.sun_family = AF_UNIX;
        strncpy(sa.sun_path, socketPath, sizeof(sa.sun_path));
        if (bind(wrapper, (struct sockaddr *)&sa, socklen) != 0) {
            if (maxTries == 0) {
                puts("bind() failed");
                fprintf(stderr, "Could not bind to socket '%s'\n", socketPath);
                close(wrapper);
                exit(255);
            }
            maxTries--;
        } else {
            break;
        }
    }

    // set permissions
    if (chmod(socketPath, 0600) != 0) {
        puts("Can not set permissions on socket");
        fprintf(stderr, "Wrong permissions of socket '%s'\n", socketPath);
        unlink(socketPath);
        close(wrapper);
        exit(255);
    }

    if (listen(wrapper, SOMAXCONN) < 0) {
        puts("listen() failed");
        unlink(socketPath);
        close(wrapper);
        exit(255);
    }

    return wrapper;
}

int main(int argc, char *argv[])
{
    char *socketPath = generatePath(".socket");
    printf("%s\n", socketPath);
    checkExisting(socketPath);
    int conn = createSocket();
    free(socketPath);

    std::vector<void*> libraryHandles;

    static const std::vector<std::string> libraries = {
        "Bookmarks",
        "Completion",
        "ConfigCore",
        "ConfigGui",
        "ConfigWidgets",
        "CoreAddons",
        "Crash",
        "DBusAddons",
        "GlobalAccel",
        "GuiAddons",
        "I18n",
        "IconThemes",
        "KIOCore",
        "KIOFileWidgets",
        "KIOGui",
        "KIOWidgets",
        "NewStuff",
        "NewStuffCore",
        "Notifications",
        "NotifyConfig",
        "Parts",
        "Pty",
        "Service",
        "TextWidgets",
        "WidgetsAddons",
        "WindowSystem",
        "XmlGui",
    };

    puts("Preloading...");
    for (const std::string &library : libraries) {
        std::string path = CMAKE_INSTALL_PREFIX "/" LIB_INSTALL_DIR "/libKF5" + library + ".so";
        void *handle = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL);
        if (handle == nullptr) {
            fprintf(stderr, "Failed to load %s\n", path.c_str());
            continue;
        }
        libraryHandles.push_back(handle);
    }
    puts("Loaded!");

    for (void *handle : libraryHandles) {
        dlclose(handle);
    }

    FcInit();

    return 0;
}

