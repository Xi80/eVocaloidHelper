#include "paConvert.hpp"
#include "paTable.hpp"

static int isMultiChar(uint8_t c);
static int inTable(uint16_t token);
static int getTypeN(uint8_t current, uint8_t next);


int GetPAString(char* dest, uint8_t* paArray, size_t maxDestLen, size_t srcLen)
{
	int strLen = 0;
	const char* token;

	if (srcLen <= 0) {
		/* �K�؂ȉ̎��f�[�^�Ȃ� */
		return -1;
	}

	for (int i = 0; i < srcLen; i++) {
		/* ���䕶�� */
		if (paArray[i] >= 0xF0)continue;

		if (paArray[i] & 0x80) {
			token = cPATableMulti[paArray[i] & 0x7F];
		}
		else {
			token = cPATableSingle[paArray[i]];
		}

		int tokenLength = strlen(token);
#ifdef _WIN32
		memcpy_s(dest + strLen, maxDestLen, token, tokenLength);
#else
		memcpy(dest + strLen, token, tokenLength);
#endif
		strLen += tokenLength;
		dest[strLen++] = ',';
	}
	dest[strLen - 1] = 0x00;
	return strLen;
}

int GetPASysEx(uint8_t* dest, char* srcPA, size_t maxDestLen, size_t srcLen)
{
	return 0;
}

const char* GetPAChar(uint8_t index)
{
	if (index & 0x80) {
		return cPATableMulti[index & 0x7F];
	}
	else {
		return cPATableSingle[index];
	}
}

int GetPAIndexArray(uint8_t* dest, uint8_t* src, size_t maxDestLen, size_t srcLen) {
	/* ���p�����ƂЂ炪�ȈȊO�ɂ͊�{�I�ɑΉ����Ȃ� */
	int tokenCount = 0;
	int idx = -1;

	for (int i = 0; i < srcLen; i++) {
		if (isMultiChar(src[i])) {
			/* 2�o�C�g���� */
			uint8_t firstChar = src[++i];

			/* �����g�[�N������ */
			if ((i < (srcLen - 2)) && isMultiChar(src[i + 1])) {
				/* �����g�[�N���łȂ��\��������̂Ń|�C���^��i�߂Ă͂����Ȃ� */
				uint8_t secondChar = src[i + 2];
				uint16_t token = ((uint16_t)secondChar & 0x00FF) | ((uint16_t)firstChar << 8);

				/* �e�[�u������ */

				if ((idx = inTable(token)) >= 0) {
					dest[tokenCount++] = 0x80 | (uint8_t)idx;
					i += 2;
					continue;
				}

				/* ������Ȃ� */
				if ((idx = inTable(firstChar)) >= 0) {
					/* �� */
					if (idx == 72) {
						int idx2 = inTable(secondChar);
						dest[tokenCount++] = getTypeN(idx, idx2);
					}
					else {
						dest[tokenCount++] = (uint8_t)idx;
					}

				}
			}
			else {
				/* ����������2�o�C�g�����łȂ����ߔ��ʂ��Ȃ� */
				if ((idx = inTable((uint16_t)firstChar)) >= 0) {
					dest[tokenCount++] = (uint8_t)idx;
				}
			}
		}
		else {
			/* "|","-",","�ȊO�̕����͓ǂݔ�΂� */
			switch (src[i]) {
			case '|':
				dest[tokenCount++] = 0xF0;
				break;
			case '-':
				dest[tokenCount++] = 0xF1;
				break;
			case ',':
				dest[tokenCount++] = 0xF2;
				break;
			default:
				break;
			}
		}
	}
	return tokenCount;
}

static int isMultiChar(uint8_t c) {
	return ((c > 0x80 && c < 0xA0) || (c >= 0xE0 && c <= 0xEF));
}

static int inTable(uint16_t token) {
	if (token & 0xFF00) {
		for (int j = 0; j < sizeof(cKanaTableMulti) / sizeof(cKanaTableMulti[0]); j++) {
			if (token == cKanaTableMulti[j]) {
				return j;
			}
		}
	}
	else {
		for (int j = 0; j < sizeof(cKanaTableSingle) / sizeof(cKanaTableSingle[0]); j++) {
			if (token == cKanaTableSingle[j]) {
				return j;
			}
		}
	}
	return -1;
}

static int getTypeN(uint8_t current, uint8_t next) {
	int type = -1;
	char c1;
	char c2[2];

	if (next & 0xFF00) {
		c1 = cPATableMulti[next][0];
		c2[0] = cPATableMulti[next][0];
		c2[1] = cPATableMulti[next][1];
	}
	else {
		c1 = cPATableSingle[next][0];
		c2[0] = cPATableSingle[next][0];
		c2[1] = cPATableSingle[next][1];
	}

	/* �ꖖ? */
	if (next >= 0xF0) {
		type = 0;
	}

	/* �ꉹ/���ꉹ/���C�� */
	if (type == -1 && (c1 == 'a' || c1 == 'i' || c1 == 'M' || c1 == 'e' || c1 == 'o')) {
		type = 0;
	}

	if (type == -1 && (c1 == 's' || c1 == 'j' || c1 == 'w')) {
		type = 0;
	}

	/* ���O��(p,,b,,m,) */
	if (type == -1 && (memcmp(c2, "p ", 2) == 0 || memcmp(c2, "b ", 2) == 0 || memcmp(c2, "m ", 2) == 0)) {
		type = 1;
	}

	/* ���O��(p',b',m') */
	if (type == -1 && (memcmp(c2, "p'", 2) == 0 || memcmp(c2, "b'", 2) == 0 || memcmp(c2, "m'", 2) == 0)) {
		type = 2;
	}

	/* ����W��(k,,g,,N) */
	if (type == -1 && (memcmp(c2, "k ", 2) == 0 || memcmp(c2, "g ", 2) == 0 || memcmp(c2, "N ", 2) == 0)) {
		type = 3;
	}

	/* ����W��(k',g',N') */
	if (type == -1 && (memcmp(c2, "k'", 2) == 0 || memcmp(c2, "g'", 2) == 0 || memcmp(c2, "N'", 2) == 0)) {
		type = 4;
	}

	/* ���s�d���W��(J) */
	if (type == -1 && c1 == 'J') {
		type = 5;
	}

	/* ����ȊO */
	if (type == -1) {
		type = 6;
	}

	return type + 72;
}