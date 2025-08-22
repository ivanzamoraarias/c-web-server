#include "greet_service.h"
#include "../objects/person.h"
#include <stdio.h>

void greet_person(const Person *p) {
    printf("Hello, %s! You are %d years old.\n", p->name, p->age);
}