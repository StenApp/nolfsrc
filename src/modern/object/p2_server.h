// ----------------------------------------------------------------------- //
//
// MODULE  : p2_server.h
//
// PURPOSE : P2's server half (NOLFSRC_MODERN only): console commands from
//           the host (M-P2-08), big head mode (M-P2-10), trigger boxes
//           (M-P2-11)
//
// ----------------------------------------------------------------------- //
//
// Call-outs from GameServerShell.cpp (a hot spot: calls only), GameBase.cpp,
// Attachments.cpp and CharacterHitBox.cpp, each in its own #ifdef block.

#ifndef __P2_SERVER_H__
#define __P2_SERVER_H__

class GameBase;

// CGameServerShell's constructor: the server variables.
void    P2Server_Init();

// CGameServerShell::Update, every server frame.
void    P2Server_Update();

// M-P2-08: MID_CONSOLE_COMMAND_CLIENT. Runs the string as a server console
// command if it comes from the local client (single player, or the player
// hosting the game); anyone else's is ignored.
void    P2Server_OnConsoleCommand(HCLIENT hSender, HMESSAGEREAD hMessage);

// M-P2-14: MID_P2_SETPOS. Teleports the sender's player object to the
// position, then the client's to it, as a respawn does: nothing between
// the two places is touched (a client-side move is swept through the
// world by the server, which can set off a trigger on the way, and is a
// fall if it goes down). Single player, from the local client only.
void    P2Server_OnSetPos(HCLIENT hSender, HMESSAGEREAD hMessage);

// M-P2-10: the head scale factor now (1 when big head mode is off, and
// always 1 in multiplayer).
LTFLOAT P2Server_BigHeadFactor();

// M-P2-10: CAttachments::Update -- scale an attachment's model if it sits
// on one of the head's positions (Head, Eyes, Chin).
void    P2Server_ScaleHeadAttachment(const char* pPositionName, HOBJECT hModel);

// M-P2-11: GameBase -- should this object's bounding box show because it is
// a trigger and DisplayTriggers is on? And then in which colour.
LTBOOL  P2Server_ShowTriggerBox(GameBase* pObj);
LTBOOL  P2Server_TriggerBoxColor(GameBase* pObj, LTVector* pColor, LTFLOAT* pAlpha);

#endif // __P2_SERVER_H__
