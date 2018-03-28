#ifndef DTO_TRAITS_H
#define DTO_TRAITS_H

#include <QList>
#include <QSharedPointer>

namespace dto
{
    class Channel;

    using ChannelPtr = QSharedPointer<Channel>;

    using ChannelPtrList = QList<ChannelPtr>;
}

#endif // DTO_TRAITS_H
