#include "arrow.h"
//
#include <arrow/csv/api.h>
#include <arrow/io/api.h>
#include <arrow/ipc/api.h>
#include <arrow/pretty_print.h>
#include <arrow/result.h>
#include <arrow/status.h>
#include <arrow/table.h>

namespace rp {

Arrow::Arrow() {

}

bool Arrow::append(double value)
{
    //ARROW_RETURN_NOT_OK(builder.Append(value));
    return true;
}

void Arrow::finish()
{
    //ARROW_ASSIGN_OR_RAISE(arr, builder.Finish());
}

std::string Arrow::to_string()
{
    return arr->ToString();
}

} // namespace rp
