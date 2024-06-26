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

void emu_r_type(rv_state *state, uint32_t iw) {
	uint32_t rd = get_bits(iw, 7, 5);
	uint32_t rs1 = get_bits(iw, 15, 5);
	uint32_t rs2 = get_bits(iw, 20, 5);
	uint32_t funct3 = get_bits(iw, 12, 3);
	uint32_t funct7 = get_bits(iw, 25, 7);
	uint32_t opcode = get_bits(iw, 0, 7);

	switch (funct3) {
		case 0b000:
			switch (funct7) {
				case 0b0000000: 
					// ADD
					state->regs[rd] = state->regs[rs1] + state->regs[rs2];
					break;
				case 0b0100000:
					// SUB
					state->regs[rd] = state->regs[rs1] - state->regs[rs2];
					break;
				case 0b0000001: 
					// MUL
					state->regs[rd] = state->regs[rs1] * state->regs[rs2];
					break;
				default:
					unsupported("R-type", funct7);
					break;
			}
			break;
		case 0b100:
			// DIV
			state->regs[rd] = state->regs[rs1] / state->regs[rs2];
			break;
		case 0b111: 
			// AND 
			state->regs[rd] = state->regs[rs1] & state->regs[rs2];
			break;
		case 0b110: 
			// OR
			state->regs[rd] = state->regs[rs1] | state->regs[rs2];
			break;
		case 0b101:
			if (funct7 == 0b0000000 && opcode == 0b0110011) {
				// SRL
				state->regs[rd] = state->regs[rs1] >> state->regs[rs2];
			} 
			break;
		case 0b001:
			if (funct7 == 0b0000000) {
				if (opcode == 0b0110011) {
					// SLL
					state->regs[rd] = state->regs[rs1] << state->regs[rs2];
				}
			}
			break;
		default:
			unsupported("R-type", funct3);
			break;
	}

	state->analysis.i_count++;
	state->analysis.ir_count++;
	state->pc += 4; 
}

void emu_shifts(rv_state *state, uint32_t iw) {

	uint32_t rd = get_bits(iw, 7, 5);
	uint32_t rs1 = get_bits(iw, 15, 5);
	uint32_t rs2 = get_bits(iw, 20, 5);
	uint32_t funct3 = get_bits(iw, 12, 3);
	uint32_t funct7 = get_bits(iw, 25, 7);


	if (funct3 == 0b001) {
		// SLLW
		uint32_t shamt = state->regs[rs2] & 0x1F;
		uint32_t res = (uint32_t)state->regs[rs1] << shamt;
		state->regs[rd] = (int64_t)(int32_t) res;
	} else if (funct7 == 0b0000000 && funct3 == 0b101) {
		// SRLW
		uint32_t shamt = state->regs[rs2] & 0x1F;
		uint32_t res = (uint32_t)state->regs[rs1] >> shamt;
		state->regs[rd] = (int64_t)(int32_t) res;
	} else if (funct7 == 0b0100000  && funct3 == 0b101) {
		// SRAW
		uint32_t shamt = state->regs[rs2] & 0x1F;
		int32_t temp = (int32_t)state->regs[rs1];
		int32_t res = temp >> shamt;
		state->regs[rd] = (int64_t) res;
	}

	state->pc += 4;
}

void emu_i_type(rv_state *state, uint32_t iw) {
	uint32_t rd = get_bits(iw, 7, 5);
	uint32_t rs1 = get_bits(iw, 15, 5);
	int32_t imm12 = sign_extend(get_bits(iw, 20, 12), 11);
	uint32_t funct3 = get_bits(iw, 12, 3);

	uint64_t ta = state->regs[rs1] + imm12;

	uint32_t opcode = get_bits(iw, 0, 7);

	switch(opcode) {
		case 0b0010011:
			if (funct3 == 0b101) {
				// SRLI
				uint32_t shamt = imm12 & 0x3F;
				state->regs[rd] = state->regs[rs1] >> shamt;
			} else if (funct3 == 0b000) {
				if (rs1 == 0) {
					// LI
					state->regs[rd] = imm12;
					state->analysis.i_count++;
					state->analysis.ir_count++;		
				} else if (imm12 == 0) {
					// MV
					state->regs[rd] = state->regs[rs1];   
					state->analysis.i_count++;
					state->analysis.ir_count++;	
				} else {
					// ADDI
					state->regs[rd] = state->regs[rs1] + imm12;
					state->analysis.i_count++;
					state->analysis.ir_count++;	
				}
			}
		break;
		case 0b0000011:
			if (funct3 == 0b000) {	
				// LB
				state->regs[rd] = *((uint8_t*) ta);
			} else if (funct3 == 0b010) {
				//LW
				state->regs[rd] = *((uint32_t*) ta);	
			} else if (funct3 == 0b011) {
				//LD 
				state->regs[rd] = *((uint64_t*) ta);
			}
			state->analysis.i_count++;
			state->analysis.ld_count++;	
		break;
	}

	state->pc += 4;
}

