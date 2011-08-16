#include <unistd.h>
#include <stdlib.h>
#include <strings.h>

#include "rbus.h"
#include "rbus_local.h"

extern Ixp9Srv p9srv;
extern struct rbus_root* RbusRoot;

struct rbus_root * rbus_init(char *address) {
    struct rbus_root *priv;
    int fd;

    priv = malloc(sizeof(struct rbus_root));
    if (priv == NULL)
            return NULL;


    // accept
    IxpServer *srv = malloc(sizeof(IxpServer));
    if (srv == NULL)
            return NULL;

    bzero(priv, sizeof(struct rbus_root));
    bzero(srv, sizeof(IxpServer));


    fd = ixp_announce(address);
    if(fd < 0) {
            err(1, "ixp_announce");
    }

    ixp_listen(srv, fd, &p9srv, rbus_ixp_serve9conn, NULL);

    priv->srv = srv;
    priv->rbus.root = priv;

    RbusRoot = priv;

    return priv;

};

void rbus_callbacks(void *reg, void *unreg) {
    ixp_set_fd_callbacks(reg, unreg);
}
