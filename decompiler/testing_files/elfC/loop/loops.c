//
// This file was generated by the Retargetable Decompiler
// Website: https://retdec.com
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// ------------------- Function Prototypes --------------------

int64_t __do_global_dtors_aux(void);
int64_t _fini(void);
int64_t _init(void);
int64_t _start(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6);
int64_t deregister_tm_clones(void);
int32_t for_loop(int32_t j);
int64_t frame_dummy(void);
void function_1050(int64_t * d);
int32_t function_1060(char * s);
int32_t function_1070(char * format, ...);
int32_t n_way_conditional_switch(uint32_t n);
int64_t register_tm_clones(void);
int32_t while_post_tested_loop(int32_t flag);
int32_t while_pre_tested_loop(int32_t flag_2);

// --------------------- Global Variables ---------------------

int64_t g1 = 0; // 0x4010
int32_t g2;

// ------- Dynamically Linked Functions Without Header --------

void __cxa_finalize(int64_t * a1);
void __gmon_start__(void);
int32_t __libc_start_main(int64_t a1, int32_t a2, char ** a3, void (*a4)(), void (*a5)(), void (*a6)());

// ------------------------ Functions -------------------------

// Address range: 0x1000 - 0x101b
int64_t _init(void) {
    int64_t result = 0; // 0x1012
    if (*(int64_t *)0x3fe8 != 0) {
        // 0x1014
        __gmon_start__();
        result = &g2;
    }
    // 0x1016
    return result;
}

// Address range: 0x1050 - 0x105b
void function_1050(int64_t * d) {
    // 0x1050
    __cxa_finalize(d);
}

// Address range: 0x1060 - 0x106b
int32_t function_1060(char * s) {
    // 0x1060
    return puts(s);
}

// Address range: 0x1070 - 0x107b
int32_t function_1070(char * format, ...) {
    // 0x1070
    return printf(format);
}

// Address range: 0x1080 - 0x10a6
int64_t _start(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6) {
    // 0x1080
    int64_t v1; // 0x1080
    __libc_start_main(0x12b2, (int32_t)a6, (char **)&v1, NULL, NULL, (void (*)())a3);
    __asm_hlt();
    // UNREACHABLE
}

// Address range: 0x10b0 - 0x10d9
int64_t deregister_tm_clones(void) {
    // 0x10b0
    return &g1;
}

// Address range: 0x10e0 - 0x1119
int64_t register_tm_clones(void) {
    // 0x10e0
    return 0;
}

// Address range: 0x1120 - 0x1159
int64_t __do_global_dtors_aux(void) {
    // 0x1120
    if (*(char *)&g1 != 0) {
        // 0x1158
        int64_t result; // 0x1120
        return result;
    }
    // 0x112d
    if (*(int64_t *)0x3ff8 != 0) {
        // 0x113b
        __cxa_finalize((int64_t *)*(int64_t *)0x4008);
    }
    int64_t result2 = deregister_tm_clones(); // 0x1147
    *(char *)&g1 = 1;
    return result2;
}

// Address range: 0x1160 - 0x1169
int64_t frame_dummy(void) {
    // 0x1160
    return register_tm_clones();
}

// From module:   /home/users/dragoss/Documents/licenta/Reverse-Engineering-Tool/testing_files/source/loops.c
// Address range: 0x1169 - 0x11d8
// Line range:    5 - 22
int32_t n_way_conditional_switch(uint32_t n) {
    if (n == 5) {
        // 0x11bd
        puts("Found myself!");
        // 0x11d6
        return 5;
    }
    // 0x1185
    if (n > 5) {
        // 0x11d6
        return -1;
    }
    // 0x118b
    if (n == 3) {
        // 0x11d6
        return 16;
    }
    int32_t result = -1; // 0x1195
    if (n <= 3) {
        // 0x1197
        result = n == 1 ? 20 : n == 2 ? 18 : -1;
    }
    // 0x11d6
    return result;
}

// From module:   /home/users/dragoss/Documents/licenta/Reverse-Engineering-Tool/testing_files/source/loops.c
// Address range: 0x11d8 - 0x1224
// Line range:    24 - 35
int32_t for_loop(int32_t j) {
    int32_t result = 0; // 0x121d
    if (j < 16) {
        // 0x11f2
        result = 7 * (15 - j) / 2 + 7;
    }
    // 0x121f
    return result;
}

// From module:   /home/users/dragoss/Documents/licenta/Reverse-Engineering-Tool/testing_files/source/loops.c
// Address range: 0x1224 - 0x126c
// Line range:    37 - 49
int32_t while_pre_tested_loop(int32_t flag_2) {
    // 0x1224
    return flag_2 < 16 ? -7 * flag_2 + 112 : 0;
}

// From module:   /home/users/dragoss/Documents/licenta/Reverse-Engineering-Tool/testing_files/source/loops.c
// Address range: 0x126c - 0x12b2
// Line range:    51 - 63
int32_t while_post_tested_loop(int32_t flag) {
    int32_t v1 = flag + 1;
    return 7 * ((v1 > 16 ? v1 : 16) - flag);
}

// From module:   /home/users/dragoss/Documents/licenta/Reverse-Engineering-Tool/testing_files/source/loops.c
// Address range: 0x12b2 - 0x1341
// Line range:    65 - 73
int main() {
    // 0x12b2
    printf("%d\n", (int64_t)(uint32_t)n_way_conditional_switch(5));
    printf("%d\n", (int64_t)for_loop(5));
    printf("%d\n", (int64_t)while_pre_tested_loop(5));
    printf("%d\n", (int64_t)while_post_tested_loop(5));
    return 0;
}

// Address range: 0x1344 - 0x1351
int64_t _fini(void) {
    // 0x1344
    int64_t result; // 0x1344
    return result;
}

// --------------------- Meta-Information ---------------------

// Detected compiler/packer: gcc (11.2.0)
// Detected functions: 15

