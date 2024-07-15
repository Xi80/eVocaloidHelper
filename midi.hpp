#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>

#define CH_COUNT 16
#define SYSEX_BUFFER_LENGTH 128
#define MANUFACTURE_ID 0x43
#define MODEL_ID 0x4C

struct tMidiChannel {
	uint8_t rpnMsb = 0x7F, rpnLsb = 0x7F;
	uint8_t bank = 0, programNumber = 0;
	/* コントロールチェンジ */
	uint8_t modulation = 0, channelVolume = 100, pan = 64, expression = 127,
		sustainPedal = 0;

	int16_t pitchBend = 0;
	int8_t pitchBendSensitivity = 2;
};

enum eMidiModuleMode {
	MidiModuleMode_GM = 0,
	MidiModuleMode_XG,
	MidiModuleMode_GS
};

enum eMidiMessageType {
	MidiMessageType_NoteOff = 0x80,
	MidiMessageType_NoteOn = 0x90,
	MidiMessageType_PolyphonicKeyPressure = 0xA0,
	MidiMessageType_ControlChange = 0xB0,
	MidiMessageType_ProgramChange = 0xC0,
	MidiMessageType_ChannelPressure = 0xD0,
	MidiMessageType_PitchBend = 0xE0,
	MidiMessageType_BeginSystemExclusive = 0xF0,
	MidiMessageType_MidiTimeCode = 0xF1,
	MidiMessageType_SongPosition = 0xF2,
	MidiMessageType_SongSelect = 0xF3,
	MidiMessageType_EndSystemExclusive = 0xF7,
	MidiMessageType_ActiveSensing = 0xFE,
};

enum eMidiControlChangeType {
	MidiControlChangeType_BankSelect = 0,
	MidiControlChangeType_Modulation = 1,
	MidiControlChangeType_DataEntry = 6,
	MidiControlChangeType_ChannelVolume = 7,
	MidiControlChangeType_Pan = 10,
	MidiControlChangeType_Expression = 11,
	MidiControlChangeType_SustainPedal = 64,
	MidiControlChangeType_RpnMsb = 100,
	MidiControlChangeType_RpnLsb = 101,
	/* 以下チャネルモードメッセージ */
	MidiControlChangeType_AllSoundOff = 120,
	MidiControlChangeType_ResetAllController = 121,
	MidiControlChangeType_AllNoteOff = 123
};

enum eMidiRpnType { MidiRpnTypePitchBendSensitivity = 0x0000 };

/* 定数(エクスクルーシブ) */
static const uint8_t cRESET_GM[] = { 0xF0, 0x7E, 0x7F, 0x09, 0x01, 0xF7 };

static const uint8_t cRESET_GS[] = { 0xF0, 0x41, 0x10, 0x42, 0x12, 0x40,
									0x00, 0x7F, 0x00, 0x41, 0xF7 };
static const uint8_t cRESET_XG[] = { 0xF0, 0x43, 0x10, 0x4C, 0x00,
									0x00, 0x7E, 0x00, 0xF7 };

static const uint8_t cMASTER_VOL[] = { 0xF0, 0x7E, 0x7F, 0x04, 0x01 };

struct tMidiCallback {
	/* channel, note, velocity */
	void (*noteOn)(struct tMidiChannel*, uint8_t, uint8_t, uint8_t);
	/* channel, note */
	void (*noteOff)(uint8_t, uint8_t);
	/* channel, value */
	void (*pitchBend)(struct tMidiChannel*, uint8_t, int16_t);
	/* channel, number, value */
	void (*controlChange)(struct tMidiChannel*, uint8_t, uint8_t, uint8_t);
	/* channel, number, value */
	void (*rpnSet)(uint8_t, uint8_t, uint8_t);
	/* channel, bank, value */
	void (*programChange)(uint8_t, uint8_t, uint8_t);
	/* format */
	void (*reset)(eMidiModuleMode);
	/* address(h), address(m), address(l), value[], length*/
	void (*parameterSet)(uint8_t, uint8_t, uint8_t, uint8_t*, size_t);
	/* address(h), address(m), address(l)*/
	void (*parameterGet)(uint8_t, uint8_t, uint8_t);
	/* value*/
	void (*setMasterVolume)(uint16_t);
};
class kMidi {
public:
	kMidi(tMidiCallback* callbacks) { cb = callbacks; }

	void Parse(uint8_t);

	void ResetAllParameters(uint8_t);

private:
	tMidiChannel channels[CH_COUNT];

	bool isSystemExclusiveReceiving = false;
	uint8_t systemExclusiveBuffer[SYSEX_BUFFER_LENGTH];
	size_t systemExclusiveBufferLength = 0;

	uint8_t formattedMessage[3];
	bool threeBytesMessage = false;

	tMidiCallback* cb = NULL;

	void decodeSystemExclusive(void);
	void decodeMidiMessage(void);
};