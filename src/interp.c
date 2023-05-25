#include "rvemu.h"

#include "interp_util.h"

static void func_empty(state_t *state, insn_t *insn){}

//I_type
#define FUNC(typ)                                          \
    u64 addr = state->gp_regs[insn->rs1] + (i64)insn->imm; \
    state->gp_regs[insn->rd] = *(typ *)TO_HOST(addr);      \
    while(0)  

//lb -> load byte
//lb a1,   4    (a0)
//   rd   imm   rs1
//rd = *((i8*)(rs1 +imm))
//字节加载 (Load Byte)
static void func_lb(state_t *state, insn_t *insn){
    FUNC(i8);
}

//lh -> load half word
//lh a1,   4    (a0)
//   rd   imm   rs1
//rd = *((i16*)(rs1 +imm))
//半字加载 (Load Halfword)
static void func_lh(state_t *state, insn_t *insn){
    FUNC(i16);
}

//lw -> load word
//lw a1,     4      (a0)
//   rd     imm      rs1
//rd = *((i32*)(rs1 +imm))
//字加载 (Load Word)
static void func_lw(state_t *state, insn_t *insn){
    FUNC(i32);
}

//ld -> load double word
//ld a1,     4      (a0)
//   rd     imm      rs1
//rd = *((i64*)(rs1 +imm))
//双字加载 (Load Doubleword)
static void func_ld(state_t *state, insn_t *insn){
    FUNC(i64);
}

//lbu -> load byte unsigned
//lbu a1,     4      (a0)
//    rd     imm      rs1
//rd = *((u8*)(rs1 +imm))
//无符号字节加载 (Load Byte, Unsigned)
static void func_lbu(state_t *state, insn_t *insn){
    FUNC(u8);
}

//lhu -> load half word
//lhu a1,   4    (a0)
//   rd   imm   rs1
//rd = *((u16*)(rs1 +imm))
//无符号半字加载 (Load Halfword, Unsigned)
static void func_lhu(state_t *state, insn_t *insn){
    FUNC(u16);
}

//lwu -> load word
//lwu a1,     4      (a0)
//   rd     imm      rs1
//rd = *((u32*)(rs1 +imm))
//无符号字加载 (Load Word, Unsigned)
static void func_lwu(state_t *state, insn_t *insn){
    FUNC(u32);
}
#undef FUNC

//I_type
#define FUNC(expr)                       \
    u64 rs1 = state->gp_regs[insn->rs1]; \
    i64 imm = (i64)insn->imm;            \
    state->gp_regs[insn->rd] = (expr);   \
    while(0)

//加立即数(Add Immediate)
static void func_addi(state_t *state, insn_t *insn) {
    FUNC(rs1 + imm);
}

//加立即数字(Add Word Immediate)
static void func_addiw(state_t *state, insn_t *insn) {
    FUNC((i64)(i32)(rs1 + imm));
}

//立即数逻辑左移(Shift Left Logical Immediate)
static void func_slli(state_t *state, insn_t *insn) {
    FUNC(rs1 << (imm & 0x3f));
}

//立即数逻辑左移字(Shift Left Logical Word Immediate)
static void func_slliw(state_t *state, insn_t *insn) {
    FUNC((i64)(i32)(rs1 << (imm & 0x1f)));
}

//立即数逻辑右移(Shift Right Logical Immediate)
static void func_srli(state_t *state, insn_t *insn) {
    FUNC(rs1 >> (imm & 0x3f));
}

//立即数逻辑右移字(Shift Right Logical Word Immediate)
static void func_srliw(state_t *state, insn_t *insn) {
    FUNC((i64)(i32)((u32)rs1 >> (imm & 0x1f)));
}

//立即数算术右移(Shift Right Arithmetic Immediate)
static void func_srai(state_t *state, insn_t *insn) {
    FUNC((i64)rs1 >> (imm & 0x3f));
}

//立即数算术右移字(Shift Right Arithmetic Word Immediate)
static void func_sraiw(state_t *state, insn_t *insn) {
    FUNC((i64)((i32)rs1 >> (imm & 0x1f)));
}

//小于立即数则置位(Set if Less Than Immediate)
static void func_slti(state_t *state, insn_t *insn) {
    FUNC((i64)rs1 < (i64)imm);
}

//无符号小于立即数则置位(Set if Less Than Immediate, Unsigned)
static void func_sltiu(state_t *state, insn_t *insn) {
    FUNC((u64)rs1 < (u64)imm);
}

////与立即数 (And Immediate)
static void func_andi(state_t *state, insn_t *insn) {
    FUNC(rs1 & (u64)imm);
}

