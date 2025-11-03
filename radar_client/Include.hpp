#pragma once
#define WIN32_LEAN_AND_MEAN
#define _WINSOCKAPI_
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <vector>
#include <time.h>
#include <fstream>
#include <sstream>
#include <thread>
#include <memory>
#include <chrono>
#include <dwmapi.h>
#include <dwrite.h>
#include <array>
#include <codecvt>
#include <Wininet.h>
#pragma comment( lib, "dwrite.lib" )
#pragma comment( lib, "Dwmapi.lib" )
#pragma comment( lib, "Winmm.lib" )
#pragma comment(lib, "Wininet.lib")
#include "Vector.hpp"

