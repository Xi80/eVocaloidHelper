#pragma once

#include <cstdio>
#include <cstring>

#include "paConvert.hpp"

struct tLyrics {
	/* sysExà»äOÇÕÇ∑Ç◊ÇƒNULLèIí[ */
	uint8_t kanji[64];
	uint8_t kana[64];
	uint8_t rawPA[128];
	char strPA[128];
	int countPA;
};

class kLyricsHelper {
public:
	kLyricsHelper(void (*sendMsg)(uint8_t*, size_t), void (*sendSysEx)(uint8_t*, size_t));

	int Open(const char*);
	int Close();

	int Convert();
	int OnNoteOn();
	int OnNoteOff();

	uint8_t GetCurrentPA();
	uint8_t* GetCurrentLyrics();
	int GetCurrentIndex();

	void SetNextLine();
	void SetPrevLine();
	void SetInitLine();
private:
	void SendLyrics(int line);
	void SendHold(bool);

	void (*pSendMsg)(uint8_t*, size_t);
	void (*pSendSysEx)(uint8_t*, size_t);

	int lineCount;
	int currentLineIndex;
	int currentPAIndex;

#if defined(__APPLE__) || defined(_WIN32)
	FILE* fr;
#else

#endif
	struct tLyrics lyrics[128];
};