//立即数取或(OR Immediate)
static void func_ori(state_t *state, insn_t *insn) {
    FUNC(rs1 | (u64)imm);
}

//立即数异或(Exclusive-OR Immediate)
static void func_xori(state_t *state, insn_t *insn) {
    FUNC(rs1 ^ imm);
}
#undef FUNC

//U_type
//PC 加立即数 (Add Upper Immediate to PC)
static void func_auipc(state_t *state, insn_t *insn) {
    u64 val = state->pc + (i64)insn->imm;
    state->gp_regs[insn->rd] = val;
}

//S_type
#define FUNC(typ)                                \
    u64 rs1 = state->gp_regs[insn->rs1];         \
    u64 rs2 = state->gp_regs[insn->rs2];         \
    *(typ *)TO_HOST(rs1 + insn->imm) = (typ)rs2; \
    while(0)

//存字节(Store Byte)
static void func_sb(state_t *state, insn_t *insn) {
    FUNC(u8);
}

//存半字(Store Halfword)
static void func_sh(state_t *state, insn_t *insn) {
    FUNC(u16);
}

//存字(Store Word)
static void func_sw(state_t *state, insn_t *insn) {
    FUNC(u32);
}

//存双字(Store Doubleword)
static void func_sd(state_t *state, insn_t *insn) {
    FUNC(u64);
}
#undef FUNC

//R_type
#define FUNC(expr) \
    u64 rs1 = state->gp_regs[insn->rs1]; \
    u64 rs2 = state->gp_regs[insn->rs2]; \
    state->gp_regs[insn->rd] = (expr);   \
    while(0)

//加 (Add)
static void func_add(state_t *state, insn_t *insn) {
    FUNC(rs1 + rs2);
}

//加字(Add Word)
static void func_addw(state_t *state, insn_t *insn) {
    FUNC((i64)(i32)(rs1 + rs2));
}

//减(Substract)
static void func_sub(state_t *state, insn_t *insn) {
    FUNC(rs1 - rs2);
}

//减去字(Substract Word)
static void func_subw(state_t *state, insn_t *insn) {
    FUNC((i64)(i32)(rs1 - rs2));
}

//乘(Multiply)
static void func_mul(state_t *state, insn_t *insn) {
    FUNC(rs1 * rs2);
}

//乘字(Multiply Word)
static void func_mulw(state_t *state, insn_t *insn) {
    FUNC((i64)(i32)(rs1 * rs2));
}

//高位乘(Multiply High)
static void func_mulh(state_t *state, insn_t *insn) {
    FUNC(mulh(rs1, rs2));
}

//高位有符号-无符号乘(Multiply High Signed-Unsigned)
static void func_mulhsu(state_t *state, insn_t *insn) {
    FUNC(mulhsu(rs1, rs2));
}

//高位无符号乘(Multiply High Unsigned)
static void func_mulhu(state_t *state, insn_t *insn) {
    FUNC(mulhu(rs1, rs2));
}

//字除法(Divide Word)
static void func_divw(state_t *state, insn_t *insn) {
    FUNC(rs2 == 0 ? UINT64_MAX : (i32)((i64)(i32)rs1 / (i64)(i32)rs2));
}

//无符号字除法(Divide Word, Unsigned)
static void func_divuw(state_t *state, insn_t *insn) {
    FUNC(rs2 == 0 ? UINT64_MAX : (i32)((u32)rs1 / (u32)rs2));
}

//逻辑左移(Shift Left Logical)
static void func_sll(state_t *state, insn_t *insn) {
    FUNC(rs1 << (rs2 & 0x3f));
}

//逻辑左移字(Shift Left Logical Word)
static void func_sllw(state_t *state, insn_t *insn) {
    FUNC((i64)(i32)(rs1 << (rs2 & 0x1f)));
}

//逻辑右移(Shift Right Logical)
static void func_srl(state_t *state, insn_t *insn) {
    FUNC(rs1 >> (rs2 & 0x3f));
}

//逻辑右移字(Shift Right Logical Word)
static void func_srlw(state_t *state, insn_t *insn) {
    FUNC((i64)(i32)((u32)rs1 >> (rs2 & 0x1f)));
}

//算术右移(Shift Right Arithmetic)
static void func_sra(state_t *state, insn_t *insn) {
    FUNC((i64)rs1 >> (rs2 & 0x3f));
}

//算术右移字(Shift Right Arithmetic Word)
static void func_sraw(state_t *state, insn_t *insn) {
    FUNC((i64)(i32)((i32)rs1 >> (rs2 & 0x1f)));
}

