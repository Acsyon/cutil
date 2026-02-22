#include <cutil/status.h>

const char *
cutil_strerror(cutil_Status status)
{
    switch (status) {
    case CUTIL_STATUS_SUCCESS:
        return "Success";
    case CUTIL_STATUS_FAILURE:
        return "Failure";
    default:
        return "Unknown status code";
    }
}
