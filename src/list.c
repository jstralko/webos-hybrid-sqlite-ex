#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "list.h"

int is_empty(struct list *ilist) {
	return ilist->head == 0 ? 1 : 0;
}	

void initlist(struct list *ilist) {
	ilist->head = 0;
}

void del(struct list *ilist, struct listitem *prev, struct listitem *pos) {
	if (prev != NULL) {
		prev->next = pos->next;
	} else {
		initlist(ilist);
	}
	
	free(pos);
}

void add(struct list *ilist, void *v) {
	struct listitem *ptr;
	struct listitem *newitem;

	newitem = (struct listitem *)malloc(sizeof(*newitem));
	if (!newitem) {

	}
	newitem->private = v;
	newitem->next = NULL;

	if (!ilist->head) {
		ilist->head = newitem;
		return;
	}

	ptr = ilist->head;
	while (ptr->next) {
		ptr = ptr->next;
	}
	ptr->next = newitem;
}

int length(struct list ilist) {
	struct listitem *ptr;
	int count = 1;

	if (!ilist.head) return 0;
	ptr = ilist.head;
	while (ptr->next) {
		ptr = ptr->next;
		count++;
	}

	return count;
}
