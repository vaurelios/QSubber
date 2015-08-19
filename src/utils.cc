#include "utils.h"
#include <iostream>
#include <fstream>
#include <algorithm>


QString QSubber::calculate_hash_for_file(const char* filename) {
    ifstream f;
    uint64_t hash, fsize;

    f.open(filename, ios::in|ios::binary|ios::ate);
    if(!f.is_open()) return NULL;

    f.seekg(0, ios::end);
    fsize = f.tellg();
    f.seekg(0, ios::beg);

    hash = fsize;
    for(uint64_t tmp = 0, i = 0; i < 65536/sizeof(tmp) && f.read((char*)&tmp, sizeof(tmp)); i++, hash += tmp);
    f.seekg(std::max<uint64_t>((uint64_t)fsize - 65536, 0), ios::beg);
    for(uint64_t tmp = 0, i = 0; i < 65536/sizeof(tmp) && f.read((char*)&tmp, sizeof(tmp)); i++, hash += tmp);

    char* tmp = (char *) malloc(256);
    snprintf(tmp, 256, "%lx", hash);

    QString shash(tmp);

    free(tmp);

    return shash;
}
