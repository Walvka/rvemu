#ifndef _RVEMU_H_
#define _RVEMU_H_

    #include <assert.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <inttypes.h>
    #include <math.h>
    #include <stdarg.h>
    #include <stdbool.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <sys/time.h>
    #include <unistd.h>

    #include "types.h"
    #include "elfdef.h"

    #define fatalf(fmt, ...) (fprintf(stderr,"fatal: %s:%d " fmt "\n", __FILE__, __LINE__, __VA_ARGS__), exit(1))
    #define fatal(msg) fatalf("%s", msg)

    /***mmu.c***/
    typedef struct _MMU_T_{
        u64 entry;
    } mmu_t;

    void mmu_load_elf(mmu_t *mmu, int fd);

    /***state.c***/
    typedef struct _STATE_T_{
        u64 gp_regs[32];
        u64 pc;
    } state_t;    

    /***machine.c***/
    typedef struct _MACHINE_T_{
        state_t state;
        mmu_t mmu;
    } machine_t;//type

    void machine_load_program(machine_t *m, char *prog);

#endif