#include "old/string_test.h"
//
// Created by n424613 on 07/10/2022.
//


std::string string_to_hex(const char* raw_input)
{
    static const char hex_digits[] = "0123456789ABCDEF";
    std::string input(raw_input);
    std::string output;
    output.reserve(input.length() * 2);
    for (unsigned char c : input)
    {
        output.push_back(hex_digits[c >> 4]);
        output.push_back(hex_digits[c & 15]);
    }
    return output;
}

void recv_string(const char* str)
{
//    std::locale loc;        // global locale
//    std::cout << "The global locale is: " << loc.name() << '\n';
    std::cout << "c++: " << string_to_hex(str) << " (hex)" << std::endl;
    std::cout << "c++: " << str << std::endl;
}

void recv_string_b(const wchar_t* str)
{
    std::wcout << "c++: " << str << std::endl;
}

const char* send_string_1a()
{
    // a
    // return "\x61";
    return "a";
}

const char* send_string_1b()
{
    // á
    // return "\xc3\xa1";
    return "á";
}

const char* send_string_2()
{
    // ✉
    // return "\xe2\x9c\x89";
    return "✉";
}

const char* send_string_4()
{
    // 😂
    // return "\xf0\x9f\x98\x82";
    return "😂";
}
