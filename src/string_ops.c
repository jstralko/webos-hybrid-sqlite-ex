#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include "string_ops.h"

char to_lower_case(char c) {
	char d;
	if (c >='A' && c <='Z') {
		d = c+32;
	} else {
		d = c;
	}
	return d;
}

char * replace(char const * const original, 
		char const * const pattern, 
		char const * const replacement) {

	size_t const replen = strlen(replacement);
	size_t const patlen = strlen(pattern);
	size_t const orilen = strlen(original);

	size_t patcnt = 0;
	const char * oriptr;
	const char * patloc;

	if (replen < patlen) {
		syslog(LOG_ERR, "replacement is less than pattern");
	}

	// find how many times the pattern occurs in the original string
	for (oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen) {
		patcnt++;
	}

	// allocate memory for the new string
	size_t const retlen = orilen + patcnt * (replen - patlen);
	char * const returned = (char *) malloc( sizeof(char) * (retlen + 1) );

	if (returned != NULL) {
		// copy the original string, 
		// replacing all the instances of the pattern
		char * retptr = returned;
		for (oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen) {
			size_t const skplen = patloc - oriptr;
			// copy the section until the occurence of the pattern
			strncpy(retptr, oriptr, skplen);
			retptr += skplen;
			 // copy the replacement 
			strncpy(retptr, replacement, replen);
			retptr += replen;
		}
		// copy the rest of the string.
		strcpy(retptr, oriptr);
	}
	return returned;
}

char * escape_for_json(const char *text) {
	char *str = replace(text, "\"", "\\\"");
	return str;
} 

void append(char **buffer, char *addition) {
	if (*buffer == NULL) {
		*buffer = malloc(strlen(addition) + sizeof(char));
		memset(*buffer, '\0', strlen(addition));
	} else {
		*buffer = realloc(*buffer, strlen(*buffer) + strlen(addition) + sizeof(char));
		memset(*buffer+strlen(*buffer), '\0', strlen(addition));
	}

	if (!*buffer) {
		syslog(LOG_ERR, "malloc failed");
		return;
	}

   	strncat(*buffer, addition, strlen(addition));
}
