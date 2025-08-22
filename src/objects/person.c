#include "person.h"
#include <string.h>

Person create_person(const char *name, int age) {
    Person p;
    strncpy(p.name, name, sizeof(p.name) - 1);
    p.name[sizeof(p.name) - 1] = '\0';
    p.age = age;
    return p;
}