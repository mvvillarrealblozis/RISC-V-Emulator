#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rv_emu.h"
#include "bits.h"

#define DEBUG 0

static void unsupported(char *s, uint32_t n) {
    printf("unsupported %s 0x%x\n", s, n);
    exit(-1);
}

void emu_r_type(struct rv_state *rsp, uint32_t iw) {
    uint32_t rd = get_bits(iw, 7, 5);
    uint32_t rs1 = get_bits(iw, 15, 5);
    uint32_t rs2 = get_bits(iw, 20, 5);
    uint32_t funct3 = get_bits(iw, 12, 3);
    uint32_t funct7 = get_bits(iw, 25, 7);

    if (funct3 == 0b000) {
    	if (funct7 == 0b0000000) { // if its add 
    		rsp->regs[rd] = rsp->regs[rs1] + rsp->regs[rs2];
    	} else if (funct7 == 0b0100000) { // if its sub
    		rsp->regs[rd] = rsp->regs[rs1] - rsp->regs[rs2];
    	} else if (funct7 == 0b0000001) { // if its mul
    		rsp->regs[rd] = rsp->regs[rs1] * rsp->regs[rs2];
    	} else {
    		unsupported("R-type", funct7);
    	}
    } else if (funct3 == 0b111){
    	//and 
        rsp->regs[rd] = rsp->regs[rs1] & rsp->regs[rs2];
    } else if (funct3 == 0b110) {
    	//or 
    	rsp->regs[rd] = rsp->regs[rs1] | rsp->regs[rs2];
    } else if (funct3 == 0b101) {
    	if (funct7 == 0b0000000) { //srl 
    		rsp->regs[rd] = rsp->regs[rs1] >> rsp->regs[rs2];
    	}
    } else if (funct3 == 0b001) { //ssl
    	rsp->regs[rd] = rsp->regs[rs1] << rsp->regs[rs2];
    } else {
    	unsupported("R-type", funct3);
    }
    rsp->pc += 4; // Next instruction
}

void emu_i_type(struct rv_state *rsp, uint32_t iw) {
	uint32_t rd = get_bits(iw, 7, 5);
	uint32_t rs1 = get_bits(iw, 15, 5);
	int32_t imm12 = sign_extend(get_bits(iw, 20, 12), 11);
	uint32_t funct3 = get_bits(iw, 12, 3);

	if (funct3 == 0b101) {
		// srli 
		uint32_t shamt = imm12 & 0x3F;
		rsp->regs[rd] = rsp->regs[rs1] >> shamt;
	} else if (funct3 == 0b000) {
		if (rs1 == 0) {
			rsp->regs[rd] = imm12;		// li
		} else if (imm12 == 0) {
			rsp->regs[rd] = rsp->regs[rs1];   //mv
		} else {
			rsp->regs[rd] = rsp->regs[rs1] + imm12;
		}
	}

	rsp->pc += 4;
}

void emu_u_type(struct rv_state *rsp, uint32_t iw) {
	uint32_t rd = get_bits(iw, 7, 5);

	int32_t imm20 = sign_extend(get_bits(iw, 12, 20), 19);

	rsp->regs[rd] = imm20;

	rsp->pc += 4;
}

void emu_j_type(struct rv_state *rsp, uint32_t iw) {
	uint32_t rd = get_bits(iw, 7, 5);
	int32_t imm20 = sign_extend(
			get_bits(iw, 31, 1) << 11 |
			get_bits(iw, 7, 1) << 6 |
			get_bits(iw, 25, 6) << 4 |
			get_bits(iw, 8, 4),
			11
	);

	imm20 = (imm20 << 12) >> 12;

	if (rd != 0) {
		rsp->regs[rd] = rsp->pc + 4;
	}

	rsp->pc += imm20;
}


