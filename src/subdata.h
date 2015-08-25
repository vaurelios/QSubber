#ifndef SUBDATA_H
#define SUBDATA_H


#include <xmlrpc-c/client_simple.hpp>
#include <QtCore>

using namespace xmlrpc_c;

class SubData {
public:
    SubData(cstruct data);

    /* Getters / Setters */
    qint64 getID();
    qint64 getSize();
    QString getURL();
    QString getFilename();

protected:
    qint64 id;
    qint64 size;
    QString url;
    QString filename;
};

#endif // SUBDATA_H
