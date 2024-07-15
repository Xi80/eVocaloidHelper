//
// Created by Luna Tsukinashi on 2024/07/15.
//

#include "lyricsHelper.hpp"

kLyricsHelper::kLyricsHelper(void(*sendMsg)(uint8_t*, size_t), void(*sendSysEx)(uint8_t*, size_t))
{
	pSendMsg = sendMsg;
	pSendSysEx = sendSysEx;
}

int kLyricsHelper::Open(const char* filePath) {
#if defined(_WIN32)
	/* use fopen_s */
	if (fopen_s(&fr, filePath, "rb") != 0) {
		return -1;
	}
#elif defined(__APPLE__)
	/* use fopen */
	fr = fopen(filePath, "rb");
	if (fr == NULL) {
		return -1;
	}
#else
	/* use fatfs */
#endif

	/* ç\ë¢ëÃÇÃèâä˙âª */
	for (int i = 0; i < 128; i++) {
		memset(lyrics[i].kanji, 0x00, sizeof(lyrics[i].kanji));
		memset(lyrics[i].kana, 0x00, sizeof(lyrics[i].kana));
		memset(lyrics[i].rawPA, 0x00, sizeof(lyrics[i].rawPA));
		memset(lyrics[i].strPA, 0x00, sizeof(lyrics[i].strPA));
		lyrics[i].countPA = 0;
	}

	currentLineIndex = 0;
	currentPAIndex = 0;
	lineCount = 0;
	return 0;
}

int kLyricsHelper::Close() {

	return 0;
}

int kLyricsHelper::Convert() {
	uint8_t buffer[128];
	int line = 0;
	int length = 0;
#if defined(_WIN32) || defined(__APPLE__)
	while (fgets((char*)buffer, sizeof(buffer), fr))
#else
	/* for fatfs */
#endif
	{
		if (line % 2 == 0) {
			/* ì«Ç› */
#if defined(_WIN32)
			/* use strcpy_s */
			strcpy_s((char*)lyrics[line / 2].kana, 64, (const char*)buffer);
#else
			/* use strcpy */
			strcpy((char*)lyrics[line / 2].kana, (const char*)buffer);
#endif
		}
		else {
			/* âÃéå */
#if defined(_WIN32)
			/* use strcpy_s */
			strcpy_s((char*)lyrics[line / 2].kanji, 64, (const char*)buffer);
#else
			/* use strcpy */
			strcpy((char*)lyrics[line / 2].kanji, (const char*)buffer);
#endif
			/* PAïœä∑ */
			lyrics[line / 2].countPA = GetPAIndexArray(lyrics[line / 2].rawPA, lyrics[line / 2].kana, 128, strlen((const char*)lyrics[line / 2].kana));
			GetPAString(lyrics[line / 2].strPA, lyrics[line / 2].rawPA, 128, lyrics[line / 2].countPA);
			lineCount++;
		}
		line++;
	}

	printf("Read %d lines\n", line);


#if defined(_WIN32) || defined(__unix)
	if (fr == NULL) {
		return -1;
	}
	fclose(fr);
#else
	/* use fatfs */
#endif

	/* ç≈èâÇÃçsÇëóêMÇ∑ÇÈ */
	printf("LYRICS XFER\n");
	SendLyrics(0);

	/* î≠âπíxâÑÇ50msÇ…å≈íË */
	uint8_t msg1[3] = { 0xB0,98,12 };
	uint8_t msg2[3] = { 0xB0,99,112 };
	uint8_t msg3[3] = { 0xB0,6,1 };

	if (pSendMsg != NULL) {
		pSendMsg(msg1, 3);
		pSendMsg(msg2, 3);
		pSendMsg(msg3, 3);
	}
	return 0;
}

int kLyricsHelper::OnNoteOn()
{
	uint8_t currentPA = lyrics[currentLineIndex].rawPA[currentPAIndex];

	if (currentPA >= 0xF0) {
		/* êßå‰ï∂éö */
		currentPAIndex++;
		currentPA = lyrics[currentLineIndex].rawPA[currentPAIndex];
	}

	printf("ON:%d %d %s\n", currentLineIndex, currentPAIndex, GetPAChar(currentPA));
	currentPAIndex++;
	return 0;
}

int kLyricsHelper::OnNoteOff()
{
	if (currentPAIndex >= lyrics[currentLineIndex].countPA) {
		SetNextLine();
	}
	return 0;
}

uint8_t kLyricsHelper::GetCurrentPA()
{
	return 0;
}

uint8_t* kLyricsHelper::GetCurrentLyrics()
{
	return nullptr;
}

int kLyricsHelper::GetCurrentIndex()
{
	return currentPAIndex;
}

void kLyricsHelper::SetNextLine()
{
	currentLineIndex++;
	currentPAIndex = 0;
	if (currentLineIndex >= lineCount) {
		/* ç≈èâÇ…ñﬂÇ∑ */
		currentLineIndex = 0;
	}
	printf("Lyrics:Next\n");
	SendLyrics(currentLineIndex);
}

void kLyricsHelper::SetPrevLine()
{
	if (currentPAIndex) {
		currentPAIndex = 0;
	}
	else {
		if (currentLineIndex) {
			currentLineIndex--;
		}
		else {
			currentPAIndex = 0;
		}
	}

	printf("Lyrics:Prev\n");
	SendLyrics(currentLineIndex);
}

void kLyricsHelper::SetInitLine()
{
	currentLineIndex = 0;
	currentPAIndex = 0;

	printf("Lyrics:Init\n");
	SendLyrics(currentLineIndex);
}

void kLyricsHelper::SendLyrics(int line)
{
	printf("%s\n", lyrics[line].kanji);
	uint8_t buf[128] = { 0xF0,0x43,0x79,0x09,0x00,0x50,0x10 };
#if defined(_WIN32)
	strcpy_s((char*)&buf[7], 120, lyrics[line].strPA);
#else

#endif
	int len = strlen(lyrics[line].strPA) + 9;
	buf[len - 1] = 0xF7;

	if (pSendSysEx != NULL) {
		pSendSysEx(buf, len);
	}
}

void kLyricsHelper::SendHold(bool value)
{
	uint8_t msg[] = { 0xB0,64,0x00 };
	if (value) {
		msg[2] = 127;
	}
	else {
		msg[2] = 0;
	}
	if (pSendMsg != NULL) {
		pSendMsg(msg, 3);
	}
}
