#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <PDL_types.h>
#include <SDL_events.h> 
#include <SDL.h>
#include <sqlite3.h>
#include <pthread.h>
#include "list.h"
#include "string_ops.h"
#include "database.h"


PDL_bool file_exists(PDL_JSParameters *params) {
	const char *filename = (const char *)PDL_GetJSParamString(params, 0);
	if (strlen(filename) == 0) {
		PDL_JSReply(params, "{\"exists\":-1,\"errorText\":\"Invalid arguments\"}");
		return PDL_FALSE;
	}
	
	if (!database_file_exists(filename)) {
		PDL_JSReply(params, "{\"exists\": \"1\"}");
	} else {
		PDL_JSReply(params, "{\"exists\": \"0\"}");
	}
	return PDL_TRUE;
}

PDL_bool openDatabase(PDL_JSParameters *param) {
	int num =  PDL_GetNumJSParams(param);
	if (num <= 0) {
		PDL_JSReply(param, "{\"open\":\"0\", \"errorText\":\"invalid args\"}");
		return PDL_FALSE;
	}
	
	const char *db_name = (const char *)PDL_GetJSParamString(param, 0);
	if (!strlen(db_name) || strlen(db_name) > 255) {
		PDL_JSReply(param, "{\"open\":\"0\", \"errorText\":\"invalid args\"}");
		return PDL_FALSE;
	}
	int ret = open_database(db_name);
	if (ret) {
		char *reply = 0;
		asprintf(&reply, "{\"open\":\"0\", \"errorText\":\"open_database failed %d\"}", ret);
		PDL_JSReply(param, reply);
		free(reply);
		return PDL_FALSE;
	}
	
	PDL_JSReply(param, "{\"open\":\"1\"}");	
	return PDL_TRUE;
}

PDL_bool executeSql(PDL_JSParameters *param) {
	const char *dbname = (const char *)PDL_GetJSParamString(param, 0);
	const char *sql = (const char *)PDL_GetJSParamString(param, 1);
	
	char *reply;
	reply = execute_sql(dbname, sql, param);
	PDL_JSReply(param, reply);
	free(reply);
	
	return PDL_TRUE;
}

PDL_bool closeDatabase(PDL_JSParameters *param) {
	int num =  PDL_GetNumJSParams(param);
	if (num <= 0) {
		PDL_JSReply(param, "{\"returnValue\":\"1\", \"errorText\": \"Invalid arguments\"}");
		return PDL_FALSE;
	}

	const char *db_name = (const char *)PDL_GetJSParamString(param, 0);
	if (close_database(db_name)) {
		PDL_JSReply(param, "{\"returnValue\":\"1\", \"errorText\": \"error closing database\"}");
		return PDL_FALSE;
	}
	
	PDL_JSReply(param, "{\"returnValue\":\"0\"}");	
	return PDL_TRUE;	
}

/*
 * PDK house keeping below 
 */
int plugin_client_init() {
	int ret = 0;
	ret += PDL_RegisterJSHandler("openDatabase", openDatabase);
	ret += PDL_RegisterJSHandler("closeDatabase", closeDatabase);
	ret += PDL_RegisterJSHandler("executeSql", executeSql);
	ret += PDL_RegisterJSHandler("file_exists", file_exists);
	return ret;
}

void plugin_start() {
	SDL_Event Event;
	do {
		SDL_WaitEvent(&Event);
	} while (Event.type != SDL_QUIT);
}

PDL_Err plugin_initialize() {

	SDL_Init(SDL_INIT_VIDEO);
	PDL_Init(0);

	if (plugin_client_init() > 0) {
		syslog(LOG_ERR, "[PDK] JS handler registration failed");
		return -1;
	}

	initlist(&db_list);

	PDL_Err ret = PDL_JSRegistrationComplete();
	return ret;
}

void cleanup(int sig) {
	syslog(LOG_INFO, "[PDK] Cleanup caused by: %d", sig);
	closelog();
	PDL_Quit();
}

void sighandler(int sig) {
	cleanup(sig);
	exit(0);
}

int main(int argc, char *argv[]) {
	//What header file do i need?
	//signal(SIGINT, sighandler);
	//signal(SIGTERM, sighandler);
	//signal(SIGQUIT, sighandler);
	//signal(SIGHUP, sighandler);
	//signal(SIGKILL, sighandler);

	openlog("com.webos-internals.db",0, LOG_USER);

	int ret = plugin_initialize();
	if (ret == PDL_NOERROR) {
		syslog(LOG_NOTICE, "[PDK] JS handler registration complete");
		plugin_start();
	} else {
		syslog(LOG_ERR, "[PDK] JS handler registration failed: %d", ret);
	}

	cleanup(-1);
	return 0;
}
