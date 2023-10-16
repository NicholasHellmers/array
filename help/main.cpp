#include <iostream>

char shift_char(char c, int n) {
    return (c - 96) + n <= 0 ? (((c - 96) + n) % 26) + 122 : (((c - 96) + n) % 26) + 96;
}

int main() {
    char test = shift_char('x', 10);
    std::cout << test << std::endl;
    char test1 = shift_char('a', 100);
    std::cout << test1 << std::endl;
    char test2 = shift_char('z', 1);
    std::cout << test2 << std::endl;
    char test3 = shift_char('a', 26);
    std::cout << test3 << std::endl;
    char test4 = shift_char('a', -26);
    std::cout << test4 << std::endl;
    char test5 = shift_char('a', 0);
    std::cout << test5 << std::endl;
    char test6 = shift_char('a', -1);
    std::cout << test6 << std::endl;
    char test7 = shift_char('a', -27);
    std::cout << test7 << std::endl;

    return 0;
}