//小于则置位(Set if Less Than)
static void func_slt(state_t *state, insn_t *insn) {
    FUNC((i64)rs1 < (i64)rs2);
}

//无符号小于则置位(Set if Less Than, Unsigned)
static void func_sltu(state_t *state, insn_t *insn) {
    FUNC((u64)rs1 < (u64)rs2);
}

//与 (And)
static void func_and(state_t *state, insn_t *insn) {
    FUNC(rs1 & rs2);
}

//取或(OR)
static void func_or(state_t *state, insn_t *insn) {
    FUNC(rs1 | rs2);
}

//异或(Exclusive-OR)
static void func_xor(state_t *state, insn_t *insn) {
    FUNC(rs1 ^ rs2);
}

//求余数字(Remainder Word)
static void func_remw(state_t *state, insn_t *insn) {
    FUNC(rs2 == 0 ? (i64)(i32)rs1 : (i64)(i32)((i64)(i32)rs1 % (i64)(i32)rs2));
}

//求无符号数的余数字(Remainder Word, Unsigned)
static void func_remuw(state_t *state, insn_t *insn) {
    FUNC(rs2 == 0 ? (i64)(i32)(u32)rs1 : (i64)(i32)((u32)rs1 % (u32)rs2));
}

//求无符号数的余数(Remainder, Unsigned)
static void func_remu(state_t *state, insn_t *insn) {
    FUNC(rs2 == 0 ? rs1 : rs1 % rs2);
}
#undef FUNC

//R_type
//除法(Divide)
static void func_div(state_t *state, insn_t *insn) {
    u64 rs1 = state->gp_regs[insn->rs1];
    u64 rs2 = state->gp_regs[insn->rs2];
    u64 rd = 0;
    if (rs2 == 0) {
        rd = UINT64_MAX;
    } 
    else if (rs1 == INT64_MIN && rs2 == UINT64_MAX) {
        rd = INT64_MIN;
    } 
    else {
        rd = (i64)rs1 / (i64)rs2;
    }
    state->gp_regs[insn->rd] = rd;
}

//R_type
//无符号除法(Divide, Unsigned)
static void func_divu(state_t *state, insn_t *insn) {
    u64 rs1 = state->gp_regs[insn->rs1];
    u64 rs2 = state->gp_regs[insn->rs2];
    u64 rd = 0;
    if (rs2 == 0) {
        rd = UINT64_MAX;
    } 
    else {
        rd = rs1 / rs2;
    }
    state->gp_regs[insn->rd] = rd;
}

//R_type
//求余数(Remainder)
static void func_rem(state_t *state, insn_t *insn) {
    u64 rs1 = state->gp_regs[insn->rs1];
    u64 rs2 = state->gp_regs[insn->rs2];
    u64 rd = 0;
    if (rs2 == 0) {
        rd = rs1;
    } 
    else if (rs1 == INT64_MIN && rs2 == UINT64_MAX) {
        rd = 0;
    } 
    else {
        rd = (i64)rs1 % (i64)rs2;
    }
    state->gp_regs[insn->rd] = rd;
}

//R_type
//高位立即数加载 (Load Upper Immediate)
static void func_lui(state_t *state, insn_t *insn) {
    state->gp_regs[insn->rd] = (i64)insn->imm;
}

//B_type
#define FUNC(expr)                                   \
    u64 rs1 = state->gp_regs[insn->rs1];             \
    u64 rs2 = state->gp_regs[insn->rs2];             \
    u64 target_addr = state->pc + (i64)insn->imm;    \
    if (expr) {                                      \
        state->reenter_pc = state->pc = target_addr; \
        state->exit_reason = direct_branch;          \
        insn->cont = true;                           \
    }                                                \
    while(0)

//相等时分支 (Branch if Equal)
static void func_beq(state_t *state, insn_t *insn) {
    FUNC((u64)rs1 == (u64)rs2);
}

//不相等时分支 (Branch if Not Equal)
static void func_bne(state_t *state, insn_t *insn) {
    FUNC((u64)rs1 != (u64)rs2);
}

//小于时分支 (Branch if Less Than)
static void func_blt(state_t *state, insn_t *insn) {
    FUNC((i64)rs1 < (i64)rs2);
}

//无符号小于时分支 (Branch if Less Than, Unsigned)
static void func_bltu(state_t *state, insn_t *insn) {
    FUNC((u64)rs1 < (u64)rs2);
}

//大于等于时分支 (Branch if Greater Than or Equal)
static void func_bge(state_t *state, insn_t *insn) {
    FUNC((i64)rs1 >= (i64)rs2);
}

