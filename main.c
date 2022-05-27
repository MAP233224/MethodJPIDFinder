#include "common.h"

int main(void) {
    while (1)
    {
        u32 pid = 0;
        u32 state = 0;
        clock_t start = clock();
        ScanValue("PID (hex): ", &pid, "%x", 0xffffffff);
        printf("Searching through all possible seeds for PID %08X...\n", pid);
        do {
            PKMN pk = { 0 };
            MethodJSeedToPID(state, &pk);
            if (pk.pid == pid)
            {
                REVERSEDSEED rs = ReverseSeed(state);
                printf("Hit seed %08X, advance %u frames to state %08X.\n", rs.reversed, rs.frames, rs.seed);
            }
        } while (++state);
        printf("Finished in %.2f seconds.\n\n", ((double)clock() - (double)start) / CLOCKS_PER_SEC);
    }
    return 0;
}