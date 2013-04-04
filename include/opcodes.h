#ifndef __OPCODES_H__
# define __OPCODES_H__

void _call_brk (struct cpu *, uint16_t);
void _call_ora (struct cpu *, uint16_t);
void _call_non (struct cpu *, uint16_t);
void _call_asl (struct cpu *, uint16_t);
void _call_php (struct cpu *, uint16_t);
void _call_adc (struct cpu *, uint16_t);
void _call_and (struct cpu *, uint16_t);
void _call_bcc (struct cpu *, uint16_t);
void _call_bcs (struct cpu *, uint16_t);
void _call_beq (struct cpu *, uint16_t);
void _call_bit (struct cpu *, uint16_t);
void _call_bmi (struct cpu *, uint16_t);
void _call_bne (struct cpu *, uint16_t);
void _call_bpl (struct cpu *, uint16_t);
void _call_bvc (struct cpu *, uint16_t);
void _call_bvs (struct cpu *, uint16_t);
void _call_clc (struct cpu *, uint16_t);
void _call_cld (struct cpu *, uint16_t);
void _call_cli (struct cpu *, uint16_t);
void _call_clv (struct cpu *, uint16_t);
void _call_cmp (struct cpu *, uint16_t);
void _call_cpx (struct cpu *, uint16_t);
void _call_cpy (struct cpu *, uint16_t);
void _call_dec (struct cpu *, uint16_t);
void _call_dex (struct cpu *, uint16_t);
void _call_dey (struct cpu *, uint16_t);
void _call_eor (struct cpu *, uint16_t);
void _call_inc (struct cpu *, uint16_t);
void _call_inx (struct cpu *, uint16_t);
void _call_iny (struct cpu *, uint16_t);
void _call_jmp (struct cpu *, uint16_t);
void _call_jsr (struct cpu *, uint16_t);
void _call_lda (struct cpu *, uint16_t);
void _call_ldx (struct cpu *, uint16_t);
void _call_ldy (struct cpu *, uint16_t);
void _call_lsr (struct cpu *, uint16_t);
void _call_nop (struct cpu *, uint16_t);
void _call_pha (struct cpu *, uint16_t);
void _call_pla (struct cpu *, uint16_t);
void _call_plp (struct cpu *, uint16_t);
void _call_rol (struct cpu *, uint16_t);
void _call_ror (struct cpu *, uint16_t);
void _call_rti (struct cpu *, uint16_t);
void _call_rts (struct cpu *, uint16_t);
void _call_sbc (struct cpu *, uint16_t);
void _call_sec (struct cpu *, uint16_t);
void _call_sed (struct cpu *, uint16_t);
void _call_sei (struct cpu *, uint16_t);
void _call_sta (struct cpu *, uint16_t);
void _call_stx (struct cpu *, uint16_t);
void _call_sty (struct cpu *, uint16_t);
void _call_tax (struct cpu *, uint16_t);
void _call_tay (struct cpu *, uint16_t);
void _call_tsx (struct cpu *, uint16_t);
void _call_txa (struct cpu *, uint16_t);
void _call_txs (struct cpu *, uint16_t);
void _call_tya (struct cpu *, uint16_t);

enum addr_mode {
    ADDR_MODE_ZPA,
    ADDR_MODE_REL,
    ADDR_MODE_IMP,
    ADDR_MODE_ABS,
    ADDR_MODE_ACC,
    ADDR_MODE_IMM,
    ADDR_MODE_ZPX,
    ADDR_MODE_ZPY,
    ADDR_MODE_ABX,
    ADDR_MODE_ABY,
    ADDR_MODE_INX,
    ADDR_MODE_INY,
    ADDR_MODE_IAB
};

