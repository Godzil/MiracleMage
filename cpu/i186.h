/* i186 instruction implement */

static INLINE2 void i_pusha(void)
{
    /* Opcode 0x60 */

	unsigned tmp;
	unsigned tmp2;

	tmp = (WORD)ReadWord(&wregs[SP]);
	PushWordReg(AX);
	PushWordReg(CX);
	PushWordReg(DX);
	PushWordReg(BX);
	tmp2 = (WORD)ReadWord(&wregs[SP]);
	tmp2 -= 2;
	WriteWord(&wregs[SP], (WORD)tmp2);
	PutMemW(c_stack, (WORD)tmp2, (WORD)tmp);
	PushWordReg(BP);
	PushWordReg(SI);
	PushWordReg(DI);
}

static INLINE2 void i_popa(void)
{
	/* Opcode 0x61 */

	unsigned tmp;

	PopWordReg(DI);
	PopWordReg(SI);
	PopWordReg(BP);
	tmp = (WORD)ReadWord(&wregs[SP]);
	tmp += 2;
	WriteWord(&wregs[SP], (WORD)tmp);
	PopWordReg(BX);
	PopWordReg(DX);
	PopWordReg(CX);
	PopWordReg(AX);
}

static INLINE2 void i_bound(void)
{
	/* Opcode 0x62 */

	unsigned ModRM = (unsigned)GetMemInc(c_cs, ip);
} 

static INLINE2 void i_push_d16(void)
{
	/* Opcode 0x68 */
	unsigned tmp1;
	unsigned tmp2;

	tmp1 = GetMemInc(c_cs, ip);
	tmp1 |= GetMemInc(c_cs, ip) << 8;
	tmp2 = (WORD)ReadWord(&wregs[SP]);
	tmp2 -= 2;
	WriteWord(&wregs[SP], (WORD)tmp2);
	PutMemW(c_stack, (WORD)tmp2, (WORD)tmp1);
}

static INLINE2 void i_imul_d16(void)
{
	/* Opcode 0x69 */
	INT32 result;
	unsigned tmp1;
	unsigned tmp2;

	unsigned ModRM = GetMemInc(c_cs, ip);
	WORD *dest = GetModRMRegW(ModRM);
	tmp1 = *(GetModRMRMW(ModRM));
	tmp2 = (unsigned)GetMemInc(c_cs, ip);
	tmp2 |= (unsigned)GetMemInc(c_cs, ip) << 8;

	result = (INT16)(tmp1) * (INT16)(tmp2);

	WriteWord(dest, result);
	*dest = (INT16)result;

	CF = OF = (result >> 16) > 0;
}

static INLINE2 void i_push_d8(void)
{
	/* Opcode 0x6a */
	unsigned tmp1;
	unsigned tmp2;

	tmp1 = GetMemInc(c_cs, ip);
	tmp2 = (WORD)ReadWord(&wregs[SP]);
	tmp2 -= 2;
	WriteWord(&wregs[SP], (WORD)tmp2);
	PutMemW(c_stack, (WORD)tmp2, (WORD)tmp1);
}

static INLINE2 void i_imul_d8(void)
{
	/* Opcode 0x6b */
	INT32 result;
	unsigned tmp1;
	unsigned tmp2;

	unsigned ModRM = GetMemInc(c_cs, ip);
	WORD *dest = GetModRMRegW(ModRM);
	tmp1 = *(GetModRMRMW(ModRM));
	tmp2 = (unsigned)GetMemInc(c_cs, ip);

	result = (INT16)(tmp1) * (INT16)(tmp2);

	WriteWord(dest, result);
	*dest = (INT16)result;

	CF = OF = (result >> 16) > 0;
}

static INLINE2 void i_insb(void)
{
	/* Opcode 0x6c */
	;
}

static INLINE2 void i_insw(void)
{
	/* Opcode 0x6d */
	;
}

static INLINE2 void i_outsb(void)
{
	/* Opcode 0x6e */
	;
}

static INLINE2 void i_outsw(void)
{
	/* Opcode 0x6f */
	;
}