void emu_b_type(struct rv_state *rsp, uint32_t iw) {
	uint32_t rs1 = get_bits(iw, 15, 5);
	uint32_t rs2 = get_bits(iw, 20, 5);

	uint32_t funct3 = get_bits(iw, 12, 3);
	
	int64_t imm12 = sign_extend(
		get_bits(iw, 31, 1) << 12 |
		get_bits(iw, 7, 1) << 11 |
		get_bits(iw, 25, 6) << 5 |
		get_bits(iw, 8, 4) << 1,
		12
	);
	
	if (funct3 == 0b001) { //bne 
		if (rsp->regs[rs1] != rsp->regs[rs2]) {
				rsp->pc += imm12;
		} else {
			rsp->pc += 4;
		}
	} else if (funct3 == 0b100) { //blt
		if ((int64_t) rsp->regs[rs1] < (int64_t) rsp->regs[rs2]) {
				rsp->pc += imm12;
		} else {
			rsp->pc += 4;
		}
	} 
}


void emu_jalr(struct rv_state *rsp, uint32_t iw) {
	uint32_t rs1 = get_bits(iw, 15, 5);
	uint64_t val = rsp->regs[rs1];

	rsp->pc = val;
}

static void rv_one(rv_state *state) {
    uint32_t iw  = *((uint32_t*) state->pc);
    //iw = cache_lookup(&state->i_cache, (uint64_t) state->pc);

    uint32_t opcode = get_bits(iw, 0, 7);


#if DEBUG
    printf("iw: %08x\n", iw);
#endif

    switch (opcode) {
        case 0b0110011:
            // R-type instructions have two register operands
            emu_r_type(rsp, iw);
            break;
        case 0b1100111:
            // JALR (aka RET) is a variant of I-type instructions
            emu_jalr(rsp, iw);
            break;
        case 0b0010011:
        	emu_i_type(rsp, iw);
        	break;
        case 0b0110111:
        	emu_u_type(rsp, iw);
        	break;
        case 0b1101111:
        	emu_j_type(rsp, iw);
        	break;
        case 0b1100011:
        	emu_b_type(rsp, iw);
        	break;
        default:
            unsupported("Unknown opcode: ", opcode);
    }
}

void rv_init(rv_state *state, uint32_t *target, 
             uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3) {
    state->pc = (uint64_t) target;
    state->regs[RV_A0] = a0;
    state->regs[RV_A1] = a1;
    state->regs[RV_A2] = a2;
    state->regs[RV_A3] = a3;

    state->regs[RV_ZERO] = 0;  // zero is always 0  (:
    state->regs[RV_RA] = RV_STOP;
    state->regs[RV_SP] = (uint64_t) &state->stack[STACK_SIZE];

    memset(&state->analysis, 0, sizeof(rv_analysis));
    cache_init(&state->i_cache);
}

uint64_t rv_emulate(rv_state *state) {
    while (state->pc != RV_STOP) {
        rv_one(state);
    }
    return state->regs[RV_A0];
}

static void print_pct(char *fmt, int numer, int denom) {
    double pct = 0.0;

    if (denom)
        pct = (double) numer / (double) denom * 100.0;
    printf(fmt, numer, pct);
}

void rv_print(rv_analysis *a) {
    int b_total = a->b_taken + a->b_not_taken;

    printf("=== Analysis\n");
    print_pct("Instructions Executed  = %d\n", a->i_count, a->i_count);
    print_pct("R-type + I-type        = %d (%.2f%%)\n", a->ir_count, a->i_count);
    print_pct("Loads                  = %d (%.2f%%)\n", a->ld_count, a->i_count);
    print_pct("Stores                 = %d (%.2f%%)\n", a->st_count, a->i_count);    
    print_pct("Jumps/JAL/JALR         = %d (%.2f%%)\n", a->j_count, a->i_count);
    print_pct("Conditional branches   = %d (%.2f%%)\n", b_total, a->i_count);
    print_pct("  Branches taken       = %d (%.2f%%)\n", a->b_taken, b_total);
    print_pct("  Branches not taken   = %d (%.2f%%)\n", a->b_not_taken, b_total);
}
