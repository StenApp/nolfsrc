// ----------------------------------------------------------------------- //
//
// MODULE  : p1_timing.cpp
//
// PURPOSE : Package P1 (MODERN.md): the client's clock and frame limiter.
//
//           M-P1-05  CWinUtil::GetTime from the performance counter
//                    instead of GetTickCount (10-16 ms steps).
//           M-P1-01  NOLF's movement, animation and sound timing were
//                    tuned for about 60 fps; much faster, things went
//                    wrong, so each client frame ends by waiting until its
//                    share of a second has passed since the previous one.
//                    M-P1-23 put right what went wrong in single player
//                    (the server's clock, the animations' milliseconds,
//                    the engine clock, per-frame physics and effects), so
//                    the game is held at FramerateLimit frames a second
//                    (M-P1-25: 60, 120, 144, 240 -- the default -- or 0,
//                    none), for high refresh rate screens.  Multiplayer
//                    too (M-P6-15): it was held at 60 because weapons
//                    fired faster at a higher frame rate, which M-P1-23's
//                    animation clock put right.  The loading screen
//                    suspends it.
//           M-P1-02  The hosting client tells its own server not to limit
//                    as well (they share one loop).
//
//           Modern build only (NOLFSRC_MODERN).
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include <mmsystem.h>
#include "MsgIDs.h"
#include "cshell/p1_modern.h"

namespace
{
	bool			s_bInit				= false;
	LARGE_INTEGER	s_liFreq;			// counts per second (0: no counter)
	LARGE_INTEGER	s_liStart;			// GetTime's zero
	LARGE_INTEGER	s_liLastFrame;		// when the previous frame was let go
	bool			s_bHaveLastFrame	= false;
	volatile LONG	s_nSuspended		= 0;	// set from the loading thread

	void Init()
	{
		if (s_bInit)
			return;
		s_bInit = true;

		if (!QueryPerformanceFrequency(&s_liFreq) || s_liFreq.QuadPart <= 0)
			s_liFreq.QuadPart = 0;
		else
			QueryPerformanceCounter(&s_liStart);

		// Sleep(1) then sleeps about 1 ms instead of a whole scheduler
		// tick, so the limiter can sleep most of its wait.  For the life
		// of the process.
		timeBeginPeriod(1);
	}
}

float P1Timing_GetTime()
{
	Init();

	if (!s_liFreq.QuadPart)
		return (float)GetTickCount() / 1000.0f;

	// Seconds since the first call: a float keeps sub-millisecond
	// precision for days this way (the retail uptime-based value does not).
	LARGE_INTEGER liNow;
	QueryPerformanceCounter(&liNow);
	return (float)((double)(liNow.QuadPart - s_liStart.QuadPart) / (double)s_liFreq.QuadPart);
}

void P1Timing_SuspendFrameLimit(bool bSuspend)
{
	InterlockedExchange((LONG*)&s_nSuspended, bSuspend ? 1 : 0);
}

void P1Timing_EndFrame()
{
	Init();

	int nFPS = P1Config_FramerateLimit();

	if (!s_liFreq.QuadPart || s_nSuspended || nFPS <= 0)
	{
		// Unlimited: start afresh when the limit comes back.
		s_bHaveLastFrame = false;
		return;
	}

	LARGE_INTEGER liNow;
	QueryPerformanceCounter(&liNow);

	if (!s_bHaveLastFrame)
	{
		s_liLastFrame = liNow;
		s_bHaveLastFrame = true;
		return;
	}

	LONGLONG nPeriod = s_liFreq.QuadPart / nFPS;
	LONGLONG nTarget = s_liLastFrame.QuadPart + nPeriod;

	while (liNow.QuadPart < nTarget)
	{
		// Sleep while more than ~2 ms remain, then spin the rest.
		LONGLONG nLeftMs = ((nTarget - liNow.QuadPart) * 1000) / s_liFreq.QuadPart;
		Sleep(nLeftMs > 2 ? 1 : 0);
		QueryPerformanceCounter(&liNow);
	}

	// Keep the cadence when a frame was only a little late; after a
	// long frame (a hitch, a load), start counting from now.
	if (liNow.QuadPart - nTarget > nPeriod)
		s_liLastFrame = liNow;
	else
		s_liLastFrame.QuadPart = nTarget;
}

void P1Timing_SendDontLockServer(bool bHosting)
{
	if (!bHosting)
		return;

	// M-P1-02: the hosting client and its server share one loop, which the
	// client's limiter already paces.  (A retail server ignores the
	// message.)
	HMESSAGEWRITE hWrite = g_pLTClient->StartMessage(MID_DONT_LOCK_SERVER_FPS);
	g_pLTClient->WriteToMessageByte(hWrite, 1);
	g_pLTClient->EndMessage(hWrite);
}
