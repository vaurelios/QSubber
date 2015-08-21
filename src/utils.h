#ifndef UTILS_H
#define UTILS_H

#include "globals.h"

namespace QSubber{
QString calculate_hash_for_file(const char* filename);
QString getConfigFilename();
QString getStringFromUnsignedChar(const unsigned char *str);
}

#endif // UTILS_H

