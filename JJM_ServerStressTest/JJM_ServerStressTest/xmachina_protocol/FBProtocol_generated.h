// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_FBPROTOCOL_FBPROTOCOL_H_
#define FLATBUFFERS_GENERATED_FBPROTOCOL_FBPROTOCOL_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 24 &&
              FLATBUFFERS_VERSION_MINOR == 3 &&
              FLATBUFFERS_VERSION_REVISION == 7,
             "Non-compatible flatbuffers version included");

#include "Enum_generated.h"
#include "FBP_Bullet_generated.h"
#include "FBP_Chat_generated.h"
#include "FBP_Custom_generated.h"
#include "FBP_Enemy_generated.h"
#include "FBP_Enter_generated.h"
#include "FBP_Item_generated.h"
#include "FBP_LogIn_generated.h"
#include "FBP_NetworkLatency_generated.h"
#include "FBP_Phero_generated.h"
#include "FBP_Player_generated.h"
#include "Struct_generated.h"
#include "Transform_generated.h"

namespace FBProtocol {

enum FBsProtocolID : uint8_t {
  FBsProtocolID_None = 0,
  FBsProtocolID_CPkt_LogIn = 1,
  FBsProtocolID_SPkt_LogIn = 2,
  FBsProtocolID_CPkt_SignUp = 3,
  FBsProtocolID_SPkt_SignUp = 4,
  FBsProtocolID_CPkt_Chat = 5,
  FBsProtocolID_SPkt_Chat = 6,
  FBsProtocolID_CPkt_NetworkLatency = 7,
  FBsProtocolID_SPkt_NetworkLatency = 8,
  FBsProtocolID_CPkt_EnterLobby = 9,
  FBsProtocolID_SPkt_EnterLobby = 10,
  FBsProtocolID_CPkt_PlayGame = 11,
  FBsProtocolID_SPkt_PlayGame = 12,
  FBsProtocolID_CPkt_EnterGame = 13,
  FBsProtocolID_SPkt_EnterGame = 14,
  FBsProtocolID_CPkt_Custom = 15,
  FBsProtocolID_SPkt_Custom = 16,
  /// +-----------------------------------------------------------------------
  ///  PLAYER PACKET HEADER PROTOCOL TYPE
  /// -----------------------------------------------------------------------+
  FBsProtocolID_CPkt_NewPlayer = 17,
  FBsProtocolID_SPkt_NewPlayer = 18,
  FBsProtocolID_CPkt_RemovePlayer = 19,
  FBsProtocolID_SPkt_RemovePlayer = 20,
  FBsProtocolID_CPkt_PlayerOnSkill = 21,
  FBsProtocolID_SPkt_PlayerOnSkill = 22,
  FBsProtocolID_CPkt_Player_Transform = 23,
  FBsProtocolID_SPkt_Player_Transform = 24,
  FBsProtocolID_CPkt_Player_Animation = 25,
  FBsProtocolID_SPkt_Player_Animation = 26,
  FBsProtocolID_CPkt_Player_Weapon = 27,
  FBsProtocolID_SPkt_Player_Weapon = 28,
  FBsProtocolID_CPkt_Player_AimRotation = 29,
  FBsProtocolID_SPkt_Player_AimRotation = 30,
  FBsProtocolID_CPkt_Player_State = 31,
  FBsProtocolID_SPkt_Player_State = 32,
  /// +-----------------------------------------------------------------------
  ///  MONSTER PACKET HEADER PROTOCOL TYPE
  /// -----------------------------------------------------------------------+
  FBsProtocolID_CPkt_NewMonster = 33,
  FBsProtocolID_SPkt_NewMonster = 34,
  FBsProtocolID_CPkt_DeadMonster = 35,
  FBsProtocolID_SPkt_DeadMonster = 36,
  FBsProtocolID_CPkt_RemoveMonster = 37,
  FBsProtocolID_SPkt_RemoveMonster = 38,
  FBsProtocolID_CPkt_Monster_Transform = 39,
  FBsProtocolID_SPkt_Monster_Transform = 40,
  FBsProtocolID_CPkt_Monster_HP = 41,
  FBsProtocolID_SPkt_Monster_HP = 42,
  FBsProtocolID_CPkt_Monster_State = 43,
  FBsProtocolID_SPkt_Monster_State = 44,
  FBsProtocolID_CPkt_Monster_Target = 45,
  FBsProtocolID_SPkt_Monster_Target = 46,
  /// +-----------------------------------------------------------------------
  ///  PHERO PACKET HEADER PROTOCOL TYPE
  /// -----------------------------------------------------------------------+
  FBsProtocolID_CPkt_GetPhero = 47,
  FBsProtocolID_SPkt_GetPhero = 48,
  /// +-----------------------------------------------------------------------
  ///  BULLET PACKET HEADER PROTOCOL TYPE
  /// -----------------------------------------------------------------------+
  FBsProtocolID_CPkt_Bullet_OnShoot = 49,
  FBsProtocolID_SPkt_Bullet_OnShoot = 50,
  FBsProtocolID_CPkt_Bullet_OnHitEnemy = 51,
  FBsProtocolID_SPkt_Bullet_OnHitEnemy = 52,
  FBsProtocolID_CPkt_Bullet_OnHitExpEnemy = 53,
  FBsProtocolID_SPkt_Bullet_OnHitExpEnemy = 54,
  FBsProtocolID_CPkt_Bullet_OnCollision = 55,
  FBsProtocolID_SPkt_Bullet_OnCollision = 56,
  /// +-----------------------------------------------------------------------
  ///  ITEM PACKET HEADER PROTOCOL TYPE
  /// -----------------------------------------------------------------------+
  FBsProtocolID_CPkt_Item_Interact = 57,
  FBsProtocolID_SPkt_Item_Interact = 58,
  FBsProtocolID_CPkt_Item_ThrowAway = 59,
  FBsProtocolID_SPkt_Item_ThrowAway = 60,
  FBsProtocolID_MIN = FBsProtocolID_None,
  FBsProtocolID_MAX = FBsProtocolID_SPkt_Item_ThrowAway
};

inline const FBsProtocolID (&EnumValuesFBsProtocolID())[61] {
  static const FBsProtocolID values[] = {
    FBsProtocolID_None,
    FBsProtocolID_CPkt_LogIn,
    FBsProtocolID_SPkt_LogIn,
    FBsProtocolID_CPkt_SignUp,
    FBsProtocolID_SPkt_SignUp,
    FBsProtocolID_CPkt_Chat,
    FBsProtocolID_SPkt_Chat,
    FBsProtocolID_CPkt_NetworkLatency,
    FBsProtocolID_SPkt_NetworkLatency,
    FBsProtocolID_CPkt_EnterLobby,
    FBsProtocolID_SPkt_EnterLobby,
    FBsProtocolID_CPkt_PlayGame,
    FBsProtocolID_SPkt_PlayGame,
    FBsProtocolID_CPkt_EnterGame,
    FBsProtocolID_SPkt_EnterGame,
    FBsProtocolID_CPkt_Custom,
    FBsProtocolID_SPkt_Custom,
    FBsProtocolID_CPkt_NewPlayer,
    FBsProtocolID_SPkt_NewPlayer,
    FBsProtocolID_CPkt_RemovePlayer,
    FBsProtocolID_SPkt_RemovePlayer,
    FBsProtocolID_CPkt_PlayerOnSkill,
    FBsProtocolID_SPkt_PlayerOnSkill,
    FBsProtocolID_CPkt_Player_Transform,
    FBsProtocolID_SPkt_Player_Transform,
    FBsProtocolID_CPkt_Player_Animation,
    FBsProtocolID_SPkt_Player_Animation,
    FBsProtocolID_CPkt_Player_Weapon,
    FBsProtocolID_SPkt_Player_Weapon,
    FBsProtocolID_CPkt_Player_AimRotation,
    FBsProtocolID_SPkt_Player_AimRotation,
    FBsProtocolID_CPkt_Player_State,
    FBsProtocolID_SPkt_Player_State,
    FBsProtocolID_CPkt_NewMonster,
    FBsProtocolID_SPkt_NewMonster,
    FBsProtocolID_CPkt_DeadMonster,
    FBsProtocolID_SPkt_DeadMonster,
    FBsProtocolID_CPkt_RemoveMonster,
    FBsProtocolID_SPkt_RemoveMonster,
    FBsProtocolID_CPkt_Monster_Transform,
    FBsProtocolID_SPkt_Monster_Transform,
    FBsProtocolID_CPkt_Monster_HP,
    FBsProtocolID_SPkt_Monster_HP,
    FBsProtocolID_CPkt_Monster_State,
    FBsProtocolID_SPkt_Monster_State,
    FBsProtocolID_CPkt_Monster_Target,
    FBsProtocolID_SPkt_Monster_Target,
    FBsProtocolID_CPkt_GetPhero,
    FBsProtocolID_SPkt_GetPhero,
    FBsProtocolID_CPkt_Bullet_OnShoot,
    FBsProtocolID_SPkt_Bullet_OnShoot,
    FBsProtocolID_CPkt_Bullet_OnHitEnemy,
    FBsProtocolID_SPkt_Bullet_OnHitEnemy,
    FBsProtocolID_CPkt_Bullet_OnHitExpEnemy,
    FBsProtocolID_SPkt_Bullet_OnHitExpEnemy,
    FBsProtocolID_CPkt_Bullet_OnCollision,
    FBsProtocolID_SPkt_Bullet_OnCollision,
    FBsProtocolID_CPkt_Item_Interact,
    FBsProtocolID_SPkt_Item_Interact,
    FBsProtocolID_CPkt_Item_ThrowAway,
    FBsProtocolID_SPkt_Item_ThrowAway
  };
  return values;
}

inline const char * const *EnumNamesFBsProtocolID() {
  static const char * const names[62] = {
    "None",
    "CPkt_LogIn",
    "SPkt_LogIn",
    "CPkt_SignUp",
    "SPkt_SignUp",
    "CPkt_Chat",
    "SPkt_Chat",
    "CPkt_NetworkLatency",
    "SPkt_NetworkLatency",
    "CPkt_EnterLobby",
    "SPkt_EnterLobby",
    "CPkt_PlayGame",
    "SPkt_PlayGame",
    "CPkt_EnterGame",
    "SPkt_EnterGame",
    "CPkt_Custom",
    "SPkt_Custom",
    "CPkt_NewPlayer",
    "SPkt_NewPlayer",
    "CPkt_RemovePlayer",
    "SPkt_RemovePlayer",
    "CPkt_PlayerOnSkill",
    "SPkt_PlayerOnSkill",
    "CPkt_Player_Transform",
    "SPkt_Player_Transform",
    "CPkt_Player_Animation",
    "SPkt_Player_Animation",
    "CPkt_Player_Weapon",
    "SPkt_Player_Weapon",
    "CPkt_Player_AimRotation",
    "SPkt_Player_AimRotation",
    "CPkt_Player_State",
    "SPkt_Player_State",
    "CPkt_NewMonster",
    "SPkt_NewMonster",
    "CPkt_DeadMonster",
    "SPkt_DeadMonster",
    "CPkt_RemoveMonster",
    "SPkt_RemoveMonster",
    "CPkt_Monster_Transform",
    "SPkt_Monster_Transform",
    "CPkt_Monster_HP",
    "SPkt_Monster_HP",
    "CPkt_Monster_State",
    "SPkt_Monster_State",
    "CPkt_Monster_Target",
    "SPkt_Monster_Target",
    "CPkt_GetPhero",
    "SPkt_GetPhero",
    "CPkt_Bullet_OnShoot",
    "SPkt_Bullet_OnShoot",
    "CPkt_Bullet_OnHitEnemy",
    "SPkt_Bullet_OnHitEnemy",
    "CPkt_Bullet_OnHitExpEnemy",
    "SPkt_Bullet_OnHitExpEnemy",
    "CPkt_Bullet_OnCollision",
    "SPkt_Bullet_OnCollision",
    "CPkt_Item_Interact",
    "SPkt_Item_Interact",
    "CPkt_Item_ThrowAway",
    "SPkt_Item_ThrowAway",
    nullptr
  };
  return names;
}

inline const char *EnumNameFBsProtocolID(FBsProtocolID e) {
  if (::flatbuffers::IsOutRange(e, FBsProtocolID_None, FBsProtocolID_SPkt_Item_ThrowAway)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesFBsProtocolID()[index];
}

}  // namespace FBProtocol

#endif  // FLATBUFFERS_GENERATED_FBPROTOCOL_FBPROTOCOL_H_
