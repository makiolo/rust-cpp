//
// Created by n424613 on 07/10/2022.
//

#ifndef RED_PANDAS_PROJECT_API_H
#define RED_PANDAS_PROJECT_API_H

#ifdef DISABLE_API
    #define red_pandas_API
    #define red_pandas_API_C
#else
    #ifdef _MSC_VER
        #ifdef red_pandas_EXPORTS
            #define red_pandas_API __declspec(dllexport)
            #define red_pandas_API_C extern "C" __declspec(dllexport)
        #else
            #define red_pandas_API __declspec(dllimport)
            #define red_pandas_API_C extern "C" __declspec(dllimport)
        #endif
    #else
        #ifdef red_pandas_EXPORTS
            #define red_pandas_API __attribute__((visibility("default")))
            #define red_pandas_API_C extern "C" __attribute__((visibility("default")))
        #else
            #define red_pandas_API
            #define red_pandas_API_C extern "C"
        #endif
    #endif
#endif

#endif //RED_PANDAS_PROJECT_API_H
