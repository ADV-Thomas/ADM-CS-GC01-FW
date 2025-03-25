/**************************************************************************************************
 * 
 * \file file.c
 * 
 * \brief File driver implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/drv/file.h"

#include "inc/net/file.h"

#include <stdio.h>
#include <stddef.h>

/**************************************************************************************************
 * 
 * Private structure definition
 * 
 *************************************************************************************************/
NET_PRIV_STRUCT(file,
    FILE *fp;
);

/**************************************************************************************************
 * 
 * file_new()
 * 
 *************************************************************************************************/
const struct net *
file_new(void)
{
    NET_PRIV_NEW(file, NET_FILE,
        .fp = NULL
    );

    NET_OBJ_NEW(file, NULL);

    /**
     * Although file does not connect to I/O pins, the \e connect flag must be set to \e true.
     * Otherwise, the \e file_open() function will fail.
     */
    priv.connect = true;

    return &net;
}

/**************************************************************************************************
 * 
 * Net layer implementation. Private functions linked to \b net object.
 * 
 *************************************************************************************************/

/**************************************************************************************************
 * 
 * file__open()
 * 
 *************************************************************************************************/
static int
file__open(const struct net *net, const struct file_usr *usr)
{
    struct file_priv *priv = net->priv;
    
    /* Open binary file in read and write mode */
    priv->fp = fopen(usr->path, "rb+");

    if (!priv->fp) {
        return -1;
    }

    priv->open = true;
    
    return 0;
}

/**************************************************************************************************
 * 
 * file__close()
 * 
 *************************************************************************************************/
static int
file__close(const struct net *net)
{
    struct file_priv *priv = net->priv;

    priv->open = false;

    if (priv->fp) {
        fclose(priv->fp);
    }

    priv->fp = NULL;

    return 0;
}

/**************************************************************************************************
 * 
 * file__read()
 * 
 *************************************************************************************************/
static int
file__read(const struct net *net, struct file_f *f, unsigned length)
{
    struct file_priv *priv = net->priv;
    
    fseek(priv->fp, f->address, SEEK_SET);

    fread(f->data, f->length, 1U, priv->fp);

    return 1;
}

/**************************************************************************************************
 * 
 * file__write()
 * 
 *************************************************************************************************/
static int
file__write(const struct net *net, struct file_f *f, unsigned length)
{
    struct file_priv *priv = net->priv;
    
    fseek(priv->fp, f->address, SEEK_SET);

    fwrite(f->data, f->length, 1U, priv->fp);

    return 1;
}
