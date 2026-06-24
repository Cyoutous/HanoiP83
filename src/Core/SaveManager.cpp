// Core/SaveManager.cpp
#include "SaveManager.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "Core/Resource.h"
#include "Type/SetupCache.h"
#include "Component/SessionState.h"
#include "Component/NextSessionConfig.h"
#include "Component/BestRecord.h"
#include "Component/NeedleStack.h"
#include "Component/NeedleIndex.h"
#include "Component/DiskData.h"

namespace fs = std::filesystem;

namespace {
    std::string savesDir() {
        return std::string(GetApplicationDirectory()) + "/saves";
    }
    std::string sessionPath() { return savesDir() + "/session.dat"; }
    std::string bestPath()    { return savesDir() + "/best.dat"; }


    void ensureDirectory() {
        fs::create_directory(savesDir());
    }

    bool fileExists(const std::string path) {
        return fs::exists(path);
    }

    std::vector<int> parseDisks(const std::string& value) {
        std::vector<int> result;
        if (value.empty()) return result;
        std::istringstream ss(value);
        std::string token;
        while (std::getline(ss, token, ',')) {
            if (!token.empty())
                result.push_back(std::stoi(token));
        }
        return result;
    }

    std::string serializeDisks(const std::vector<entt::entity>& disks, entt::registry& reg) {
        std::string result;
        for (size_t i = 0; i < disks.size(); i++) {
            if (i > 0) result += ",";
            result += std::to_string(reg.get<const DiskData>(disks[i]).size);
        }
        return result;
    }
}

// ========== 读 ==========

void SaveManager::load(Resource& res) {
    ensureDirectory();
    auto& cache = res.setupCache;

    // session.dat
    if (fileExists(sessionPath())) {
        std::ifstream in(sessionPath());
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            auto eq = line.find('=');
            if (eq == std::string::npos) continue;
            std::string key = line.substr(0, eq);
            std::string val = line.substr(eq + 1);

            if      (key == "diskCount")     cache.diskCount     = std::stoi(val);
            else if (key == "stepCount")     cache.stepCount     = std::stoi(val);
            else if (key == "isAutoDemo")    cache.isAutoDemo    = (val == "1");
            else if (key == "completed")     cache.completed     = (val == "1");
            else if (key == "nextDiskCount") cache.nextDiskCount = std::stoi(val);
            else if (key == "nextAutoDemo")  cache.nextAutoDemo  = (val == "1");
            else if (key == "n0") cache.needles[0].diskSizes = parseDisks(val);
            else if (key == "n1") cache.needles[1].diskSizes = parseDisks(val);
            else if (key == "n2") cache.needles[2].diskSizes = parseDisks(val);
        }
    }

    // best.dat
    if (fileExists(bestPath())) {
        std::ifstream in(bestPath());
        cache.bestRecords.clear();
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            auto eq = line.find('=');
            if (eq == std::string::npos) continue;
            int k = std::stoi(line.substr(0, eq));
            int v = std::stoi(line.substr(eq + 1));
            cache.bestRecords[k] = v;
        }
    }
}

// ========== 写 ==========

void SaveManager::save(entt::registry& reg) {
    ensureDirectory();

    // session.dat
    {
        std::ofstream out(sessionPath());

        auto sessionView = reg.view<const SessionState>();
        auto nextView = reg.view<const NextSessionConfig>();

        // 获取盘数（自动分支）
        int diskCount = 3;
        if (sessionView.begin() != sessionView.end()) {
            diskCount = reg.get<const SessionState>(*sessionView.begin()).diskCount;
        }

        bool isAutoDemo = false;
        if (sessionView.begin() != sessionView.end()) {
            isAutoDemo = reg.get<const SessionState>(*sessionView.begin()).isAutoDemo;
        }

        if (isAutoDemo) {
            // 自动演示状态不保存 — 写干净开局
            out << "diskCount="  << diskCount << "\n";
            out << "stepCount="  << 0 << "\n";
            out << "isAutoDemo=" << 0 << "\n";
            out << "completed="  << 0 << "\n";
            out << "nextDiskCount=" << diskCount << "\n";
            out << "nextAutoDemo="  << 0 << "\n";
            out << "n0=";
            for (int i = 0; i < diskCount; i++) out << (i > 0 ? "," : "") << i;
            out << "\nn1=\nn2=\n";
        }
        else {
            // 不处于自动演示状态
            if (sessionView.begin() != sessionView.end()) {
                auto& s = reg.get<const SessionState>(*sessionView.begin());
                out << "diskCount="  << s.diskCount  << "\n";
                out << "stepCount="  << s.stepCount  << "\n";
                out << "isAutoDemo=" << (s.isAutoDemo ? 1 : 0) << "\n";
                out << "completed="  << (s.completed  ? 1 : 0) << "\n";
            }
    
            if (nextView.begin() != nextView.end()) {
                auto& n = reg.get<const NextSessionConfig>(*nextView.begin());
                out << "nextDiskCount=" << n.diskCount << "\n";
                out << "nextAutoDemo="  << (n.autoDemo ? 1 : 0) << "\n";
            }
    
            auto needleView = reg.view<const NeedleStack, const NeedleIndex>();
            std::vector<std::pair<int, std::string>> needleLines;
            for (auto [entity, stack, idx] : needleView.each()) {
                needleLines.push_back({
                    idx.index,
                    "n" + std::to_string(idx.index) + "=" + serializeDisks(stack.disks, reg)
                });
            }
            std::sort(needleLines.begin(), needleLines.end());
            for (auto& [_, line] : needleLines) {
                out << line << "\n";
            }

        }

    }

    // best.dat
    {
        std::ofstream out(bestPath());
        auto bestView = reg.view<const BestRecord>();
        if (bestView.begin() != bestView.end()) {
            auto& b = reg.get<const BestRecord>(*bestView.begin());
            for (auto& [k, v] : b.record) {
                if (v > 0) out << k << "=" << v << "\n";
            }
        }
    }
}
