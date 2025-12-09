#pragma once
#include <vector>

// 抽象类 DecoderBase，定义解码器接口
class __declspec(dllexport) DecoderBase {
public:
    virtual ~DecoderBase() {}
    virtual std::vector<double> decode(
        const std::vector<double>& chrom,
        bool rewrite
    ) const = 0;
};
