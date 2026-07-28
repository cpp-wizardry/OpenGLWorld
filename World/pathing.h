//pathing.h
#pragma once
#include <filesystem>
#include <string>

inline std::string RootPath(const std::string& relative) {
    static std::filesystem::path root = std::filesystem::path(__FILE__).parent_path();
    return (root / relative).string();
}