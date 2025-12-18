#include <chrono>
#include <random>

#include "BrkgaWrapper.h"
#include "nsbrkga.hpp"      // ★★★ 唯一 BRKGA求解器源码 引入点 ★★★

using namespace std::chrono;
using NSBRKGA::NsbrkgaParams; 
using NSBRKGA::ExternalControlParams; 
   

//--------------------------- PIMPL 实现 ---------------------------//
 
class __declspec(dllexport) BrkgaWrapper::Impl {

// 成员变量 
public:
    NsbrkgaParams params;

// 成员函数
public:
	// 构造函数
    Impl() {}

    double elapsed_seconds(const steady_clock::time_point& start) {
        return duration_cast<duration<double>>(steady_clock::now() - start).count();
    }

    // 求解，调用BRKGA
    double run(
        DecoderBase& decoder,
        unsigned chromSize,
        double absImprovementThreshold,
        double relImprovementThreshold,
        unsigned maxGenerations,
        unsigned maxStallGenerations,
        double maxSeconds,
        const std::vector<std::vector<double>>& initSolutions
    ) {
        using namespace std::chrono;

        std::vector<NSBRKGA::Sense> senses = {
            NSBRKGA::Sense::MINIMIZE    // 单目标，最小化
        };

        std::random_device rd;
        unsigned int seed = rd();  

		// --- 构造BRKGA求解对象，保存解码器引用、参数配置
        NSBRKGA::NSBRKGA<DecoderBase> brkga(
			decoder,             // 自定义解码器
            senses,
            seed,                // 随机种子
            chromSize,
            params,
            1,                   // maxThreads
            true                 // evo mechanism
        );

        // --- 创建初始随机种群 ---
        brkga.initialize();            
        // --- 注入给定初始方案（如果有） ---
        unsigned pos = 0;
        for (const auto& sol : initSolutions) {
            if (sol.size() == chromSize) {
				brkga.injectChromosome(sol, 0, pos);        // 注入第一个种群中。若多种群，可调整第二个参数
                pos++;
                if (pos >= params.population_size) break;
            }
        }        

        // 打印初始种群染色体值
        if (0) {
            auto& population = brkga.getCurrentPopulation();
            // 遍历种群中的每个染色体并打印
            for (unsigned i = 0; i < population.population.size(); ++i) {
                const auto& chromosome = population.population[i];  // 直接访问 population 中的染色体
                std::cout << "Initial chromosome " << i << ": ";
                for (double gene : chromosome) {
                    std::cout << gene << " ";
                }
                std::cout << std::endl;
            }
        }

        //--- 求解迭代(时间控制) ---
        auto start = steady_clock::now();

        double bestFitness = std::numeric_limits<double>::infinity();
        unsigned stallCount = 0;

        // ★ 最大代数内探索
        for (unsigned g = 0; g < maxGenerations; ++g) {

            brkga.evolve();

            auto& pop = brkga.getCurrentPopulation();
            double curr = pop.getBestFitnesses(senses)[0][0];
            std::cout << "Generation " << g << " Best Fitness: " << curr << std::endl;

            // ★ 无改善停止
            bool improved = false;
            if (bestFitness < std::numeric_limits<double>::infinity()) {

                double absImprovement = bestFitness - curr;
                double relImprovement = 0.0;

                if (fabs(bestFitness) > 1e-15) {
                    relImprovement = absImprovement / fabs(bestFitness);
                }

                // 最小改进判定（绝对 OR 相对）
                if (absImprovement >= absImprovementThreshold ||
                    relImprovement >= relImprovementThreshold)
                {
                    improved = true;
                }
            }
            if (improved) {
                bestFitness = curr;
                stallCount = 0;
            }
            else {
                stallCount++;
            }
            if (stallCount >= maxStallGenerations) {
                std::cout << "[STOP] stall condition reached.\n";
                break;
            }

            // ★ 最大计算时间停止判定
            if (elapsed_seconds(start) >= maxSeconds) {
                std::cout << "[STOP] time limit reached.\n";
                break;
            }
        }

        // --- 输出最终结果 ---
        auto& pop = brkga.getCurrentPopulation();
        auto best = pop.getBestFitnesses(senses);
        bestFitness = best[0][0];            // 单目标优化时，最优适应度值

        std::cout << "Best fitness: " << bestFitness << std::endl;

        return bestFitness;      // 单目标优化 最佳值
    }
};


//--------------------------- Wrapper 外围接口 ---------------------------//

BrkgaWrapper::BrkgaWrapper() : pImpl(new Impl()) {

}

BrkgaWrapper::~BrkgaWrapper() { 
    delete pImpl; 
}

void BrkgaWrapper::loadConfig(const std::string& filename) 
{
    NSBRKGA::readConfiguration(filename, pImpl->params);
}
 
double BrkgaWrapper::solve(
    DecoderBase& decoder,
    unsigned chromosomeSize,
    double absImprovementThreshold,
    double relImprovementThreshold,
    unsigned maxGenerations,
    unsigned maxStallGenerations,
    double maxSeconds,
    const std::vector<std::vector<double>>& initSolutions
) {
    return pImpl->run(
        decoder,
        chromosomeSize,
        absImprovementThreshold,
        relImprovementThreshold,
        maxGenerations,
        maxStallGenerations,
        maxSeconds,
        initSolutions
    );
}