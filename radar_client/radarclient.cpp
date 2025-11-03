#include "include.hpp"
#include "radarclient.h"
#include <shlobj.h>
#include <limits>
#include "structs.h"
#include "offsets.h"
#include "headers.h"
#include "nlohmann/json.hpp"
#include "easywsclient.hpp"
#include <regex>
#include <string>
#include <windows.h>
#include <shellapi.h>

#include <shlwapi.h> 
#include "Memory.hpp"
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "Shlwapi.lib") // Link Shlwapi

std::string mapNameRadar = "";
DWORD64 g_dwEntList = 0;
int localIndex = 0;

struct PlayerInfo
{
    uintptr_t entity;
    uintptr_t pawn;
    int health;
    int teamId;
    int distance;
    bool spotted;
    uint32_t spottedMask;
    Vector position;
    std::string weaponName;
    std::string playerName;
    bool isLocalPlayer;
    vec3 eyeAngles{};
};
std::vector<PlayerInfo> playerInfos;

struct BombInfo
{
    uintptr_t entity;
    float time;
    float defuseTime;
    bool defusing;
    Vector position;

};
std::vector<BombInfo> bombInfos;


DWORD MyGetProcessId(LPCTSTR ProcessName) 
{
    
    PROCESSENTRY32 pt;
    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    pt.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hsnap, &pt)) { 
        do {
            if (!lstrcmpi(pt.szExeFile, ProcessName)) {
                CloseHandle(hsnap);
                return pt.th32ProcessID;
            }
        } while (Process32Next(hsnap, &pt));
    }
    CloseHandle(hsnap);

    return 0;
}




//JSON Serialization
nlohmann::json build_radar_json(const std::string& mapName)
{
    nlohmann::json data;
    data["m_map"] = mapName;

    nlohmann::json players_json = nlohmann::json::array();

    int idx = 0;
    for (const auto& info : playerInfos)
    {
        nlohmann::json player;
        player["m_idx"] = idx++;
        player["m_name"] = info.playerName;
        player["m_health"] = info.health;
        player["m_team"] = info.teamId;
        player["m_spotted"] = info.spotted;
        player["m_weapon"] = info.weaponName;
        player["m_is_local"] = info.isLocalPlayer;
        player["m_eye_angle"] = {
            { "x", info.eyeAngles.x },
            { "y", info.eyeAngles.y },
            { "z", info.eyeAngles.z }
        };
        player["m_position"] = {
            {"x", info.position.x},
            {"y", info.position.y},
            {"z", info.position.z}
        };

        players_json.push_back(player);
    }

    data["m_players"] = players_json;

    // Bombs
    nlohmann::json bombs_json = nlohmann::json::array();
    for (const auto& bomb : bombInfos)
    {
        nlohmann::json b;
        b["m_position"] = {
            { "x", bomb.position.x },
            { "y", bomb.position.y },
            { "z", bomb.position.z }
        };
        b["m_time"] = bomb.time;
        b["m_defuse_time"] = bomb.defuseTime;
        b["m_defusing"] = bomb.defusing;

        bombs_json.push_back(b);
    }
    data["m_bombs"] = bombs_json;

    return data;
}

