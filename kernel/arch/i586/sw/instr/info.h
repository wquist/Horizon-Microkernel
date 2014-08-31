/* Horizon Microkernel - Hobby Operating System
 * Copyright (C) 2014 Wyatt Lindquist
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file arch/i586/sw/instr/info.h
 *  \date June 2014
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

//! Information codes for CPUID.
/*! Intel has extra, non-standard codes. */
typedef enum cpuid_code CPUID_CODE;
enum cpuid_code
{
	CPUID_CODE_VENDOR = 0x0,
	CPUID_CODE_FEATURE,
	CPUID_CODE_TLB,
	CPUID_CODE_SERIAL,

	CPUID_CODE_INTEL_EXTENDED = 0x80000000,
	CPUID_CODE_INTEL_FEATURE,
	CPUID_CODE_INTEL_BRAND0,
	CPUID_CODE_INTEL_BRAND1,
	CPUID_CODE_INTEL_BRAND2
};

//! CPUID feature bits stored in ECX.
typedef union cpuid_ecx cpuid_ecx_t;
union cpuid_ecx
{
	struct __packed
	{
		uint32_t SSE3    : 1; //!< SSE3 available.
		uint32_t PCMUL   : 1; //!< PCLMULDQ available.
		uint32_t DTES64  : 1; //!< 64-bit Debug Trace.
		uint32_t MONITOR : 1; //!< MONITOR and MWAIT instructions available.
		uint32_t DSCPL   : 1; //!< CPL Debug Store MSR.
		uint32_t VMX     : 1; //!< Virtual Machine Extensions.
		uint32_t SMX     : 1; //!< Safer Mode Extensions.
		uint32_t EST     : 1; //!< Enhanced SpeedStep Technology.
		uint32_t TM2     : 1; //!< THERM_INT + THERM_STAT MSR, APIC thermal LVT.
		uint32_t SSSE3   : 1; //!< Supplemental SSE3 instructions.
		uint32_t CID     : 1; //!< Context ID of the L1 Cache.
		uint32_t         : 1;
		uint32_t FMA     : 1; //!< FP Fused Multiply-Add.
		uint32_t CX16    : 1; //!< 16-bit CMPXCHG.
		uint32_t ETPRD   : 1; //!< Can disable task priority messages.
		uint32_t PDCM    : 1; //!< Performance Debug Capability MSR.
		uint32_t         : 1;
		uint32_t PCID    : 1; //!< Process Context Identifiers.
		uint32_t DCA     : 1; //!< Direct Cache Access.
		uint32_t SSE4_1  : 1; //!< SSE4.1 available.
		uint32_t SSE4_2  : 1; //!< SSE4.2 available.
		uint32_t X2APIC  : 1; //!< x2APIC onboard.
		uint32_t MOVBE   : 1; //!< MOVBE instruction available.
		uint32_t POPCNT  : 1; //!< POPCNT instruction available.
		uint32_t TSCD    : 1; //!< lAPIC supports 1-shot mode with TSC deadline.
		uint32_t AES     : 1; //!< AES instruction set present.
		uint32_t XSAVE   : 1; //!< XSAVE and XRESTOR.
		uint32_t OSXSAVE : 1; //!< Read only copy of XSAVE (in CR4).
		uint32_t AVX     : 1; //!< Advanced Vector Extensions.
		uint32_t F16C    : 1; //!< Half-precision FP available.
		uint32_t RDRAND  : 1; //!< On-chip random number generator.
		uint32_t HYPVSR  : 1; //!< CPU running as a hypervisor. (Always 0 on real CPU)
	};
	uint32_t raw;
};

