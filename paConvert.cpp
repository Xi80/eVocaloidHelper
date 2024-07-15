#include "paConvert.h"
#include "paTable.h"



static int makeTokenList(uint16_t* dest, uint8_t* src, size_t maxDestLen, size_t srcLen);

static int isMultiChar(uint8_t c);
static int inTable(uint16_t token);
static int checkN(char* dest, uint8_t current, uint8_t next);

int GetPAString(char* dest, uint8_t* srcText, size_t maxDestLen, size_t srcLen)
{
	int strLen = 0;
	uint16_t tokens[32];
	int tokenCount = makeTokenList(tokens, srcText, sizeof(tokens) / sizeof(tokens[0]), srcLen);

	if (tokenCount <= 0) {
		/* �K�؂ȉ̎��f�[�^�Ȃ� */
		return -1;
	}

	for (int i = 0; i < tokenCount; i++) {
		uint8_t currentIndex = inTable(tokens[i]);

		if (tokens[i] == '|' || tokens[i] == '-' || tokens[i] == ',')continue;

		/* "��" */
		if (tokens[i] == 0x00F1) {
			strLen += checkN(dest + strLen, currentIndex, (i < tokenCount - 1) ? inTable(tokens[i + 1]) : 0x0000);
		}
		else {
			if (tokens[i] & 0xFF00) {
				int tokenLength = strlen(cPATableMulti[currentIndex]);
#ifdef _WIN32
				memcpy_s(dest + strLen, maxDestLen, cPATableMulti[currentIndex], tokenLength);
#else 
				memcpy(dest + strLen, cPATableMulti[currentIndex], tokenLength);
#endif
				strLen += tokenLength;
			}
			else {
				int tokenLength = strlen(cPATableSingle[currentIndex]);
#ifdef _WIN32
				memcpy_s(dest + strLen, maxDestLen, cPATableSingle[currentIndex], tokenLength);
#else 
				memcpy(dest + strLen cPATableSingle[currentIndex], tokenLength);
#endif
				strLen += tokenLength;
			}
		}

		dest[strLen++] = ',';
	}

	dest[strLen] = 0x00;

	return tokenCount;
}

int GetPASysEx(uint8_t* dest, char* srcPA, size_t maxDestLen, size_t srcLen)
{
	return 0;
}

static int makeTokenList(uint16_t* dest, uint8_t* src, size_t maxDestLen, size_t srcLen) {
	/* ���p�����ƂЂ炪�ȈȊO�ɂ͊�{�I�ɑΉ����Ȃ� */
	int tokenCount = 0;

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
				if (inTable(token)) {
					dest[tokenCount++] = token;
					i += 2;
					continue;
				}

				/* ������Ȃ� */
				if (inTable((uint16_t)firstChar)) {
					dest[tokenCount++] = (uint16_t)firstChar;
				}
			}
			else {
				/* ����������2�o�C�g�����łȂ����ߔ��ʂ��Ȃ� */
				if (inTable((uint16_t)firstChar)) {
					dest[tokenCount++] = (uint16_t)firstChar;
				}
			}
		}
		else {
			/* "|","-",","�ȊO�̕����͓ǂݔ�΂� */
			if (src[i] == '|' || src[i] == '-' || src[i] == ',') {
				dest[tokenCount++] = (uint16_t)src[i];
			}
			else {
				continue;
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
	return 0;
}

static int checkN(char* dest, uint8_t current, uint8_t next) {
	const char pattern[][6] = {
		"N\\","m","m'","N","N'","J","n"
	};

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
	if (next == '|' || next == 0x00) {
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
#ifdef _WIN32
	strcpy_s(dest, 6, pattern[type]);
#else 
	strcpy(dest, pattern[type]);
#endif

	return strlen(pattern[type]);
}