struct _opcode {
    char *              name;
    void                (*call)(struct cpu *, uint16_t);
    uint8_t             len;
    uint8_t             time;
    enum addr_mode      addr_mode;
} opcodes[] = {
    {"BRK",     _call_brk,  1,      7,  ADDR_MODE_IMP   }, // 0x00
    {"ORA",     _call_ora,  2,      6,  ADDR_MODE_INX   }, // 0x01
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x02
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x03
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x04
    {"ORA",     _call_ora,  2,      3,  ADDR_MODE_ZPA   }, // 0x05
    {"ASL",     _call_asl,  2,      5,  ADDR_MODE_ZPA   }, // 0x06
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x07
    {"PHP",     _call_php,  1,      3,  ADDR_MODE_IMP   }, // 0x08
    {"ORA",     _call_ora,  2,      2,  ADDR_MODE_IMM   }, // 0x09
    {"ASL",     _call_asl,  1,      2,  ADDR_MODE_ACC   }, // 0x0A
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x0B
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x0C
    {"ORA",     _call_ora,  3,      4,  ADDR_MODE_ABS   }, // 0x0D
    {"ASL",     _call_asl,  3,      6,  ADDR_MODE_ABS   }, // 0x0E
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x0F
    {"BPL",     _call_bpl,  2,      0,  ADDR_MODE_REL   }, // 0x10
    {"ORA",     _call_ora,  2,      5,  ADDR_MODE_INY   }, // 0x11
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x12
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x13
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x14
    {"ORA",     _call_ora,  2,      4,  ADDR_MODE_ZPX   }, // 0x15
    {"ASL",     _call_asl,  2,      6,  ADDR_MODE_ZPX   }, // 0x16
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x17
    {"CLC",     _call_clc,  1,      2,  ADDR_MODE_IMP   }, // 0x18
    {"ORA",     _call_ora,  3,      4,  ADDR_MODE_INY   }, // 0x19
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x1A
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x1B
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x1C
    {"ORA",     _call_ora,  3,      4,  ADDR_MODE_ABY   }, // 0x1D
    {"ASL",     _call_asl,  3,      7,  ADDR_MODE_ABX   }, // 0x1E
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x1F
    {"JSR",     _call_jsr,  3,      6,  ADDR_MODE_ABS   }, // 0x20
    {"AND",     _call_and,  2,      6,  ADDR_MODE_INX   }, // 0x21
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x22
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x23
    {"BIT",     _call_bit,  2,      3,  ADDR_MODE_ZPA   }, // 0x24
    {"AND",     _call_and,  2,      3,  ADDR_MODE_ZPA   }, // 0x25
    {"ROL",     _call_rol,  2,      5,  ADDR_MODE_ZPA   }, // 0x26
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x27
    {"PLP",     _call_plp,  1,      4,  ADDR_MODE_IMP   }, // 0x28
    {"AND",	_call_and,  2,      2,  ADDR_MODE_IMM   }, // 0x29
    {"ROL",     _call_rol,  1,      2,  ADDR_MODE_ACC   }, // 0x2A
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x2B
    {"BIT",     _call_bit,  3,      4,  ADDR_MODE_ABS   }, // 0x2C
    {"AND",	_call_and,  3,      4,  ADDR_MODE_ABS   }, // 0x2D
    {"ROL",     _call_rol,  3,      6,  ADDR_MODE_ABS   }, // 0x2E
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x2F
    {"BMI",     _call_bmi,  2,      0,  ADDR_MODE_REL   }, // 0x30
    {"AND",	_call_and,  2,      5,  ADDR_MODE_INY   }, // 0x31
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x32
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x33
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x34
    {"AND",	_call_and,  2,      4,  ADDR_MODE_ZPX   }, // 0x35
    {"ROL",     _call_rol,  2,      6,  ADDR_MODE_ZPX   }, // 0x36
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x37
    {"SEC",     _call_sec,  1,      2,  ADDR_MODE_IMP   }, // 0x38
    {"AND",	_call_and,  3,      4,  ADDR_MODE_ABY   }, // 0x39
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x3A
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x3B
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x3C
    {"AND",	_call_and,  3,      4,  ADDR_MODE_ABX   }, // 0x3D
    {"ROL",     _call_rol,  3,      7,  ADDR_MODE_ABX   }, // 0x3E
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x3F
    {"RTI",     _call_rti,  1,      6,  ADDR_MODE_IMP   }, // 0x40
    {"EOR",     _call_eor,  2,      6,  ADDR_MODE_INX   }, // 0x41
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x42
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x43
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x44
    {"EOR",     _call_eor,  2,      3,  ADDR_MODE_ZPA   }, // 0x45
    {"LSR",     _call_lsr,  2,      5,  ADDR_MODE_ZPA   }, // 0x46
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x47
    {"PHA",     _call_pha,  1,      3,  ADDR_MODE_IMP   }, // 0x48
    {"EOR",     _call_eor,  2,      2,  ADDR_MODE_IMM   }, // 0x49
    {"LSR",     _call_lsr,  1,      2,  ADDR_MODE_ACC   }, // 0x4A
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x4B
    {"JMP",     _call_jmp,  3,      3,  ADDR_MODE_ABS   }, // 0x4C
    {"EOR",     _call_eor,  3,      4,  ADDR_MODE_ABS   }, // 0x4D
    {"LSR",     _call_lsr,  3,      6,  ADDR_MODE_ABS   }, // 0x4E
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x4F
    {"BVC",     _call_bvc,  2,      0,  ADDR_MODE_REL   }, // 0x50
    {"EOR",     _call_eor,  2,      5,  ADDR_MODE_INY   }, // 0x51
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x52
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x53
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x54
    {"EOR",     _call_eor,  2,      4,  ADDR_MODE_ZPX   }, // 0x55
    {"LSR",     _call_lsr,  2,      6,  ADDR_MODE_ZPX   }, // 0x56
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x57
    {"CLI",     _call_cli,  1,      2,  ADDR_MODE_IMP   }, // 0x58
    {"EOR",     _call_eor,  3,      4,  ADDR_MODE_ABY   }, // 0x59
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x5A
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x5B
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x5C
    {"EOR",     _call_eor,  3,      4,  ADDR_MODE_ABX   }, // 0x5D
    {"LSR",     _call_lsr,  3,      7,  ADDR_MODE_ABX   }, // 0x5E
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x5F
    {"RTS",     _call_rts,  1,      6,  ADDR_MODE_IMP   }, // 0x60
    {"ADC",     _call_adc,  2,      6,  ADDR_MODE_INX   }, // 0x61
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x62
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x63
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x64
    {"ADC",     _call_adc,  2,      3,  ADDR_MODE_ZPA   }, // 0x65
    {"ROR",     _call_ror,  2,      5,  ADDR_MODE_ZPA   }, // 0x66
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x67
    {"PLA",     _call_pla,  1,      4,  ADDR_MODE_IMP   }, // 0x68
    {"ADC",     _call_adc,  2,      2,  ADDR_MODE_IMM   }, // 0x69
    {"ROR",     _call_ror,  1,      2,  ADDR_MODE_ACC   }, // 0x6A
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x6B
    {"JMP",     _call_jmp,  3,      5,  ADDR_MODE_IAB   }, // 0x6C
    {"ADC",     _call_adc,  3,      4,  ADDR_MODE_ABS   }, // 0x6D
    {"ROR",     _call_ror,  3,      6,  ADDR_MODE_ABS   }, // 0x6E
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x6F
    {"BVS",     _call_bvs,  2,      0,  ADDR_MODE_REL   }, // 0x70
    {"ADC",     _call_adc,  2,      5,  ADDR_MODE_INY   }, // 0x71
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x72
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x73
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x74
    {"ADC",     _call_adc,  2,      4,  ADDR_MODE_ZPX   }, // 0x75
    {"ROR",     _call_ror,  2,      6,  ADDR_MODE_ZPX   }, // 0x76
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x77
    {"SEI",     _call_sei,  1,      2,  ADDR_MODE_IMP   }, // 0x78
    {"ADC",     _call_adc,  3,      4,  ADDR_MODE_ABY   }, // 0x79
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x7A
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x7B
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x7C
    {"ADC",     _call_adc,  3,      4,  ADDR_MODE_ABX   }, // 0x7D
    {"ROR",     _call_ror,  3,      7,  ADDR_MODE_ABX   }, // 0x7E
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x7F
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x80
    {"STA",     _call_sta,  2,      6,  ADDR_MODE_INX   }, // 0x81
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x82
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x83
    {"STY",     _call_sty,  2,      3,  ADDR_MODE_ZPA   }, // 0x84
    {"STA",     _call_sta,  2,      3,  ADDR_MODE_ZPA   }, // 0x85
    {"STX",     _call_stx,  2,      3,  ADDR_MODE_ZPA   }, // 0x86
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x87
    {"DEY",     _call_dey,  1,      2,  ADDR_MODE_IMP   }, // 0x88
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x89
    {"TXA",     _call_txa,  1,      2,  ADDR_MODE_IMP   }, // 0x8A
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x8B
    {"STY",     _call_sty,  3,      4,  ADDR_MODE_ABS   }, // 0x8C
    {"STA",     _call_sta,  3,      4,  ADDR_MODE_ABS   }, // 0x8D
    {"STX",     _call_stx,  3,      4,  ADDR_MODE_ABS   }, // 0x8E
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x8F
    {"BCC",     _call_bcc,  2,      0,  ADDR_MODE_REL   }, // 0x90
    {"STA",     _call_sta,  2,      6,  ADDR_MODE_INY   }, // 0x91
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x92
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x93
    {"STY",     _call_sty,  2,      4,  ADDR_MODE_ZPX   }, // 0x94
    {"STA",     _call_sta,  2,      4,  ADDR_MODE_ZPX   }, // 0x95
    {"STX",     _call_stx,  2,      4,  ADDR_MODE_ZPX   }, // 0x96
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x97
    {"TYA",     _call_tya,  1,      2,  ADDR_MODE_IMP   }, // 0x98
    {"STA",     _call_sta,  3,      5,  ADDR_MODE_ABY   }, // 0x99
    {"TXS",     _call_txs,  1,      2,  ADDR_MODE_IMP   }, // 0x9A
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 9x9B
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x9C
    {"STA",     _call_sta,  3,      5,  ADDR_MODE_ABX   }, // 0x9D
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x9E
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0x9F
    {"LDY",     _call_ldy,  2,      2,  ADDR_MODE_IMM   }, // 0xA0
    {"LDA",     _call_lda,  2,      6,  ADDR_MODE_INX   }, // 0xA1
    {"LDX",     _call_ldx,  2,      2,  ADDR_MODE_IMM   }, // 0xA2
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xA3
    {"LDY",     _call_ldy,  2,      3,  ADDR_MODE_ZPA   }, // 0xA4
    {"LDA",     _call_lda,  2,      3,  ADDR_MODE_ZPA   }, // 0xA5
    {"LDX",     _call_ldx,  2,      3,  ADDR_MODE_ZPA   }, // 0xA6
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xA7
    {"TAY",     _call_tay,  1,      2,  ADDR_MODE_IMP   }, // 0xA8
    {"LDA",     _call_lda,  2,      2,  ADDR_MODE_IMM   }, // 0xA9
    {"TAX",     _call_tax,  1,      2,  ADDR_MODE_IMP   }, // 0xAA
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xAB
    {"LDY",     _call_ldy,  3,      4,  ADDR_MODE_ABS   }, // 0xAC
    {"LDA",     _call_lda,  3,      4,  ADDR_MODE_ABS   }, // 0xAD
    {"LDX",     _call_ldx,  3,      4,  ADDR_MODE_ABS   }, // 0xAE
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xAF
    {"BCS",     _call_bcs,  2,      0,  ADDR_MODE_REL   }, // 0xB0
    {"LDA",     _call_lda,  2,      5,  ADDR_MODE_INY   }, // 0xB1
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xB2
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xB3
    {"LDY",     _call_ldy,  2,      4,  ADDR_MODE_ZPX   }, // 0xB4
    {"LDA",     _call_lda,  2,      4,  ADDR_MODE_ZPX   }, // 0xB5
    {"LDX",     _call_ldx,  2,      4,  ADDR_MODE_ZPY   }, // 0xB6
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xB7
    {"CLV",     _call_clv,  1,      2,  ADDR_MODE_IMP   }, // 0xB8
    {"LDA",     _call_lda,  3,      4,  ADDR_MODE_INY   }, // 0xB9
    {"TSX",     _call_tsx,  1,      2,  ADDR_MODE_IMP   }, // 0xBA
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xBB
    {"LDY",     _call_ldy,  3,      4,  ADDR_MODE_ABX   }, // 0xBC
    {"LDA",     _call_lda,  3,      4,  ADDR_MODE_ABX   }, // 0xBD
    {"LDX",     _call_ldx,  3,      4,  ADDR_MODE_ABY   }, // 0xBE
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xBF
    {"CPY",     _call_cpy,  2,      2,  ADDR_MODE_IMM   }, // 0xC0
    {"CMP",     _call_cmp,  2,      6,  ADDR_MODE_INX   }, // 0xC1
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xC2
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xC3
    {"CPY",     _call_cpy,  2,      3,  ADDR_MODE_ZPA   }, // 0xC4
    {"CMP",     _call_cmp,  2,      3,  ADDR_MODE_ZPA   }, // 0xC5
    {"DEC",     _call_dec,  2,      5,  ADDR_MODE_ZPA   }, // 0xC6
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xC7
    {"INY",     _call_iny,  1,      2,  ADDR_MODE_IMP   }, // 0xC8
    {"CMP",     _call_cmp,  2,      2,  ADDR_MODE_IMM   }, // 0xC9
    {"DEX",     _call_dex,  1,      2,  ADDR_MODE_IMP   }, // 0xCA
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xCB
    {"CPY",     _call_cpy,  3,      4,  ADDR_MODE_ABS   }, // 0xCC
    {"CMP",     _call_cmp,  3,      4,  ADDR_MODE_ABS   }, // 0xCD
    {"DEC",     _call_dec,  3,      6,  ADDR_MODE_ABS   }, // 0xCE
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xCF
    {"BNE",     _call_bne,  2,      0,  ADDR_MODE_REL   }, // 0xD0
    {"CMP",     _call_cmp,  2,      5,  ADDR_MODE_INY   }, // 0xD1
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xD2
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xD3
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xD4
    {"CMP",     _call_cmp,  2,      4,  ADDR_MODE_ZPX   }, // 0xD5
    {"DEC",     _call_dec,  2,      6,  ADDR_MODE_ZPX   }, // 0xD6
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xD7
    {"CLD",     _call_cld,  1,      2,  ADDR_MODE_IMP   }, // 0xD8
    {"CMP",     _call_cmp,  3,      4,  ADDR_MODE_ABY   }, // 0xD9
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xDA
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xDB
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xDC
    {"CMP",     _call_cmp,  3,      4,  ADDR_MODE_ABX   }, // 0xDD
    {"DEC",     _call_dec,  3,      7,  ADDR_MODE_ABX   }, // 0xDE
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xDF
    {"CPX",     _call_cpx,  2,      2,  ADDR_MODE_IMM   }, // 0xE0
    {"SBC",     _call_sbc,  2,      6,  ADDR_MODE_INX   }, // 0xE1
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xE2
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xE3
    {"CPX",     _call_cpx,  2,      3,  ADDR_MODE_ZPA   }, // 0xE4
    {"SBC",     _call_sbc,  2,      3,  ADDR_MODE_ZPA   }, // 0xE5
    {"INC",     _call_inc,  2,      5,  ADDR_MODE_ZPA   }, // 0xE6
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xE7
    {"INX",     _call_inx,  1,      2,  ADDR_MODE_IMP   }, // 0xE8
    {"SBC",     _call_sbc,  2,      2,  ADDR_MODE_IMM   }, // 0xE9
    {"NOP",     _call_nop,  1,      2,  ADDR_MODE_IMP   }, // 0xEA
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xEB
    {"CPX",     _call_cpx,  3,      4,  ADDR_MODE_ABS   }, // 0xEC
    {"SBC",     _call_sbc,  3,      4,  ADDR_MODE_ABS   }, // 0xED
    {"INC",     _call_inc,  3,      6,  ADDR_MODE_ABS   }, // 0xEE
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xEF
    {"BEQ",     _call_beq,  2,      0,  ADDR_MODE_REL   }, // 0xF0
    {"SBC",     _call_sbc,  2,      5,  ADDR_MODE_INY   }, // 0xF1
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xF2
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xF3
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xF4
    {"SBC",     _call_sbc,  2,      4,  ADDR_MODE_ZPX   }, // 0xF5
    {"INC",     _call_inc,  2,      6,  ADDR_MODE_ZPX   }, // 0xF6
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xF7
    {"SED",     _call_sed,  1,      2,  ADDR_MODE_IMP   }, // 0xF8
    {"SBC",     _call_sbc,  3,      4,  ADDR_MODE_ABY   }, // 0xF9
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xFA
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xFB
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xFC
    {"SBC",     _call_sbc,  3,      4,  ADDR_MODE_ABX   }, // 0xFD
    {"INC",     _call_inc,  3,      7,  ADDR_MODE_ABX   }, // 0xFE
    {"NONE",    _call_non,  1,      0,  ADDR_MODE_IMP   }, // 0xFF
};

#endif /* !__OPCODES_H__ */