static INLINE2 void i_rotshft_bd8(void)
{
	/* Opcode 0xc0 */

	unsigned ModRM = GetMemInc(c_cs, ip);
	BYTE *dest = GetModRMRMB(ModRM);
	unsigned tmp = *dest;
	unsigned tmp2;
	unsigned count = GetMemInc(c_cs, ip);
	
	switch(ModRM & 0x38) {
	case 0x00: /* ROL r/m8, imm8 */
		for(; count > 0; count--) {
			CF = (tmp & 0x80) != 0;
			tmp = (tmp << 1) + CF;
		}
		*dest = (BYTE)tmp;
		break;
	case 0x08: /* ROR r/m8, imm8 */
		for(; count > 0; count--) {
			CF = (tmp & 0x01) != 0;
			tmp = (tmp >> 1) + (CF << 7);
		}
		*dest = (BYTE)tmp;
		break;
	case 0x10: /* RCL r/m8, imm8 */
		for(; count > 0; count--) {
			tmp2 = CF;
			CF = (tmp & 0x80) != 0;
			tmp = (tmp << 1) + tmp2;
		}
		*dest = (BYTE)tmp;
		break;
	case 0x18: /* RCR r/m8, imm8 */
		for(; count > 0; count--) {
			tmp2 = (tmp >> 1) + (CF << 7);
			CF = (tmp & 0x01) != 0;
			tmp = tmp2;
		}
		*dest = (BYTE)tmp;
		break;
	case 0x20: /* SHL r/m8, imm8 */
		if(count >= 9) {
			CF = 0;
			tmp = 0;
		} else {
			CF = ((tmp << (count - 1)) & 0x80) != 0;
			tmp <<= count;
		}

		AF = 1;
		SetZFB(tmp);
		SetSFB(tmp);
		SetPF(tmp);

		*dest = (BYTE)tmp;
		break;
	case 0x28: /* SHR r/m8, imm8 */
		if(count >= 9) {
			CF = 0;
			tmp = 0;
		} else {
			CF = ((tmp >> (count - 1)) & 0x1) != 0;
			tmp >>= count;
		}

		SetSFB(tmp);
		SetPF(tmp);
		SetZFB(tmp);
		AF = 1;
		*dest = (BYTE)tmp;
		break;
	case 0x38: /* SAR r/m8, imm8 */
		tmp2 = tmp & 0x80;
		CF = (((INT8)tmp >> (count - 1)) & 0x01) != 0;
		for(; count > 0; count--)
			tmp = (tmp >> 1) | tmp2;

		SetSFB(tmp);
		SetPF(tmp);
		SetZFB(tmp);
		AF = 1;
		*dest = (BYTE)tmp;
		break;

	default:
		break;
	}
}

static INLINE2 void i_rotshft_wd8(void)
{
	/* Opcode 0xc1 */
	unsigned ModRM = GetMemInc(c_cs, ip);
	WORD *dest = GetModRMRMW(ModRM);
	unsigned tmp = *dest;
	unsigned tmp2;
	unsigned count = GetMemInc(c_cs, ip);
	
	switch(ModRM & 0x38) {
	case 0x00: /* ROL r/m16, imm8 */
		for(; count > 0; count--) {
			CF = (tmp & 0x80) != 0;
			tmp = (tmp << 1) + CF;
		}
		*dest = (WORD)tmp;
		break;
	case 0x08: /* ROR r/m16, imm8 */
		for(; count > 0; count--) {
			CF = (tmp & 0x01) != 0;
			tmp = (tmp >> 1) + (CF << 7);
		}
		*dest = (WORD)tmp;
		break;
	case 0x10: /* RCL r/m16, imm8 */
		for(; count > 0; count--) {
			tmp2 = CF;
			CF = (tmp & 0x80) != 0;
			tmp = (tmp << 1) + tmp2;
		}
		*dest = (WORD)tmp;
		break;
	case 0x18: /* RCR r/m16, imm8 */
		for(; count > 0; count--) {
			tmp2 = (tmp >> 1) + (CF << 7);
			CF = (tmp & 0x01) != 0;
			tmp = tmp2;
		}
		*dest = (WORD)tmp;
		break;
	case 0x20: /* SHL r/m16, imm8 */
		if(count >= 9) {
			CF = 0;
			tmp = 0;
		} else {
			CF = ((tmp << (count - 1)) & 0x80) != 0;
			tmp <<= count;
		}

		AF = 1;
		SetZFB(tmp);
		SetSFB(tmp);
		SetPF(tmp);

		*dest = (WORD)tmp;
		break;
	case 0x28: /* SHR r/m16, imm8 */
		if(count >= 9) {
			CF = 0;
			tmp = 0;
		} else {
			CF = ((tmp >> (count - 1)) & 0x1) != 0;
			tmp >>= count;
		}

		SetSFB(tmp);
		SetPF(tmp);
		SetZFB(tmp);
		AF = 1;
		*dest = (WORD)tmp;
		break;
	case 0x38: /* SAR r/m16, imm8 */
		tmp2 = tmp & 0x80;
		CF = (((INT16)tmp >> (count - 1)) & 0x01) != 0;
		for(; count > 0; count--)
			tmp = (tmp >> 1) | tmp2;

		SetSFB(tmp);
		SetPF(tmp);
		SetZFB(tmp);
		AF = 1;
		*dest = (WORD)tmp;
		break;
	default:
		break;
	}
}

