//
// Created by n424613 on 07/10/2022.
//

#ifndef RED_PANDAS_PROJECT_STRING2_TEST_H
#define RED_PANDAS_PROJECT_STRING2_TEST_H

#include <iostream>
#include <string>
//

std::string _string_to_hex(const char* raw_input)
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

void _recv_string(const char* str)
{
    std::cout << "c++: " << _string_to_hex(str) << " (hex)" << std::endl;
    std::cout << "c++: " << str << std::endl;
}

void _recv_string_b(const char* str)
{
    std::cout << "c++: " << str << std::endl;
}

const char* _send_string_1a()
{
    // a
    // return "\x61";
    return "a";
}

const char* _send_string_1b()
{
    // á
    // return "\xc3\xa1";
    return "á";
}

const char* _send_string_2()
{
    // ✉
    // return "\xe2\x9c\x89";
    return "✉";
}

const char* _send_string_4()
{
    // 😂
    // return "\xf0\x9f\x98\x82";
    return "😂";
}

#endif //RED_PANDAS_PROJECT_STRING2_TEST_H
