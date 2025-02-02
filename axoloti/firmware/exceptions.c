/**
 * Copyright (C) 2015 Johannes Taelman
 *
 * This file is part of Axoloti.
 *
 * Axoloti is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Axoloti is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Axoloti. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ch.h"
#include "hal.h"
#include "ff.h"
#include "codec.h"
#include "chprintf.h"
#include "pconnection.h"
#include "axoloti_board.h"
#include "exceptions.h"

__attribute__ ((naked))
static void report_exception(void) {
    __asm volatile
        (
            " tst lr, #4                                                \n"
            " ite eq                                                    \n"
            " mrseq r0, msp                                             \n"
            " mrsne r0, psp                                             \n"
            " ldr r1, [r0, #24]                                         \n"
            " ldr r2, handler2_address_const                            \n"
            " bx r2                                                     \n"
            " handler2_address_const: .word prvGetRegistersFromStack    \n"
            );
}

#define ERROR_MAGIC_NUMBER 0xE1212012

typedef enum {
    fault = 0,
    watchdog_soft,
    watchdog_hard,
    brownout,
    goto_DFU,
    fatfs_error,
    patch_load_crc_fail,
    patch_load_sdram_overflow,
    usbh_midi_ringbuffer_overflow,
    halt
} faulttype;

// TODO: attach patch name or index to report!
typedef struct {
    volatile uint32_t magicnumber;
    volatile faulttype type;
    volatile uint32_t r0;
    volatile uint32_t r1;
    volatile uint32_t r2;
    volatile uint32_t r3;
    volatile uint32_t r12;
    volatile uint32_t lr;
    volatile uint32_t pc;
    volatile uint32_t psr;
    volatile uint32_t ipsr;
    volatile uint32_t cfsr;
    volatile uint32_t hfsr;
    volatile uint32_t mmfar;
    volatile uint32_t bfar;
    volatile uint32_t i;
} exceptiondump_t;

#if BOARD_AKSO
#define BKPSRAM_BASE D3_BKPSRAM_BASE
exceptiondump_t _exceptiondump;
#define exceptiondump ((exceptiondump_t *) &_exceptiondump)
#else
#define exceptiondump ((exceptiondump_t *) BKPSRAM_BASE)
#endif


/**
 * @brief   Jumps into the System ROM bootloader
 * @details This will only work before other initializations!
 */
void BootLoaderInit() {
    int reg, psp;
    reg = 0;
    asm volatile ("msr     CONTROL, %0" : : "r" (reg));
    asm volatile ("isb");
    psp = 0;
    asm volatile ("cpsie   i");
    asm volatile ("msr     PSP, %0" : : "r" (psp));
    FPU->FPCCR = 0;
    asm volatile ("LDR     R0, =0x40023844 ;");
    // RCC_APB2ENR (+0x18)
    asm volatile ("LDR     R1, =0x4000 ;");
    // ENABLE SYSCFG CLOCK (1)
    asm volatile ("STR     R1, [R0, #0]    ;");
    asm volatile ("NOP");
    asm volatile ("NOP");
    asm volatile ("NOP");
    asm volatile ("LDR     R0, =0x40013800 ;");
    // SYSCFG_CFGR1 (+0x00)
    asm volatile ("LDR     R1, =0x1 ;");
    // MAP ROM
    asm volatile ("STR     R1, [R0, #0]    ;");
    // MAP ROM AT ZERO (1)
    asm volatile ("NOP");
    asm volatile ("NOP");
    asm volatile ("NOP");
    asm volatile ("MOVS    R1, #0          ;");
    //  ADDRESS OF ZERO
    asm volatile ("LDR     R0, [R1, #0]    ;");
    // SP @ +0
    asm volatile ("MOV     SP, R0");
    asm volatile ("LDR     R0, [R1, #4]    ;");
    // PC @ +4
    asm volatile ("NOP");
    asm volatile ("BX      R0");
}

/**
 * @brief   Check exception magic bytes for DFU mode request.
 * @details Enables access to battery backup SRAM.
 */
#if DEAD_CODE
void exception_check_DFU(void) {
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_DBP;
    RCC->AHB1ENR |= RCC_AHB1ENR_BKPSRAMEN;
    asm volatile ("NOP");
    asm volatile ("NOP");
    asm volatile ("NOP");
    if (exception_check() && (exceptiondump->type == goto_DFU)) {
        exception_clear();
        BootLoaderInit();
    }
}
#endif

