#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
// create a TCP Server

static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

// Define die function for error handling
static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d]%s\n", err, msg);
    exit(1);
}

//Read & write
static void do_something(int connfd) {
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
    if (n < 0) {
        msg("read() error");
        return;
    }
    printf("client says: %s\n", rbuf);

    char wbuf[] = "world";
    write(connfd, wbuf, strlen(wbuf));
}


static int32_t read_full(int fd, char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = read(fd, buf, n);
        if (rv <= 0) {
            return -1;  // error, or unexpected EOF
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}

static int32_t write_all(int fd, const char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = write(fd, buf, n);
        if (rv <= 0) {
            return -1;  // error
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}


int main(){
    //Obtain a socket handle

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0 ){
        die("socket()");
    }

    //Set socket option
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    //Bind to an address

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);        // port
    addr.sin_addr.s_addr = htonl(0);    // wildcard IP 0.0.0.0
    int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) { 
        die("bind()"); 
    }

    printf("starting connection!\n");

    // listen
    rv = listen(fd, SOMAXCONN);
    if (rv) { 
        die("listen()"); 
    }

    //Accept connections
    while (true) {
        // accept
        struct sockaddr_in client_addr = {};
        socklen_t addrlen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
        if (connfd < 0) {
            continue;   // error
        }

        // do_something(connfd);  //Read & write

        while(true) {
            int32_t err = one_request(connfd); //The one_request function will read 1 request and write 1 response.
            if (err) {
                break;
            }
        }
        close(connfd);
    }

    return 0;

}

