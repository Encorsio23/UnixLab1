#include <iostream>
#include <cmath>

bool is_prime(int n) {
    if (n < 2) return false;
    for (int i = 2; i <= std::sqrt(n); ++i) {
        if (n % i == 0) return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <number>" << std::endl;
        return 1;
    }
    int num = std::stoi(argv[1]);
    std::cout << (is_prime(num) ? "true" : "false") << std::endl;
    return 0;
}
