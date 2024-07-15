#pragma once
#include <cstdint>

const uint16_t cKanaTableSingle[] = {
	0x00A0,	0x00A2,	0x00A4,	0x00A6,	0x00A8,	/* あ　い　う　え　お */
	0x00A9,	0x00AB,	0x00AD,	0x00AF,	0x00B1,	/* か　き　く　け　こ */
	0x00B3,	0x00B5,	0x00B7,	0x00B9,	0x00BB,	/* さ　し　す　せ　そ */
	0x00BD,	0x00BF,	0x00C2,	0x00C4,	0x00C6,	/* た　ち　つ　て　と */
	0x00C8,	0x00C9,	0x00CA,	0x00CB,	0x00CC,	/* な　に　ぬ　ね　の */
	0x00CD,	0x00D0,	0x00D3,	0x00D6,	0x00D9,	/* は　ひ　ふ　へ　ほ */
	0x00DC,	0x00DD,	0x00DE,	0x00DF,	0x00E0,	/* ま　み　む　め　も */
	0x00E7,	0x00E8,	0x00E9,	0x00EA,	0x00EB,	/* ら　リ　る　れ　ろ */
	0x00AA,	0x00AC,	0x00AE,	0x00B0,	0x00B2,	/* が　ぎ　ぐ　げ　ご */
	0x00B4,	0x00B6,	0x00B8,	0x00BA,	0x00BC,	/* ざ　じ　ず　ぜ　ぞ */
	0x00BE,	0x00C0,	0x00C3,	0x00C5,	0x00C7,	/* だ　ぢ　づ　で　ど */
	0x00CE,	0x00D1,	0x00D4,	0x00D7,	0x00DA,	/* ば　び　ぶ　べ　ぼ */
	0x00CF,	0x00D2,	0x00D5,	0x00D8,	0x00DB,	/* ぱ　ぴ　ぷ　ぺ　ぽ */
	0x00E2,	0x00E4,	0x00E6,					/* や　ゆ　よ */
	0x00ED,	0x00EE,	0x00EF,	0x00F0,	0x00F1	/* わ　ゐ　ゑ　を　ん*/
};

const uint16_t cKanaTableMulti[] = {
	0xD39F,	0xC29F,							/* ふぁ　つぁ */
	0xA4A1,	0xB7A1,	0xB8A1,	0xC2A1,	0xC4A1,	/* うぃ　すぃ　ずぃ　つぃ　てぃ */
	0xC5A1,	0xD3A1,							/* でぃ　ふぃ */
	0xC6A3,	0xC7A3,							/* とぅ　どぅ */
	0xA2A5,	0xA4A5,	0xABA5,	0xB5A5,	0xBFA5,	/* いぇ　うぇ　きぇ　しぇ　ちぇ */
	0xC2A5,	0xC4A5,	0xC9A5,	0xD0A5,	0xDDA5,	/* つぇ　てぇ　にぇ　ひぇ　みぇ */
	0xE8A5,	0xACA5,	0xB6A5,	0xC5A5,	0xD1A5,	/* りぇ　ぎぇ　じぇ　でぇ　びぇ */
	0xD2A5,	0xD3A5,							/* ぴぇ　ふぇ */
	0xA4A7,	0xC2A7,	0xD3A7,					/* うぉ　つぉ　ふぉ */
	0xABE1,	0xB5E1,	0xBFE1, 0xC4E1,	0xC9E1,	/* きゃ　しゃ　ちゃ　てゃ　にゃ */
	0xD0E1,	0xDDE1, 0xE8E1, 0xACE1, 0xB6E1,	/* ひゃ　みゃ　りゃ　ぎゃ　じゃ */
	0xC5E1,	0xD1E1,	0xD2E1, 0xD3E1,			/* でゃ　びゃ　ぴゃ　ふゃ */
	0xABE3, 0xB5E3, 0xBFE3, 0xC4E3, 0xC9E3,	/* きゅ　しゅ　ちゅ　てゅ　にゅ */
	0xD0E3, 0xDDE3, 0xE8E3, 0xACE3, 0xB6E3,	/* ひゅ　みゅ　りゅ　ぎゅ　じゅ */
	0xC5E3, 0xD1E3, 0xD2E3, 0xD3E3,			/* でゅ　びゅ　ぴゅ　ふゅ */
	0xABE5, 0xB5E5, 0xBFE5, 0xC4E5, 0xC9E5,	/* きょ　しょ　ちょ　てょ　にょ */
	0xD0E5, 0xDDE5, 0xE8E5, 0xACE5, 0xB6E5,	/* ひょ　みょ　りょ　ぎょ　じょ */
	0xC5E5, 0xD1E5, 0xD2E5					/* でょ　びょ　ぴょ */
};

