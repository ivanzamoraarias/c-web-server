#include <stdio.h>
#include "../utils/math_utils.h"
#include "../objects/person.h"
#include "../services/greet_service.h"
#include "../services/https_service.h"

int main() {
    Person p = create_person("Alice", 30);
    greet_person(&p);
    int sum = add(5, 7);
    printf("Sum: %d\n", sum);

    start_https_server();
    return 0;
}