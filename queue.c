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
    if (!node)
        return false;

    node->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!node->value) {
        free(node);
        return false;
    }
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
    if (!node)
        return false;

    node->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!node->value) {
        free(node);
        return false;
    }
    strncpy(node->value, s, strlen(s) + 1);

    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *entry = list_entry(head->next, element_t, list);

    strncpy(sp, entry->value, bufsize - 1);
    sp[bufsize - 1] = '\0';

    list_del_init(head->next);
    return entry;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *pos = list_entry(head->prev, element_t, list);

    strncpy(sp, pos->value, bufsize - 1);
    sp[bufsize - 1] = '\0';

    list_del_init(head->prev);
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

void merge(struct list_head *head, struct list_head *left, bool descend)
{
    struct list_head *curOfLeft = left->next;
    struct list_head *curOfRight = head->next;
    struct list_head *nextOps = NULL;
    struct list_head result;

    INIT_LIST_HEAD(&result);

    while (curOfLeft != left && curOfRight != head && descend) {
        if (strcmp(list_entry(curOfLeft, element_t, list)->value,
                   list_entry(curOfRight, element_t, list)->value) < 0) {
            nextOps = curOfRight->next;
            list_move_tail(curOfRight, &result);
            curOfRight = nextOps;
        } else {
            nextOps = curOfLeft->next;
            list_move_tail(curOfLeft, &result);
            curOfLeft = nextOps;
        }
    }
    while (curOfLeft != left && curOfRight != head && !descend) {
        if (strcmp(list_entry(curOfLeft, element_t, list)->value,
                   list_entry(curOfRight, element_t, list)->value) > 0) {
            nextOps = curOfRight->next;
            list_move_tail(curOfRight, &result);
            curOfRight = nextOps;
        } else {
            nextOps = curOfLeft->next;
            list_move_tail(curOfLeft, &result);
            curOfLeft = nextOps;
        }
    }

    if (list_empty(left)) {
        // Append remaining nodes from left
        list_splice_tail_init(head, &result);
    } else {
        // Append remaining nodes from right
        list_splice_tail_init(left, &result);
    }
    list_splice_tail_init(&result, head);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || head->next->next == head)
        return;

    // find mid
    struct list_head *forward = head->next, *backward = head->prev;
    while (forward != backward && forward->next != backward) {
        forward = forward->next;
        backward = backward->prev;
    }
    struct list_head left;
    struct list_head *mid = forward;

    list_cut_position(&left, head, mid);
    q_sort(&left, descend);
    q_sort(head, descend);
    merge(head, &left, descend);
}


/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    int elementNum = q_size(head);

    const struct list_head *min = head->prev;
    struct list_head *cur = head->prev;

    while (cur != head) {
        if (strcmp(list_entry(min, element_t, list)->value,
                   list_entry(cur, element_t, list)->value) < 0) {
            struct list_head *del = cur;
            cur = cur->prev;
            element_t *entry = list_entry(del, element_t, list);
            list_del(del);
            q_release_element(entry);
            elementNum--;
        } else {
            min = cur;
            cur = cur->prev;
        }
    }
    return elementNum;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    int elementNum = q_size(head);

    const struct list_head *max = head->prev;
    struct list_head *cur = head->prev;

    while (cur != head) {
        if (strcmp(list_entry(max, element_t, list)->value,
                   list_entry(cur, element_t, list)->value) > 0) {
            struct list_head *del = cur;
            cur = cur->prev;
            element_t *entry = list_entry(del, element_t, list);
            list_del(del);
            q_release_element(entry);
            elementNum--;
        } else {
            max = cur;
            cur = cur->prev;
        }
    }
    return elementNum;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return q_size(head);

    queue_contex_t *first = list_entry(head->next, queue_contex_t, chain);
    struct list_head *node = head->next->next;

    while (node != head) {
        queue_contex_t *cur = list_entry(node, queue_contex_t, chain);
        if (cur->q) {
            merge(first->q, cur->q, descend);
        }
        node = node->next;
    }
    return first->size;
}