#if BOARD_AKSO
void exception_init(void) {
    chThdSleepMilliseconds(1);

    if (!exception_check()) {
        if ((RCC->RSR & RCC_RSR_BORRSTF) && !(RCC->RSR & RCC_RSR_PORRSTF)) {
            exceptiondump->magicnumber = ERROR_MAGIC_NUMBER;
            exceptiondump->type = brownout;
        }
        else {
            exceptiondump->type = -1;
            exceptiondump->lr = 0;
            exceptiondump->pc = 0;
            exceptiondump->psr = 0;
            exceptiondump->ipsr = 0;
        }
        // clear reset flags
        RCC->RSR |= RCC_RSR_RMVF;
    }
#if WATCHDOG_ENABLED
    WWDG->SR = 0;
    WWDG->CR = 0x7F;
#endif
}
#else
void exception_init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_BKPSRAMEN;
    RCC->APB1ENR |= RCC_APB1ENR_WWDGEN;

    chThdSleepMilliseconds(1);

    if (!exception_check()) {
        if (RCC->CSR & RCC_CSR_WWDGRSTF) {
            // no exception found, but watchdog caused a reset?
            exceptiondump->magicnumber = ERROR_MAGIC_NUMBER;
            exceptiondump->type = watchdog_hard;
        }
        else if ((RCC->CSR & RCC_CSR_BORRSTF) && !(RCC->CSR & RCC_CSR_PORRSTF)) {
            exceptiondump->magicnumber = ERROR_MAGIC_NUMBER;
            exceptiondump->type = brownout;
        }
        else {
            exceptiondump->type = -1;
            exceptiondump->lr = 0;
            exceptiondump->pc = 0;
            exceptiondump->psr = 0;
            exceptiondump->ipsr = 0;
        }
        // clear reset flags
        RCC->CSR |= RCC_CSR_RMVF;
    }
#if WATCHDOG_ENABLED
    WWDG->SR = 0;
    WWDG->CR = 0x7F;
#endif
}
#endif

int exception_check(void) {
    if (exceptiondump->magicnumber == ERROR_MAGIC_NUMBER) {
        return 1; // exception happened
    }
    return 0; // all fine
}

void exception_clear(void) {
    exceptiondump->magicnumber = 0;
}

/**
 * @brief   Initiate jumping into the system ROM bootloader.
 * @details By writing magic bytes and going through a soft reboot...
 */
void exception_initiate_dfu(void) {
    exceptiondump->r0 = 1;
    exceptiondump->r1 = 2;
    exceptiondump->r2 = 3;
    exceptiondump->r3 = 4;
    palSetPadMode(GPIOA, 11, PAL_MODE_INPUT);
    palSetPadMode(GPIOA, 12, PAL_MODE_INPUT);
    volatile int i = 20;
    while (i--) {
        volatile int j = 1 << 12;
        palTogglePad(LED1_PORT, LED1_PIN);
        while (j--) {
            volatile int k = 1 << 8;
            while (k--) {
            }
            watchdog_feed();
        }
    }
    exceptiondump->magicnumber = ERROR_MAGIC_NUMBER;
    exceptiondump->type = goto_DFU;
    NVIC_SystemReset();
}

const char * const fs_err_name[] = {
    "FR_OK",
    "FR_DISK_ERR",
    "FR_INT_ERR",
    "FR_NOT_READY",
    "FR_NO_FILE",
    "FR_NO_PATH",
    "FR_INVALID_NAME",
    "FR_DENIED",
    "FR_EXIST",
    "FR_INVALID_OBJECT",
    "FR_WRITE_PROTECTED",
    "FR_INVALID_DRIVE",
    "FR_NOT_ENABLED",
    "FR_NO_FILESYSTEM",
    "FR_MKFS_ABORTED",
    "FR_TIMEOUT",
    "FR_LOCKED",
    "FR_NOT_ENOUGH_CORE",
    "FR_TOO_MANY_OPEN_FILES",
    "FR_INVALID_PARAMETER"
};

