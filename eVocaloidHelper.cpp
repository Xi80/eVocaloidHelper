#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

#include "paConvert.h"
#include "midi.hpp"

#pragma comment(lib,"winmm.lib")

/* Win32APIのコールバック関数 */
void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

/* MIDIパーサのコールバック関数 */
void NoteOnCallback(struct tMidiChannel* channelInfo, uint8_t channel, uint8_t noteNumber, uint8_t velocity);
void NoteOffCallback(uint8_t channel, uint8_t noteNumber);

struct tMidiCallback tMidiCb = {
	NoteOnCallback,
	NoteOffCallback,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
};

kMidi kMidiParser(&tMidiCb);

int main(int argc, char** argv) {
	uint8_t lyrics[] = {
		0x82,0xc6,0x82,0xa4,0x82,0xab,0x82,0xe5,0x82,0xa4,0x82,0xc6,0x82,0xc1,0x82,0xab,0x82,0xe5,0x82,0xab,0x82,0xe5,0x82,0xa9,0x82,0xab,0x82,0xe5,0x82,0xad
	};
	char buf[512];
	GetPAString(buf, lyrics, 512, sizeof(lyrics));
	printf("%s\n", buf);
	return 0;

	HMIDIIN hMidiIn = NULL;
	HMIDIOUT hMidiOut = NULL;
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

	printf("MIDI input started. Press Enter to stop...\n");
	getchar(); // 最初のgetcharは改行を受け取るため
	getchar(); // 実際の入力を待機するため

	if (hMidiIn) {
		midiInStop(hMidiIn);
		midiInClose(hMidiIn);
	}

	if (hMidiOut) {
		midiOutClose(hMidiOut);
	}

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
	printf("Note On  %02X %02X %02X\n", channel, noteNumber, velocity);
}

void NoteOffCallback(uint8_t channel, uint8_t noteNumber) {
	printf("Note Off %02X %02X\n", channel, noteNumber);
}