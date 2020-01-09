#ifndef ONLINESTATUSNAMESPACE_H
#define ONLINESTATUSNAMESPACE_H

#include <QMetaEnum>

namespace OnlineStatusNamespace
{
    Q_NAMESPACE

    enum State
    {
        ONLINE,
        OFFLINE,
        INROOM,
        NOLOGIN,
    };
    Q_ENUM_NS(State);

    static QString messages[] =
    {
        "Online",
        "Offline",
        "In Room",
        "Signed out",
    };
};

#ifdef QT_GUI_LIB
#include <QIcon>
namespace OnlineStatusNamespace
{
    static QIcon icons[] =
    {
        QIcon("online.svg"),
        QIcon("offline.svg"),
        QIcon("inroom.svg"),
        QIcon("nologin.svg"),
    };
}
#endif  //QT_GUI_LIB

#endif  //ONLINESTATUSNAMESPACE_H
