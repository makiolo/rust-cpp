//
// Created by n424613 on 06/10/2022.
//

#ifndef RED_PANDAS_PROJECT_STRING_TEST_H
#define RED_PANDAS_PROJECT_STRING_TEST_H

#include <iostream>
#include "api.h"

std::string string_to_hex(const char* raw_input);
red_pandas_API_C void recv_string(const char* str);
red_pandas_API_C void recv_string_b(const char* str);
red_pandas_API_C const char* send_string_1a();
red_pandas_API_C const char* send_string_1b();
red_pandas_API_C const char* send_string_2();
red_pandas_API_C const char* send_string_4();

#endif //RED_PANDAS_PROJECT_STRING_TEST_H