void exception_checkandreport(void) {
    if (exception_check()) {
        // clear first, so in case reporting causes another exception we're not stuck
        exception_clear();
        bool report_registers = 0;
        if (exceptiondump->type == fault) {
            LogTextMessage("exception report:");
            report_registers = 1;
        }
        else if (exceptiondump->type == watchdog_soft) {
            LogTextMessage("exception: soft watchdog");
            report_registers = 1;
        }
        else if (exceptiondump->type == watchdog_hard) {
            LogTextMessage("exception: hard watchdog i=0x%x", exceptiondump->i);
        }
        else if (exceptiondump->type == brownout) {
            LogTextMessage("exception: brownout");
        }
        else if (exceptiondump->type == fatfs_error) {
        }
        else if (exceptiondump->type == patch_load_crc_fail) {
            LogTextMessage("failed to load patch, firmware mismatch? \"%s\"",(char *)(BKPSRAM_BASE)+12);

        }
        else if (exceptiondump->type == patch_load_sdram_overflow) {
            LogTextMessage("sdram overflow by %d bytes",exceptiondump->r0);
        }
        else if (exceptiondump->type == usbh_midi_ringbuffer_overflow) {
            LogTextMessage("midi output overflow");
        }
        else if (exceptiondump->type == halt) {
            LogTextMessage("Panic: %s",exceptiondump->r0);
        }
        else {
            LogTextMessage("unknown exception?");
        }

#if 0 // causes crash?
        if (report_registers) {
            LogTextMessage("pc=0x%x", exceptiondump->pc);
            LogTextMessage("psr=0x%x", exceptiondump->psr);
            LogTextMessage("lr=0x%x", exceptiondump->lr);
            LogTextMessage("r12=0x%x", exceptiondump->r12);
            LogTextMessage("cfsr=0x%x",exceptiondump->cfsr);

            if (exceptiondump->cfsr & (1 << 15)) {
                // BFARVALID
                LogTextMessage("bfar=0x%x",exceptiondump->bfar);
            }

            if (exceptiondump->cfsr & (1 << 7)) {
                // MMARVALID
                LogTextMessage("mmfar=0x%x",exceptiondump->mmfar);
            }
        }
#else
        (void)report_registers;
#endif
    }
}

void report_fatfs_error(int errno, const char *fn) {
	if (exceptiondump->magicnumber == ERROR_MAGIC_NUMBER)
        return;

    char *p;
    p = (char *)(BKPSRAM_BASE)+12;

    if (fn!=0) {
        if (*fn != '/') {
            // prepend CWD
            f_getcwd(p,40);
            while(*p!=0) p++;
            *p++ = '/';
        }
        int i = 20;
        while(i-- && *fn){
            *p++ = *fn++;
        }
    }
    *p = 0;
    exceptiondump->magicnumber = ERROR_MAGIC_NUMBER;
    exceptiondump->type = fatfs_error;
    exceptiondump->r0 = errno;
}

void report_patchLoadFail(const char *fn) {
    if (exceptiondump->magicnumber == ERROR_MAGIC_NUMBER)
        return;
    exceptiondump->magicnumber = ERROR_MAGIC_NUMBER;
    exceptiondump->type = patch_load_crc_fail;
    char *p;
    p = (char *)(BKPSRAM_BASE)+12;
    if (fn!=0) {
        if (*fn != '/') {
            // prepend CWD
            f_getcwd(p,40);
            while(*p!=0) p++;
            *p++ = '/';
        }
        int i = 20;
        while(i-- && *fn){
            *p++ = *fn++;
        }
    }
    *p = 0;
}

void report_patchLoadSDRamOverflow(const char *fn, int amount) {
    if (exceptiondump->magicnumber == ERROR_MAGIC_NUMBER)
        return;
    exceptiondump->magicnumber = ERROR_MAGIC_NUMBER;
    exceptiondump->r0 = amount;
    exceptiondump->type = patch_load_sdram_overflow;
    char *p;
    p = (char *)(BKPSRAM_BASE)+12;
    if (fn!=0) {
        if (*fn != '/') {
            // prepend CWD
            f_getcwd(p,40);
            while(*p!=0) p++;
            *p++ = '/';
        }
        int i = 20;
        while(i-- && *fn){
            *p++ = *fn++;
        }
    }
    *p = 0;
}

void report_usbh_midi_ringbuffer_overflow(void) {
    if (exceptiondump->magicnumber == ERROR_MAGIC_NUMBER)
        return;
    exceptiondump->magicnumber = ERROR_MAGIC_NUMBER;
    exceptiondump->type = usbh_midi_ringbuffer_overflow;
}