//! CPUID feature bits stored in EDX.
typedef union cpuid_edx cpuid_edx_t;
union cpuid_edx
{
	struct __packed
	{
		uint32_t FPU   : 1; //!< FPU onboard.
		uint32_t VME   : 1; //!< Virtual Mode Extensions.
		uint32_t DE    : 1; //!< Debugging Extensions.
		uint32_t PSE   : 1; //!< Page Size Extension.
		uint32_t TSC   : 1; //!< RDTSC available.
		uint32_t MSR   : 1; //!< Model Specific Registers present.
		uint32_t PAE   : 1; //!< Paging Address Extension.
		uint32_t MCE   : 1; //!< Machine Check Exception.
		uint32_t CX8   : 1; //!< CMPXCHG8 available.
		uint32_t APIC  : 1; //!< APIC onboard.
		uint32_t       : 1;
		uint32_t SEP   : 1; //!< SYSENTER and SYSEXIT instructions.
		uint32_t MTRR  : 1; //!< Memory Type Range Registers.
		uint32_t PGE   : 1; //!< Paging Global Enable.
		uint32_t MCA   : 1; //!< Machine Check Architecture.
		uint32_t CMOV  : 1; //!< Conditional move and FCMOV instructions.
		uint32_t PAT   : 1; //!< Page Attribute Table.
		uint32_t PSE36 : 1; //!< 36-bit Page Size Extension.
		uint32_t PSN   : 1; //!< Processor Serial Number available.
		uint32_t CLF   : 1; //!< CLFLUSH instruction.
		uint32_t       : 1;
		uint32_t DTES  : 1; //!< Debug Trace MSR.
		uint32_t ACPI  : 1; //!< THERM_CTL MSR.
		uint32_t MMX   : 1; //!< MMX instruction set.
		uint32_t FXSR  : 1; //!< FXSAVE and FXRESTOR instructions.
		uint32_t SSE   : 1; //!< SSE available.
		uint32_t SSE2  : 1; //!< SSE2 available.
		uint32_t SS    : 1; //!< Self Snoop.
		uint32_t HTT   : 1; //!< Hyper-Threading Technology.
		uint32_t TM1   : 1; //!< THERM_INT + THERM_STAT MSR, APIC thermal LVT.
		uint32_t IA64  : 1; //!< IA64 processor emulating x86.
		uint32_t PBE   : 1; //!< Pending Break Event.
	};
	uint32_t raw;
};

//! Available bits in the EFLAGS register.
typedef union eflags eflags_t;
union eflags
{
	struct __packed
	{
		uint32_t CF   : 1;  //!< Carry Flag.
		uint32_t      : 1; 
		uint32_t PF   : 1;  //!< Parity Flag.
		uint32_t      : 1;
		uint32_t AF   : 1;  //!< Adjust Flag.
		uint32_t      : 1;
		uint32_t ZF   : 1;  //!< Zero Flag.
		uint32_t SF   : 1;  //!< Sign Flag.
		uint32_t TF   : 1;  //!< Trap Flag.
		uint32_t IF   : 1;  //!< Interrupt Enable Flag.
		uint32_t DF   : 1;  //!< Direction Flag.
		uint32_t OF   : 1;  //!< Overflow Flag.
		uint32_t IOPL : 2;  //!< I/O Privelege.
		uint32_t NT   : 1;  //!< Nested Task.
		uint32_t      : 1; 
		uint32_t RF   : 1;  //!< Resume Flag.
		uint32_t VM   : 1;  //!< Virtual 8086 Mode.
		uint32_t AC   : 1;  //!< Alignment Check.
		uint32_t VIF  : 1;  //!< Virtual Interrupt Flag.
		uint32_t VIP  : 1;  //!< Virtual Interrupt Pending.
		uint32_t ID   : 1;  //!< CPUID instruction available.
		uint32_t      : 10;
	};
	uint32_t raw;
};

//! The available architecture MSRs.
/*! (*) MSRs have associated bitfield/flag layouts. (*!) MSRs
 *	have associated layouts, but are not defined here. Readability
 *	and writability stated in brackets.
 */
typedef enum msr MSR;
enum msr
{
	MSR_MC_ADDR = 0x0,            //!< Machine Check Exception Address.
	MSR_MC_TYPE,                  //!< Machine Check Exception Type.

	MSR_MONITOR_FSIZE = 0x6,      //!< Monitor Filter Size.

	MSR_TSC       = 0x10,         //!< Time Stamp Counter.
	MSR_PFID      = 0x17,         //!< Platform ID.                                 [R]   (*!)
	MSR_APIC_BASE = 0x1B,         //!< lAPIC enable/base/flags.                     [R/W] (*)
	MSR_FEAT_CTL  = 0x3A,         //!< Control Features for Intel 64.               [R/W] (*!)
	MSR_TSC_ADJUST,               //!< Local offset value for TSC.                  [R/WC]

	MSR_BIOS_UPDT_TRIG = 0x79,    //!< Writing enables a microcode update.          [W]
	MSR_BIOS_SIGN_ID   = 0x8B,    //!< BIOS Update Signature.                       [R]

	MSR_SMM_MONITOR_CTL = 0x9B,   //!< SMM Monitor Configuration.                   [R/W] (*!)
	MSR_SMBASE          = 0x9E,   //!< Logical processor SMRAM image address.       [R]

