#include "midi.hpp"

void kMidi::Parse(uint8_t message) {
	if (isSystemExclusiveReceiving) {
		systemExclusiveBuffer[systemExclusiveBufferLength++] = message;
		if (message == MidiMessageType_EndSystemExclusive) {
			isSystemExclusiveReceiving = false;
			decodeSystemExclusive();
		}
	}
	else {
		if (message & 0x80) {
			/* ステータスバイト */
			if (message > MidiMessageType_EndSystemExclusive) return;

			if (message == MidiMessageType_BeginSystemExclusive) {
				systemExclusiveBufferLength = 0;
				systemExclusiveBuffer[systemExclusiveBufferLength++] = message;
				isSystemExclusiveReceiving = true;
				return;
			}

			threeBytesMessage = false;
			formattedMessage[0] = message;
		}
		else {
			/* データバイト */

			if (threeBytesMessage) {
				formattedMessage[2] = message;
				decodeMidiMessage();
				return;
			}

			uint8_t messageType = (formattedMessage[0] & 0xF0);

			formattedMessage[1] = message;

			switch (messageType) {
			case MidiMessageType_NoteOff:
			case MidiMessageType_NoteOn:
			case MidiMessageType_PolyphonicKeyPressure:
			case MidiMessageType_ControlChange:
				threeBytesMessage = true;
				break;
			case MidiMessageType_ProgramChange:
			case MidiMessageType_ChannelPressure:
				decodeMidiMessage();
				break;
			case MidiMessageType_PitchBend:
				threeBytesMessage = true;
				break;
			case MidiMessageType_MidiTimeCode:
				/* Do Nothing */
				break;
			case MidiMessageType_SongPosition:
			case MidiMessageType_SongSelect:
				threeBytesMessage = true;
				break;
			default:
				/* Illegal Messagges */
				break;
			}
		}
	}

	return;
}

void kMidi::ResetAllParameters(uint8_t targetChannel) {
	channels[targetChannel].rpnMsb = 0x7F;
	channels[targetChannel].rpnLsb = 0x7F;
	channels[targetChannel].bank = 0;
	channels[targetChannel].programNumber = 0;
	channels[targetChannel].modulation = 0;
	channels[targetChannel].channelVolume = 100;
	channels[targetChannel].pan = 64;
	channels[targetChannel].expression = 127;
	channels[targetChannel].sustainPedal = 0;
	channels[targetChannel].pitchBend = 0;
	channels[targetChannel].pitchBendSensitivity = 2;
}

void kMidi::decodeSystemExclusive(void) {
	/* リセット系 */
	if (memcmp(cRESET_GM, systemExclusiveBuffer, sizeof(cRESET_GM)) == 0) {
		if (cb->reset != NULL) {
			cb->reset(MidiModuleMode_GM);
		}
	}

	if (memcmp(cRESET_XG, systemExclusiveBuffer, sizeof(cRESET_XG)) == 0) {
		if (cb->reset != NULL) {
			cb->reset(MidiModuleMode_XG);
		}
	}

	if (memcmp(cRESET_GS, systemExclusiveBuffer, sizeof(cRESET_GS)) == 0) {
		if (cb->reset != NULL) {
			cb->reset(MidiModuleMode_GS);
		}
	}

	/* XGパラメータセット/リクエスト */
	if (systemExclusiveBuffer[1] == MANUFACTURE_ID) {
		if ((systemExclusiveBuffer[2] & 0xF0) == 0x10) {
			if (systemExclusiveBuffer[3] == MODEL_ID) {
				if (cb->parameterSet != NULL) {
					size_t length = systemExclusiveBufferLength - 8;
					cb->parameterSet(systemExclusiveBuffer[4], systemExclusiveBuffer[5],
						systemExclusiveBuffer[6], &systemExclusiveBuffer[7],
						length);
				}
			}
		}
		else if ((systemExclusiveBuffer[2] & 0xF0) == 0x30) {
			if (systemExclusiveBuffer[3] == MODEL_ID) {
				if (cb->parameterGet != NULL) {
					cb->parameterGet(systemExclusiveBuffer[4], systemExclusiveBuffer[5],
						systemExclusiveBuffer[6]);
				}
			}
		}
	}

	/* マスターボリューム */
	if (memcmp(systemExclusiveBuffer, cMASTER_VOL,
		std::min(systemExclusiveBufferLength, sizeof(cMASTER_VOL))) == 0) {
		if (cb->setMasterVolume != NULL) {
			uint16_t volume =
				((uint16_t)systemExclusiveBuffer[6] << 8) | systemExclusiveBuffer[5];
			cb->setMasterVolume(volume);
		}
	}
}

