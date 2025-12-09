#include "BrkgaAdapter.h"
#include "BrkgaWrapper.h"
#include <fstream>
#include <iostream>
#include "SimpleDecoder.h"

BrkgaAdapter::BrkgaAdapter() {
    // 初始化参数配置路径
    configFilePath = "C:\\SwiftPack\\temp\\BRKGA\\param.txt";

    chromosomeSize = 1;                     // 每条染色体基因数量
    absImprovementThreshold = 1;            // 绝对改进阈值
    relImprovementThreshold = 5e-4;         // 相对改进阈值(1e-4 = 0.01%)
    maxGenerations = 100;                   // 最大迭代代数
    maxStallGenerations = 20;               // 最大无改善代数
    maxSeconds = 3600.0;                    // 最大运行时间（秒）

    // 设置默认参数
    paramMap["POPULATION_SIZE"] = "50";
    paramMap["MIN_ELITES_PERCENTAGE"] = "0.20";
    paramMap["MAX_ELITES_PERCENTAGE"] = "0.30";
    paramMap["MUTATION_PROBABILITY"] = "0.10";
    paramMap["MUTATION_DISTRIBUTION"] = "20.0";
    paramMap["NUM_ELITE_PARENTS"] = "2";
    paramMap["TOTAL_PARENTS"] = "3";
    paramMap["BIAS_TYPE"] = "QUADRATIC";
    paramMap["DIVERSITY_TYPE"] = "NONE";
    paramMap["NUM_INDEPENDENT_POPULATIONS"] = "1";
    paramMap["NUM_INCUMBENT_SOLUTIONS"] = "1";
    paramMap["PR_TYPE"] = "ALLOCATION";
    paramMap["PR_PERCENTAGE"] = "1.0";
}

void BrkgaAdapter::test()
{
    setParam("POPULATION_SIZE", "60");
    SimpleDecoder sm;
    solve(sm);
}

void BrkgaAdapter::setParam(const std::string& key, const std::string& value) {
    paramMap[key] = value;
}

void BrkgaAdapter::setConfigFile(const std::string& path) {
    configFilePath = path;
}

void BrkgaAdapter::writeConfig() {
    std::ofstream out(configFilePath);
    if (!out) {
        std::cerr << "Cannot write config file: " << configFilePath << "\n";
        return;
    }

    for (auto& kv : paramMap) {
        out << kv.first << " " << kv.second << "\n";
    }

    out.close();
}

double BrkgaAdapter::solve(
    DecoderBase& decoder,
    const std::vector<std::vector<double>>& initSolutions
) {
    // 将 adapter 实例的参数表 paramMap 写入 cfg 文件
    writeConfig();                     

    BrkgaWrapper wrapper;
    // 读取配置文件参数
    wrapper.loadConfig(configFilePath);     
    // 求解
    return wrapper.solve(
        decoder,
        absImprovementThreshold,
        relImprovementThreshold,
        chromosomeSize,
        maxGenerations,
        maxStallGenerations,
        maxSeconds,
        initSolutions
    );
}
