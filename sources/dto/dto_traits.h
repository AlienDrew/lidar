#ifndef DTO_TRAITS_H
#define DTO_TRAITS_H

#include <QList>
#include <QSharedPointer>

namespace dto
{
    class Channel;
    class Command;
    class Results;

    using ChannelPtr = QSharedPointer<Channel>;
    using CommandPtr = QSharedPointer<Command>;
    using ResultsPtr = QSharedPointer<Results>;

    using ChannelPtrList = QList<ChannelPtr>;
}

#endif // DTO_TRAITS_H
