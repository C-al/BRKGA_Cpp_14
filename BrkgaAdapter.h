#pragma once
#include <unordered_map>
#include <string>
#include <vector>

class __declspec(dllexport) BrkgaAdapter {
public:
    BrkgaAdapter();

    void test();

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
	// --- BRKGA 基础/高阶 参数
    std::string configFilePath;  // 配置文件路径
	std::unordered_map<std::string, std::string> paramMap;  // 算法参数哈希表

    // --- 宏观参数
    unsigned chromosomeSize;                    // 每条染色体基因数量
    double absImprovementThreshold;             // 绝对改进阈值
    double relImprovementThreshold;             // 相对改进阈值(1e-4 = 0.01%)
    unsigned maxGenerations;                    // 最大迭代代数
    unsigned maxStallGenerations;               // 最大无改善代数
    double maxSeconds;                          // 最大运行时间（秒）
};
