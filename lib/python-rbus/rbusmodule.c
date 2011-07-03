#include <Python.h>
#include "structmember.h"

#include "../librbus/rbus.h"


typedef struct {
    PyObject_HEAD
    PyObject *address;
    struct rbus_root *native;
    int number;
} RbusRoot;

static void
RbusRoot_dealloc(RbusRoot* self)
{
    Py_XDECREF(self->address);
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
RbusRoot_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    RbusRoot *self;

    self = (RbusRoot *)type->tp_alloc(type, 0);
    if (self == NULL)
        goto out;

    self->address = PyString_FromString("");
    if(self->address == NULL)
        goto dec;

out:
    return (PyObject *)self;

dec:
    Py_DECREF(self);
    goto out;
}

static int
RbusRoot_init(RbusRoot *self, PyObject *args, PyObject *kwds)
{
    PyObject *address=NULL, *tmp;

    static char *kwlist[] = {"address", NULL};
    char *str_adrress;

    if (! PyArg_ParseTupleAndKeywords(args, kwds, "|S", kwlist, 
                                      &address))
        return -1; 

    if(!address) {
        PyErr_SetString(PyExc_ValueError, "require address");
        return -1;
    }

    tmp = self->address;
    Py_INCREF(address);
    self->address = address;
    Py_DECREF(tmp);

    /*
     * FIXME: check if here is proper dial string
     * */
    str_adrress = PyString_AsString(address);

    self->native = rbus_init(str_adrress);

    return 0;
}

static PyMemberDef RbusRoot_members[] = {
    {NULL}  /* Sentinel */
};

static PyObject *
RbusRoot_getaddress(RbusRoot *self, void *closure)
{
    Py_INCREF(self->address);
    return self->address;
}

static int
RbusRoot_setaddress(RbusRoot *self, PyObject *value, void *closure)
{
    
  PyErr_SetString(PyExc_ValueError, "immm");
  return -1;

}

static PyObject *
RbusRoot_getchildren(RbusRoot *self, void *closure) {
    struct rbus_child *child = self->native->rbus.childs;

    while (child) {
        printf("children: %s %x, next %x\n", child->name,
                child->rbus, child->next);
        child = child->next;
    }


    Py_RETURN_NONE;

}

static int
RbusRoot_setchildren(RbusRoot *self, PyObject *value, void *closure) {
    Py_ssize_t i, size;
    PyObject *item;
    size = PyList_Size(value);

    struct rbus_child *child, *prev = NULL;
    struct rbus_root *rbus = self->native;

    child = calloc(size+1, sizeof(struct rbus_child));
    rbus->rbus.childs = child;

    /* *
     *      TODO: drop old list if any
     * */

    for(i=0; i<size; i++) {
        item = PyList_GetItem(value, i);

        if(! PyString_CheckExact(item)) {
            continue; //throw exc?
        }

        strcpy(child->name, PyString_AsString(item));

        if(prev)
            prev->next = child;

        prev = child;

        child++;

    }


    return 0;
}


static PyGetSetDef RbusRoot_getseters[] = {
    {"address", 
     (getter)RbusRoot_getaddress, (setter)RbusRoot_setaddress,
     "dial string",
     NULL},

    {"children_types",
     (getter)RbusRoot_getchildren, (setter)RbusRoot_setchildren,
     "child object types",
     NULL},

    {NULL}  /* Sentinel */
};

static PyObject *
RbusRoot_run(RbusRoot* self) {
    fd_set qfds;
    IxpConn *c;
    struct rbus_root *rbus = self->native;
    struct timeval tv;

    tv.tv_sec = 1; //FIXME
    tv.tv_usec = 0;


reset:

    for(c = rbus->srv->conn; c; c = c->next) {
        if(c->read) {
            FD_SET(c->fd, &qfds);
        }
    }

    if (select(FD_SETSIZE, &qfds, NULL, NULL, &tv) < 0) {
            if (errno == EINTR)
                goto reset;

            goto out;
    }

    for(c = rbus->srv->conn; c; c = c->next) {
        if(c->read && FD_ISSET(c->fd, &qfds)) {
            c->read(c);
        }
    }

out:

    Py_RETURN_NONE;
};

static PyObject *
RbusRoot_put_event(RbusRoot* self, PyObject *event) {
    char * str_event;
    struct rbus_root *rbus = self->native;

    str_event = PyString_AsString(event);

    rbus_event(&rbus->rbus.events, "%s\n", str_event);

    Py_RETURN_NONE;

}

static PyObject *
RbusRoot_append_child(RbusRoot* self, PyObject *pychild) {
    PyObject *type = PyObject_GenericGetAttr(pychild, PyString_FromString("RBUS_TYPE"));
    PyObject *name = PyObject_GenericGetAttr(pychild, PyString_FromString("__name__"));

    struct rbus_t *rbus = &self->native->rbus;
    struct rbus_child *child;

    if(name == NULL)
        goto out;

    if(type == NULL)
        goto out;

    char *strname = PyString_AsString(name);
    char *strtype = PyString_AsString(type);

    if(! rbus->childs )
        goto out;

    for(child = rbus->childs; child; child = child->next) {
        if(! child->name)
            continue;

        if(!strcmp(child->name, strtype))

            goto found;
    }

out:
    Py_RETURN_NONE;

found:

    while (child->next)
        child = child->next;

    child->next = calloc(1, sizeof(struct rbus_child));

    struct rbus_t *priv = calloc(1, sizeof(struct rbus_t));

    child = child->next;
    strcpy(child->name, strtype);
    strcpy(priv->name, strname);
    child->rbus = priv;
    child->rbus->native = pychild;

    goto out;

}

static PyMethodDef RbusRoot_methods[] = {
    {"run", (PyCFunction)RbusRoot_run, METH_NOARGS,
        "Loop handling 9P proto"},
    {"put_event", (PyCFunction)RbusRoot_put_event, METH_O,
        "Put event in root event bus"},
    {"append_child", (PyCFunction)RbusRoot_append_child, METH_O,
        "Add child object"},
    {NULL}  /* Sentinel */
};

static PyTypeObject RbusRootType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "rbus.RbusRoot",             /*tp_name*/
    sizeof(RbusRoot),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)RbusRoot_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "RbusRoot objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    RbusRoot_methods,             /* tp_methods */
    RbusRoot_members,             /* tp_members */
    RbusRoot_getseters,           /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)RbusRoot_init,      /* tp_init */
    0,                         /* tp_alloc */
    RbusRoot_new,                 /* tp_new */
};

static PyMethodDef module_methods[] = {
    {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initrbus(void) 
{
    PyObject* m;

    if (PyType_Ready(&RbusRootType) < 0)
        return;

    m = Py_InitModule3("rbus", module_methods,
                       "Example module that creates an extension type.");

    if (m == NULL)
      return;

    Py_INCREF(&RbusRootType);
    PyModule_AddObject(m, "RbusRoot", (PyObject *)&RbusRootType);
}