void kMidi::decodeMidiMessage(void) {
	uint8_t messageType = (formattedMessage[0] & 0xF0);
	uint8_t targetChannel = (formattedMessage[0] & 0x0F);

	switch (messageType) {
	case MidiMessageType_NoteOn:
		/* Velocity=0のノートオンはノートオフとして解釈する */
		if (formattedMessage[2]) {
			if (cb->noteOn != NULL)
				cb->noteOn(&channels[targetChannel], targetChannel,
					formattedMessage[1], formattedMessage[2]);
			break;
		}
	case MidiMessageType_NoteOff:
		if (cb->noteOff != NULL) {
			cb->noteOff(targetChannel, formattedMessage[1]);
		}
		break;
	case MidiMessageType_ControlChange:
		switch (formattedMessage[1]) {
		case MidiControlChangeType_BankSelect:
			channels[targetChannel].bank = formattedMessage[2];
			break;
		case MidiControlChangeType_DataEntry:
			/* RPNヌルであればスキップ*/
			if (channels[targetChannel].rpnMsb != 0x7F &&
				channels[targetChannel].rpnLsb != 0x7F) {
				uint16_t rpn = (channels[targetChannel].rpnMsb << 8) |
					channels[targetChannel].rpnLsb;

				if (rpn == (uint16_t)MidiRpnTypePitchBendSensitivity) {
					channels[targetChannel].pitchBendSensitivity =
						formattedMessage[2];
				}

				if (cb->rpnSet != NULL) {
					cb->rpnSet(channels[targetChannel].rpnMsb,
						channels[targetChannel].rpnLsb, formattedMessage[2]);
				}
				/* RPNをクリア */
				channels[targetChannel].rpnMsb = 0x7F;
				channels[targetChannel].rpnLsb = 0x7F;
			}
			break;
		case MidiControlChangeType_RpnMsb:
			channels[targetChannel].rpnMsb = formattedMessage[2];
			break;
		case MidiControlChangeType_RpnLsb:
			channels[targetChannel].rpnLsb = formattedMessage[2];
			break;
		case MidiControlChangeType_Modulation:
			channels[targetChannel].modulation = formattedMessage[2];
			break;
		case MidiControlChangeType_ChannelVolume:
			channels[targetChannel].channelVolume = formattedMessage[2];
			break;
		case MidiControlChangeType_Pan:
			channels[targetChannel].pan = formattedMessage[2];
			break;
		case MidiControlChangeType_Expression:
			channels[targetChannel].expression = formattedMessage[2];
			break;
		case MidiControlChangeType_SustainPedal:
			channels[targetChannel].sustainPedal = formattedMessage[2];
			break;
		default:
			break;
		}
		if (cb->controlChange != NULL) {
			cb->controlChange(&channels[targetChannel], targetChannel,
				formattedMessage[1], formattedMessage[2]);
		}
		break;
	case MidiMessageType_ProgramChange:
		channels[targetChannel].programNumber = formattedMessage[1];
		if (cb->programChange != NULL) {
			cb->programChange(targetChannel, channels[targetChannel].bank,
				formattedMessage[1]);
		}
		break;
	case MidiMessageType_PitchBend:
		if (cb->pitchBend != NULL) {
			int16_t bend =
				((int16_t)formattedMessage[2] - 0x40) << 7 | formattedMessage[1];
			channels[targetChannel].pitchBend = bend;
			cb->pitchBend(&channels[targetChannel], targetChannel, bend);
		}
		break;
	default:
		/* Illegal Messagges */
		break;
	}
}
