#include "BrkgaAdapter.h"
#include <fstream>
#include <iostream>
#include "SimpleDecoder.h"
#include <nlohmann/json.hpp>

BrkgaAdapter::BrkgaAdapter(int _chromosomeSize): chromosomeSize(_chromosomeSize){
    // 初始化BRKGA源码读取参数配置的路径
    configFilePath = "C:\\SwiftPack\\temp\\BRKGA\\param.txt";
       
    // 高级参数暂时直接给定
    setParam("SHAKE_INTERVAL", "0");
    setParam("RESET_INTERVAL", "0");
    setParam("EXCHANGE_INTERVAL", "0");
    setParam("NUM_EXCHANGE_INDIVIDUALS", "0");
    setParam("PATH_RELINK_INTERVAL", "0");
}

void BrkgaAdapter::LoadBrkgaJsonSettings(const std::string& jsonPath)
{
    std::ifstream in(jsonPath);
    if (!in.is_open()) {
        return;
    }
    nlohmann::json j;
    in >> j;

    // 外部参数（注意：JSON 没 chromosomeSize，这个你运行时计算后再填）
    if (j.contains("absImprovementThreshold")) brkgaSetting.absImprovementThreshold = j["absImprovementThreshold"].get<double>();
    if (j.contains("relImprovementThreshold")) brkgaSetting.relImprovementThreshold = j["relImprovementThreshold"].get<double>();
    if (j.contains("maxGenerations")) brkgaSetting.maxGenerations = j["maxGenerations"].get<unsigned>();
    if (j.contains("maxStallGenerations")) brkgaSetting.maxStallGenerations = j["maxStallGenerations"].get<unsigned>();
    if (j.contains("maxSeconds")) brkgaSetting.maxSeconds = j["maxSeconds"].get<double>();

    // 内部参数（这些在 JSON 里是 string）
    static const std::vector<std::string> keys = {
        "POPULATION_SIZE",
        "MIN_ELITES_PERCENTAGE",
        "MAX_ELITES_PERCENTAGE",
        "MUTATION_PROBABILITY",
        "MUTATION_DISTRIBUTION",
        "NUM_ELITE_PARENTS",
        "TOTAL_PARENTS",
        "BIAS_TYPE",
        "DIVERSITY_TYPE",
        "NUM_INDEPENDENT_POPULATIONS",
        "NUM_INCUMBENT_SOLUTIONS",
        "PR_TYPE",
        "PR_PERCENTAGE"
    };

    for (const auto& k : keys) {
        if (j.contains(k)) {
            // JSON里是 string（你现在就是这样）
            brkgaSetting.paramMap[k] = j[k].get<std::string>();
        }
    }
}

void BrkgaAdapter::test()
{
    setParam("POPULATION_SIZE", "60");
    SimpleDecoder sm;
    solve(sm);
}

void BrkgaAdapter::setParam(const std::string& key, const std::string& value) {
    brkgaSetting.paramMap[key] = value;
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

    for (auto& kv : brkgaSetting.paramMap) {
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
    
    std::cout << "[BRKGA] chromosome_size = " << chromosomeSize << std::endl;
    std::cout << "[BRKGA] population_size = " << brkgaSetting.paramMap["POPULATION_SIZE"] << std::endl;

    // 求解
    return wrapper.solve(
        decoder,
        chromosomeSize,
        brkgaSetting.absImprovementThreshold,
        brkgaSetting.relImprovementThreshold,
        brkgaSetting.maxGenerations,
        brkgaSetting.maxStallGenerations,
        brkgaSetting.maxSeconds,
        initSolutions
    );
}
