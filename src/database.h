#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <sqlite3.h>

struct sqlite3_list_entry {
	sqlite3 *db;
	char *dbname; 	/* used for lookups */
};

//TODO do this better
int call_js_flag;	/* need to check this flag before calling PDL_JSCall - this prevents a race condition */

struct list db_list;

const char *execute_sql(const char *database_name, const char *sql, PDL_JSParameters *sql_params);

int open_database(const char *database_name);

int close_database(const char *database_name);

struct sqlite3_list_entry *lookup_by_filename(const char *dbname);

#endif /* _DATABASE_H_ */
