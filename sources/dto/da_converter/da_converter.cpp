#include "da_converter.h"

#include <QBitArray>

using namespace dto;

void DAConverter::setDigitalVal(int val)
{
    m_digitalVal = val;
}

int DAConverter::digitalVal() const
{
    return m_digitalVal;
}

int DAConverter::bitDepth() const
{
    return m_bitDepth;
}

void DAConverter::dec2bin()
{

}
