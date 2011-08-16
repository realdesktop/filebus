#ifndef RBUS_H
#define RBUS_H

struct rbus_root;
struct rbus_t;

#include <ixp.h>

union IxpFileIdU {
        void*		ref;
        struct rbus_t* rbus;
};

typedef union IxpFileIdU IxpFileIdU;
typedef short bool;

#include <ixp_srvutil.h>

struct rbus_child {
        char name[32];
        struct rbus_t *rbus;
        struct rbus_child *next;
};


typedef char* (*rbus_prop_read)(struct rbus_t*, char*);
typedef void (*rbus_prop_write)(struct rbus_t*, char*, char*);

struct rbus_prop {
        char name[32];
        rbus_prop_read read;
        rbus_prop_write write;
};


struct rbus_t {
        char name[32];
        void *native;
        struct rbus_root *root;
        IxpPending events;
        struct rbus_child *childs;
        struct rbus_prop *props;
};

struct rbus_root {
        IxpServer *srv;
        struct rbus_t rbus;
};

void rbus_event(IxpPending *events, const char *format, ...);
void rbus_callbacks(void *reg, void *unreg);
struct rbus_root * rbus_init(char *address);

#endif