//无符号大于等于时分支 (Branch if Greater Than or Equal, Unsigned)
static void func_bgeu(state_t *state, insn_t *insn) {
    FUNC((u64)rs1 >= (u64)rs2);
}
#undef FUNC

//I_type
//跳转并寄存器链接 (Jump and Link Register)
static void func_jalr(state_t *state, insn_t *insn) {
    u64 rs1 = state->gp_regs[insn->rs1];
    state->gp_regs[insn->rd] = state->pc + (insn->rvc ? 2 : 4);
    state->exit_reason = indirect_branch;
    state->reenter_pc = (rs1 + (i64)insn->imm) & ~(u64)1;
}

//J_type
//跳转并链接 (Jump and Link)
static void func_jal(state_t *state, insn_t *insn) {
    state->gp_regs[insn->rd] = state->pc + (insn->rvc ? 2 : 4);
    state->reenter_pc = state->pc = state->pc + (i64)insn->imm;
    state->exit_reason = direct_branch;
}

//I_type
//环境调用 (Environment Call)
static void func_ecall(state_t *state, insn_t *insn) {
    state->exit_reason = ecall;
    state->reenter_pc = state->pc + 4;
}

//I_type
#define FUNC()                              \
    switch (insn->csr) {                    \
        case fflags:                        \
        case frm:                           \
        case fcsr:{                         \
            break;                          \
        }                                   \
        default:                            \
            fatal("unsupported csr");       \
    }                                       \
    state->gp_regs[insn->rd] = 0;           \
    while(0)
   
//读后写控制状态寄存器 (Control and Status Register Read and Write)
static void func_csrrw(state_t *state, insn_t *insn) { 
    FUNC(); 
}

//读后置位控制状态寄存器 (Control and Status Register Read and Set)
static void func_csrrs(state_t *state, insn_t *insn) { 
    FUNC(); 
}

//读后清除控制状态寄存器 (Control and Status Register Read and Clear)
static void func_csrrc(state_t *state, insn_t *insn) { 
    FUNC(); 
}

//立即数读后写控制状态寄存器 (Control and Status Register Read and Write Immediate)
static void func_csrrwi(state_t *state, insn_t *insn) { 
    FUNC(); 
}

//立即数置位 控 制 状 态 寄 存 器 (Control and Status Register Set Immediate)
//等同于csrci
static void func_csrrsi(state_t *state, insn_t *insn) { 
    FUNC(); 
}

//立即数读后设置控制状态寄存器 (Control and Status Register Read and Set Immediate)
static void func_csrrci(state_t *state, insn_t *insn) { 
    FUNC(); 
}
#undef FUNC

//I_type
//浮点加载字(Floating-point Load Word)
static void func_flw(state_t *state, insn_t *insn) {
    u64 addr = state->gp_regs[insn->rs1] + (i64)insn->imm;
    state->fp_regs[insn->rd].v = *(u32 *)TO_HOST(addr) | ((u64)-1 << 32);
}

//I_type
//浮点加载双字(Floating-point Load Doubleword)
static void func_fld(state_t *state, insn_t *insn) {
    u64 addr = state->gp_regs[insn->rs1] + (i64)insn->imm;
    state->fp_regs[insn->rd].v = *(u64 *)TO_HOST(addr);
}

//S_type
#define FUNC(typ)                                \
    u64 rs1 = state->gp_regs[insn->rs1];         \
    u64 rs2 = state->fp_regs[insn->rs2].v;       \
    *(typ *)TO_HOST(rs1 + insn->imm) = (typ)rs2; \

//单精度浮点存储(Floating-point Store Word)
static void func_fsw(state_t *state, insn_t *insn) {
    FUNC(u32);
}

//双精度浮点存储(Floating-point Store Doubleword)
static void func_fsd(state_t *state, insn_t *insn) {
    FUNC(u64);
}
#undef FUNC

//R4_type
#define FUNC(expr)                            \
    f32 rs1 = state->fp_regs[insn->rs1].f;    \
    f32 rs2 = state->fp_regs[insn->rs2].f;    \
    f32 rs3 = state->fp_regs[insn->rs3].f;    \
    state->fp_regs[insn->rd].f = (f32)(expr); \
    while(0)

//单精度浮点乘加(Floating-point Fused Multiply-Add, Single-Precision)
static void func_fmadd_s(state_t *state, insn_t *insn) {
    FUNC(rs1 * rs2 + rs3);
}

//单精度浮点乘减(Floating-point Fused Multiply-Subtarct, Single-Precision)
static void func_fmsub_s(state_t *state, insn_t *insn) {
    FUNC(rs1 * rs2 - rs3);
}

