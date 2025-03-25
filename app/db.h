

#ifndef _APP_DB_H
#define _APP_DB_H

#include <stdbool.h>

struct db_priv;
struct can_f;

bool handle_db_exceptions(const struct db_priv *db_priv, const struct can_f *f);


#endif
