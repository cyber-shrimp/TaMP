#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <openssl/sha.h>

// получение SHA-384 хеша строки
std::string sha384(const std::string& input) {
    unsigned char hash[SHA384_DIGEST_LENGTH];
    SHA384(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

    std::ostringstream oss;
    for (int i = 0; i < SHA384_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return oss.str();
}

int main() {
    std::string text;

    std::cout << "Enter your message: ";
    std::getline(std::cin, text); // ввод строки с клавиатуры

    std::cout << "SHA-384: " << sha384(text) << std::endl;

    return 0;
}