void emu_u_type(rv_state *state, uint32_t iw) {
	uint32_t rd = get_bits(iw, 7, 5);

	int32_t imm20 = sign_extend(get_bits(iw, 12, 20), 19);

	state->regs[rd] = imm20;

	state->pc += 4;
}


void emu_j_type(rv_state *state, uint32_t iw) {
	// JAL
	uint32_t rd = get_bits(iw, 7, 5);
	int32_t imm20 = sign_extend(
		get_bits(iw, 31, 1) << 20 |
		get_bits(iw, 21, 10) << 1 |
		get_bits(iw, 20, 1) << 11 |
		get_bits(iw, 12, 8)  << 12,
		20
	);

	if (rd != 0) {
		state->regs[rd] = state->pc + 4;
	}

	state->analysis.i_count++;
	state->analysis.j_count++;
	state->pc += imm20;
}


void emu_b_type(rv_state *state, uint32_t iw) {
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

	bool branch = false; 

	switch(funct3) {
		// BNE
		case 0b001:  
			branch = state->regs[rs1] != state->regs[rs2];
			break;
		// BLT
		case 0b100: 
			branch = (int64_t) state->regs[rs1] < (int64_t) state->regs[rs2];
			break;
		// BEQ
		case 0b000:
			branch = state->regs[rs1] == state->regs[rs2];
			break;
		// BGE
		case 0b101:
			branch = state->regs[rs1] >= state->regs[rs2];
			break;
		default:
			break; 
}


	state->analysis.i_count++;

	if(branch) {
		state->analysis.b_taken++;
		state->pc += imm12; 
	} else {
		state->analysis.b_not_taken++;
		state->pc += 4;
	}
}


void emu_s_type(rv_state *state, uint32_t iw) {
	uint32_t rd = get_bits(iw, 7, 5);
	uint32_t rs1 = get_bits(iw, 15, 5);
	uint32_t rs2 = get_bits(iw, 20, 5);
	uint32_t funct3 = get_bits(iw, 12, 3);

	int64_t imm11 = sign_extend(
		get_bits(iw, 31, 1) << 11 |
		get_bits(iw, 7, 5) << 0 |
		get_bits(iw, 25, 6)  << 5,
		11
	);

	uint64_t ta = state->regs[rs1] + imm11;


	if (funct3 == 0b000){
		// SB
		*((uint8_t *) ta) = (uint8_t) state->regs[rs2];
		state->analysis.i_count++;
		state->analysis.st_count++;
	}  else if (funct3 == 0b010) {
		// SW
		*((uint32_t *) ta) = state->regs[rs2];
		state->analysis.i_count++;
		state->analysis.st_count++;
	} else if (funct3 == 0b011) {
		// SD 
		*((uint64_t *) ta) = state->regs[rs2];
		state->analysis.i_count++;
		state->analysis.st_count++;
	}

	state->pc += 4;
}


void emu_jalr(rv_state *state, uint32_t iw) {
	uint32_t rs1 = get_bits(iw, 15, 5);
	uint64_t val = state->regs[rs1];
	state->analysis.i_count++;
	state->analysis.j_count++;
	state->pc = val;
}

static void rv_one(rv_state *state) {
	uint32_t iw  = *((uint32_t*) state->pc);
	iw = cache_lookup(&state->i_cache, (uint64_t) state->pc);

	uint32_t opcode = get_bits(iw, 0, 7);


#if DEBUG
	printf("iw: %08x\n", iw);
#endif

	switch (opcode) {
		case FMT_R:
			// R-type instructions have two register operands
			emu_r_type(state, iw);
		break;
		case FMT_I_JALR:
			// JALR (aka RET) is a variant of I-type instructions
			emu_jalr(state, iw);
		break;
		case FMT_I_ARITH:	
			// I-type instructions
			emu_i_type(state, iw);
		break;
		case FMT_I_LOAD:
			emu_i_type(state, iw);
			break;
		case FMT_U:
			// U-type instructions 
			emu_u_type(state, iw);
			break;
		case FMT_UJ:
			// J-type instructions 
			emu_j_type(state, iw);
			break;
		case FMT_SB:
			// B-type instructions 
			emu_b_type(state, iw);
			break;
		case FMT_S:
			// S-type instructions
			emu_s_type(state, iw);
			break;
		case FMT_SHIFTS:
			// SHIFTS
			emu_shifts(state, iw);
			break;
		default:
			unsupported("Unknown opcode: ", opcode);
	}
}

void rv_init(rv_state *state, uint32_t *target, uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3) {
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