//单精度浮点乘取反减(Floating-point Fused Negative Multiply-Subtract, Single-Precision)
static void func_fnmsub_s(state_t *state, insn_t *insn) {
    FUNC(-(rs1 * rs2) + rs3);
}

//单精度浮点乘取反加(Floating-point Fused Negative Multiply-Add, Single-Precision)
static void func_fnmadd_s(state_t *state, insn_t *insn) {
    FUNC(-(rs1 * rs2) - rs3);
}
#undef FUNC

//R4_type
#define FUNC(expr)                         \
    f64 rs1 = state->fp_regs[insn->rs1].d; \
    f64 rs2 = state->fp_regs[insn->rs2].d; \
    f64 rs3 = state->fp_regs[insn->rs3].d; \
    state->fp_regs[insn->rd].d = (expr);   \
    while(0)

//双精度浮点乘加(Floating-point Fused Multiply-Add, Double-Precision)
static void func_fmadd_d(state_t *state, insn_t *insn) {
    FUNC(rs1 * rs2 + rs3);
}

//双精度浮点乘减(Floating-point Fused Multiply-Subtract, Double-Precision)
static void func_fmsub_d(state_t *state, insn_t *insn) {
    FUNC(rs1 * rs2 - rs3);
}

//双精度浮点乘取反减(Floating-point Fused Negative Multiply-Subtract, Double-Precision)
static void func_fnmsub_d(state_t *state, insn_t *insn) {
    FUNC(-(rs1 * rs2) + rs3);
}

//双精度浮点乘取反加(Floating-point Fused Negative Multiply-Add, Double-Precision)
static void func_fnmadd_d(state_t *state, insn_t *insn) {
    FUNC(-(rs1 * rs2) - rs3);
}
#undef FUNC

//R_type
#define FUNC(expr)                                                 \
    f32 rs1 = state->fp_regs[insn->rs1].f;                         \
    __attribute__((unused)) f32 rs2 = state->fp_regs[insn->rs2].f; \
    state->fp_regs[insn->rd].f = (f32)(expr);                      \
    while(0)

//单精度浮点加(Floating-point Add, Single-Precision)
static void func_fadd_s(state_t *state, insn_t *insn) {
    FUNC(rs1 + rs2);
}

//单精度浮点减(Floating-point Subtract, Single-Precision)
static void func_fsub_s(state_t *state, insn_t *insn) {
    FUNC(rs1 - rs2);
}

//单精度浮点乘(Floating-point Multiply, Single-Precision)
static void func_fmul_s(state_t *state, insn_t *insn) {
    FUNC(rs1 * rs2);
}

//单精度浮点除法(Floating-point Divide, Single-Precision)
static void func_fdiv_s(state_t *state, insn_t *insn) {
    FUNC(rs1 / rs2);
}

//单精度浮点平方根(Floating-point Square Root, Single-Precision)
static void func_fsqrt_s(state_t *state, insn_t *insn) {
    FUNC(sqrtf(rs1));
}

//单精度浮点最小值(Floating-point Minimum, Single-Precision)
static void func_fmin_s(state_t *state, insn_t *insn) {
    FUNC(rs1 < rs2 ? rs1 : rs2);
}

//单精度浮点最大值(Floating-point Maximum, Single-Precision)
static void func_fmax_s(state_t *state, insn_t *insn) {
    FUNC(rs1 > rs2 ? rs1 : rs2);
}
#undef FUNC

//R_type
#define FUNC(expr)                                                 \
    f64 rs1 = state->fp_regs[insn->rs1].d;                         \
    __attribute__((unused)) f64 rs2 = state->fp_regs[insn->rs2].d; \
    state->fp_regs[insn->rd].d = (expr);                           \
    while(0)

//双精度浮点加(Floating-point Add, Double-Precision)
static void func_fadd_d(state_t *state, insn_t *insn) {
    FUNC(rs1 + rs2);
}

//双精度浮点减(Floating-point Subtract, Double-Precision)
static void func_fsub_d(state_t *state, insn_t *insn) {
    FUNC(rs1 - rs2);
}

//双精度浮点乘(Floating-point Multiply, Double-Precision)
static void func_fmul_d(state_t *state, insn_t *insn) {
    FUNC(rs1 * rs2);
}

//双精度浮点除法(Floating-point Divide, Double-Precision)
static void func_fdiv_d(state_t *state, insn_t *insn) {
    FUNC(rs1 / rs2);
}

//双精度浮点平方根(Floating-point Square Root, Double-Precision)
static void func_fsqrt_d(state_t *state, insn_t *insn) {
    FUNC(sqrt(rs1));
}

