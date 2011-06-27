#include <unistd.h>

#include "rbus.h"
#include "rbus_local.h"

extern Ixp9Srv p9srv;
extern struct rbus_root* RbusRoot;

struct rbus_root * rbus_init(char *address) {
    struct rbus_root *priv;

    priv = malloc(sizeof(*priv));
    if (priv == NULL)
            return NULL;

    // accept
    IxpServer *srv = malloc(sizeof(IxpServer));
    int fd;

    fd = ixp_announce(address);
    if(fd < 0) {
            err(1, "ixp_announce");
    }

    ixp_listen(srv, fd, &p9srv, rbus_ixp_serve9conn, NULL);

    priv->srv = srv;

    RbusRoot = priv;

    return priv;

};

void rbus_callbacks(void *reg, void *unreg) {
    ixp_set_fd_callbacks(reg, unreg);
}
