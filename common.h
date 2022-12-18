#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <thread>
#include <filesystem>
#include <vector>
#include <TlHelp32.h>
#include <optional>
#include <random>
#include <future>
#include <experimental/coroutine>
#include <mutex>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>

struct Vector3 { float x; float y; float z; };
struct Vector2 { float x; float y; };

#define HEX_TO_UPPER(value) "0x" << std::hex << std::uppercase << (DWORD64)value << std::dec << std::nouppercase