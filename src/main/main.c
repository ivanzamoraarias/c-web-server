#include <stdio.h>
#include "../utils/math_utils.h"
#include "../objects/person.h"
#include "../services/greet_service.h"
#include "../services/https_service.h"
#include "../services/frontend_service.h"
#include "../services/product_service.h"

int main() {
    Person p = create_person("Alice", 30);
    greet_person(&p);
    int sum = add(5, 7);
    printf("Sum: %d\n", sum);

    register_endpoint("GET", "/", serve_frontend); // homepage           // Loads frontend/index.html
    register_endpoint("GET", "/style.css", serve_frontend);    // Loads frontend/style.css
    register_endpoint("GET", "/index.js", serve_frontend);     // Loads frontend/index.js
    register_endpoint("GET", "/product", handle_get_product); // Returns product list JSON
    start_https_server();
    return 0;
}