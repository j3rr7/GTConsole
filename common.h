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

struct Vector3 { float x; float y; float z; };

#define HEX_TO_UPPER(value) "0x" << std::hex << std::uppercase << (DWORD64)value << std::dec << std::nouppercase