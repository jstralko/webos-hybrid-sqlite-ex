#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <PDL_types.h>
#include "list.h"
#include "database.h"
#include "string_ops.h"

#define ARRAY_LEN(array) sizeof(array) / sizeof(array[0])

/* private functions */
int remove_database_from_list(struct sqlite3_list_entry *sqe);

const char *execute_sql(const char *database_name, const char *sql, PDL_JSParameters *sql_params) {
	struct sqlite3_list_entry *entry;
	entry = lookup_by_filename(database_name);
	if (entry == NULL) {
		syslog(LOG_ERR, "[PDK] dbname %s isn't open!", database_name);
		return "";
	}

	/*
	 * TODO make list 
	*/
	sqlite3_stmt *index_list_statement = NULL;
	if (index_list_statement == NULL) {
		if (sqlite3_prepare_v2(entry->db, sql, -1, &index_list_statement, NULL) != SQLITE_OK) {
			syslog(LOG_ERR, "[PDK] Failed to prepare statement with message: %s", sqlite3_errmsg(entry->db));
			return "";
		}
	}
	
	int num =  PDL_GetNumJSParams(sql_params);
	const char *sql_param_set[num-2];
	int i;
	for (i = 0; i < num; i++) {
		sql_param_set[i] =(const char *)PDL_GetJSParamString(sql_params, i+2);
		sqlite3_bind_text(index_list_statement, i+1, sql_param_set[i], -1, SQLITE_STATIC);
	}
	
	char *reply = NULL;
	char *rows = NULL;
	i = 0;
	while (sqlite3_step(index_list_statement) == SQLITE_ROW) {
		char *row = 0;
		int j;
		int column_count = sqlite3_column_count(index_list_statement);
		
		if (i == 0) {
			asprintf(&row, "{");
		} else {
			asprintf(&row, ",{");
		}

		for (j = 0; j < column_count; j++) {
			char *r;
			if (j == 0) {
				asprintf(&r, "\"%s\":", sqlite3_column_name(index_list_statement, j));
			} else {
				asprintf(&r, ",\"%s\":", sqlite3_column_name(index_list_statement, j));
			}

			append(&row, r);
			free(r);

			char *l = NULL;
			switch(sqlite3_column_type(index_list_statement, j)) {
			case SQLITE_INTEGER:
				asprintf(&l, "\"%d\"", sqlite3_column_int(index_list_statement, j));
				break;
			case SQLITE_TEXT: {
				const unsigned char *text = sqlite3_column_text(index_list_statement, j);
				char *repstr = escape_for_json(text);
				asprintf(&l, "\"%s\"", repstr);
				free(repstr);
				break;
				}
			case SQLITE_NULL: {
				asprintf(&l, "\"\"");
				break;
				}
			}
			
			if (l != NULL) {
				append(&row, l);
				free(l);  /* blows up on interact???? */
			}
		}

		append(&row, "}");
		append(&rows, row);
		free(row);
		i++;
	}
	
	if (rows != NULL) {
		asprintf(&reply, "{\"rows\":[%s]}", rows);
		free(rows);
	} else {
		asprintf(&reply, "{\"rows\": \"\"}");
	}
	
	//sqlite3_reset(index_list_statement);
	sqlite3_finalize(index_list_statement);
	
	return reply;
}

int database_file_exists(const char *database_name) {
	int ret;
	char *path = 0;
	asprintf(&path, "databases/%s", database_name);
	ret = access(path, F_OK);
	free(path);
	return ret;
}


int close_database(const char *database_name) {
	struct sqlite3_list_entry *entry;
	/*
	 * check if the db is already open
	 */
	entry = lookup_by_filename(database_name);
	if (!entry) {
		syslog(LOG_ERR, "trying to close db %s that isn't open", database_name);
		return -1;
	}
	
	sqlite3_close(entry->db);
	remove_database_from_list(entry);
	free(entry);
	return 0;
}

int open_database(const char *database_name) {
	struct listitem *pos;
	struct sqlite3_list_entry *entry;
	/*
	 * check if the db is already open
	 */
	entry = lookup_by_filename(database_name);
	if (entry) 
		/* already open */
		return 0;

	int err = 0;
	sqlite3 *db;
	/*
	 * check if the file exists
	 */
	if (database_file_exists(database_name)) {
		//syslog(LOG_ERR, "[PDK] Can't open database file doesn't exists.");
		return -1;	
	}
	char *path = 0;
	asprintf(&path, "databases/%s", database_name);
	err  = sqlite3_open(path, &db);
	free(path);
	
	if (err) {
		syslog(LOG_ERR, "[PDK] Can't open database %s", sqlite3_errmsg(db));
		return -2;
	}
	
	entry = (struct sqlite3_list_entry *)malloc(sizeof(*entry));
	entry->db = db;

	/*
	 * copy the string so we have our own copy
	 */
	entry->dbname = (char *)malloc(sizeof(char) + strlen(database_name));
	memset(entry->dbname, '\0', strlen(database_name)+1);
	strncpy(entry->dbname, database_name, strlen(database_name));

	add(&db_list, entry);
	
	return 0;
}

int remove_database_from_list(struct sqlite3_list_entry *sqe) {
	int found = 0;
	struct listitem *pos;
	struct listitem *prev = NULL;
	struct sqlite3_list_entry *e;
	for_each(pos, db_list.head) {
		e = (struct sqlite3_list_entry *)pos->private;
		if (strncmp(e->dbname, sqe->dbname, strlen(sqe->dbname)) == 0) {
			del(&db_list, prev, pos);
			found = 1;
			break;
		}
		prev = pos;
	}

	if (!found) {
		syslog(LOG_ERR, "didn't find sqe for %s", sqe->dbname);
		return 0;
	}
	return 1;
}

struct sqlite3_list_entry *lookup_by_filename(const char *dbname) {
	/*
	 * check cached version first
	 */
	int found = 0;
	
	struct listitem *pos;
	struct sqlite3_list_entry *e;
	for_each(pos, db_list.head) {
		e = (struct sqlite3_list_entry *)pos->private;
		if (strncmp(e->dbname, dbname, strlen(dbname)) == 0) {
			found = 1;
			break;
		}
	}

	if (!found) {
		//syslog(LOG_ERR, "PDK list_entry not found for given name: %s", dbname);
		return NULL;
	}
	
	return e;
}
