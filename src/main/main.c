#include <stdio.h>
#include "../utils/math_utils.h"
#include "../objects/person.h"
#include "../services/greet_service.h"
#include "../services/https_service.h"
#include "../services/frontend_service.h"
#include "../services/product_service.h"
#include "../services/graphql_service.h"

int main() {
    Person p = create_person("Alice", 30);
    greet_person(&p);
    int sum = add(5, 7);
    printf("Sum: %d\n", sum);

    register_endpoint("GET", "/", frontend_service.serve_frontend); // homepage           // Loads frontend/index.html
    register_endpoint("GET", "/style.css", frontend_service.serve_frontend);    // Loads frontend/style.css
    register_endpoint("GET", "/index.js", frontend_service.serve_frontend);     // Loads frontend/index.js
    register_endpoint("GET", "/product", product_service.handle_get_product); // Returns product list JSON
    register_endpoint("POST", "/graphql", graphql_service.handle_graphql); // GraphQL endpoint
    start_https_server();
    return 0;
}