.globl _start
    mov sp,#0x10000000
    bl main
hang: b hang

.globl mmu_configure
    mmu_configure:
    // Initialize translation table control registers
    LDR X1, =0x3520 // 4GB space 4KB granularity
    // Inner-shareable.
    MSR TCR_EL3, X1 // Normal Inner and Outer Cacheable.
    LDR X1, =0xFF440400 // ATTR0 Device-nGnRnE ATTR1 Device.
    MSR MAIR_EL3, X1 // ATTR2 Normal Non-Cacheable.
    ADR X0, ttb0_base // ttb0_base must be a 4KB-aligned address.
    MSR TTBR0_EL3, X0
    RET

.global mmu_enable
    mmu_enable:
    MRS X0, S3_1_C15_C2_1
    ORR X0, X0, #(0x1 << 6) // The SMP bit.
    MSR S3_1_C15_C2_1, X0
    
    MRS X0, SCTLR_EL3
    ORR X0, X0, #(0x1 << 2)  // Enable data cache.
    ORR X0, X0, #(0x1 << 12) // Enable instruction cache.
    ORR X0, X0, #0x1         // Enable MMU.
    MSR SCTLR_EL3, X0
    DSB SY
    ISB
    RET

//
// The page tables (created at compile time)
//
    // Put a 64-bit value with little endianness.
    .macro PUT_64B high, low
    .word \low
    .word \high
    .endm
    // Create an entry pointing to a next-level table.
    .macro TABLE_ENTRY PA, ATTR
    PUT_64B \ATTR, (\PA) + 0x3
    .endm
    // Create an entry for a 1GB block.
    .macro BLOCK_1GB PA, ATTR_HI, ATTR_LO
    PUT_64B \ATTR_HI, ((\PA) & 0xC0000000) | \ATTR_LO | 0x1
    .endm
    // Create an entry for a 2MB block.
    .macro BLOCK_2MB PA, ATTR_HI, ATTR_LO
    PUT_64B \ATTR_HI, ((\PA) & 0xFFE00000) | \ATTR_LO | 0x1
    .endm
    .align 12 // 12 for 4KB granule.
    ttb0_base:
    TABLE_ENTRY level2_pagetable, 0
    .align 12 // 12 for 4KB granule.
    level2_pagetable:
    // Create entries for (cacheable) RAM
    .set ADDR, 0x000 // The current page address.
    .rept 0x1f8
    BLOCK_2MB (ADDR << 20), 0, 0x74C
    .set ADDR, ADDR+2
    .endr
    // The MMIO peripherals start at > 0x1f8 * 2MB = 0x3f00...
    // and should not be cached.
    .rept 0x008
    BLOCK_2MB (ADDR << 20), 0, 0x740
    .set ADDR, ADDR+2
    .endr
