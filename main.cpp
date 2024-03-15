#include <iostream>
#include "user.h"
#include "utils.h"

int main() {
    Utils u;
    std::cout<<u.base64_encode({'1', '2', '3', '4', '4', '4'});
}