//双精度浮点最小值(Floating-point Minimum, Double-Precision)
static void func_fmin_d(state_t *state, insn_t *insn) {
    FUNC(rs1 < rs2 ? rs1 : rs2);
}

//双精度浮点最大值(Floating-point Maximum, Double-Precision)
static void func_fmax_d(state_t *state, insn_t *insn) {
    FUNC(rs1 > rs2 ? rs1 : rs2);
}
#undef FUNC


//R_type
#define FUNC(n, x)                                                                    \
    u32 rs1 = state->fp_regs[insn->rs1].w;                                            \
    u32 rs2 = state->fp_regs[insn->rs2].w;                                            \
    state->fp_regs[insn->rd].v = (u64)fsgnj32(rs1, rs2, n, x) | ((uint64_t)-1 << 32); \
    while(0)

//单精度浮点符号注入(Floating-point Sign Inject, Single-Precision)
static void func_fsgnj_s(state_t *state, insn_t *insn) {
    FUNC(false, false);
}

//单精度浮点符号取反注入(Floating-point Sign Inject-Negate, Single-Precision)
static void func_fsgnjn_s(state_t *state, insn_t *insn) {
    FUNC(true, false);
}

//单精度浮点符号异或注入(Floating-point Sign Inject-XOR, Single-Precision)
static void func_fsgnjx_s(state_t *state, insn_t *insn) {
    FUNC(false, true);
}
#undef FUNC


//R_type
#define FUNC(n, x)                                        \
    u64 rs1 = state->fp_regs[insn->rs1].v;                \
    u64 rs2 = state->fp_regs[insn->rs2].v;                \
    state->fp_regs[insn->rd].v = fsgnj64(rs1, rs2, n, x); \
    while(0)

//双精度浮点符号注入(Floating-point Sign Inject, Double-Precision)
static void func_fsgnj_d(state_t *state, insn_t *insn) {
    FUNC(false, false);
}

//双精度浮点符号取反注入(Floating-point Sign Inject-Negate, Double-Precision)
static void func_fsgnjn_d(state_t *state, insn_t *insn) {
    FUNC(true, false);
}

//双精度浮点符号异或注入(Floating-point Sign Inject-XOR, Double-Precision)
static void func_fsgnjx_d(state_t *state, insn_t *insn) {
    FUNC(false, true);
}
#undef FUNC


//单精度浮点向字转换(Floating-point Convert to Word from Single)
static void func_fcvt_w_s(state_t *state, insn_t *insn) {
    state->gp_regs[insn->rd] = (i64)(i32)llrintf(state->fp_regs[insn->rs1].f);
}

//单精度浮点向无符号字转换(Floating-point Convert to Unsigned Word from Single)
static void func_fcvt_wu_s(state_t *state, insn_t *insn) {
    state->gp_regs[insn->rd] = (i64)(i32)(u32)llrintf(state->fp_regs[insn->rs1].f);
}

//双精度浮点向字转换(Floating-point Convert to Word from Double)
static void func_fcvt_w_d(state_t *state, insn_t *insn) {
    state->gp_regs[insn->rd] = (i64)(i32)llrint(state->fp_regs[insn->rs1].d);
}

//双精度浮点向无符号字转换(Floating-point Convert to Unsigned Word from Double)
static void func_fcvt_wu_d(state_t *state, insn_t *insn) {
    state->gp_regs[insn->rd] = (i64)(i32)(u32)llrint(state->fp_regs[insn->rs1].d);
}

//字向单精度浮点转换(Floating-point Convert to Single from Word)
static void func_fcvt_s_w(state_t *state, insn_t *insn) {
    state->fp_regs[insn->rd].f = (f32)(i32)state->gp_regs[insn->rs1];
}

//无符号字向单精度浮点转换(Floating-point Convert to Single from Unsigned Word)
static void func_fcvt_s_wu(state_t *state, insn_t *insn) {
    state->fp_regs[insn->rd].f = (f32)(u32)state->gp_regs[insn->rs1];
}

//字向双精度浮点转换(Floating-point Convert to Double from Word)
static void func_fcvt_d_w(state_t *state, insn_t *insn) {
    state->fp_regs[insn->rd].d = (f64)(i32)state->gp_regs[insn->rs1];
}

//无符号字向双精度浮点转换(Floating-point Convert to Double from Unsigned Word)
static void func_fcvt_d_wu(state_t *state, insn_t *insn) {
    state->fp_regs[insn->rd].d = (f64)(u32)state->gp_regs[insn->rs1];
}