bool SendRadarPOST(const std::string& json)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return false;

    addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    addrinfo* result = nullptr;
    if (getaddrinfo("127.0.0.1", "22006", &hints, &result) != 0)
    {
        WSACleanup();
        return false;
    }

    SOCKET sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock == INVALID_SOCKET)
    {
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }

    if (connect(sock, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
    {
        closesocket(sock);
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }

    freeaddrinfo(result); 

    std::ostringstream oss;
    oss << "POST /update HTTP/1.1\r\n";
    oss << "Host: localhost\r\n";
    oss << "Authorization: Basic cmFkYXI6b2x5bXB1czI=\r\n";
    oss << "Content-Type: application/json\r\n";
    oss << "Content-Length: " << json.length() << "\r\n";
    oss << "Connection: close\r\n\r\n";
    oss << json;

    const std::string request = oss.str();
    send(sock, request.c_str(), static_cast<int>(request.size()), 0);

    closesocket(sock);
    WSACleanup();
    return true;
}


static void radar()
{
    while (true)
    {

        if (mapNameRadar.empty() || mapNameRadar == "")
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }

        auto start = std::chrono::system_clock::now();

        for (;;)
        {
            const auto now = std::chrono::system_clock::now();
            const auto duration = now - start;
            if (duration >= std::chrono::milliseconds(10))
            {
                start = now;

                try
                {
                    nlohmann::json radar_data = build_radar_json(mapNameRadar);
                    SendRadarPOST(radar_data.dump());
                }
                catch (const std::exception& ex)
                {
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}







namespace memory
{
    template<typename T>
    T read(uint64_t target)
    {
        T buffer{};

        if (!ReadProcessMemory(processMemory.process_handle, reinterpret_cast<LPCVOID>(target), &buffer, sizeof(T), nullptr))
            return {};

        return buffer;
    }

    std::string read_string(uint64_t target)
    {
        const size_t maxBufferSize = 256;
        char buffer[maxBufferSize]{};

        if (!ReadProcessMemory(processMemory.process_handle,
            reinterpret_cast<LPCVOID>(target),
            buffer, sizeof(buffer), nullptr))
            return {};

        buffer[maxBufferSize - 1] = '\0'; // ensure null-terminated
        return std::string(buffer);
      
    }
}

void collectEntityData() {

    if (!sdk::module_base)
        return;

    uint64_t localplayer = memory::read<uint64_t>(sdk::module_base + dwLocalPlayerController);
    if (!localplayer)
        return;

    uintptr_t globalVarsPointer = memory::read<uintptr_t>(sdk::module_base + dwGlobalVars);
    if (!globalVarsPointer)
        return;

    uintptr_t mapName = memory::read<uintptr_t>(globalVarsPointer + m_szCurrentMapName); 
    std::string mapRealName = "";

    if (mapName != NULL)
        mapRealName = memory::read_string(mapName);

    mapNameRadar = mapRealName;

   printf("passing map: %s\n", mapRealName.c_str());


    int localteam = memory::read<int>(localplayer + s_teamnum_Offset);
    if (!localteam)
        return;

    uint64_t localPawn = memory::read<uint64_t>(sdk::module_base + dwLocalPlayerPawn);
    if (!localPawn)
        return;

    Vector localpos = memory::read<Vector>(localPawn + s_Position_Offset);
    if (localpos.x == 0.0f && localpos.y == 0.0f && localpos.z == 0.0f)
        return;

    g_dwEntList = memory::read<uint64_t>(sdk::module_base + dwEntityList);
    if (!g_dwEntList)
        return;

    playerInfos.clear();
    for (int nId = 0; nId < 64; nId++)
    {
   
                uintptr_t list_entry = memory::read<uintptr_t>(g_dwEntList + (8 * (nId & 0x7FFF) >> 9) + 16);
                if (!list_entry) continue;
                uintptr_t player = memory::read<uintptr_t>(list_entry + 112 * (nId & 0x1FF));
                if (!player) continue;

                PlayerInfo playerInfo;
                playerInfo.entity = player;
                playerInfo.isLocalPlayer = false;

                if (player == localplayer)
                {
                    playerInfo.isLocalPlayer = true;
                    localIndex = nId;
                    playerInfo.position = localpos;
                    playerInfo.teamId = localteam;
                    playerInfo.playerName = "LocalPlayer";


                    std::uint32_t playerpawn = memory::read<std::uint32_t>(player + s_dwPlayerPawn_Offset);
                    if (!playerpawn)
                        continue;
                    uintptr_t list_entry2 = memory::read<uintptr_t>(g_dwEntList + 0x8 * ((playerpawn & 0x7FFF) >> 9) + 16);
                    if (!list_entry2) continue;
                    uintptr_t pCSPlayerPawn = memory::read<uintptr_t>(list_entry2 + 112 * (playerpawn & 0x1FF));
                    auto pawn = pCSPlayerPawn;
                    vec3 eyeAngles = memory::read<vec3>(pawn + m_angEyeAngles);
                    playerInfo.eyeAngles = eyeAngles;

                    playerInfos.push_back(playerInfo);
                    continue;
                }

                int  eTeam = memory::read<int>(player + s_teamnum_Offset);

                //need to get pawn after we get the player controller for further entity reads..
                std::uint32_t playerpawn = memory::read<std::uint32_t>(player + s_dwPlayerPawn_Offset);
                if (!playerpawn)
                    continue;

                uintptr_t list_entry2 = memory::read<uintptr_t>(g_dwEntList + 0x8 * ((playerpawn & 0x7FFF) >> 9) + 16);
                if (!list_entry2) continue;
                uintptr_t pCSPlayerPawn = memory::read<uintptr_t>(list_entry2 + 112 * (playerpawn & 0x1FF));

                auto pawn = pCSPlayerPawn;
                playerInfo.teamId = eTeam;
                playerInfo.pawn = pawn;


                int eHealth = memory::read<int>(pawn + controllerHealth_Offset);
                if (eHealth <= 0 || eHealth > 100) continue;

                playerInfo.health = eHealth;

                auto m_bSpottedState = memory::read<bool>(pawn + m_entitySpottedState + m_bSpotted);
                auto spottedByMaskState = memory::read<uint32_t>(pawn + m_entitySpottedState + m_bSpottedByMask);

                playerInfo.spotted = m_bSpottedState;
                playerInfo.spottedMask = spottedByMaskState;

                uint64_t gamescene = memory::read<uint64_t>(pawn + cs2_gameSceneMode);
                uint64_t bonearray = memory::read<uint64_t>(gamescene + cs2_modelState + cs2_boneArray);

                Vector playerpos = memory::read<Vector>((DWORD64)pawn + s_Position_Offset);//pPlayerController->pos(pPlayerController->m_hPlayerPawn());
                Vector pos;

                playerInfo.position = playerpos;

                Vector head_pos = memory::read<Vector>(bonearray + 6 * 32);
                Vector head;

                int distance = static_cast<int>(distTo(localpos, playerpos) / 100);
                if (distance == 0) continue;

                playerInfo.distance = distance;

                std::string playerName = "";
                playerName = memory::read_string(player + m_iszPlayerName);
                playerInfo.playerName = playerName;


                vec3 eyeAngles = memory::read<vec3>(pawn + m_angEyeAngles);
                playerInfo.eyeAngles = eyeAngles;

     
                std::uintptr_t weapon_pointer = memory::read<std::uintptr_t>(pawn + m_pClippingWeapon);

                std::uint16_t weaponIndex = NULL;
                if (weapon_pointer)
                {
                    weaponIndex = memory::read<std::uint16_t>(weapon_pointer + m_AttributeManager + m_Item + m_iItemDefinitionIndex);
                }

                if (weaponIndex)
                {
                    std::string weapon = "";

                    switch (weaponIndex) {
                    case 7:
                        weapon = "AK47";
                        break;
                    case 9:
                        weapon = "AWP";
                        break;
                    case 11:
                        weapon = "G3SG1";
                        break;
                    case 38:
                        weapon = "SCAR-20";
                        break;
                    case 40:
                        weapon = "SCOUT";
                        break;
                    case 23:
                        weapon = "MP5-SD";
                        break;
                    case 17:
                        weapon = "MAC-10";
                        break;
                    case 24:
                        weapon = "UMP-45";
                        break;
                    case 33:
                        weapon = "MP7";
                        break;
                    case 26:
                        weapon = "BIZON";
                        break;
                    case 34:
                        weapon = "MP9";
                        break;
                    case 19:
                        weapon = "P90";
                        break;
                    case 8:
                        weapon = "AUG";
                        break;
                    case 10:
                        weapon = "FAMAS";
                        break;
                    case 13:
                        weapon = "GALIL";
                        break;
                    case 60:
                        weapon = "M4A1S";
                        break;
                    case 16:
                        weapon = "M4A4";
                        break;
                    case 39:
                        weapon = "SG553";
                        break;
                    case 27:
                        weapon = "MAG-7";
                        break;
                    case 28:
                        weapon = "NEGEV";
                        break;
                    case 29:
                        weapon = "Sawed-Off";
                        break;
                    case 35:
                        weapon = "Nova";
                        break;
                    case 14:
                        weapon = "M249";
                        break;
                    case 25:
                        weapon = "XM1014";
                        break;
                    case 63:
                        weapon = "CZ75";
                        break;
                    case 1:
                        weapon = "DEAGLE";
                        break;
                    case 2:
                        weapon = "DUALS";
                        break;
                    case 3:
                        weapon = "Five-Seven";
                        break;
                    case 4:
                        weapon = "Glock";
                        break;
                    case 32:
                        weapon = ("P2000");
                        break;
                    case 36:
                        weapon = "P250";
                        break;
                    case 64:
                        weapon = "R8";
                        break;
                    case 30:
                        weapon = "Tec-9";
                        break;
                    case 61:
                        weapon = "USP-S";
                        break;
                    case 45:
                        weapon = "Smoke";
                        break;
                    case 43:
                        weapon = "Flash";
                        break;
                    case 48:
                        weapon = "Incendiary";
                        break;
                    case 46:
                        weapon = "Molotov";
                        break;
                    case 44:
                        weapon = "HE-Grenade";
                        break;
                    case 49:
                        weapon = "C4";
                        break;
                    default:
                        weapon = "Knife";
                        break;
                    }
                    playerInfo.weaponName = weapon;
                }
            
                //fill the data
                playerInfos.push_back(playerInfo);
               
    }
    
    
    //collect data for bomb..
    bombInfos.clear();

    bool planted_c4_state = memory::read<bool>(sdk::module_base + dwPlantedC4 - 0x8);
    if (planted_c4_state != 0)
    {
        uintptr_t planted_c4 = memory::read<uintptr_t>(memory::read<uintptr_t>(sdk::module_base + dwPlantedC4));
       
        if (!planted_c4)
            return;

        uintptr_t c4Node = 0;
        Vector c4Origin = {};
        Vector c4ScreenPos = {};
        float bomb_time = 0.0f;
        float cur_time = 0.0f;
        float bomb_timer = 0.0f;
        BombInfo bombInfo{};
 
        bombInfo.entity = planted_c4;
        c4Node = memory::read<uintptr_t>(planted_c4 + cs2_gameSceneMode);

        if (c4Node)
        {
            c4Origin = memory::read<Vector>(c4Node + 0xD0); // m_vecAbsOrigin
            bombInfo.position = c4Origin;
        }

        bomb_time = memory::read<float>(planted_c4 + m_flC4Blow); // m_flC4Blow
        cur_time = memory::read<float>(globalVarsPointer + 0x30); // curtime in globalvars table
  
        bomb_timer = bomb_time - cur_time;
        bombInfo.time = bomb_timer;

        bool isDefused = memory::read<bool>(planted_c4 + m_bBeingDefused);
        float defuseTimer = 0;
        if (isDefused)
        {
            bombInfo.defusing = true;

            float defuseCountdown = memory::read<float>(planted_c4 + m_flDefuseCountDown);
            defuseTimer = defuseCountdown - cur_time;

            bombInfo.defuseTime = defuseTimer;
        }

        bombInfos.push_back(bombInfo);
    }  
}



static void dataRead() {
    LoadLibraryA("user32.dll");

        while (true) {

            collectEntityData();
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
          
        }
}


bool loadStuff()
{
        printf("[+] Waiting for CS..\n");

        int processId = NULL;
        while (!processId)
        {
            processId = MyGetProcessId(TEXT("cs2.exe"));
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        printf("[+] CS Found.. Loading..\n");

        if (!processMemory.initialize("cs2"))
        {
            if (!processMemory.initialize("cs2.exe"))
            {
                printf("[+] Failed to init game.. \n");
                Beep(1000, 300);
                Beep(1000, 300);
                Beep(1000, 300);
                Beep(1000, 300);

                return false;
            }
        }


        printf("Initialized..\n");
        sdk::module_base = processMemory.client_dll.module_base;

        if (!sdk::module_base)
        {
            printf("Cannot get Module base..\n");
            return false;
        }


    printf("Got Module base: client.dll -> 0x%p \n", sdk::module_base);
  
    std::thread radarT(radar);
    radarT.detach();

    std::thread r(dataRead);
    r.join();

    return true;

}

