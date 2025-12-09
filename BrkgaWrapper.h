#pragma once
#include <string>
#include <vector>
#include "DecoderBase.h"

// 前置声明 params (避免头文件污染)
namespace NSBRKGA {
    struct NsbrkgaParams;
}

// Wrapper（指针 + 转发）：前台服务员，接受输入（点菜），转发任务给后台厨师（Impl），输出结果（端菜）
class __declspec(dllexport)  BrkgaWrapper {
public: 
    BrkgaWrapper();
    ~BrkgaWrapper();

    // 加载配置文件参数（覆盖）
    void loadConfig(const std::string& filename);

	// 求解
    double solve(
        DecoderBase& decoder,
        unsigned chromosomeSize,
        double absImprovementThreshold,
        double relImprovementThreshold,
        unsigned maxGenerations,
        unsigned maxStallGenerations,
        double maxSeconds,
        const std::vector<std::vector<double>>& initSolutions
    );

private:
	class Impl;    // 前置声明。头文件不用包含第三方文件，极大降低编译依赖
	Impl* pImpl;   // 实际发挥作用的类指针。cpp中包含该类的具体实现细节，内部可以使用第三方库功能
};
