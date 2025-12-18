#pragma once
#include "BrkgaWrapper.h"
#include <unordered_map>
#include <string>
#include <vector>
  
struct BrkgaJsonSettings {
    // 外部参数
    double absImprovementThreshold;             // 绝对改进阈值
    double relImprovementThreshold;             // 相对改进阈值(1e-4 = 0.01%)
    unsigned maxGenerations;                    // 最大迭代代数
    unsigned maxStallGenerations;               // 最大无改善代数
    double maxSeconds;                          // 最大运行时间（秒）

    // 内部参数（cfg）
    std::unordered_map<std::string, std::string> paramMap;
};

class __declspec(dllexport) BrkgaAdapter {
public:
    BrkgaAdapter(int _chromosomeSize);
    void test();   

    void LoadBrkgaJsonSettings(const std::string& jsonPath);

    // 设置参数（字符串形式，便于业务层改动）
    void setParam(const std::string& key, const std::string& value);
    // 设置 cfg 文件路径
    void setConfigFile(const std::string& path);
    // 写入 cfg 文件（用来给 Wrapper.loadConfig 读取）
    void writeConfig();

    // 主求解接口（Wrapper.execute）
    double solve(
		class DecoderBase& decoder,			// 解码器引用   
		const std::vector<std::vector<double>>& initSolutions = {}  // 初始解列表（单种群），<染色体<基因1，基因2...>>
    );

private:
	// --- 内部参数：BRKGA 参数
    std::string configFilePath;  // 配置文件路径

public:
    // BRKGA参数
    unsigned chromosomeSize;                    // 每条染色体基因数量，程序算法层面决定
    BrkgaJsonSettings brkgaSetting;
};
