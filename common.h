#pragma once

#include <stdio.h>
#include <string.h>
#include <time.h>

typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NATURES_MAX         (25)    // Number of natures
#define SEED_MAX_B          (24)    // Hours in a day
#define SEED_OFF_C          (3600)  // Delay, 1 minute of leeway
#define MIN_DELAY_DPPT      (700)   // Minimum delay you can get in DPPT from a Save&Quit
#define MAX_DELAY_DPPT      (MIN_DELAY_DPPT + SEED_OFF_C)
#define STRING_LENGTH_MAX   (16)

typedef struct {
    u8 nature;
    u32 pid;
    //Size: 8 bytes (aligned)
} PKMN;


typedef struct {
    u32 seed; //original seed
    u32 reversed; //reversed seed
    u32 frames; //distance between seed and reversed
    //Size: 12 bytes
} REVERSEDSEED;


static void ScanValue(u8* message, u32* value, u8* format, u64 max) {
    /* General purpose safe scan. Instruction message, value to change, string format and max value */
    do {
        printf("%s", message);
        u8 userInput[STRING_LENGTH_MAX];
        fgets(userInput, STRING_LENGTH_MAX, stdin);
        if (strlen(userInput) == 0 || strlen(userInput) >= STRING_LENGTH_MAX) {
            continue;
        }
        if (sscanf_s(userInput, format, value) != 1) {
            *value = max + 1; //doesn't work for seed, puts it at 0 and becomes valid
            continue;
        }
    } while (*value > max);
}

static u32 RngNext(u32* state) {
    /* General purpose LCRNG, advance and return state */
    *state = *state * 0x41C64E6D + 0x6073;
    return *state;
}

static void MethodJSeedToPID(u32 state, PKMN* pkmn) {
    /* Calculate PID, Nature and IVs according to Method J Stationary (no Synchronize) from a given state */
    pkmn->nature = (RngNext(&state) >> 16) / 0x0A3E;
    do { pkmn->pid = (RngNext(&state) >> 16) | (RngNext(&state) & 0xffff0000); } while (pkmn->pid % NATURES_MAX != pkmn->nature); //roll PID until the 2 natures are the same
}

static REVERSEDSEED ReverseSeed(u32 seed) {
    /* Find the nearest console-hitable seed provided the current state of the RNG */
    u32 state = seed;
    u16 frame = 0;
    u8 a = state >> 24;
    u8 b = state >> 16;
    u16 c = state & 0xffff;
    /* Search loop */
    while (b > SEED_MAX_B || c < MIN_DELAY_DPPT || c > MAX_DELAY_DPPT)
    {
        state = state * 0xEEB9EB65 + 0x0A3561A1; //reverse LCRNG
        a = state >> 24;
        b = state >> 16;
        c = state & 0xffff;
        frame++;
    }
    return (REVERSEDSEED) { seed, state, frame };
}
