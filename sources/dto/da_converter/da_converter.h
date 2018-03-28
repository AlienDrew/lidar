#ifndef DA_CONVERTER_H
#define DA_CONVERTER_H

namespace dto
{
    class DAConverter
    {
    public:
        void setDigitalVal(int val);
        int digitalVal() const;
        int bitDepth() const;
        void dec2bin();
    private:
        int m_bitDepth;
        int m_digitalVal;
    };
}
#endif // DA_CONVERTER_H
