#pragma once

#include <string>

#if defined(__linux__)
const std::string kAssetFolder = "assets/";
#else
const std::string kAssetFolder = "../../assets/";
#endif
