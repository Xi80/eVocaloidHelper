#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

#include "lyricsHelper.hpp"
#include "midi.hpp"

#pragma comment(lib,"winmm.lib")

HMIDIIN hMidiIn = NULL;
HMIDIOUT hMidiOut = NULL;

/* Win32APIのコールバック関数 */
void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

/* MIDIパーサのコールバック関数 */
void NoteOnCallback(struct tMidiChannel* channelInfo, uint8_t channel, uint8_t noteNumber, uint8_t velocity);
void NoteOffCallback(uint8_t channel, uint8_t noteNumber);
void ControlChangeCallback(struct tMidiChannel* channelInfo, uint8_t channel, uint8_t number, uint8_t value);
void PitchBendCallback(struct tMidiChannel* channelInfo, uint8_t channel, int16_t value);

struct tMidiCallback tMidiCb = {
	NoteOnCallback,
	NoteOffCallback,
	PitchBendCallback,
	ControlChangeCallback,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
};

kMidi kMidiParser(&tMidiCb);

void SendSysExMessage(uint8_t* sysexData, size_t length) {
	MIDIHDR midiHdr;
	ZeroMemory(&midiHdr, sizeof(MIDIHDR));
	midiHdr.lpData = (LPSTR)sysexData;
	midiHdr.dwBufferLength = length;
	midiHdr.dwBytesRecorded = length;
	midiHdr.dwFlags = 0;

	midiOutPrepareHeader(hMidiOut, &midiHdr, sizeof(MIDIHDR));
	midiOutLongMsg(hMidiOut, &midiHdr, sizeof(MIDIHDR));
	midiOutUnprepareHeader(hMidiOut, &midiHdr, sizeof(MIDIHDR));
}

void SendShortMessage(uint8_t* msg, size_t length) {
	DWORD message = msg[0];
	message |= (msg[1] << 8);
	message |= (((length > 2) ? msg[2] : 0) << 16);
	midiOutShortMsg(hMidiOut, message);
}

kLyricsHelper helper(SendShortMessage, SendSysExMessage);

int main(int argc, char** argv) {

	helper.Open("rufuran.txt");


	UINT numInDevs, numOutDevs;
	MMRESULT result;
	UINT inDevID = -1, outDevID = -1;
	MIDIINCAPS inCaps;
	MIDIOUTCAPS outCaps;

	numInDevs = midiInGetNumDevs();
	if (numInDevs == 0) {
		printf("No MIDI input devices available.\n");
	}
	else {
		printf("Available MIDI input devices:\n");
		for (UINT i = 0; i < numInDevs; i++) {
			midiInGetDevCaps(i, &inCaps, sizeof(MIDIINCAPS));
			printf("%u: %ls\n", i, inCaps.szPname);
		}
		printf("Select MIDI input device: ");
		scanf_s("%u", &inDevID);
	}

	numOutDevs = midiOutGetNumDevs();
	if (numOutDevs == 0) {
		printf("No MIDI output devices available.\n");
	}
	else {
		printf("Available MIDI output devices:\n");
		for (UINT i = 0; i < numOutDevs; i++) {
			midiOutGetDevCaps(i, &outCaps, sizeof(MIDIOUTCAPS));
			printf("%u: %ls\n", i, outCaps.szPname);
		}
		printf("Select MIDI output device: ");
		scanf_s("%u", &outDevID);
	}

	if (inDevID != -1) {
		result = midiInOpen(&hMidiIn, inDevID, (DWORD_PTR)MidiInProc, 0, CALLBACK_FUNCTION);
		if (result != MMSYSERR_NOERROR) {
			printf("Failed to open MIDI input device.\n");
			return 1;
		}

		result = midiInStart(hMidiIn);
		if (result != MMSYSERR_NOERROR) {
			printf("Failed to start MIDI input.\n");
			midiInClose(hMidiIn);
			return 1;
		}
	}

	if (outDevID != -1) {
		result = midiOutOpen(&hMidiOut, outDevID, 0, 0, CALLBACK_NULL);
		if (result != MMSYSERR_NOERROR) {
			printf("Failed to open MIDI output device.\n");
			if (hMidiIn) {
				midiInStop(hMidiIn);
				midiInClose(hMidiIn);
			}
			return 1;
		}
	}
	/* XGシステムオン */
	uint8_t xgon[] = {
		0xF0,0x43,0x10,0x4C,0x00,0x00,0x7E,0x00,0xF7
	};
	SendSysExMessage(xgon, sizeof(xgon));
	printf("MIDI input started. Press Enter to stop...\n");
	helper.Convert();

	getchar(); // 最初のgetcharは改行を受け取るため
	getchar(); // 実際の入力を待機するため

	/* オールサウンドオフ */
	uint8_t aso[] = { 0xB0,120,0 };
	for (int i = 0; i < 16; i++) {
		aso[0] = 0xB0 | i;
		SendShortMessage(aso, 3);
	}

	if (hMidiIn) {
		midiInStop(hMidiIn);
		midiInClose(hMidiIn);
	}

	if (hMidiOut) {
		midiOutClose(hMidiOut);
	}

	helper.Close();

	return 0;
}

// MIDI入力コールバック関数
void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
	switch (wMsg) {
	case MIM_DATA:
	{
		DWORD midiMessage = (DWORD)dwParam1;
		BYTE status = (BYTE)(midiMessage & 0xFF);
		BYTE data1 = (BYTE)((midiMessage >> 8) & 0xFF);
		BYTE data2 = (BYTE)((midiMessage >> 16) & 0xFF);
		kMidiParser.Parse(status);
		kMidiParser.Parse(data1);
		kMidiParser.Parse(data2);
	}
	break;
	case MIM_LONGDATA:
		// システムエクスクルーシブメッセージなどの長いメッセージの処理
		printf("Long MIDI message received.\n");
		break;
	case MIM_ERROR:
		printf("MIDI input error.\n");
		break;
	default:
		break;
	}
}

void NoteOnCallback(struct tMidiChannel* channelInfo, uint8_t channel, uint8_t noteNumber, uint8_t velocity) {
	uint8_t message[3];
	message[0] = 0x90 | (channel & 0x0F);
	message[1] = noteNumber;
	message[2] = 127;
	SendShortMessage(message, 3);
	helper.OnNoteOn();
}

void NoteOffCallback(uint8_t channel, uint8_t noteNumber) {
	uint8_t message[3];
	message[0] = 0x80 | (channel & 0x0F);
	message[1] = noteNumber;
	message[2] = 0;
	SendShortMessage(message, 3);
	helper.OnNoteOff();
}

void ControlChangeCallback(struct tMidiChannel* channelInfo, uint8_t channel, uint8_t number, uint8_t value) {
	uint8_t message[3];
	switch (number) {
	case 112:
		if (value)helper.SetPrevLine();
		break;
	case 113:
		if (value)helper.SetNextLine();
		break;
	case 114:
		if (value)helper.SetInitLine();
		break;
	default:
		message[0] = 0xB0 | (channel & 0x0F);
		message[1] = number;
		message[2] = value;
		SendShortMessage(message, 3);
		break;
	}
}

void PitchBendCallback(struct tMidiChannel* channelInfo, uint8_t channel, int16_t value) {
	uint16_t pitchBend = (uint16_t)(value + 0x2000) & 0x3FFF;

	uint8_t message[3];
	message[0] = 0xE0 | (channel & 0x0F);        // ステータスバイト (ピッチベンド + チャンネル)
	message[1] = pitchBend & 0x7F;               // LSB
	message[2] = (pitchBend >> 7) & 0x7F;        // MSB
	SendShortMessage(message, 3);
}