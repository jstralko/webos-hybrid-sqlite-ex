#ifndef _STRING_OPS_H_
#define _STRING_OPS_H_

char *replace(char const * const orginal,
		char const * const pattern,
		char const * const replacement);

char *escape_for_json(const char *text);

void append(char **buffer, char *addition);

char to_lower_case(char c);

#endif /* _STRING_OPS_H_ */
