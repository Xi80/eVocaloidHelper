#pragma once

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>


int GetPAString(char* dest, uint8_t* srcText, size_t maxDestLen, size_t srcLen);

int GetPASysEx(uint8_t* dest, char* srcPA, size_t maxDestLen, size_t srcLen);