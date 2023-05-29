#include "../server/webserver.h"

int main() {
    WebServer webServer(8080, 3, -1, 8);
    webServer.Start();
    return 0;
}