#if DEAD_CODE
void dbg_set_i(int i) {
    exceptiondump->i = i;
}
#endif

static void terminator(void) {
    // float usb inputs, hope the host notices detach...
    palSetPadMode(GPIOA, 11, PAL_MODE_INPUT);
    palSetPadMode(GPIOA, 12, PAL_MODE_INPUT);
    volatile int i = 20;
    while (i--) {
        volatile int j = 1 << 12;
        palTogglePad(LED1_PORT, LED1_PIN);
        while (j--) {
            volatile int k = 1 << 8;
            while (k--) {
            }
            watchdog_feed();
        }
    }

    if (CoreDebug->DHCSR&CoreDebug_DHCSR_C_DEBUGEN_Msk) {
        // software breakpoint
        asm("BKPT 255");
    }

    NVIC_SystemReset();
}

void prvGetRegistersFromStack(uint32_t *pulFaultStackAddress) {
    volatile uint32_t r0;
    volatile uint32_t r1;
    volatile uint32_t r2;
    volatile uint32_t r3;
    volatile uint32_t r12;
    volatile uint32_t lr; /* Link register. */
    volatile uint32_t pc; /* Program counter. */
    volatile uint32_t psr;/* Program status register. */

    r0 = pulFaultStackAddress[0];
    r1 = pulFaultStackAddress[1];
    r2 = pulFaultStackAddress[2];
    r3 = pulFaultStackAddress[3];

    r12 = pulFaultStackAddress[4];
    lr = pulFaultStackAddress[5];
    pc = pulFaultStackAddress[6];
    psr = pulFaultStackAddress[7];

    exceptiondump->magicnumber = ERROR_MAGIC_NUMBER;
#if BOARD_AKSO
    if (WWDG1->SR & WWDG_SR_EWIF) {
        exceptiondump->type = watchdog_soft;
    }
    else {
        exceptiondump->type = fault;
    }
#else
    if (WWDG->SR & WWDG_SR_EWIF) {
        exceptiondump->type = watchdog_soft;
    }
    else {
        exceptiondump->type = fault;
    }
#endif
    exceptiondump->r0 = r0;
    exceptiondump->r1 = r1;
    exceptiondump->r2 = r2;
    exceptiondump->r3 = r3;
    exceptiondump->r12 = r12;
    exceptiondump->lr = lr;
    exceptiondump->pc = pc;
    exceptiondump->psr = psr;
    exceptiondump->ipsr = __get_IPSR();
    exceptiondump->cfsr = SCB->CFSR;
    exceptiondump->hfsr = SCB->HFSR;
    exceptiondump->mmfar = SCB->MMFAR;
    exceptiondump->bfar = SCB->BFAR;

#if WATCHDOG_ENABLED
    WWDG->CR = WWDG_CR_T;
#endif

    palClearPad(LED1_PORT, LED1_PIN);

    codec_clearbuffer();

    terminator();
}

void HardFault_Handler(void) __attribute__((alias("report_exception")));
void MemManage_Handler(void) __attribute__((alias("report_exception")));
void BusFault_Handler(void) __attribute__((alias("report_exception")));
void UsageFault_Handler(void) __attribute__((alias("report_exception")));

__attribute__ ((naked))
CH_IRQ_HANDLER(WWDG_IRQHandler){
    __asm volatile
        (
            " tst lr, #4                                                \n"
            " ite eq                                                    \n"
            " mrseq r0, msp                                             \n"
            " mrsne r0, psp                                             \n"
            " ldr r1, [r0, #24]                                         \n"
            " ldr r2, handler2_address_const                            \n"
            " bx r2                                                     \n"
            );
}

#if DEAD_CODE
void report_halt(const char *reason) {
    /* Pointing to the passed message.*/
    ch.dbg.panic_msg = reason;

    exceptiondump->magicnumber = ERROR_MAGIC_NUMBER;
    exceptiondump->type = halt;
    exceptiondump->r0 = (uint32_t)reason;

    if (!(CoreDebug->DHCSR&CoreDebug_DHCSR_C_DEBUGEN_Msk)) {
        // no debugger connected, do system reset
        NVIC_SystemReset();
    } else {
        // software breakpoint
        asm("BKPT 255");
    }
}
#endif
