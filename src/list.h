#ifndef _LIST_H_
#define _LIST_H_

#define for_each(pos, head) \
	for (pos = head; pos != 0; pos = pos->next)

struct listitem {
	void *private;
	struct listitem *next;	
};

struct list {
	struct listitem *head;
};

void initlist(struct list *l);
int is_empty(struct list *l);
void add(struct list *l, void *v);
void del(struct list *l, struct listitem *prev, struct listitem *pos);
int length(struct list l);

#endif /* _LIST_H__ */
