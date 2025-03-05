#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head)
        INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *cur, *safe;
    list_for_each_safe (cur, safe, head) {
        list_del(cur);
        q_release_element(list_entry(cur, element_t, list));
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node = malloc(sizeof(element_t));

    node->value = malloc((strlen(s) + 1) * sizeof(char));
    strncpy(node->value, s, strlen(s) + 1);

    list_add(&node->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node = malloc(sizeof(element_t));

    node->value = malloc((strlen(s) + 1) * sizeof(char));
    strncpy(node->value, s, strlen(s) + 1);

    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *pos = list_entry(head->next, element_t, list);

    if (sp && bufsize) {
        strncpy(sp, pos->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(&pos->list);
    return pos;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *pos = list_entry(head->prev, element_t, list);

    if (sp && bufsize) {
        strncpy(sp, pos->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(&pos->list);
    return pos;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *forward = head->next, *backward = head->prev;
    while (forward != backward && forward->next != backward) {
        forward = forward->next;
        backward = backward->prev;
    }
    list_del(forward);
    q_release_element(list_entry(forward, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;

    bool del = false;
    struct list_head *cur, *next;

    list_for_each_safe (cur, next, head) {
        element_t *entry = list_entry(cur, element_t, list);
        if (next != head &&
            !strcmp(entry->value, list_entry(next, element_t, list)->value)) {
            list_del(cur);
            q_release_element(entry);
            del = true;
        } else if (del) {
            list_del(cur);
            q_release_element(entry);
            del = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || !head->next)
        return;

    struct list_head *prev = head;
    struct list_head *cur = head->next;

    head->next = head->next->next;

    do {
        struct list_head *pair = cur->next;
        cur->next = pair->next;
        pair->next->prev = cur;

        pair->next = cur;
        cur->prev = pair;

        prev->next = pair;
        pair->prev = prev;

        prev = cur;
        cur = cur->next;
    } while (cur != head && cur != head->prev);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *cur = NULL, *temp = NULL;

    for (cur = head; cur != head->prev; cur = cur->next) {
        temp = cur->next;
        cur->next = cur->prev;
        cur->prev = temp;
    }
    temp = cur->next;
    cur->next = cur->prev;
    cur->prev = temp;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || k < 2)
        return;
    struct list_head *cur = head->next;

    while (cur != head) {
        struct list_head *front = cur, *tail = NULL, *prev = NULL;
        int count = 0;
        while (count < k && cur != head) {
            cur = cur->next;
            count++;
        }
        if (count < k)
            return;

        tail = cur->prev;
        prev = front->prev;
        front->prev = tail;
        tail->next = front;
        q_reverse(front);

        prev->next = tail;
        tail->prev = prev;
        front->next = cur;
        cur->prev = front;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
