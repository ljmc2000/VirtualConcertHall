#ifndef BIHASH_H
#define BIHASH_H

#include <QHash>

template <typename T1, typename T2>
class BiHash
{
public:
    T2 get(const T1);
    T1 xget(const T2);

    void set(const T1, const T2);
    void remove(const T1);
    void xremove(const T2);
    void clear();

    bool key_contains(const T1);
    bool value_contains(const T2);

private:
    QHash<T1,T2> first;
    QHash<T2,T1> second;
};

#endif // BIHASH_H
