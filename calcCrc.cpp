#include <climits>
#include <cstdint>
#include <iostream>
#include <vector>

template <uint16_t polynomial, uint16_t initialCrc, uint16_t xorOut>
class Crc16
{
public:
    uint16_t static calculate(uint8_t const * const data, size_t const octectCount) noexcept
    {
        uint16_t crc_(initialCrc);
        for (uint8_t const * datumPointer = data; (data + octectCount) > datumPointer; ++datumPointer)
        {
            crc_ ^= (static_cast<uint16_t>(*datumPointer) << 8);

            for (int bitIndex = 0; bitIndex < CHAR_BIT; ++bitIndex)
            {
                bool const applyPolynomial = (0 != (0x8000 & crc_));
                crc_ <<= 1;
                if (applyPolynomial)
                {
                    crc_ ^= polynomial;
                }
            }
        }
        return (xorOut ^ crc_);
    }
};

typedef Crc16<0x1021, 0x1d0f, 0x0000> Crc16SpiFujitsu;

int main()
{
    std::vector<uint8_t> data = {0xC9, 0x24, 0x2C, 0x0C, 0x2A, 0x00, 0x03, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00};
    auto crcValue=Crc16SpiFujitsu::calculate(data.data(), data.size());
    std::cout<<std::hex<<crcValue<<std::endl;
}
