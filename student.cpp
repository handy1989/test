#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _student_t
{
    int id;
    char name[100];
    float score;
    _student_t *next;
}student_t;

void insertLink(student_t* &head, int id, const char *name, float score)
{
    printf("insert id[%d] name[%s] score[%.2f]\n", id, name, score);
    student_t *data = (student_t *)malloc(sizeof(student_t));
    data->id = id;
    strncpy(data->name, name, 100);
    data->score = score;
    data->next = NULL;

    if (head == NULL)
    {
        head = data;
        return ;
    }

    //找到这样的节点，使得q->id < data.id < p->id
    student_t *p = head;
    student_t *q = head;
    while (p && p->id < data->id)
    {
        q = p;
        p = p->next;
    }
    if (p && p->id == data->id)
    {
        printf("can't add, id[%d] has exits!\n", id);
        return ;
    }
    if (p == head)
    {
        data->next = p;
        head = data;
    }
    else
    {
        data->next = p;
        q->next = data;
    }
}

student_t* createLink(const char *file)
{
    if (!file)
    {
        fprintf(stderr, "input file is NULL\n");
        return NULL;
    }
    FILE *fp = fopen(file, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "open %s failed!\n", file);
        return NULL;
    }
    student_t *head = NULL;
    student_t *tail = NULL;
    student_t data;
    while (fscanf(fp, "%d%s%f", &data.id, data.name, &data.score) == 3)
    {
        student_t *q = (student_t *)malloc(sizeof(student_t));
        q->id = data.id;
        strncpy(q->name, data.name, 100);
        q->score = data.score;
        q->next = NULL;
        insertLink(head, data.id, data.name, data.score);
    }
    fclose(fp);
    return head;
}

void deleteLink(student_t* &head, int id)
{
    printf("delete id[%d]\n", id);
    if (head == NULL)
    {
        printf("can't delete id[%d], link is NULL!\n", id);
        return ;
    }

    //找到这样的节点，使得q->next = p && p->id == id
    student_t *p = head;
    student_t *q = head;
    while (p && p->id != id)
    {
        q = p;
        p = p->next;
    }
    if (p == NULL)
    {
        printf("can't find id[%d]\n", id);
    }
    else if (p == head)
    {
        head = p->next;
        free(p);
    }
    else
    {
        q->next = p->next;
        free(p);
    }
}

void printLink(student_t *head)
{
    printf("print Link:\n");
    printf("id\tname\tscore\n");
    student_t *p = head;
    while (p != NULL)
    {
        printf("%d\t%s\t%.2f\n", p->id, p->name, p->score);
        p = p->next;
    }
}


int main()
{
    student_t *head;
    head = createLink("student.txt");
    printLink(head);
    insertLink(head, 123,"Jay", 101);
    insertLink(head, 121,"Jay", 101);
    printLink(head);
    deleteLink(head, 100) ;
    deleteLink(head, 10) ;
    deleteLink(head, 123) ;
    printLink(head);
    return 0;
}
