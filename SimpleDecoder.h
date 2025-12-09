#pragma once
#include "DecoderBase.h"

class __declspec(dllexport) SimpleDecoder : public DecoderBase {
public:
    std::vector<double> decode(
        const std::vector<double>& chrom,
        bool
    ) const override
    {
        double s = 0.0;
        for (double x : chrom) s += x * x;
        return { s }; // µ¥Ä¿±ê
    }
};