//单精度浮点移动(Floating-point Move Word to Integer)
static void func_fmv_x_w(state_t *state, insn_t *insn) {
    state->gp_regs[insn->rd] = (i64)(i32)state->fp_regs[insn->rs1].w;
}

//单精度浮点移动(Integer to Floating-point Move Word)
static void func_fmv_w_x(state_t *state, insn_t *insn) {
    state->fp_regs[insn->rd].w = (u32)state->gp_regs[insn->rs1];
}

//双精度浮点移动(Floating-point Move Doubleword to Integer)
static void func_fmv_x_d(state_t *state, insn_t *insn) {
    state->gp_regs[insn->rd] = state->fp_regs[insn->rs1].v;
}

//双精度浮点移动(Floating-point Move Doubleword from Integer)
static void func_fmv_d_x(state_t *state, insn_t *insn) {
    state->fp_regs[insn->rd].v = state->gp_regs[insn->rs1];
}


//R_type
#define FUNC(expr)                         \
    f32 rs1 = state->fp_regs[insn->rs1].f; \
    f32 rs2 = state->fp_regs[insn->rs2].f; \
    state->gp_regs[insn->rd] = (expr);     \
    while(0)

//单精度浮点相等(Floating-point Equals, Single-Precision)
static void func_feq_s(state_t *state, insn_t *insn) {
    FUNC(rs1 == rs2);
}

//单精度浮点小于 (Floating-point Less Than, Single-Precision)
static void func_flt_s(state_t *state, insn_t *insn) {
    FUNC(rs1 < rs2);
}

//单精度浮点小于等于(Floating-point Less Than or Equal, Single-Precision)
static void func_fle_s(state_t *state, insn_t *insn) {
    FUNC(rs1 <= rs2);
}
#undef FUNC

//R_type
#define FUNC(expr)                         \
    f64 rs1 = state->fp_regs[insn->rs1].d; \
    f64 rs2 = state->fp_regs[insn->rs2].d; \
    state->gp_regs[insn->rd] = (expr);     \


//双精度浮点相等(Floating-point Equals, Double-Precision)
static void func_feq_d(state_t *state, insn_t *insn) {
    FUNC(rs1 == rs2);
}

////双精度浮点小于(Floating-point Less Than, Double-Precision)
static void func_flt_d(state_t *state, insn_t *insn) {
    FUNC(rs1 < rs2);
}

//双精度浮点小于等于(Floating-point Less Than or Equal, Double-Precision)
static void func_fle_d(state_t *state, insn_t *insn) {
    FUNC(rs1 <= rs2);
}
#undef FUNC


//单精度浮点分类(Floating-point Classify, Single-Precision)
static void func_fclass_s(state_t *state, insn_t *insn) {
    state->gp_regs[insn->rd] = f32_classify(state->fp_regs[insn->rs1].f);
}

//双精度浮点分类(Floating-point Classify, Double-Precision)
static void func_fclass_d(state_t *state, insn_t *insn) {
    state->gp_regs[insn->rd] = f64_classify(state->fp_regs[insn->rs1].d);
}

//单精度浮点向长整型转换(Floating-point Convert to Long from Single)
static void func_fcvt_l_s(state_t *state, insn_t *insn) {
    state->gp_regs[insn->rd] = (i64)llrintf(state->fp_regs[insn->rs1].f);
}

//单精度浮点向无符号长整型转换(Floating-point Convert to Unsigned Long from Single)
static void func_fcvt_lu_s(state_t *state, insn_t *insn) {
    state->gp_regs[insn->rd] = (u64)llrintf(state->fp_regs[insn->rs1].f);
}

//双精度浮点向长整型转换(Floating-point Convert to Long from Double)
static void func_fcvt_l_d(state_t *state, insn_t *insn) {
    state->gp_regs[insn->rd] = (i64)llrint(state->fp_regs[insn->rs1].d);
}

//双精度浮点向无符号长整型转换(Floating-point Convert to Unsigned Long from Double)
static void func_fcvt_lu_d(state_t *state, insn_t *insn) {
    state->gp_regs[insn->rd] = (u64)llrint(state->fp_regs[insn->rs1].d);
}

//长整型向单精度浮点转换(Floating-point Convert to Single from Long)
static void func_fcvt_s_l(state_t *state, insn_t *insn) {
    state->fp_regs[insn->rd].f = (f32)(i64)state->gp_regs[insn->rs1];
}

//无符号长整型向单精度浮点转换(Floating-point Convert to Single from Unsigned Long)
static void func_fcvt_s_lu(state_t *state, insn_t *insn) {
    state->fp_regs[insn->rd].f = (f32)(u64)state->gp_regs[insn->rs1];
}