	MSR_PERFCTR0 = 0xC1,          //!< General Performance Counter 0.               [R/W]
	MSR_PERFCTR1,                 //!< General Performance Counter 1.               [R/W]
	/* Note that there are 6 more performance counters, PM2-PM7. */
	MSR_MPERF    = 0xE7,          //!< Maximum Qualified Performance Clock Counter. [R/WC]
	MSR_APERF,                    //!< Actual Performance Clock Counter.            [R/WC]

	MSR_MTRRCAP = 0xFE,           //!< MTRR Capability.                             [R] (*!)

	MSR_SYSENTER_CS = 0x174,      //!< SYSENTER CS selector.                        [R/W]
	MSR_SYSENTER_ESP,             //!< SYSENTER ESP address.                        [R/W]
	MSR_SYSENTER_EIP,             //!< SYSENTER EIP address.                        [R/W]

	MSR_MCG_CAP = 0x179,          //!< Global Machine Check Capability.             [R]   (*!)
	MSR_MCG_STATUS,               //!< Global Machine Check Status.                 [R]
	MSR_MCG_CTL,                  //!< Global Machine Check Control.                [R/W]

	MSR_PERFEVTSEL0 = 0x186,      //!< Performance Event Select Register 0.         [R/W] (*!)
	/*! Note that there are 3 more performance event select registers. */
	MSR_PERF_STATUS = 0x198,      //!< Current Performance State Value.             [R]
	MSR_PERF_CTL,                 //!< Target Performance State Value.              [R/W] (*!)
	MSR_CLOCK_MOD,                //!< Clock Modulation Control.                    [R/W] (*!)
	MSR_THERM_INT,                //!< Thermal Interrupt Control.                   [R/W] (*!)
	MSR_THERM_STATUS,             //!< Thermal Status Information.                  [R]   (*!)

	MSR_MISC_ENABLE = 0x1A0,      //!< Enable misc. processor features.             [R/W] (*)

	MSR_ENERGY_PERF_BIAS = 0x1B0, //!< Energy saving. 0-15 high to low energy use.  [R/W]
	MSR_PKG_THERM_STATUS,         //!< Package Thermal Status Information.          [R]   (*!)
	MSR_PKG_THERM_INT,            //!< Package Thermal Interrupt Control.           [R/W] (*!)

	MSR_DEBUGCTL = 0x1D9,         //!< Trace/Profile Resource Control.              [R/W] (*!)

	MSR_SMRR_PHYSBASE = 0x1F2,    //!< SMRR Base Address.                           [W]   (*!)
	MSR_SMRR_PHYSMASK,            //!< SMRR Range Mask.                             [W]   (*!)

	MSR_PF_DCA_CAP = 0x1F8,       //!< DCA Capabiity.                               [R]
	MSR_CPU_DCA_CAP,              //!< CPU supports Prefetch-Hint.
	MSR_DCA_0_CAP,                //!< DCA type 0 Status and Control Register.      [R/W] (*!)

	MSR_MTRR_PHYSBASE0 = 0x200,   //!< Variable Range MTRR Base Address.
	MSR_MTRR_PHYSMASK0,           //!< Variable Range MTRR Range Mask.
	/*! Note that there are 9 more MTRR bases and masks. */

	MSR_FIXED_CTR0    = 0x309,    //!< Fixed-Function Performance Counter 0.        [R/W]
	/*! Note that there are 2 more fixed counters. */
	MSR_PERF_CAPS     = 0x345,    //!< Fixed Performance Capabilities.              [R]   (*!)
	MSR_FIXED_CTR_CTL = 0x38D,    //!< Fixed Performance Counter Control.           [R/W] (*!)
	MSR_GPERF_STATUS  = 0x38E,    //!< Global Performance Counter Status.           [R]   (*!)
	MSR_GPERF_CTL, 	              //!< Global Performance Counter Control.          [R/W] (*!)
	MSR_GPERF_OVF_CTL,            //!< Global Performance Cunter Overflow Control.  [R/W] (*!)

	MSR_PEBS_ENABLE  = 0x3F1,     //!< PEBS Control.                                [R/W]
	MSR_DS_AREA      = 0x600,     //!< DS Save Area.                                [R/W] (*!)
	MSR_TSC_DEADLINE = 0x6E0,     //!< TSC Target of lAPIC TSC Deadline Mode.       [R/W]