static INLINE2 void i_enter(void)
{
	/* Opcode 0xc8 */
	unsigned size;
	unsigned level;
	unsigned tmp;
	unsigned tmp2;
	unsigned i;
	

	size = GetMemInc(c_cs, ip);
	size |= GetMemInc(c_cs, ip) << 8;
	level = GetMemInc(c_cs, ip);
/*
	PushWordReg(BP);
	tmp = ReadWord(&wregs[SP]);
	WriteWord(&wregs[BP], (WORD)tmp);
	WriteWord(&wregs[SP], (WORD)(tmp - size));
	for(i = 1; i < level; i++) {
		tmp = GetMemW(c_stack, ReadWord(&wregs[BP]) - i * 2);
		tmp2 = (WORD)(ReadWord(&wregs[SP]) - 2);
		WriteWord(&wregs[SP], tmp2);
		PutMemW(c_stack, tmp2, tmp);
	}
	if(level) {
		PushWordReg(BP);
	}
	*/
	level %= 32;
	PushWordReg(BP);
	tmp = (WORD)ReadWord(&wregs[SP]);

	if(level > 0) {
		for(i = 1; i < level; i++) {
			tmp2 = (WORD)ReadWord(&wregs[BP]);
			tmp2 -= 2;
			WriteWord(&wregs[BP], (WORD)tmp2);
			PushWordReg(BP);
		}
		tmp2 = (WORD)ReadWord(&wregs[SP]);
		tmp2 -= 2;
		WriteWord(&wregs[SP], (WORD)tmp2);
		PutMemW(c_stack, tmp2, (WORD)tmp);
	}

	WriteWord(&wregs[BP], (WORD)tmp);
	WriteWord(&wregs[SP], (WORD)(tmp - size));
}

static INLINE2 void i_leave(void)
{
	/* Opcode 0xc9 */
	unsigned tmp;

	tmp = ReadWord(&wregs[BP]);
	WriteWord(&wregs[SP], (WORD)tmp);
	PopWordReg(BP);
}

static INLINE void i_das(void)
{
	/* Opcode 0x2f */
	unsigned tmp;

	if(((*bregs[AL] & 0x0f) > 9) || AF) {
		tmp = *bregs[AL] - 6;
		*bregs[AL] = tmp;
		CF = CF | ((tmp & 0x100) ? 1 : 0);
		AF = 0x10;
	} else {
		AF = 0;
	}
	if((*bregs[AL] > 0x9f) || CF) {
		*bregs[AL] = *bregs[AL] - 0x60;
		CF = 1;
	}
}

static INLINE void i_aas(void)
{
	/* Opcode 0x3f */

	if(((*bregs[AL] & 0x0f) > 9) || AF) {
		*bregs[AL] = (BYTE)(*bregs[AL] - 6);
		*bregs[AH] = (BYTE)(*bregs[AH] - 1);
		AF = 0x10;
		CF = 1;
	} else {
		AF = 0;
		CF = 0;
	}
	*bregs[AL] = *bregs[AL] & 0x0f;
}