//长整型向双精度浮点转换(Floating-point Convert to Double from Long)
static void func_fcvt_d_l(state_t *state, insn_t *insn) {
    state->fp_regs[insn->rd].d = (f64)(i64)state->gp_regs[insn->rs1];
}

//无符号长整型向双精度浮点转换(Floating-point Convert to Double from Unsigned Long)
static void func_fcvt_d_lu(state_t *state, insn_t *insn) {
    state->fp_regs[insn->rd].d = (f64)(u64)state->gp_regs[insn->rs1];
}

//双精度向单精度浮点转换(Floating-point Convert to Single from Double)
static void func_fcvt_s_d(state_t *state, insn_t *insn) {
    state->fp_regs[insn->rd].f = (f32)state->fp_regs[insn->rs1].d;
}

//单精度向双精度浮点转换(Floating-point Convert to Double from Single)
static void func_fcvt_d_s(state_t *state, insn_t *insn) {
    state->fp_regs[insn->rd].d = (f64)state->fp_regs[insn->rs1].f;
}


typedef void (func_t)(state_t *, insn_t *);

static func_t * funcs[] = {
    func_lb,
    func_lh,
    func_lw,
    func_ld,
    func_lbu,
    func_lhu,
    func_lwu,
    func_empty,
    func_empty,
    func_addi,
    func_slli,
    func_slti,
    func_sltiu,
    func_xori,
    func_srli,
    func_srai,
    func_ori,
    func_andi,
    func_auipc,
    func_addiw,
    func_slliw,
    func_srliw,
    func_sraiw,
    func_sb,
    func_sh,
    func_sw,
    func_sd,
    func_add,
    func_sll,
    func_slt,
    func_sltu,
    func_xor,
    func_srl,
    func_or,
    func_and,
    func_mul,
    func_mulh,
    func_mulhsu,
    func_mulhu,
    func_div,
    func_divu,
    func_rem,
    func_remu,
    func_sub,
    func_sra,
    func_lui,
    func_addw,
    func_sllw,
    func_srlw,
    func_mulw,
    func_divw,
    func_divuw,
    func_remw,
    func_remuw,
    func_subw,
    func_sraw,
    func_beq,
    func_bne,
    func_blt,
    func_bge,
    func_bltu,
    func_bgeu,
    func_jalr,
    func_jal,
    func_ecall,
    func_csrrc,
    func_csrrci,
    func_csrrs,
    func_csrrsi,
    func_csrrw,
    func_csrrwi,
    func_flw,
    func_fsw,
    func_fmadd_s,
    func_fmsub_s,
    func_fnmsub_s,
    func_fnmadd_s,
    func_fadd_s,
    func_fsub_s,
    func_fmul_s,
    func_fdiv_s,
    func_fsqrt_s,
    func_fsgnj_s,
    func_fsgnjn_s,
    func_fsgnjx_s,
    func_fmin_s,
    func_fmax_s,
    func_fcvt_w_s,
    func_fcvt_wu_s,
    func_fmv_x_w,
    func_feq_s,
    func_flt_s,
    func_fle_s,
    func_fclass_s,
    func_fcvt_s_w,
    func_fcvt_s_wu,
    func_fmv_w_x,
    func_fcvt_l_s,
    func_fcvt_lu_s,
    func_fcvt_s_l,
    func_fcvt_s_lu,
    func_fld,
    func_fsd,
    func_fmadd_d,
    func_fmsub_d,
    func_fnmsub_d,
    func_fnmadd_d,
    func_fadd_d,
    func_fsub_d,
    func_fmul_d,
    func_fdiv_d,
    func_fsqrt_d,
    func_fsgnj_d,
    func_fsgnjn_d,
    func_fsgnjx_d,
    func_fmin_d,
    func_fmax_d,
    func_fcvt_s_d,
    func_fcvt_d_s,
    func_feq_d,
    func_flt_d,
    func_fle_d,
    func_fclass_d,
    func_fcvt_w_d,
    func_fcvt_wu_d,
    func_fcvt_d_w,
    func_fcvt_d_wu,
    func_fcvt_l_d,
    func_fcvt_lu_d,
    func_fmv_x_d,
    func_fcvt_d_l,
    func_fcvt_d_lu,
    func_fmv_d_x,
};

void exec_block_interp(state_t *state){
    static insn_t insn = {0};

    while (true){
        u32 data = *(u32*)TO_HOST(state->pc);
        insn_decode(&insn, data);
        funcs[insn.type](state, &insn);
        state->gp_regs[zero] = 0;

        if(insn.cont){
            break;
        }

        state->pc += insn.rvc ? 2 : 4;

    }
}