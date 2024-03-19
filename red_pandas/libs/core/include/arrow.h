#ifndef _CORE_ARROW_H_
#define _CORE_ARROW_H_

#include <arrow/api.h>

namespace rp
{

// Arrow is a C++ library for in-memory data that is being developed as part of the Apache Arrow project.
// https://arrow.apache.org/docs/cpp/tutorials/basic_arrow.html
class Arrow
{
public:
    explicit Arrow();
    bool append(double value);
    void finish();
    std::string to_string();
protected:
    arrow::DoubleBuilder builder;
    std::shared_ptr<arrow::Array> arr;
};

} // namespace rp

#endif // _CORE_ARROW_H_
