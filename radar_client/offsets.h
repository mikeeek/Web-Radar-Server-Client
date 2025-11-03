#pragma once
#include <basetsd.h>
#include <cstddef>
constexpr std::ptrdiff_t dwCSGOInput = 0x1E34D90;
constexpr std::ptrdiff_t dwEntityList = 0x1D0C9F8;
constexpr std::ptrdiff_t dwGameEntitySystem = 0x1FB15D0;
constexpr std::ptrdiff_t dwGameEntitySystem_highestEntityIndex = 0x20F0;
constexpr std::ptrdiff_t dwGameRules = 0x1E2A080;
constexpr std::ptrdiff_t dwGlobalVars = 0x1BDD078;
constexpr std::ptrdiff_t dwGlowManager = 0x1E26F18;
constexpr std::ptrdiff_t dwLocalPlayerController = 0x1E16870;
constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x1BE7DA0;
constexpr std::ptrdiff_t dwPlantedC4 = 0x1E30360;
constexpr std::ptrdiff_t dwPrediction = 0x1BE7CD0;
constexpr std::ptrdiff_t dwSensitivity = 0x1E27978;
constexpr std::ptrdiff_t dwSensitivity_sensitivity = 0x50;
constexpr std::ptrdiff_t dwViewAngles = 0x1E35440;
constexpr std::ptrdiff_t dwViewMatrix = 0x1E2AEC0;
constexpr std::ptrdiff_t dwViewRender = 0x1E2BA38;
constexpr std::ptrdiff_t dwWeaponC4 = 0x1DC7DC8;


constexpr std::ptrdiff_t m_pWeaponServices = 0x13F0; // CPlayer_WeaponServices*
constexpr DWORD64 m_szCurrentMapName = 0x0178;
constexpr std::ptrdiff_t m_flPenetration = 0x82C; // float32


constexpr DWORD64 controllerHealth_Offset = 0x34C;// 0x32C; // C_BaseEntity { // CEntityInstance
constexpr DWORD64 s_dwPlayerPawn_Offset = 0x8FC;//0x7EC;//0x7fc;//0x5dc; m_hplayerPawn
constexpr DWORD64 s_dwSanitizedName_Offset = 0x850;
constexpr DWORD64 s_Position_Offset = 0x15A0;// m_vOldOrigin ;
constexpr DWORD64 s_teamnum_Offset = 0x3EB;// 0x3bf; m_iteamnum
constexpr DWORD64 m_bDormant = 0x10B;
constexpr DWORD64 m_entitySpottedState = 0x2700;
constexpr DWORD64 m_bSpotted = 0x8;
constexpr DWORD64 m_bSpottedByMask = 0xC;
constexpr DWORD64 cs2_gameSceneMode = 0x330;//0x310; //C_BaseEntity.m_pGameSceneNode //
constexpr DWORD64 cs2_modelState = 0x190;  //CSkeletonInstance : wwCGameSceneNode -> m_modelState // 
constexpr DWORD64 cs2_boneArray = 0x80; //CModelState.m_boneArray // 
constexpr DWORD64 m_aimPunchCache = 0x1708;
constexpr DWORD64 m_iShotsFired = 0x272C;
constexpr DWORD64 m_iIDEntIndex_off = 0x3ECC;
constexpr DWORD64 m_pClippingWeapon = 0x3DE0; // C_CSWeaponBase*
constexpr DWORD64 m_AttributeManager = 0x1390;// 0x1040; //C_EconEntity  C_AttributeContainer
constexpr DWORD64 m_Item = 0x50; // C_EconItemView
constexpr DWORD64 m_iItemDefinitionIndex = 0x1BA; // uint16
constexpr DWORD64 m_iszPlayerName = 0x6E8;
constexpr DWORD64 m_pObserverServicesOff = 0x1418; // CPlayer_ObserverServices*
constexpr DWORD64  m_hObserverTargetOff = 0x44; // CHandle<C_BaseEntity>
constexpr std::ptrdiff_t m_flEmitSoundTime = 0x2720; // GameTime_t

constexpr std::ptrdiff_t m_angEyeAngles = 0x3DF0; // QAngle


constexpr std::ptrdiff_t m_bBombTicking = 0x1160; // bool
constexpr std::ptrdiff_t m_nBombSite = 0x1164; // int32
constexpr std::ptrdiff_t m_nSourceSoundscapeHash = 0x1168; // int32

constexpr std::ptrdiff_t m_flNextGlow = 0x1188; // GameTime_t
constexpr std::ptrdiff_t m_flNextBeep = 0x118C; // GameTime_t
constexpr std::ptrdiff_t m_flC4Blow = 0x1190; // GameTime_t
constexpr std::ptrdiff_t m_bCannotBeDefused = 0x1194; // bool
constexpr std::ptrdiff_t m_bHasExploded = 0x1195; // bool
constexpr std::ptrdiff_t m_flTimerLength = 0x1198; // float32
constexpr std::ptrdiff_t m_bBeingDefused = 0x119C; // bool
constexpr std::ptrdiff_t m_bTriggerWarning = 0x11A0; // float32
constexpr std::ptrdiff_t m_bExplodeWarning = 0x11A4; // float32
constexpr std::ptrdiff_t m_bC4Activated = 0x11A8; // bool
constexpr std::ptrdiff_t m_bTenSecWarning = 0x11A9; // bool
constexpr std::ptrdiff_t m_flDefuseLength = 0x11AC; // float32
constexpr std::ptrdiff_t m_flDefuseCountDown = 0x11B0; // GameTime_t
constexpr std::ptrdiff_t m_bBombDefused = 0x11B4; // bool
constexpr std::ptrdiff_t m_hBombDefuser = 0x11B8; // CHandle<C_CSPlayerPawn>

constexpr std::ptrdiff_t m_hDefuserMultimeter = 0x1698; // CHandle<C_Multimeter>
constexpr std::ptrdiff_t m_flNextRadarFlashTime = 0x169C; // GameTime_t
constexpr std::ptrdiff_t m_bRadarFlash = 0x16A0; // bool
constexpr std::ptrdiff_t m_pBombDefuser = 0x16A4; // CHandle<C_CSPlayerPawn>
constexpr std::ptrdiff_t m_fLastDefuseTime = 0x16A8; // GameTime_t
constexpr std::ptrdiff_t m_pPredictionOwner = 0x16B0; // CBasePlayerController*
constexpr std::ptrdiff_t m_vecC4ExplodeSpectatePos = 0x16B8; // Vector
constexpr std::ptrdiff_t m_vecC4ExplodeSpectateAng = 0x16C4; // QAngle
constexpr std::ptrdiff_t m_flC4ExplodeSpectateDuration = 0x16D0; // float32

static constexpr DWORD64 m_pActionTrackingServices = 0x1698; // CCSPlayerController_ActionTrackingServices*

static constexpr DWORD64 m_perRoundStats = 0x40; // C_UtlVectorEmbeddedNetworkVar<CSPerRoundStats_t>
static constexpr DWORD64 m_matchStats = 0xA8; // CSMatchStats_t
static constexpr DWORD64 m_iNumRoundKills = 0x128; // int32
static constexpr DWORD64 m_iNumRoundKillsHeadshots = 0x12C; // int32
static constexpr DWORD64 m_unTotalRoundDamageDealt = 0x130; // uint32

static constexpr DWORD64 m_pBulletServices = 0x1678; // CCSPlayer_BulletServices*