const char cPATableSingle[][6] = {
	"a",	"i",	"M",	"e",	"o",	/* あ　い　う　え　お */
	"k a",	"k' i", "k M",	"k e",	"k o",	/* か　き　く　け　こ */
	"s a",	"S i",	"s M",	"s e",	"s o",	/* さ　し　す　せ　そ */
	"t a",	"tS i",	"ts M",	"t e",	"t o",	/* た　ち　つ　て　と */
	"n a",	"J i",	"n M",	"n e",	"n o",	/* な　に　ぬ　ね　の */
	"h a",	"C i",	"p\\ M","h e",	"h o",	/* は　ひ　ふ　へ　ほ */
	"m a",	"m' i",	"m M",	"m e",	"m o",	/* ま　み　む　め　も */
	"4 a",	"4' i",	"4 M",	"4 e",	"4 o",	/* ら　リ　る　れ　ろ */
	"g a",	"g' i",	"g M",	"g e",	"g o",	/* が　ぎ　ぐ　げ　ご */
	"dz a",	"dZ i",	"dz M",	"dz e", "dz o",	/* ざ　じ　ず　ぜ　ぞ */
	"d a",	"dZ i",	"dz M",	"d e",	"d o",	/* だ　ぢ　づ　で　ど */
	"b a",	"b' i",	"b M",	"b e",	"b o",	/* ば　び　ぶ　べ　ぼ */
	"p a",	"p' i",	"p M",	"p e",	"p o",	/* ぱ　ぴ　ぷ　ぺ　ぽ */
	"j a",	"j M",	"j o",					/* や　ゆ　よ */
	"w a",	"w i",	"w e",	"o",	"N\\"	/* わ　ゐ　ゑ　を　ん*/
};

const char cPATableMulti[][6] = {
	"p\\ a","ts a",							/* ふぁ　つぁ */
	"w i",	"s i",	"dz i",	"ts i",	"t' i",	/* うぃ　すぃ　ずぃ　つぃ　てぃ */
	"d' i",	"p\\' i",						/* でぃ　ふぃ */
	"t M",	"d M",							/* とぅ　どぅ */
	"j e",	"w e",	"k' e",	"S e",	"tS e",	/* いぇ　うぇ　きぇ　しぇ　ちぇ */
	"ts e",	"t' e",	"J e",	"C e",	"m' e",	/* つぇ　てぇ　にぇ　ひぇ　みぇ */
	"4' e",	"g' e",	"dZ e",	"d' e",	"b' e",	/* りぇ　ぎぇ　じぇ　でぇ　びぇ */
	"p' e",	"p\\ e",						/* ぴぇ　ふぇ */
	"w o",	"ts o",	"p\\ o",				/* うぉ　つぉ　ふぉ */
	"k' a",	"S a",	"tS a",	"t' a",	"J a",	/* きゃ　しゃ　ちゃ　てゃ　にゃ */
	"C a",	"m' a",	"4' a",	"g' a",	"dZ a",	/* ひゃ　みゃ　りゃ　ぎゃ　じゃ */
	"d' a",	"b' a",	"p' a",	"p\\' a",		/* でゃ　びゃ　ぴゃ　ふゃ */
	"k' M",	"S M",	"tS M",	"t' M",	"J M",	/* きゅ　しゅ　ちゅ　てゅ　にゅ */
	"C M",	"m' M",	"4' M",	"g' M",	"dz M",	/* ひゅ　みゅ　りゅ　ぎゅ　じゅ */
	"d' M",	"b' M",	"p' M",	"p\\' M",		/* でゅ　びゅ　ぴゅ　ふゅ */
	"k' o",	"S o",	"tS o",	"t' o",	"J o",	/* きょ　しょ　ちょ　てょ　にょ */
	"C o",	"m' o",	"4' o",	"g' o",	"dZ o",	/* ひょ　みょ　りょ　ぎょ　じょ */
	"d' o", "b' o",	"p' o"					/* でょ　びょ　ぴょ */
};