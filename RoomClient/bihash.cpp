#include "bihash.h"
#include "roomcommon.h"

template<typename T1,typename T2>
T2 BiHash<T1,T2>::get(T1 key)
{
    return first[key];
}

template<typename T1,typename T2>
T1 BiHash<T1,T2>::xget(T2 key)
{
    return second[key];
}

template<typename T1,typename T2>
void BiHash<T1,T2>::set(T1 key, T2 value)
{
    first[key]=value;
    second[value]=key;
}

template<typename T1,typename T2>
void BiHash<T1,T2>::clear()
{
    first.clear();
    second.clear();
}

template<typename T1,typename T2>
void BiHash<T1,T2>::remove(T1 key)
{
    second.remove(first[key]);
    first.remove(key);
}

template<typename T1,typename T2>
void BiHash<T1,T2>::xremove(T2 key)
{
    first.remove(second[key]);
    second.remove(key);
}

template<typename T1,typename T2>
bool BiHash<T1,T2>::key_contains(T1 key)
{
    return first.contains(key);
}

template<typename T1,typename T2>
bool BiHash<T1,T2>::value_contains(T2 value)
{
    return second.contains(value);
}

template class BiHash<client_id_t,quint8>;