	MSR_QM_EVTSEL = 0xC8D,        //!< QoS Monitoring Event Select Register.        [R/W] (*!)
	MSR_QM_CTR,                   //!< QoS Monitoring Counter Register.             [R]
	MSR_PQR_ASSOC                 //!< QoS Resource Association Register.           [R/W]
};

//! The MSR conaining the APIC base address and related flags.
typedef union msr_apic_base msr_apic_base_t;
union msr_apic_base
{
	struct __packed
	{
		uint64_t               : 8;
		uint64_t bsp           : 1;
		uint64_t               : 1;
		uint64_t x2apic_enable : 1;
		uint64_t global_enable : 1;
		uint64_t base          : 24;
		uint64_t               : 28;
	};
	struct __packed
	{
		uint32_t low;
		uint32_t high;
	};
	uint64_t raw;
};

//! The MSR containing various enable/disable feature flags.
typedef union msr_misc_enable msr_misc_enable_t;
union msr_misc_enable
{
	struct __packed
	{
		uint64_t FSE     : 1; //!< Fast-Strings Enable
		uint64_t LPPE    : 1;
		uint64_t FCE     : 1;
		uint64_t TM1E    : 1; //!< Auto Thermal Control Enable (TM1)
		uint64_t SLD     : 1;
		uint64_t TCD     : 1;
		uint64_t L3D     : 1;
		uint64_t PMA     : 1; //!< Performance Monitoring Available
		uint64_t SLE     : 1;
		uint64_t PQD     : 1;
		uint64_t PBE     : 1;
		uint64_t BTSU    : 1; //!< Branch Trace Storage Unavailable
		uint64_t PEBSU   : 1; //!< Precise Event Based Storage Unavailable
		uint64_t TM2E    : 1;
		uint64_t         : 1;
		uint64_t GV1E    : 1;
		uint64_t GV3E    : 1; //!< Intel SpeedStep Technology Enable
		uint64_t BDPHE   : 1;
		uint64_t MONE    : 1; //!< FSM Monitor Enable
		uint64_t ASPD    : 1;
		uint64_t GV3L    : 1;
		uint64_t FPRE    : 1;
		uint64_t LCMV    : 1; //!< Limit CPUID Max Val
		uint64_t ETPRD   : 1; //!< xTPR Message Disable
		uint64_t L1DCCM  : 1;
		uint64_t         : 9;
		uint64_t XDD     : 1; //!< XD Bit Disable
		uint64_t         : 2;
		uint64_t L1STPRD : 1;
		uint64_t DAD     : 1;
		uint64_t L1IPPRD : 1;
		uint64_t         : 24;
	};
	struct __packed
	{
		uint32_t low;
		uint32_t high;
	};
	uint64_t raw;
};

//! Read various processor flags.
static inline uint32_t eflags_read()
{
	uint32_t ret;
	__asm ("pushfl; popl %0" : "=r" (ret));
	return ret;
}

//! Write new values to processor flags.
static inline void eflags_write(uint32_t val)
{
	__asm ("pushl %0; popfl" :: "r" (val) : "memory");
}

//! Check if the CPUID instruction is available.
static inline bool cpuid_is_available()
{
	eflags_t flags = { .raw = eflags_read() };
	bool old_id = flags.ID;
	flags.ID = !old_id;

	eflags_write(flags.raw);
	flags.raw = eflags_read();

	return !(flags.ID == old_id);
}

//! Get CPU information.
static inline void cpuid(uint32_t code, uint32_t* res[4])
{
	uint32_t regs[4] = {0};
	__asm ("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3]) : "a" (code));

	if (res[0]) *(res[0]) = regs[0];
	if (res[1]) *(res[1]) = regs[1];
	if (res[2]) *(res[2]) = regs[2];
	if (res[3]) *(res[3]) = regs[3];
}

//! Read from a Model Specific Register.
static inline void rdmsr(MSR msr, uint32_t* low, uint32_t* high)
{
	__asm ("rdmsr" : "=a" (*low), "=d" (*high) : "c" (msr));
}

//! Write to a Model Specific Register.
static inline void wrmsr(MSR msr, uint32_t low, uint32_t high)
{
	__asm ("wrmsr" :: "a" (low), "d" (high), "c" (msr) : "memory");
}

//! Get the CPU's cycle count.
static inline void rdtsc(uint32_t* low, uint32_t* high)
{
	__asm ("rdtsc" : "=a" (*low), "=d" (*high));
}
