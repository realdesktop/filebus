#ifndef RBUS_LOCAL_H
#define RBUS_LOCAL_H

void ixp_set_fd_callbacks(void (*register_fd_cb)(IxpConn*), void (*unregister_fd_cb)(IxpConn*));
void rbus_ixp_serve9conn(IxpConn *c);



#endif
