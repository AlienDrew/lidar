#ifndef SIGNAL_ANALYSER_H
#define SIGNAL_ANALYSER_H

#include <QByteArray>
#include <QScopedPointer>
#include "dto_traits.h"

namespace utils
{
    class SignalAnalyser
    {
    public:
        SignalAnalyser();
        ~SignalAnalyser();
        void goertzelInit();
        dto::Results goertzelAnalyse(const QVector<int>& ch);
    private:
        class Impl;
        QScopedPointer<Impl> d;
    };
}

#endif // SIGNAL_ANALYSER_H
