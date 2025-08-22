#ifndef PERSON_H
#define PERSON_H

#define NAME_LEN 50

typedef struct {
    char name[NAME_LEN];
    int age;
} Person;

Person create_person(const char *name, int age);

#endif