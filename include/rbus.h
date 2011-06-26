#ifndef RBUS_H
#define RBUS_H

struct rbus_root;
struct rbus_t;

#include <ixp.h>

typedef union IxpFileIdU IxpFileIdU;
typedef struct IxpPending	IxpPending;
typedef short bool;

union IxpFileIdU {
	void*		ref;
        struct rbus_t* rbus;
};



#include <ixp_srvutil.h>


typedef struct IxpServer IxpServer;


struct rbus_child {
        char name[32];
        struct rbus_t *rbus;
        struct rbus_child *next;
};


typedef char* (*rbus_prop_read)(struct rbus_t*, char*);

struct rbus_prop {
        char name[32];
        rbus_prop_read read;
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
