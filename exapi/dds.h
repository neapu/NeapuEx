#ifndef _DDS_H_
#define _DDS_H_

#include "matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

ExMatrix* DDSUncompress(const unsigned char* _data, size_t _len);

#ifdef __cplusplus
}
#endif

#endif