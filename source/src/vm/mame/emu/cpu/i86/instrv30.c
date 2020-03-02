static void PREFIXV30(_0fpre)(i8086_state *cpustate)	/* Opcode 0x0f */
{
	static const unsigned bytes[] = {
		1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768
	};

	unsigned Opcode = FETCH;
	unsigned ModRM;
	unsigned tmp;
	unsigned tmp2;

	switch (Opcode)
	{
	case 0x10:							/* 0F 10 47 30 - TEST1 [bx+30h],cl */
		ModRM = FETCH;
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.b[Mod_RM.RM.b[ModRM]];
			ICOUNT -= 3;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadByte(cpustate->ea);
			ICOUNT = old - 12;		/* my source says 14 cycles everytime and not
                                         * ModRM-dependent like GetEA[] does..hmmm */
		}
		tmp2 = cpustate->regs.b[CL] & 0x7;
		cpustate->ZeroVal = tmp & bytes[tmp2] ? 1 : 0;
/*      SetZF(tmp & (1<<tmp2)); */
		break;

	case 0x11:							/* 0F 11 47 30 - TEST1 [bx+30h],cl */
		ModRM = FETCH;
		/* tmp = GetRMWord(ModRM); */
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.w[Mod_RM.RM.w[ModRM]];
			ICOUNT -= 3;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadWord(cpustate->ea);
			ICOUNT = old - 12;		/* my source says 14 cycles everytime and not
                                         * ModRM-dependent like GetEA[] does..hmmm */
		}
		tmp2 = cpustate->regs.b[CL] & 0xF;
		cpustate->ZeroVal = tmp & bytes[tmp2] ? 1 : 0;
/*      SetZF(tmp & (1<<tmp2)); */
		break;

	case 0x12:							/* 0F 12 [mod:000:r/m] - CLR1 reg/m8,cl */
		ModRM = FETCH;
		/* need the long if due to correct cycles OB[19.07.99] */
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.b[Mod_RM.RM.b[ModRM]];
			ICOUNT -= 5;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadByte(cpustate->ea);
			ICOUNT = old - 14;		/* my source says 14 cycles everytime and not
                                         * ModRM-dependent like GetEA[] does..hmmm */
		}
		tmp2 = cpustate->regs.b[CL] & 0x7;		/* hey its a Byte so &07 NOT &0f */
		tmp &= ~(bytes[tmp2]);
		PutbackRMByte(ModRM, tmp);
		break;

	case 0x13:							/* 0F 13 [mod:000:r/m] - CLR1 reg/m16,cl */
		ModRM = FETCH;
		/* tmp = GetRMWord(ModRM); */
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.w[Mod_RM.RM.w[ModRM]];
			ICOUNT -= 5;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadWord(cpustate->ea);
			ICOUNT = old - 14;		/* my source says 14 cycles everytime and not
                                         * ModRM-dependent like GetEA[] does..hmmm */
		}
		tmp2 = cpustate->regs.b[CL] & 0xF;		/* this time its a word */
		tmp &= ~(bytes[tmp2]);
		PutbackRMWord(ModRM, tmp);
		break;

	case 0x14:							/* 0F 14 47 30 - SET1 [bx+30h],cl */
		ModRM = FETCH;
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.b[Mod_RM.RM.b[ModRM]];
			ICOUNT -= 4;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadByte(cpustate->ea);
			ICOUNT = old - 13;
		}
		tmp2 = cpustate->regs.b[CL] & 0x7;
		tmp |= (bytes[tmp2]);
		PutbackRMByte(ModRM, tmp);
		break;

	case 0x15:							/* 0F 15 C6 - SET1 si,cl */
		ModRM = FETCH;
		/* tmp = GetRMWord(ModRM); */
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.w[Mod_RM.RM.w[ModRM]];
			ICOUNT -= 4;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadWord(cpustate->ea);
			ICOUNT = old - 13;
		}
		tmp2 = cpustate->regs.b[CL] & 0xF;
		tmp |= (bytes[tmp2]);
		PutbackRMWord(ModRM, tmp);
		break;

	case 0x16:							/* 0F 16 C6 - NOT1 si,cl */
		ModRM = FETCH;
		/* need the long if due to correct cycles OB[19.07.99] */
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.b[Mod_RM.RM.b[ModRM]];
			ICOUNT -= 4;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadByte(cpustate->ea);
			ICOUNT = old - 18;		/* my source says 18 cycles everytime and not
                                         * ModRM-dependent like GetEA[] does..hmmm */
		}
		tmp2 = cpustate->regs.b[CL] & 0x7;		/* hey its a Byte so &07 NOT &0f */
		if (tmp & bytes[tmp2])
			tmp &= ~(bytes[tmp2]);
		else
			tmp |= (bytes[tmp2]);
		PutbackRMByte(ModRM, tmp);
		break;

	case 0x17:							/* 0F 17 C6 - NOT1 si,cl */
		ModRM = FETCH;
		/* tmp = GetRMWord(ModRM); */
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.w[Mod_RM.RM.w[ModRM]];
			ICOUNT -= 4;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadWord(cpustate->ea);
			ICOUNT = old - 18;		/* my source says 14 cycles everytime and not
                                         * ModRM-dependent like GetEA[] does..hmmm */
		}
		tmp2 = cpustate->regs.b[CL] & 0xF;		/* this time its a word */
		if (tmp & bytes[tmp2])
			tmp &= ~(bytes[tmp2]);
		else
			tmp |= (bytes[tmp2]);
		PutbackRMWord(ModRM, tmp);
		break;

	case 0x18:							/* 0F 18 XX - TEST1 [bx+30h],07 */
		ModRM = FETCH;
		/* tmp = GetRMByte(ModRM); */
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.b[Mod_RM.RM.b[ModRM]];
			ICOUNT -= 4;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadByte(cpustate->ea);
			ICOUNT = old - 13;		/* my source says 15 cycles everytime and not
                                         * ModRM-dependent like GetEA[] does..hmmm */
		}
		tmp2 = FETCH;
		tmp2 &= 0xF;
		cpustate->ZeroVal = tmp & (bytes[tmp2]) ? 1 : 0;
/*      SetZF(tmp & (1<<tmp2)); */
		break;

	case 0x19:							/* 0F 19 XX - TEST1 [bx+30h],07 */
		ModRM = FETCH;
		/* tmp = GetRMWord(ModRM); */
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.w[Mod_RM.RM.w[ModRM]];
			ICOUNT -= 4;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadWord(cpustate->ea);
			ICOUNT = old - 13;		/* my source says 14 cycles everytime and not
                                         * ModRM-dependent like GetEA[] does..hmmm */
		}
		tmp2 = FETCH;
		tmp2 &= 0xf;
		cpustate->ZeroVal = tmp & (bytes[tmp2]) ? 1 : 0;
/*      SetZF(tmp & (1<<tmp2)); */
		break;

	case 0x1a:							/* 0F 1A 06 - CLR1 si,cl */
		ModRM = FETCH;
		/* tmp = GetRMByte(ModRM); */
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.b[Mod_RM.RM.b[ModRM]];
			ICOUNT -= 6;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadByte(cpustate->ea);
			ICOUNT = old - 15;		/* my source says 15 cycles everytime and not
                                         * ModRM-dependent like GetEA[] does..hmmm */
		}
		tmp2 = FETCH;
		tmp2 &= 0x7;
		tmp &= ~(bytes[tmp2]);
		PutbackRMByte(ModRM, tmp);
		break;

	case 0x1B:							/* 0F 1B 06 - CLR1 si,cl */
		ModRM = FETCH;
		/* tmp = GetRMWord(ModRM); */
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.w[Mod_RM.RM.w[ModRM]];
			ICOUNT -= 6;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadWord(cpustate->ea);
			ICOUNT = old - 15;		/* my source says 15 cycles everytime and not
                                         * ModRM-dependent like GetEA[] does..hmmm */
		}
		tmp2 = FETCH;
		tmp2 &= 0xF;
		tmp &= ~(bytes[tmp2]);
		PutbackRMWord(ModRM, tmp);
		break;

	case 0x1C:							/* 0F 1C 47 30 - SET1 [bx+30h],cl */
		ModRM = FETCH;
		/* tmp = GetRMByte(ModRM); */
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.b[Mod_RM.RM.b[ModRM]];
			ICOUNT -= 5;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadByte(cpustate->ea);
			ICOUNT = old - 14;		/* my source says 15 cycles everytime and not
                                         * ModRM-dependent like GetEA[] does..hmmm */
		}
		tmp2 = FETCH;
		tmp2 &= 0x7;
		tmp |= (bytes[tmp2]);
		PutbackRMByte(ModRM, tmp);
		break;

	case 0x1D:							/* 0F 1D C6 - SET1 si,cl */
		/* logerror("PC=%06x : Set1 ",activecpu_get_pc()-2); */
		ModRM = FETCH;
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.w[Mod_RM.RM.w[ModRM]];
			ICOUNT -= 5;
			/* logerror("reg=%04x ->",tmp); */
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);			/* calculate EA */
			tmp = ReadWord(cpustate->ea);			/* read from EA */
			ICOUNT = old - 14;
			/* logerror("[%04x]=%04x ->",EA,tmp); */
		}
		tmp2 = FETCH;
		tmp2 &= 0xF;
		tmp |= (bytes[tmp2]);
		/* logerror("%04x",tmp); */
		PutbackRMWord(ModRM, tmp);
		break;

	case 0x1e:							/* 0F 1e C6 - NOT1 si,07 */
		ModRM = FETCH;
		/* tmp = GetRMByte(ModRM); */
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.b[Mod_RM.RM.b[ModRM]];
			ICOUNT -= 5;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadByte(cpustate->ea);
			ICOUNT = old - 19;
		}
		tmp2 = FETCH;
		tmp2 &= 0x7;
		if (tmp & bytes[tmp2])
			tmp &= ~(bytes[tmp2]);
		else
			tmp |= (bytes[tmp2]);
		PutbackRMByte(ModRM, tmp);
		break;

	case 0x1f:							/* 0F 1f C6 - NOT1 si,07 */
		ModRM = FETCH;
		//tmp = GetRMWord(ModRM);
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.w[Mod_RM.RM.w[ModRM]];
			ICOUNT -= 5;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadWord(cpustate->ea);
			ICOUNT = old - 19;		/* my source says 15 cycles everytime and not
                                         * ModRM-dependent like GetEA[] does..hmmm */
		}
		tmp2 = FETCH;
		tmp2 &= 0xF;
		if (tmp & bytes[tmp2])
			tmp &= ~(bytes[tmp2]);
		else
			tmp |= (bytes[tmp2]);
		PutbackRMWord(ModRM, tmp);
		break;

	case 0x20:							/* 0F 20 59 - add4s */
		{
			/* length in words ! */
			int count = (cpustate->regs.b[CL] + 1) / 2;
			int i;
			unsigned di = cpustate->regs.w[DI];
			unsigned si = cpustate->regs.w[SI];

			cpustate->ZeroVal = 1;
			cpustate->CarryVal = 0;				/* NOT ADC */
			for (i = 0; i < count; i++)
			{
				int v1, v2;
				int result;

				tmp = GetMemB(DS, si);
				tmp2 = GetMemB(ES, di);

				v1 = (tmp >> 4) * 10 + (tmp & 0xf);
				v2 = (tmp2 >> 4) * 10 + (tmp2 & 0xf);
				result = v1 + v2 + cpustate->CarryVal;
				cpustate->CarryVal = result > 99 ? 1 : 0;
				result = result % 100;
				v1 = ((result / 10) << 4) | (result % 10);
				PutMemB(ES, di, v1);
				if (v1)
					cpustate->ZeroVal = 0;
				si++;
				di++;
			}
			cpustate->OverVal = cpustate->CarryVal;
			ICOUNT -= 7 + 19 * count;	/* 7+19n, n #operand words */
		}
		break;

	case 0x22:							/* 0F 22 59 - sub4s */
		{
			int count = (cpustate->regs.b[CL] + 1) / 2;
			int i;
			unsigned di = cpustate->regs.w[DI];
			unsigned si = cpustate->regs.w[SI];

			cpustate->ZeroVal = 1;
			cpustate->CarryVal = 0;				/* NOT ADC */
			for (i = 0; i < count; i++)
			{
				int v1, v2;
				int result;

				tmp = GetMemB(ES, di);
				tmp2 = GetMemB(DS, si);

				v1 = (tmp >> 4) * 10 + (tmp & 0xf);
				v2 = (tmp2 >> 4) * 10 + (tmp2 & 0xf);
				if (v1 < (v2 + cpustate->CarryVal))
				{
					v1 += 100;
					result = v1 - (v2 + cpustate->CarryVal);
					cpustate->CarryVal = 1;
				}
				else
				{
					result = v1 - (v2 + cpustate->CarryVal);
					cpustate->CarryVal = 0;
				}
				v1 = ((result / 10) << 4) | (result % 10);
				PutMemB(ES, di, v1);
				if (v1)
					cpustate->ZeroVal = 0;
				si++;
				di++;
			}
			cpustate->OverVal = cpustate->CarryVal;
			ICOUNT -= 7 + 19 * count;
		}
		break;

	case 0x25:
		/*
         * ----------O-MOVSPA---------------------------------
         * OPCODE MOVSPA     -  Move Stack Pointer After Bank Switched
         *
         * CPU:  NEC V25,V35,V25 Plus,V35 Plus,V25 Software Guard
         * Type of Instruction: System
         *
         * Instruction:  MOVSPA
         *
         * Description:  This instruction transfer   both SS and SP  of the old register
         * bank to new register bank after the bank has been switched by
         * interrupt or BRKCS instruction.
         *
         * Flags Affected:   None
         *
         * CPU mode: RM
         *
         * +++++++++++++++++++++++
         * Physical Form:   MOVSPA
         * COP (Code of Operation)   : 0Fh 25h
         *
         * Clocks:   16
         */
		logerror("PC=%06x : MOVSPA\n", activecpu_get_pc() - 2);
		ICOUNT -= 16;
		break;
	case 0x26:							/* 0F 22 59 - cmp4s */
		{
			int count = (cpustate->regs.b[CL] + 1) / 2;
			int i;
			unsigned di = cpustate->regs.w[DI];
			unsigned si = cpustate->regs.w[SI];

			cpustate->ZeroVal = 1;
			cpustate->CarryVal = 0;				/* NOT ADC */
			for (i = 0; i < count; i++)
			{
				int v1, v2;
				int result;

				tmp = GetMemB(ES, di);
				tmp2 = GetMemB(DS, si);

				v1 = (tmp >> 4) * 10 + (tmp & 0xf);
				v2 = (tmp2 >> 4) * 10 + (tmp2 & 0xf);
				if (v1 < (v2 + cpustate->CarryVal))
				{
					v1 += 100;
					result = v1 - (v2 + cpustate->CarryVal);
					cpustate->CarryVal = 1;
				}
				else
				{
					result = v1 - (v2 + cpustate->CarryVal);
					cpustate->CarryVal = 0;
				}
				v1 = ((result / 10) << 4) | (result % 10);
/*              PutMemB(ES, di,v1); */	/* no store, only compare */
				if (v1)
					cpustate->ZeroVal = 0;
				si++;
				di++;
			}
			cpustate->OverVal = cpustate->CarryVal;
			ICOUNT -= 7 + 19 * (cpustate->regs.b[CL] + 1);	// 7+19n, n #operand bytes
		}
		break;
	case 0x28:							/* 0F 28 C7 - ROL4 bh */
		ModRM = FETCH;
		/* tmp = GetRMByte(ModRM); */
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.b[Mod_RM.RM.b[ModRM]];
			ICOUNT -= 25;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadByte(cpustate->ea);
			ICOUNT = old - 28;
		}
		tmp <<= 4;
		tmp |= cpustate->regs.b[AL] & 0xF;
		cpustate->regs.b[AL] = (cpustate->regs.b[AL] & 0xF0) | ((tmp >> 8) & 0xF);
		tmp &= 0xff;
		PutbackRMByte(ModRM, tmp);
		break;

        /* Is this a REAL instruction?? */
	case 0x29:							/* 0F 29 C7 - ROL4 bx */

		ModRM = FETCH;
		/*
         * if (ModRM >= 0xc0)
         * {
         *     tmp=cpustate->regs.w[Mod_RM.RM.w[ModRM]];
         *     ICOUNT-=29;
         * }
         * else
         * {
         *     int old=ICOUNT;
         *     (*GetEA[ModRM])();
         *     tmp=ReadWord(cpustate->ea);
         *     ICOUNT=old-33;
         * }
         * tmp <<= 4;
         * tmp |= cpustate->regs.b[AL] & 0xF;
         * cpustate->regs.b[AL] = (cpustate->regs.b[AL] & 0xF0) | ((tmp>>8)&0xF);
         * tmp &= 0xffff;
         * PutbackRMWord(ModRM,tmp);
         */
		logerror("PC=%06x : ROL4 %02x\n", activecpu_get_pc() - 3, ModRM);
		break;

	case 0x2A:							/* 0F 2a c2 - ROR4 bh */
		ModRM = FETCH;
		/* tmp = GetRMByte(ModRM); */
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.b[Mod_RM.RM.b[ModRM]];
			ICOUNT -= 29;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadByte(cpustate->ea);
			ICOUNT = old - 33;
		}
		tmp2 = (cpustate->regs.b[AL] & 0xF) << 4;
		cpustate->regs.b[AL] = (cpustate->regs.b[AL] & 0xF0) | (tmp & 0xF);
		tmp = tmp2 | (tmp >> 4);
		PutbackRMByte(ModRM, tmp);
		break;

	case 0x2B:							// 0F 2b c2 - ROR4 bx
		ModRM = FETCH;
		/*
         * /* tmp = GetRMWord(ModRM);
         * if (ModRM >= 0xc0)
         * {
         *     tmp=cpustate->regs.w[Mod_RM.RM.w[ModRM]];
         *     ICOUNT-=29;
         * }
         * else {
         *     int old=ICOUNT;
         *     (*GetEA[ModRM])();
         *     tmp=ReadWord(cpustate->ea);
         *     ICOUNT=old-33;
         * }
         * tmp2 = (cpustate->regs.b[AL] & 0xF)<<4;
         * cpustate->regs.b[AL] = (cpustate->regs.b[AL] & 0xF0) | (tmp&0xF);
         * tmp = tmp2 | (tmp>>4);
         * PutbackRMWord(ModRM,tmp);
         */
		logerror("PC=%06x : ROR4 %02x\n", activecpu_get_pc() - 3, ModRM);
		break;

	case 0x2D:							/* 0Fh 2Dh <1111 1RRR> */
		/* OPCODE BRKCS  -   Break with Contex Switch
         * CPU:  NEC V25,V35,V25 Plus,V35 Plus,V25 Software Guard
         * Description:
         *
         * Perform a High-Speed Software Interrupt with contex-switch to
         * register bank indicated by the lower 3-bits of 'bank'.
         *
         * Info:    NEC V25/V35/V25 Plus/V35 Plus Bank System
         *
         * This Chips have   8 32bytes register banks, which placed in
         * Internal chip RAM by addresses:
         * xxE00h..xxE1Fh Bank 0
         * xxE20h..xxE3Fh Bank 1
         * .........
         * xxEC0h..xxEDFh Bank 6
         * xxEE0h..xxEFFh Bank 7
         * xxF00h..xxFFFh Special Functions Register
         * Where xx is Value of IDB register.
         * IBD is Byte Register contained Internal data area base
         * IBD addresses is FFFFFh and xxFFFh where xx is data in IBD.
         *
         * Format of Bank:
         * +0   Reserved
         * +2   Vector PC
         * +4   Save   PSW
         * +6   Save   PC
         * +8   DS0     ;DS
         * +A   SS      ;SS
         * +C   PS      ;CS
         * +E   DS1     ;ES
         * +10  IY      ;DI
         * +11  IX      ;SI
         * +14  BP      ;BP
         * +16  SP      ;SP
         * +18  BW      ;BX
         * +1A  DW      ;DX
         * +1C  CW      ;CX
         * +1E  AW      ;AX
         *
         * Format of V25 etc. PSW (FLAGS):
         * Bit  Description
         * 15   1
         * 14   RB2 \
         * 13   RB1  >  Current Bank Number
         * 12   RB0 /
         * 11   V   ;OF
         * 10   IYR ;DF
         * 9    IE  ;IF
         * 8    BRK ;TF
         * 7    S   ;SF
         * 6    Z   ;ZF
         * 5    F1  General Purpose user flag #1 (accessed by Flag Special Function Register)
         * 4    AC  ;AF
         * 3    F0  General purpose user flag #0 (accessed by Flag Special Function Register)
         * 2    P   ;PF
         * 1    BRKI    I/O Trap Enable Flag
         * 0    CY  ;CF
         *
         * Flags Affected:   None
         */
		ModRM = FETCH;
		logerror("PC=%06x : BRKCS %02x\n", activecpu_get_pc() - 3, ModRM);
		ICOUNT -= 15;				/* checked ! */
		break;

	case 0x31:							/* 0F 31 [mod:reg:r/m] - INS reg8,reg8 or INS reg8,imm4 */
		ModRM = FETCH;
		logerror("PC=%06x : INS ", activecpu_get_pc() - 2);
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.b[Mod_RM.RM.b[ModRM]];
			logerror("ModRM=%04x \n", ModRM);
			ICOUNT -= 29;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadByte(cpustate->ea);
			logerror("ModRM=%04x  Byte=%04x\n", EA, tmp);
			ICOUNT = old - 33;
		}

		/* more to come
         * bfl=tmp2 & 0xf;      /* bit field length
         * bfs=tmp & 0xf;       /* bit field start (bit offset in DS:SI)
         * cpustate->regs.b[AH] =0;     /* AH =0
         */

		/* 2do: the rest is silence....yet
         * ----------O-INS------------------------------------
         * OPCODE INS  -  Insert Bit String
         *
         * CPU: NEC/Sony  all V-series
         * Type of Instruction: User
         *
         * Instruction:  INS  start,len
         *
         * Description:
         *
         * BitField [        BASE =  ES:DI
         * START BIT OFFSET =  start
         * LENGTH =  len
         * ]   <-    AX [ bits= (len-1)..0]
         *
         * Note:    di and start automatically UPDATE
         * Note:    Alternative Name of this instruction is NECINS
         *
         * Flags Affected: None
         *
         * CPU mode: RM
         *
         * +++++++++++++++++++++++
         * Physical Form         : INS  reg8,reg8
         * COP (Code of Operation)   : 0FH 31H  PostByte
         */

		/* ICOUNT-=31; */			/* 31 -117 clocks ....*/
		break;

    case 0x33:                          /* 0F 33 [mod:reg:r/m] - EXT reg8,reg8 or EXT reg8,imm4 */
		ModRM = FETCH;
		logerror("PC=%06x : EXT ", activecpu_get_pc() - 2);
		if (ModRM >= 0xc0)
		{
			tmp = cpustate->regs.b[Mod_RM.RM.b[ModRM]];
			logerror("ModRM=%04x \n", ModRM);
			ICOUNT -= 29;
		}
		else
		{
			int old = ICOUNT;

			(void)(*GetEA[ModRM])(cpustate);
			tmp = ReadByte(cpustate->ea);
			logerror("ModRM=%04x  Byte=%04x\n", EA, tmp);
			ICOUNT = old - 33;
		}
		/* 2do: the rest is silence....yet
        /*
         * bfl=tmp2 & 0xf;      /* bit field length
         * bfs=tmp & 0xf;       /* bit field start (bit offset in DS:SI)
         * cpustate->regs.b[AH] =0;     /* AH =0
         */

		/*
         *
         * ----------O-EXT------------------------------------
         * OPCODE EXT  -  Extract Bit Field
         *
         * CPU: NEC/Sony all  V-series
         * Type of Instruction: User
         *
         * Instruction:  EXT  start,len
         *
         * Description:
         *
         * AX <- BitField [
         *     BASE =  DS:SI
         *     START BIT OFFSET =  start
         *     LENGTH =  len
         * ];
         *
         * Note:    si and start automatically UPDATE
         *
         * Flags Affected: None
         *
         * CPU mode: RM
         *
         * +++++++++++++++++++++++
         * Physical Form         : EXT  reg8,reg8
         * COP (Code of Operation)   : 0FH 33H  PostByte
         *
         * Clocks:      EXT  reg8,reg8
         * NEC V20: 26-55
         */

		/* NEC_ICount-=26; */			/* 26 -55 clocks ....*/
		break;

    case 0x91:
		/*
         * ----------O-RETRBI---------------------------------
         * OPCODE RETRBI     -  Return from Register Bank Context
         * Switch  Interrupt.
         *
         * CPU:  NEC V25,V35,V25 Plus,V35 Plus,V25 Software Guard
         * Type of Instruction: System
         *
         * Instruction:  RETRBI
         *
         * Description:
         *
         * PC  <- Save PC;
         * PSW <- Save PSW;
         *
         * Flags Affected:   All
         *
         * CPU mode: RM
         *
         * +++++++++++++++++++++++
         * Physical Form:   RETRBI
         * COP (Code of Operation)   : 0Fh 91h
         *
         * Clocks:   12
         */
		logerror("PC=%06x : RETRBI\n", activecpu_get_pc() - 2);
		ICOUNT -= 12;
		break;

	case 0x94:
		/*
         * ----------O-TSKSW----------------------------------
         * OPCODE TSKSW  -    Task Switch
         *
         * CPU:  NEC V25,V35,V25 Plus,V35 Plus,V25 Software Guard
         * Type of Instruction: System
         *
         * Instruction:  TSKSW   reg16
         *
         * Description:  Perform a High-Speed task switch to the register bank indicated
         * by lower 3 bits of reg16. The PC and PSW are saved in the old
         * banks. PC and PSW save Registers and the new PC and PSW values
         * are retrived from the new register bank's save area.
         *
         * Note:         See BRKCS instruction for more Info about banks.
         *
         * Flags Affected:   All
         *
         * CPU mode: RM
         *
         * +++++++++++++++++++++++
         * Physical Form:   TSCSW reg16
         * COP (Code of Operation)   : 0Fh 94h <1111 1RRR>
         *
         * Clocks:   11
         */
		ModRM = FETCH;

		logerror("PC=%06x : TSCSW %02x\n", activecpu_get_pc() - 3, ModRM);
		ICOUNT -= 11;
		break;

    case 0x95:
		/*
         * ----------O-MOVSPB---------------------------------
         * OPCODE MOVSPB     -  Move Stack Pointer Before Bamk Switching
         *
         * CPU:  NEC V25,V35,V25 Plus,V35 Plus,V25 Software Guard
         * Type of Instruction: System
         *
         * Instruction:  MOVSPB  Number_of_bank
         *
         * Description:  The MOVSPB instruction transfers the current SP and SS before
         * the bank switching to new register bank.
         *
         * Note:          New Register Bank Number indicated by lower 3bit of Number_of_
         * _bank.
         *
         * Note:          See BRKCS instruction for more info about banks.
         *
         * Flags Affected:   None
         *
         * CPU mode: RM
         *
         * +++++++++++++++++++++++
         * Physical Form:   MOVSPB    reg16
         * COP (Code of Operation)   : 0Fh 95h <1111 1RRR>
         *
         * Clocks:   11
         */
		ModRM = FETCH;
		logerror("PC=%06x : MOVSPB %02x\n", activecpu_get_pc() - 3, ModRM);
		ICOUNT -= 11;
		break;

    case 0xbe:
		/*
         * ----------O-STOP-----------------------------------
         * OPCODE STOP    -  Stop CPU
         *
         * CPU:  NEC V25,V35,V25 Plus,V35 Plus,V25 Software Guard
         * Type of Instruction: System
         *
         * Instruction:  STOP
         *
         * Description:
         * PowerDown instruction, Stop Oscillator,
         * Halt CPU.
         *
         * Flags Affected:   None
         *
         * CPU mode: RM
         *
         * +++++++++++++++++++++++
         * Physical Form:   STOP
         * COP (Code of Operation)   : 0Fh BEh
         *
         * Clocks:   N/A
         */
		logerror("PC=%06x : STOP\n", activecpu_get_pc() - 2);
		ICOUNT -= 2;				/* of course this is crap */
		break;

    case 0xe0:
		/*
         * ----------O-BRKXA----------------------------------
         * OPCODE BRKXA   -  Break to Expansion Address
         *
         * CPU:  NEC V33/V53  only
         * Type of Instruction: System
         *
         * Instruction:  BRKXA int_vector
         *
         * Description:
         * [sp-1,sp-2] <- PSW       ; PSW EQU FLAGS
         * [sp-3,sp-4] <- PS        ; PS  EQU CS
         * [sp-5,sp-6] <- PC        ; PC  EQU IP
         * SP    <-  SP -6
         * IE    <-  0
         * BRK <-  0
         * MD    <-  0
         * PC    <- [int_vector*4 +0,+1]
         * PS    <- [int_vector*4 +2,+3]
         * Enter Expansion Address Mode.
         *
         * Note:    In NEC V53 Memory Space dividing into 1024 16K pages.
         * The programming model is Same as in Normal mode.
         *
         * Mechanism is:
         * 20 bit Logical Address:   19..14 Page Num  13..0 Offset
         *
         * page Num convertin by internal table to 23..14 Page Base
         * tHE pHYIXCAL ADDRESS is both Base and Offset.
         *
         * Address Expansion Registers:
         * logical Address A19..A14 I/O Address
         * 0                FF00h
         * 1                FF02h
         * ...              ...
         * 63               FF7Eh
         *
         * Register XAM aliased with port # FF80h indicated current mode
         * of operation.
         * Format of XAM register (READ ONLY):
         * 15..1    reserved
         * 0    XA Flag, if=1 then in XA mode.
         *
         * Format   of  V53 PSW:
         * 15..12   1
         * 11   V
         * 10   IYR
         * 9    IE
         * 8    BRK
         * 7    S
         * 6    Z
         * 5    0
         * 4    AC
         * 3    0
         * 2    P
         * 1    1
         * 0    CY
         *
         * Flags Affected:   None
         *
         * CPU mode: RM
         *
         * +++++++++++++++++++++++
         * Physical Form:   BRKXA  imm8
         * COP (Code of Operation)   : 0Fh E0h imm8
         */

		ModRM = FETCH;
		logerror("PC=%06x : BRKXA %02x\n", activecpu_get_pc() - 3, ModRM);
		ICOUNT -= 12;
		break;

    case 0xf0:
		/*
         * ----------O-RETXA----------------------------------
         * OPCODE RETXA   -  Return from  Expansion Address
         *
         * CPU:  NEC V33/V53 only
         * Type of Instruction: System
         *
         * Instruction:  RETXA int_vector
         *
         * Description:
         * [sp-1,sp-2] <- PSW       ; PSW EQU FLAGS
         * [sp-3,sp-4] <- PS        ; PS  EQU CS
         * [sp-5,sp-6] <- PC        ; PC  EQU IP
         * SP    <-  SP -6
         * IE    <-  0
         * BRK <-  0
         * MD    <-  0
         * PC    <- [int_vector*4 +0,+1]
         * PS    <- [int_vector*4 +2,+3]
         * Disable EA mode.
         *
         * Flags Affected:   None
         *
         * CPU mode: RM
         *
         * +++++++++++++++++++++++
         * Physical Form:   RETXA  imm8
         * COP (Code of Operation)   : 0Fh F0h imm8
         *
         * Clocks:   12
         */
		ModRM = FETCH;
		logerror("PC=%06x : RETXA %02x\n", activecpu_get_pc() - 3, ModRM);
		ICOUNT -= 12;
		break;

    case 0xff:                          /* 0F ff imm8 - BRKEM */
		/*
         * OPCODE BRKEM  -   Break for Emulation
         *
         * CPU: NEC/Sony V20/V30/V40/V50
         * Description:
         *
         * PUSH FLAGS
         * PUSH CS
         * PUSH IP
         * MOV  CS,0:[intnum*4+2]
         * MOV  IP,0:[intnum*4]
         * MD <- 0; // Enable 8080 emulation
         *
         * Note:
         * BRKEM instruction do software interrupt and then New CS,IP loaded
         * it switch to 8080 mode i.e. CPU will execute 8080 code.
         * Mapping Table of Registers in 8080 Mode
         * 8080 Md.   A  B   C  D  E  H  L  SP PC  F
         * native.     AL CH CL DH DL BH BL BP IP  FLAGS(low)
         * For Return of 8080 mode use CALLN instruction.
         * Note:    I.e. 8080 addressing only 64KB then "Real Address" is CS*16+PC
         *
         * Flags Affected: MD
         */
		ModRM = FETCH;
		ICOUNT -= 38;
		logerror("PC=%06x : BRKEM %02x\n", activecpu_get_pc() - 3, ModRM);
		PREFIX86(_interrupt)(cpustate, ModRM);
		cpustate->MF = 0;
		cpustate->MF_WriteDisabled = 0;
		break;
	}
}

static void PREFIXV30(_brkn)(i8086_state *cpustate)		/* Opcode 0x63 BRKN -  Break to Native Mode */
{
	/*
     * CPU:  NEC (V25/V35) Software Guard only
     * Instruction:  BRKN int_vector
     *
     * Description:
     * [sp-1,sp-2] <- PSW       ; PSW EQU FLAGS
     * [sp-3,sp-4] <- PS        ; PS  EQU CS
     * [sp-5,sp-6] <- PC        ; PC  EQU IP
     * SP    <-  SP -6
     * IE    <-  0
     * BRK <-  0
     * MD    <-  1
     * PC    <- [int_vector*4 +0,+1]
     * PS    <- [int_vector*4 +2,+3]
     *
     * Note:    The BRKN instruction switches operations in Native Mode
     * from Security Mode via Interrupt call. In Normal Mode
     * Instruction executed as   mPD70320/70322 (V25) operation mode.
     *
     * Flags Affected:   None
     *
     * CPU mode: RM
     *
     * +++++++++++++++++++++++
     * Physical Form:   BRKN  imm8
     * COP (Code of Operation)   : 63h imm8
     *
     * Clocks:   56+10T [44+10T]
     */
	/* ICOUNT-=56; */
	unsigned int_vector;

	int_vector = FETCH;
	logerror("PC=%06x : BRKN %02x\n", activecpu_get_pc() - 2, int_vector);
}

static void PREFIXV30(repc)(i8086_state *cpustate,int flagval)
{
	/* Handles repc- and repnc- prefixes. flagval is the value of ZF
     * for the loop to continue for CMPS and SCAS instructions.
     */

	unsigned next = FETCHOP;
	unsigned count = cpustate->regs.w[CX];

	switch (next)
	{
	case 0x26:							/* ES: */
		cpustate->seg_prefix = TRUE;
		cpustate->prefix_seg = ES;
		ICOUNT -= 2;
		PREFIXV30(repc)(cpustate, flagval);
		break;
	case 0x2e:							/* CS: */
		cpustate->seg_prefix = TRUE;
		cpustate->prefix_seg = CS;
		ICOUNT -= 2;
		PREFIXV30(repc)(cpustate, flagval);
		break;
	case 0x36:							/* SS: */
		cpustate->seg_prefix = TRUE;
		cpustate->prefix_seg = SS;
		ICOUNT -= 2;
		PREFIXV30(repc)(cpustate, flagval);
		break;
	case 0x3e:							/* DS: */
		cpustate->seg_prefix = TRUE;
		cpustate->prefix_seg = DS;
		ICOUNT -= 2;
		PREFIXV30(repc)(cpustate, flagval);
		break;
	case 0x6c:							/* REP INSB */
		ICOUNT -= 9 - count;
		for (; (CF == flagval) && (count > 0); count--)
			PREFIX186(_insb)(cpustate);
		cpustate->regs.w[CX] = count;
		break;
	case 0x6d:							/* REP INSW */
		ICOUNT -= 9 - count;
		for (; (CF == flagval) && (count > 0); count--)
			PREFIX186(_insw)(cpustate);
		cpustate->regs.w[CX] = count;
		break;
	case 0x6e:							/* REP OUTSB */
		ICOUNT -= 9 - count;
		for (; (CF == flagval) && (count > 0); count--)
			PREFIX186(_outsb)(cpustate);
		cpustate->regs.w[CX] = count;
		break;
	case 0x6f:							/* REP OUTSW */
		ICOUNT -= 9 - count;
		for (; (CF == flagval) && (count > 0); count--)
			PREFIX186(_outsw)(cpustate);
		cpustate->regs.w[CX] = count;
		break;
	case 0xa4:							/* REP MOVSB */
		ICOUNT -= 9 - count;
		for (; (CF == flagval) && (count > 0); count--)
			PREFIX86(_movsb)(cpustate);
		cpustate->regs.w[CX] = count;
		break;
	case 0xa5:							/* REP MOVSW */
		ICOUNT -= 9 - count;
		for (; (CF == flagval) && (count > 0); count--)
			PREFIX86(_movsw)(cpustate);
		cpustate->regs.w[CX] = count;
		break;
	case 0xa6:							/* REP(N)E CMPSB */
		ICOUNT -= 9;
		for (cpustate->ZeroVal = !flagval; (ZF == flagval) && (CF == flagval) && (count > 0); count--)
			PREFIX86(_cmpsb)(cpustate);
		cpustate->regs.w[CX] = count;
		break;
	case 0xa7:							/* REP(N)E CMPSW */
		ICOUNT -= 9;
		for (cpustate->ZeroVal = !flagval; (ZF == flagval) && (CF == flagval) && (count > 0); count--)
			PREFIX86(_cmpsw)(cpustate);
		cpustate->regs.w[CX] = count;
		break;
	case 0xaa:							/* REP STOSB */
		ICOUNT -= 9 - count;
		for (; (CF == flagval) && (count > 0); count--)
			PREFIX86(_stosb)(cpustate);
		cpustate->regs.w[CX] = count;
		break;
	case 0xab:							/* REP STOSW */
		ICOUNT -= 9 - count;
		for (; (CF == flagval) && (count > 0); count--)
			PREFIX86(_stosw)(cpustate);
		cpustate->regs.w[CX] = count;
		break;
	case 0xac:							/* REP LODSB */
		ICOUNT -= 9;
		for (; (CF == flagval) && (count > 0); count--)
			PREFIX86(_lodsb)(cpustate);
		cpustate->regs.w[CX] = count;
		break;
	case 0xad:							/* REP LODSW */
		ICOUNT -= 9;
		for (; (CF == flagval) && (count > 0); count--)
			PREFIX86(_lodsw)(cpustate);
		cpustate->regs.w[CX] = count;
		break;
	case 0xae:							/* REP(N)E SCASB */
		ICOUNT -= 9;
		for (cpustate->ZeroVal = !flagval; (ZF == flagval) && (CF == flagval) && (count > 0); count--)
			PREFIX86(_scasb)(cpustate);
		cpustate->regs.w[CX] = count;
		break;
	case 0xaf:							/* REP(N)E SCASW */
		ICOUNT -= 9;
		for (cpustate->ZeroVal = !flagval; (ZF == flagval) && (CF == flagval) && (count > 0); count--)
			PREFIX86(_scasw)(cpustate);
		cpustate->regs.w[CX] = count;
		break;
	default:
		PREFIXV30(_instruction)[next](cpustate);
	}
}

static void PREFIXV30(_repnc)(i8086_state *cpustate)	/* Opcode 0x64 */
{
	PREFIXV30(repc)(cpustate, 0);
}

static void PREFIXV30(_repc)(i8086_state *cpustate)		/* Opcode 0x65 */
{
	PREFIXV30(repc)(cpustate, 1);
}

static void PREFIXV30(_aam)(i8086_state *cpustate)    /* Opcode 0xd4 */
{
	unsigned mult = FETCH;

	ICOUNT -= timing.aam;

	cpustate->regs.b[AH] = cpustate->regs.b[AL] / 10;
	cpustate->regs.b[AL] %= 10;

	SetSZPF_Word(cpustate->regs.w[AX]);
}

static void PREFIXV30(_aad)(i8086_state *cpustate)    /* Opcode 0xd5 */
{
	unsigned mult = FETCH;

	ICOUNT -= timing.aad;

//	cpustate->regs.b[AL] = cpustate->regs.b[AH] * mult + cpustate->regs.b[AL];
	cpustate->regs.b[AL] = cpustate->regs.b[AH] * 10 + cpustate->regs.b[AL];
	cpustate->regs.b[AH] = 0;

	SetZF(cpustate->regs.b[AL]);
	SetPF(cpustate->regs.b[AL]);
	cpustate->SignVal = 0;
}

static void PREFIXV30(_setalc)(i8086_state *cpustate)	/* Opcode 0xd6 */
{
	/*
     * ----------O-SETALC---------------------------------
     * OPCODE SETALC  - Set AL to Carry Flag
     *
     * CPU:  Intel 8086 and all its clones and upward
     * compatibility chips.
     * Type of Instruction: User
     *
     * Instruction: SETALC
     *
     * Description:
     *
     * IF (CF=0) THEN AL:=0 ELSE AL:=FFH;
     *
     * Flags Affected: None
     *
     * CPU mode: RM,PM,VM,SMM
     *
     * Physical Form:        SETALC
     * COP (Code of Operation): D6H
     * Clocks:        80286    : n/a   [3]
     * 80386    : n/a   [3]
     * Cx486SLC  : n/a   [2]
     * i486     : n/a   [3]
     * Pentium  : n/a   [3]
     * Note: n/a is Time that Intel etc not say.
     * [3] is real time it executed.
     *
     */
	cpustate->regs.b[AL] = (CF) ? 0xff : 0x00;
	ICOUNT -= 3;					// V30
	logerror("PC=%06x : SETALC\n", activecpu_get_pc() - 1);
}

#if 0
static void PREFIXV30(_brks)(i8086_state *cpustate)		/* Opcode 0xf1 - Break to Security Mode */
{
	/*
     * CPU:  NEC (V25/V35) Software Guard  only
     * Instruction:  BRKS int_vector
     *
     * Description:
     * [sp-1,sp-2] <- PSW       ; PSW EQU FLAGS
     * [sp-3,sp-4] <- PS        ; PS  EQU CS
     * [sp-5,sp-6] <- PC        ; PC  EQU IP
     * SP    <-  SP -6
     * IE    <-  0
     * BRK <-  0
     * MD    <-  0
     * PC    <- [int_vector*4 +0,+1]
     * PS    <- [int_vector*4 +2,+3]
     *
     * Note:    The BRKS instruction switches operations in Security Mode
     * via Interrupt call. In Security Mode the fetched operation
     * code is executed after conversion in accordance with build-in
     * translation table
     *
     * Flags Affected:   None
     *
     * CPU mode: RM
     *
     * +++++++++++++++++++++++
     * Physical Form:   BRKS  imm8
     * Clocks:   56+10T [44+10T]
     */
	unsigned int_vector;

	int_vector = FETCH;
	logerror("PC=%06x : BRKS %02x\n", activecpu_get_pc() - 2, int_vector);
}
#endif

/* i8080 instructions */

/*
DS1	ES
PS	CS
SS	SS
DS0	DS
*/
#define I8080_AF	cpustate->regs.w[AX]
#define I8080_HL	cpustate->regs.w[BX]
#define I8080_BC	cpustate->regs.w[CX]
#define I8080_DE	cpustate->regs.w[DX]

#define I8080_F		cpustate->regs.b[AH]
#define I8080_A		cpustate->regs.b[AL]

#define I8080_H		cpustate->regs.b[BH]
#define I8080_L		cpustate->regs.b[BL]
#define I8080_B		cpustate->regs.b[CH]
#define I8080_C		cpustate->regs.b[CL]
#define I8080_D		cpustate->regs.b[DH]
#define I8080_E		cpustate->regs.b[DL]

#define I8080_SP	cpustate->regs.w[BP]

#define I8080_CF	0x01
#define I8080_NF	0x02
#define I8080_VF	0x04
#define I8080_XF	0x08
#define I8080_HF	0x10
#define I8080_YF	0x20
#define I8080_ZF	0x40
#define I8080_SF	0x80

static const int I8080_CLK[0x100] = {
	 4,10, 7, 5, 5, 5, 7, 4, 4,10, 7, 5, 5, 5, 7, 4, 4,10, 7, 5, 5, 5, 7, 4, 4,10, 7, 5, 5, 5, 7, 4,
	 4,10,16, 5, 5, 5, 7, 4, 4,10,16, 5, 5, 5, 7, 4, 4,10,13, 5,10,10,10, 4, 4,10,13, 5, 5, 5, 7, 4,
	 5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	 5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5, 7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,
	 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	 5,10,10,10,11,11, 7,11, 5,10,10,10,11,17, 7,11, 5,10,10,10,11,11, 7,11, 5,10,10,10,11,17, 7,11,
	 5,10,10,18,11,11, 7,11, 5, 5,10, 5,11,17, 7,11, 5,10,10, 4,11,11, 7,11, 5, 5,10, 4,11,17, 7,11
};

static const UINT8 I8080_ZS[256] = {
	0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80
};

static const UINT8 I8080_ZSP[256] = {
	0x44,0x00,0x00,0x04,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,
	0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00,
	0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00,
	0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,
	0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00,
	0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,
	0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,
	0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00,
	0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,
	0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,
	0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,
	0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,
	0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,
	0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,
	0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,
	0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84
};

static const UINT16 I8080_DAA[2048] = {
	0x0044,0x0100,0x0200,0x0304,0x0400,0x0504,0x0604,0x0700,0x0808,0x090c,0x1010,0x1114,0x1214,0x1310,0x1414,0x1510,
	0x1000,0x1104,0x1204,0x1300,0x1404,0x1500,0x1600,0x1704,0x180c,0x1908,0x2030,0x2134,0x2234,0x2330,0x2434,0x2530,
	0x2020,0x2124,0x2224,0x2320,0x2424,0x2520,0x2620,0x2724,0x282c,0x2928,0x3034,0x3130,0x3230,0x3334,0x3430,0x3534,
	0x3024,0x3120,0x3220,0x3324,0x3420,0x3524,0x3624,0x3720,0x3828,0x392c,0x4010,0x4114,0x4214,0x4310,0x4414,0x4510,
	0x4000,0x4104,0x4204,0x4300,0x4404,0x4500,0x4600,0x4704,0x480c,0x4908,0x5014,0x5110,0x5210,0x5314,0x5410,0x5514,
	0x5004,0x5100,0x5200,0x5304,0x5400,0x5504,0x5604,0x5700,0x5808,0x590c,0x6034,0x6130,0x6230,0x6334,0x6430,0x6534,
	0x6024,0x6120,0x6220,0x6324,0x6420,0x6524,0x6624,0x6720,0x6828,0x692c,0x7030,0x7134,0x7234,0x7330,0x7434,0x7530,
	0x7020,0x7124,0x7224,0x7320,0x7424,0x7520,0x7620,0x7724,0x782c,0x7928,0x8090,0x8194,0x8294,0x8390,0x8494,0x8590,
	0x8080,0x8184,0x8284,0x8380,0x8484,0x8580,0x8680,0x8784,0x888c,0x8988,0x9094,0x9190,0x9290,0x9394,0x9490,0x9594,
	0x9084,0x9180,0x9280,0x9384,0x9480,0x9584,0x9684,0x9780,0x9888,0x998c,0x0055,0x0111,0x0211,0x0315,0x0411,0x0515,
	0x0045,0x0101,0x0201,0x0305,0x0401,0x0505,0x0605,0x0701,0x0809,0x090d,0x1011,0x1115,0x1215,0x1311,0x1415,0x1511,
	0x1001,0x1105,0x1205,0x1301,0x1405,0x1501,0x1601,0x1705,0x180d,0x1909,0x2031,0x2135,0x2235,0x2331,0x2435,0x2531,
	0x2021,0x2125,0x2225,0x2321,0x2425,0x2521,0x2621,0x2725,0x282d,0x2929,0x3035,0x3131,0x3231,0x3335,0x3431,0x3535,
	0x3025,0x3121,0x3221,0x3325,0x3421,0x3525,0x3625,0x3721,0x3829,0x392d,0x4011,0x4115,0x4215,0x4311,0x4415,0x4511,
	0x4001,0x4105,0x4205,0x4301,0x4405,0x4501,0x4601,0x4705,0x480d,0x4909,0x5015,0x5111,0x5211,0x5315,0x5411,0x5515,
	0x5005,0x5101,0x5201,0x5305,0x5401,0x5505,0x5605,0x5701,0x5809,0x590d,0x6035,0x6131,0x6231,0x6335,0x6431,0x6535,
	0x6025,0x6121,0x6221,0x6325,0x6421,0x6525,0x6625,0x6721,0x6829,0x692d,0x7031,0x7135,0x7235,0x7331,0x7435,0x7531,
	0x7021,0x7125,0x7225,0x7321,0x7425,0x7521,0x7621,0x7725,0x782d,0x7929,0x8091,0x8195,0x8295,0x8391,0x8495,0x8591,
	0x8081,0x8185,0x8285,0x8381,0x8485,0x8581,0x8681,0x8785,0x888d,0x8989,0x9095,0x9191,0x9291,0x9395,0x9491,0x9595,
	0x9085,0x9181,0x9281,0x9385,0x9481,0x9585,0x9685,0x9781,0x9889,0x998d,0xa0b5,0xa1b1,0xa2b1,0xa3b5,0xa4b1,0xa5b5,
	0xa0a5,0xa1a1,0xa2a1,0xa3a5,0xa4a1,0xa5a5,0xa6a5,0xa7a1,0xa8a9,0xa9ad,0xb0b1,0xb1b5,0xb2b5,0xb3b1,0xb4b5,0xb5b1,
	0xb0a1,0xb1a5,0xb2a5,0xb3a1,0xb4a5,0xb5a1,0xb6a1,0xb7a5,0xb8ad,0xb9a9,0xc095,0xc191,0xc291,0xc395,0xc491,0xc595,
	0xc085,0xc181,0xc281,0xc385,0xc481,0xc585,0xc685,0xc781,0xc889,0xc98d,0xd091,0xd195,0xd295,0xd391,0xd495,0xd591,
	0xd081,0xd185,0xd285,0xd381,0xd485,0xd581,0xd681,0xd785,0xd88d,0xd989,0xe0b1,0xe1b5,0xe2b5,0xe3b1,0xe4b5,0xe5b1,
	0xe0a1,0xe1a5,0xe2a5,0xe3a1,0xe4a5,0xe5a1,0xe6a1,0xe7a5,0xe8ad,0xe9a9,0xf0b5,0xf1b1,0xf2b1,0xf3b5,0xf4b1,0xf5b5,
	0xf0a5,0xf1a1,0xf2a1,0xf3a5,0xf4a1,0xf5a5,0xf6a5,0xf7a1,0xf8a9,0xf9ad,0x0055,0x0111,0x0211,0x0315,0x0411,0x0515,
	0x0045,0x0101,0x0201,0x0305,0x0401,0x0505,0x0605,0x0701,0x0809,0x090d,0x1011,0x1115,0x1215,0x1311,0x1415,0x1511,
	0x1001,0x1105,0x1205,0x1301,0x1405,0x1501,0x1601,0x1705,0x180d,0x1909,0x2031,0x2135,0x2235,0x2331,0x2435,0x2531,
	0x2021,0x2125,0x2225,0x2321,0x2425,0x2521,0x2621,0x2725,0x282d,0x2929,0x3035,0x3131,0x3231,0x3335,0x3431,0x3535,
	0x3025,0x3121,0x3221,0x3325,0x3421,0x3525,0x3625,0x3721,0x3829,0x392d,0x4011,0x4115,0x4215,0x4311,0x4415,0x4511,
	0x4001,0x4105,0x4205,0x4301,0x4405,0x4501,0x4601,0x4705,0x480d,0x4909,0x5015,0x5111,0x5211,0x5315,0x5411,0x5515,
	0x5005,0x5101,0x5201,0x5305,0x5401,0x5505,0x5605,0x5701,0x5809,0x590d,0x6035,0x6131,0x6231,0x6335,0x6431,0x6535,
	0x0604,0x0700,0x0808,0x090c,0x0a0c,0x0b08,0x0c0c,0x0d08,0x0e08,0x0f0c,0x1010,0x1114,0x1214,0x1310,0x1414,0x1510,
	0x1600,0x1704,0x180c,0x1908,0x1a08,0x1b0c,0x1c08,0x1d0c,0x1e0c,0x1f08,0x2030,0x2134,0x2234,0x2330,0x2434,0x2530,
	0x2620,0x2724,0x282c,0x2928,0x2a28,0x2b2c,0x2c28,0x2d2c,0x2e2c,0x2f28,0x3034,0x3130,0x3230,0x3334,0x3430,0x3534,
	0x3624,0x3720,0x3828,0x392c,0x3a2c,0x3b28,0x3c2c,0x3d28,0x3e28,0x3f2c,0x4010,0x4114,0x4214,0x4310,0x4414,0x4510,
	0x4600,0x4704,0x480c,0x4908,0x4a08,0x4b0c,0x4c08,0x4d0c,0x4e0c,0x4f08,0x5014,0x5110,0x5210,0x5314,0x5410,0x5514,
	0x5604,0x5700,0x5808,0x590c,0x5a0c,0x5b08,0x5c0c,0x5d08,0x5e08,0x5f0c,0x6034,0x6130,0x6230,0x6334,0x6430,0x6534,
	0x6624,0x6720,0x6828,0x692c,0x6a2c,0x6b28,0x6c2c,0x6d28,0x6e28,0x6f2c,0x7030,0x7134,0x7234,0x7330,0x7434,0x7530,
	0x7620,0x7724,0x782c,0x7928,0x7a28,0x7b2c,0x7c28,0x7d2c,0x7e2c,0x7f28,0x8090,0x8194,0x8294,0x8390,0x8494,0x8590,
	0x8680,0x8784,0x888c,0x8988,0x8a88,0x8b8c,0x8c88,0x8d8c,0x8e8c,0x8f88,0x9094,0x9190,0x9290,0x9394,0x9490,0x9594,
	0x9684,0x9780,0x9888,0x998c,0x9a8c,0x9b88,0x9c8c,0x9d88,0x9e88,0x9f8c,0x0055,0x0111,0x0211,0x0315,0x0411,0x0515,
	0x0605,0x0701,0x0809,0x090d,0x0a0d,0x0b09,0x0c0d,0x0d09,0x0e09,0x0f0d,0x1011,0x1115,0x1215,0x1311,0x1415,0x1511,
	0x1601,0x1705,0x180d,0x1909,0x1a09,0x1b0d,0x1c09,0x1d0d,0x1e0d,0x1f09,0x2031,0x2135,0x2235,0x2331,0x2435,0x2531,
	0x2621,0x2725,0x282d,0x2929,0x2a29,0x2b2d,0x2c29,0x2d2d,0x2e2d,0x2f29,0x3035,0x3131,0x3231,0x3335,0x3431,0x3535,
	0x3625,0x3721,0x3829,0x392d,0x3a2d,0x3b29,0x3c2d,0x3d29,0x3e29,0x3f2d,0x4011,0x4115,0x4215,0x4311,0x4415,0x4511,
	0x4601,0x4705,0x480d,0x4909,0x4a09,0x4b0d,0x4c09,0x4d0d,0x4e0d,0x4f09,0x5015,0x5111,0x5211,0x5315,0x5411,0x5515,
	0x5605,0x5701,0x5809,0x590d,0x5a0d,0x5b09,0x5c0d,0x5d09,0x5e09,0x5f0d,0x6035,0x6131,0x6231,0x6335,0x6431,0x6535,
	0x6625,0x6721,0x6829,0x692d,0x6a2d,0x6b29,0x6c2d,0x6d29,0x6e29,0x6f2d,0x7031,0x7135,0x7235,0x7331,0x7435,0x7531,
	0x7621,0x7725,0x782d,0x7929,0x7a29,0x7b2d,0x7c29,0x7d2d,0x7e2d,0x7f29,0x8091,0x8195,0x8295,0x8391,0x8495,0x8591,
	0x8681,0x8785,0x888d,0x8989,0x8a89,0x8b8d,0x8c89,0x8d8d,0x8e8d,0x8f89,0x9095,0x9191,0x9291,0x9395,0x9491,0x9595,
	0x9685,0x9781,0x9889,0x998d,0x9a8d,0x9b89,0x9c8d,0x9d89,0x9e89,0x9f8d,0xa0b5,0xa1b1,0xa2b1,0xa3b5,0xa4b1,0xa5b5,
	0xa6a5,0xa7a1,0xa8a9,0xa9ad,0xaaad,0xaba9,0xacad,0xada9,0xaea9,0xafad,0xb0b1,0xb1b5,0xb2b5,0xb3b1,0xb4b5,0xb5b1,
	0xb6a1,0xb7a5,0xb8ad,0xb9a9,0xbaa9,0xbbad,0xbca9,0xbdad,0xbead,0xbfa9,0xc095,0xc191,0xc291,0xc395,0xc491,0xc595,
	0xc685,0xc781,0xc889,0xc98d,0xca8d,0xcb89,0xcc8d,0xcd89,0xce89,0xcf8d,0xd091,0xd195,0xd295,0xd391,0xd495,0xd591,
	0xd681,0xd785,0xd88d,0xd989,0xda89,0xdb8d,0xdc89,0xdd8d,0xde8d,0xdf89,0xe0b1,0xe1b5,0xe2b5,0xe3b1,0xe4b5,0xe5b1,
	0xe6a1,0xe7a5,0xe8ad,0xe9a9,0xeaa9,0xebad,0xeca9,0xedad,0xeead,0xefa9,0xf0b5,0xf1b1,0xf2b1,0xf3b5,0xf4b1,0xf5b5,
	0xf6a5,0xf7a1,0xf8a9,0xf9ad,0xfaad,0xfba9,0xfcad,0xfda9,0xfea9,0xffad,0x0055,0x0111,0x0211,0x0315,0x0411,0x0515,
	0x0605,0x0701,0x0809,0x090d,0x0a0d,0x0b09,0x0c0d,0x0d09,0x0e09,0x0f0d,0x1011,0x1115,0x1215,0x1311,0x1415,0x1511,
	0x1601,0x1705,0x180d,0x1909,0x1a09,0x1b0d,0x1c09,0x1d0d,0x1e0d,0x1f09,0x2031,0x2135,0x2235,0x2331,0x2435,0x2531,
	0x2621,0x2725,0x282d,0x2929,0x2a29,0x2b2d,0x2c29,0x2d2d,0x2e2d,0x2f29,0x3035,0x3131,0x3231,0x3335,0x3431,0x3535,
	0x3625,0x3721,0x3829,0x392d,0x3a2d,0x3b29,0x3c2d,0x3d29,0x3e29,0x3f2d,0x4011,0x4115,0x4215,0x4311,0x4415,0x4511,
	0x4601,0x4705,0x480d,0x4909,0x4a09,0x4b0d,0x4c09,0x4d0d,0x4e0d,0x4f09,0x5015,0x5111,0x5211,0x5315,0x5411,0x5515,
	0x5605,0x5701,0x5809,0x590d,0x5a0d,0x5b09,0x5c0d,0x5d09,0x5e09,0x5f0d,0x6035,0x6131,0x6231,0x6335,0x6431,0x6535,
	0x0046,0x0102,0x0202,0x0306,0x0402,0x0506,0x0606,0x0702,0x080a,0x090e,0x0402,0x0506,0x0606,0x0702,0x080a,0x090e,
	0x1002,0x1106,0x1206,0x1302,0x1406,0x1502,0x1602,0x1706,0x180e,0x190a,0x1406,0x1502,0x1602,0x1706,0x180e,0x190a,
	0x2022,0x2126,0x2226,0x2322,0x2426,0x2522,0x2622,0x2726,0x282e,0x292a,0x2426,0x2522,0x2622,0x2726,0x282e,0x292a,
	0x3026,0x3122,0x3222,0x3326,0x3422,0x3526,0x3626,0x3722,0x382a,0x392e,0x3422,0x3526,0x3626,0x3722,0x382a,0x392e,
	0x4002,0x4106,0x4206,0x4302,0x4406,0x4502,0x4602,0x4706,0x480e,0x490a,0x4406,0x4502,0x4602,0x4706,0x480e,0x490a,
	0x5006,0x5102,0x5202,0x5306,0x5402,0x5506,0x5606,0x5702,0x580a,0x590e,0x5402,0x5506,0x5606,0x5702,0x580a,0x590e,
	0x6026,0x6122,0x6222,0x6326,0x6422,0x6526,0x6626,0x6722,0x682a,0x692e,0x6422,0x6526,0x6626,0x6722,0x682a,0x692e,
	0x7022,0x7126,0x7226,0x7322,0x7426,0x7522,0x7622,0x7726,0x782e,0x792a,0x7426,0x7522,0x7622,0x7726,0x782e,0x792a,
	0x8082,0x8186,0x8286,0x8382,0x8486,0x8582,0x8682,0x8786,0x888e,0x898a,0x8486,0x8582,0x8682,0x8786,0x888e,0x898a,
	0x9086,0x9182,0x9282,0x9386,0x9482,0x9586,0x9686,0x9782,0x988a,0x998e,0x3423,0x3527,0x3627,0x3723,0x382b,0x392f,
	0x4003,0x4107,0x4207,0x4303,0x4407,0x4503,0x4603,0x4707,0x480f,0x490b,0x4407,0x4503,0x4603,0x4707,0x480f,0x490b,
	0x5007,0x5103,0x5203,0x5307,0x5403,0x5507,0x5607,0x5703,0x580b,0x590f,0x5403,0x5507,0x5607,0x5703,0x580b,0x590f,
	0x6027,0x6123,0x6223,0x6327,0x6423,0x6527,0x6627,0x6723,0x682b,0x692f,0x6423,0x6527,0x6627,0x6723,0x682b,0x692f,
	0x7023,0x7127,0x7227,0x7323,0x7427,0x7523,0x7623,0x7727,0x782f,0x792b,0x7427,0x7523,0x7623,0x7727,0x782f,0x792b,
	0x8083,0x8187,0x8287,0x8383,0x8487,0x8583,0x8683,0x8787,0x888f,0x898b,0x8487,0x8583,0x8683,0x8787,0x888f,0x898b,
	0x9087,0x9183,0x9283,0x9387,0x9483,0x9587,0x9687,0x9783,0x988b,0x998f,0x9483,0x9587,0x9687,0x9783,0x988b,0x998f,
	0xa0a7,0xa1a3,0xa2a3,0xa3a7,0xa4a3,0xa5a7,0xa6a7,0xa7a3,0xa8ab,0xa9af,0xa4a3,0xa5a7,0xa6a7,0xa7a3,0xa8ab,0xa9af,
	0xb0a3,0xb1a7,0xb2a7,0xb3a3,0xb4a7,0xb5a3,0xb6a3,0xb7a7,0xb8af,0xb9ab,0xb4a7,0xb5a3,0xb6a3,0xb7a7,0xb8af,0xb9ab,
	0xc087,0xc183,0xc283,0xc387,0xc483,0xc587,0xc687,0xc783,0xc88b,0xc98f,0xc483,0xc587,0xc687,0xc783,0xc88b,0xc98f,
	0xd083,0xd187,0xd287,0xd383,0xd487,0xd583,0xd683,0xd787,0xd88f,0xd98b,0xd487,0xd583,0xd683,0xd787,0xd88f,0xd98b,
	0xe0a3,0xe1a7,0xe2a7,0xe3a3,0xe4a7,0xe5a3,0xe6a3,0xe7a7,0xe8af,0xe9ab,0xe4a7,0xe5a3,0xe6a3,0xe7a7,0xe8af,0xe9ab,
	0xf0a7,0xf1a3,0xf2a3,0xf3a7,0xf4a3,0xf5a7,0xf6a7,0xf7a3,0xf8ab,0xf9af,0xf4a3,0xf5a7,0xf6a7,0xf7a3,0xf8ab,0xf9af,
	0x0047,0x0103,0x0203,0x0307,0x0403,0x0507,0x0607,0x0703,0x080b,0x090f,0x0403,0x0507,0x0607,0x0703,0x080b,0x090f,
	0x1003,0x1107,0x1207,0x1303,0x1407,0x1503,0x1603,0x1707,0x180f,0x190b,0x1407,0x1503,0x1603,0x1707,0x180f,0x190b,
	0x2023,0x2127,0x2227,0x2323,0x2427,0x2523,0x2623,0x2727,0x282f,0x292b,0x2427,0x2523,0x2623,0x2727,0x282f,0x292b,
	0x3027,0x3123,0x3223,0x3327,0x3423,0x3527,0x3627,0x3723,0x382b,0x392f,0x3423,0x3527,0x3627,0x3723,0x382b,0x392f,
	0x4003,0x4107,0x4207,0x4303,0x4407,0x4503,0x4603,0x4707,0x480f,0x490b,0x4407,0x4503,0x4603,0x4707,0x480f,0x490b,
	0x5007,0x5103,0x5203,0x5307,0x5403,0x5507,0x5607,0x5703,0x580b,0x590f,0x5403,0x5507,0x5607,0x5703,0x580b,0x590f,
	0x6027,0x6123,0x6223,0x6327,0x6423,0x6527,0x6627,0x6723,0x682b,0x692f,0x6423,0x6527,0x6627,0x6723,0x682b,0x692f,
	0x7023,0x7127,0x7227,0x7323,0x7427,0x7523,0x7623,0x7727,0x782f,0x792b,0x7427,0x7523,0x7623,0x7727,0x782f,0x792b,
	0x8083,0x8187,0x8287,0x8383,0x8487,0x8583,0x8683,0x8787,0x888f,0x898b,0x8487,0x8583,0x8683,0x8787,0x888f,0x898b,
	0x9087,0x9183,0x9283,0x9387,0x9483,0x9587,0x9687,0x9783,0x988b,0x998f,0x9483,0x9587,0x9687,0x9783,0x988b,0x998f,
	0xfabe,0xfbba,0xfcbe,0xfdba,0xfeba,0xffbe,0x0046,0x0102,0x0202,0x0306,0x0402,0x0506,0x0606,0x0702,0x080a,0x090e,
	0x0a1e,0x0b1a,0x0c1e,0x0d1a,0x0e1a,0x0f1e,0x1002,0x1106,0x1206,0x1302,0x1406,0x1502,0x1602,0x1706,0x180e,0x190a,
	0x1a1a,0x1b1e,0x1c1a,0x1d1e,0x1e1e,0x1f1a,0x2022,0x2126,0x2226,0x2322,0x2426,0x2522,0x2622,0x2726,0x282e,0x292a,
	0x2a3a,0x2b3e,0x2c3a,0x2d3e,0x2e3e,0x2f3a,0x3026,0x3122,0x3222,0x3326,0x3422,0x3526,0x3626,0x3722,0x382a,0x392e,
	0x3a3e,0x3b3a,0x3c3e,0x3d3a,0x3e3a,0x3f3e,0x4002,0x4106,0x4206,0x4302,0x4406,0x4502,0x4602,0x4706,0x480e,0x490a,
	0x4a1a,0x4b1e,0x4c1a,0x4d1e,0x4e1e,0x4f1a,0x5006,0x5102,0x5202,0x5306,0x5402,0x5506,0x5606,0x5702,0x580a,0x590e,
	0x5a1e,0x5b1a,0x5c1e,0x5d1a,0x5e1a,0x5f1e,0x6026,0x6122,0x6222,0x6326,0x6422,0x6526,0x6626,0x6722,0x682a,0x692e,
	0x6a3e,0x6b3a,0x6c3e,0x6d3a,0x6e3a,0x6f3e,0x7022,0x7126,0x7226,0x7322,0x7426,0x7522,0x7622,0x7726,0x782e,0x792a,
	0x7a3a,0x7b3e,0x7c3a,0x7d3e,0x7e3e,0x7f3a,0x8082,0x8186,0x8286,0x8382,0x8486,0x8582,0x8682,0x8786,0x888e,0x898a,
	0x8a9a,0x8b9e,0x8c9a,0x8d9e,0x8e9e,0x8f9a,0x9086,0x9182,0x9282,0x9386,0x3423,0x3527,0x3627,0x3723,0x382b,0x392f,
	0x3a3f,0x3b3b,0x3c3f,0x3d3b,0x3e3b,0x3f3f,0x4003,0x4107,0x4207,0x4303,0x4407,0x4503,0x4603,0x4707,0x480f,0x490b,
	0x4a1b,0x4b1f,0x4c1b,0x4d1f,0x4e1f,0x4f1b,0x5007,0x5103,0x5203,0x5307,0x5403,0x5507,0x5607,0x5703,0x580b,0x590f,
	0x5a1f,0x5b1b,0x5c1f,0x5d1b,0x5e1b,0x5f1f,0x6027,0x6123,0x6223,0x6327,0x6423,0x6527,0x6627,0x6723,0x682b,0x692f,
	0x6a3f,0x6b3b,0x6c3f,0x6d3b,0x6e3b,0x6f3f,0x7023,0x7127,0x7227,0x7323,0x7427,0x7523,0x7623,0x7727,0x782f,0x792b,
	0x7a3b,0x7b3f,0x7c3b,0x7d3f,0x7e3f,0x7f3b,0x8083,0x8187,0x8287,0x8383,0x8487,0x8583,0x8683,0x8787,0x888f,0x898b,
	0x8a9b,0x8b9f,0x8c9b,0x8d9f,0x8e9f,0x8f9b,0x9087,0x9183,0x9283,0x9387,0x9483,0x9587,0x9687,0x9783,0x988b,0x998f,
	0x9a9f,0x9b9b,0x9c9f,0x9d9b,0x9e9b,0x9f9f,0xa0a7,0xa1a3,0xa2a3,0xa3a7,0xa4a3,0xa5a7,0xa6a7,0xa7a3,0xa8ab,0xa9af,
	0xaabf,0xabbb,0xacbf,0xadbb,0xaebb,0xafbf,0xb0a3,0xb1a7,0xb2a7,0xb3a3,0xb4a7,0xb5a3,0xb6a3,0xb7a7,0xb8af,0xb9ab,
	0xbabb,0xbbbf,0xbcbb,0xbdbf,0xbebf,0xbfbb,0xc087,0xc183,0xc283,0xc387,0xc483,0xc587,0xc687,0xc783,0xc88b,0xc98f,
	0xca9f,0xcb9b,0xcc9f,0xcd9b,0xce9b,0xcf9f,0xd083,0xd187,0xd287,0xd383,0xd487,0xd583,0xd683,0xd787,0xd88f,0xd98b,
	0xda9b,0xdb9f,0xdc9b,0xdd9f,0xde9f,0xdf9b,0xe0a3,0xe1a7,0xe2a7,0xe3a3,0xe4a7,0xe5a3,0xe6a3,0xe7a7,0xe8af,0xe9ab,
	0xeabb,0xebbf,0xecbb,0xedbf,0xeebf,0xefbb,0xf0a7,0xf1a3,0xf2a3,0xf3a7,0xf4a3,0xf5a7,0xf6a7,0xf7a3,0xf8ab,0xf9af,
	0xfabf,0xfbbb,0xfcbf,0xfdbb,0xfebb,0xffbf,0x0047,0x0103,0x0203,0x0307,0x0403,0x0507,0x0607,0x0703,0x080b,0x090f,
	0x0a1f,0x0b1b,0x0c1f,0x0d1b,0x0e1b,0x0f1f,0x1003,0x1107,0x1207,0x1303,0x1407,0x1503,0x1603,0x1707,0x180f,0x190b,
	0x1a1b,0x1b1f,0x1c1b,0x1d1f,0x1e1f,0x1f1b,0x2023,0x2127,0x2227,0x2323,0x2427,0x2523,0x2623,0x2727,0x282f,0x292b,
	0x2a3b,0x2b3f,0x2c3b,0x2d3f,0x2e3f,0x2f3b,0x3027,0x3123,0x3223,0x3327,0x3423,0x3527,0x3627,0x3723,0x382b,0x392f,
	0x3a3f,0x3b3b,0x3c3f,0x3d3b,0x3e3b,0x3f3f,0x4003,0x4107,0x4207,0x4303,0x4407,0x4503,0x4603,0x4707,0x480f,0x490b,
	0x4a1b,0x4b1f,0x4c1b,0x4d1f,0x4e1f,0x4f1b,0x5007,0x5103,0x5203,0x5307,0x5403,0x5507,0x5607,0x5703,0x580b,0x590f,
	0x5a1f,0x5b1b,0x5c1f,0x5d1b,0x5e1b,0x5f1f,0x6027,0x6123,0x6223,0x6327,0x6423,0x6527,0x6627,0x6723,0x682b,0x692f,
	0x6a3f,0x6b3b,0x6c3f,0x6d3b,0x6e3b,0x6f3f,0x7023,0x7127,0x7227,0x7323,0x7427,0x7523,0x7623,0x7727,0x782f,0x792b,
	0x7a3b,0x7b3f,0x7c3b,0x7d3f,0x7e3f,0x7f3b,0x8083,0x8187,0x8287,0x8383,0x8487,0x8583,0x8683,0x8787,0x888f,0x898b,
	0x8a9b,0x8b9f,0x8c9b,0x8d9f,0x8e9f,0x8f9b,0x9087,0x9183,0x9283,0x9387,0x9483,0x9587,0x9687,0x9783,0x988b,0x998f
};

inline UINT8 I8080_RM8(i8086_state *cpustate, UINT16 a)
{
	return read_mem_byte(cpustate, (cpustate->base[DS] + a) & AMASK);
}

inline void I8080_WM8(i8086_state *cpustate, UINT16 a, UINT8 v)
{
	write_mem_byte(cpustate, (cpustate->base[DS] + a) & AMASK, v);
}

inline UINT16 I8080_RM16(i8086_state *cpustate, UINT16 a)
{
	return read_mem_word(cpustate, (cpustate->base[DS] + a) & AMASK);
}

inline void I8080_WM16(i8086_state *cpustate, UINT16 a, UINT16 v)
{
	write_mem_word(cpustate, (cpustate->base[DS] + a) & AMASK, v);
}

inline void I8080_PUSH(i8086_state *cpustate, UINT16 v)
{
	cpustate->regs.w[BP] -= 2;
	write_mem_word(cpustate, (cpustate->base[DS] + cpustate->regs.w[BP]) & AMASK, v);
}

inline UINT16 I8080_POP(i8086_state *cpustate)
{
	UINT16 bp = cpustate->regs.w[BP];
	cpustate->regs.w[BP] += 2;
	return read_mem_word(cpustate, (cpustate->base[DS] + bp) & AMASK);
}

inline UINT8 I8080_FETCH8(i8086_state *cpustate)
{
	UINT8 var = read_mem_byte(cpustate, cpustate->pc & AMASK);
	UINT16 ip = cpustate->pc - cpustate->base[CS];
	ip += 1;
	cpustate->pc = cpustate->base[CS] + ip;
	return var;
}

inline UINT16 I8080_FETCH16(i8086_state *cpustate)
{
	UINT16 var = read_mem_word(cpustate, cpustate->pc & AMASK);
	UINT16 ip = cpustate->pc - cpustate->base[CS];
	ip += 2;
	cpustate->pc = cpustate->base[CS] + ip;
	return var;
}

inline UINT8 I8080_IN8(i8086_state *cpustate, UINT16 a)
{
	return cpustate->io->read_io8(a);
}

inline void I8080_OUT8(i8086_state *cpustate, UINT16 a, UINT8 v)
{
	cpustate->io->write_io8(a, v);
}

#define I8080_INR(v) { \
	UINT8 hc = ((v & 0x0f) == 0x0f) ? I8080_HF : 0; \
	++v; \
	I8080_F = (I8080_F & I8080_CF) | I8080_ZSP[v] | hc; \
}
#define I8080_DCR(v) { \
	UINT8 hc = ((v & 0x0f) == 0x00) ? I8080_HF : 0; \
	--v; \
	I8080_F = (I8080_F & I8080_CF) | I8080_ZSP[v] | hc | I8080_NF; \
}
#define I8080_MVI(v) { \
	v = I8080_FETCH8(cpustate); \
}
#define I8080_ANA(v) { \
	int i = (((I8080_A | v) >> 3) & 1) * I8080_HF; \
	I8080_A &= v; \
	I8080_F = I8080_ZSP[I8080_A]; \
	I8080_F |= i; \
}
#define I8080_ORA(v) { \
	I8080_A |= v; \
	I8080_F = I8080_ZSP[I8080_A]; \
}
#define I8080_XRA(v) { \
	I8080_A ^= v; \
	I8080_F = I8080_ZSP[I8080_A]; \
}
#define I8080_RLC() { \
	I8080_A = (I8080_A << 1) | (I8080_A >> 7); \
	I8080_F = (I8080_F & 0xfe) | (I8080_A & I8080_CF); \
}
#define I8080_RRC() { \
	I8080_F = (I8080_F & 0xfe) | (I8080_A & I8080_CF); \
	I8080_A = (I8080_A >> 1) | (I8080_A << 7); \
}
#define I8080_RAL() { \
	int c = I8080_F & I8080_CF; \
	I8080_F = (I8080_F & 0xfe) | (I8080_A >> 7); \
	I8080_A = (I8080_A << 1) | c; \
}
#define i8080_RAR() { \
	int c = (I8080_F & I8080_CF) << 7; \
	I8080_F = (I8080_F & 0xfe) | (I8080_A & I8080_CF); \
	I8080_A = (I8080_A >> 1) | c; \
}
#define i8080_ADD(v) { \
	int q = I8080_A + v; \
	I8080_F = I8080_ZSP[q & 255] | ((q >> 8) & I8080_CF) | ((I8080_A ^ q ^ v) & I8080_HF) | (((v ^ I8080_A ^ I8080_SF) & (v ^ q) & I8080_SF) >> 5); \
	I8080_A = q; \
}
#define I8080_ADC(v) {\
	int q = I8080_A + v + (I8080_F & I8080_CF); \
	I8080_F = I8080_ZSP[q & 255] | ((q >> 8) & I8080_CF) | ((I8080_A ^ q ^ v) & I8080_HF) | (((v ^ I8080_A ^ I8080_SF) & (v ^ q) & I8080_SF) >> 5); \
	I8080_A = q; \
}
#define I8080_SUB(v) { \
	int q = I8080_A - v; \
	I8080_F = I8080_ZSP[q & 255] | ((q >> 8) & I8080_CF) | I8080_NF | ((I8080_A ^ q ^ v) & I8080_HF) | (((v ^ I8080_A) & (I8080_A ^ q) & I8080_SF) >> 5); \
	I8080_A = q; \
}
#define I8080_SBB(v) { \
	int q = I8080_A - v - (I8080_F & I8080_CF); \
	I8080_F = I8080_ZSP[q & 255] | ((q >> 8) & I8080_CF) | I8080_NF | ((I8080_A ^ q ^ v) & I8080_HF) | (((v ^ I8080_A) & (I8080_A ^ q) & I8080_SF) >> 5); \
	I8080_A = q; \
}
#define I8080_CMP(v) { \
	int q = I8080_A - v; \
	I8080_F = I8080_ZSP[q & 255] | ((q >> 8) & I8080_CF) | I8080_NF | ((I8080_A ^ q ^ v) & I8080_HF) | (((v ^ I8080_A) & (I8080_A ^ q) & I8080_SF) >> 5); \
}
#define I8080_DAD(v) { \
	int q = I8080_HL + v; \
	I8080_F = (I8080_F & ~(I8080_HF + I8080_CF)) | (((I8080_HL ^ q ^ v) >> 8) & I8080_HF) | ((q >> 16) & I8080_CF); \
	I8080_HL = q; \
}
#define I8080_RET(c) { \
	if(c) { \
		ICOUNT -= 6; \
		cpustate->pc = cpustate->base[CS] + I8080_POP(cpustate); \
	} \
}
#define I8080_JMP(c) { \
	UINT16 a = I8080_FETCH16(cpustate); \
	if(c) { \
		cpustate->pc = cpustate->base[CS] + a; \
	} \
}
#define I8080_CALL(c) { \
	UINT16 a = I8080_FETCH16(cpustate); \
	if(c) { \
		UINT16 ip = cpustate->pc - cpustate->base[CS]; \
		ICOUNT -= 6; \
		I8080_PUSH(cpustate, ip); \
		cpustate->pc = cpustate->base[CS] + a; \
	} \
}
#define RST(n) { \
	UINT16 ip = cpustate->pc - cpustate->base[CS]; \
	I8080_PUSH(cpustate, ip); \
	cpustate->pc = cpustate->base[CS] + 8 * n; \
}

static void PREFIX80(_00h)(i8086_state *cpustate)
{
	// NOP
	ICOUNT -= I8080_CLK[0x00];
}

static void PREFIX80(_01h)(i8086_state *cpustate)
{
	// LXI B,nnnn
	I8080_BC = I8080_FETCH16(cpustate);
	ICOUNT -= I8080_CLK[0x01];
}

static void PREFIX80(_02h)(i8086_state *cpustate)
{
	// STAX B
	I8080_WM8(cpustate, I8080_BC, I8080_A);
	ICOUNT -= I8080_CLK[0x02];
}

static void PREFIX80(_03h)(i8086_state *cpustate)
{
	// INX B
	I8080_BC++;
	ICOUNT -= I8080_CLK[0x03];
}

static void PREFIX80(_04h)(i8086_state *cpustate)
{
	// INR B
	I8080_INR(I8080_B);
	ICOUNT -= I8080_CLK[0x04];
}

static void PREFIX80(_05h)(i8086_state *cpustate)
{
	// DCR B
	I8080_DCR(I8080_B);
	ICOUNT -= I8080_CLK[0x05];
}

static void PREFIX80(_06h)(i8086_state *cpustate)
{
	// MVI B,nn
	I8080_MVI(I8080_B);
	ICOUNT -= I8080_CLK[0x06];
}

static void PREFIX80(_07h)(i8086_state *cpustate)
{
	// RLC
	I8080_RLC();
	ICOUNT -= I8080_CLK[0x07];
}

static void PREFIX80(_08h)(i8086_state *cpustate)
{
	// NOP
	ICOUNT -= I8080_CLK[0x08];
}

static void PREFIX80(_09h)(i8086_state *cpustate)
{
	// DAD B
	I8080_DAD(I8080_BC);
	ICOUNT -= I8080_CLK[0x09];
}

static void PREFIX80(_0ah)(i8086_state *cpustate)
{
	// LDAX B
	I8080_A = I8080_RM8(cpustate, I8080_BC);
	ICOUNT -= I8080_CLK[0x0a];
}

static void PREFIX80(_0bh)(i8086_state *cpustate)
{
	// DCX B
	I8080_BC--;
	ICOUNT -= I8080_CLK[0x0b];
}

static void PREFIX80(_0ch)(i8086_state *cpustate)
{
	// INR C
	I8080_INR(I8080_C);
	ICOUNT -= I8080_CLK[0x0c];
}

static void PREFIX80(_0dh)(i8086_state *cpustate)
{
	// DCR C
	I8080_DCR(I8080_C);
	ICOUNT -= I8080_CLK[0x0d];
}

static void PREFIX80(_0eh)(i8086_state *cpustate)
{
	// MVI C,nn
	I8080_MVI(I8080_C);
	ICOUNT -= I8080_CLK[0x0e];
}

static void PREFIX80(_0fh)(i8086_state *cpustate)
{
	// RRC
	I8080_RRC();
	ICOUNT -= I8080_CLK[0x0f];
}

static void PREFIX80(_10h)(i8086_state *cpustate)
{
	// NOP
	ICOUNT -= I8080_CLK[0x10];
}

static void PREFIX80(_11h)(i8086_state *cpustate)
{
	// LXI D,nnnn
	I8080_DE = I8080_FETCH16(cpustate);
	ICOUNT -= I8080_CLK[0x11];
}

static void PREFIX80(_12h)(i8086_state *cpustate)
{
	// STAX D
	I8080_WM8(cpustate, I8080_DE, I8080_A);
	ICOUNT -= I8080_CLK[0x12];
}

static void PREFIX80(_13h)(i8086_state *cpustate)
{
	// INX D
	I8080_DE++;
	ICOUNT -= I8080_CLK[0x13];
}

static void PREFIX80(_14h)(i8086_state *cpustate)
{
	// INR D
	I8080_INR(I8080_D);
	ICOUNT -= I8080_CLK[0x14];
}

static void PREFIX80(_15h)(i8086_state *cpustate)
{
	// DCR D
	I8080_DCR(I8080_D);
	ICOUNT -= I8080_CLK[0x15];
}

static void PREFIX80(_16h)(i8086_state *cpustate)
{
	// MVI D,nn
	I8080_MVI(I8080_D);
	ICOUNT -= I8080_CLK[0x16];
}

static void PREFIX80(_17h)(i8086_state *cpustate)
{
	// RAL
	I8080_RAL();
	ICOUNT -= I8080_CLK[0x17];
}

static void PREFIX80(_18h)(i8086_state *cpustate)
{
	// NOP
	ICOUNT -= I8080_CLK[0x18];
}

static void PREFIX80(_19h)(i8086_state *cpustate)
{
	// DAD D
	I8080_DAD(I8080_DE);
	ICOUNT -= I8080_CLK[0x19];
}

static void PREFIX80(_1ah)(i8086_state *cpustate)
{
	// LDAX D
	I8080_A = I8080_RM8(cpustate, I8080_DE);
	ICOUNT -= I8080_CLK[0x1a];
}

static void PREFIX80(_1bh)(i8086_state *cpustate)
{
	// DCX D
	I8080_DE--;
	ICOUNT -= I8080_CLK[0x1b];
}

static void PREFIX80(_1ch)(i8086_state *cpustate)
{
	// INR E
	I8080_INR(I8080_E);
	ICOUNT -= I8080_CLK[0x1c];
}

static void PREFIX80(_1dh)(i8086_state *cpustate)
{
	// DCR E
	I8080_DCR(I8080_E);
	ICOUNT -= I8080_CLK[0x1d];
}

static void PREFIX80(_1eh)(i8086_state *cpustate)
{
	// MVI E,nn
	I8080_MVI(I8080_E);
	ICOUNT -= I8080_CLK[0x1e];
}

static void PREFIX80(_1fh)(i8086_state *cpustate)
{
	// RAR
	i8080_RAR();
	ICOUNT -= I8080_CLK[0x1f];
}

static void PREFIX80(_20h)(i8086_state *cpustate)
{
	// NOP
	ICOUNT -= I8080_CLK[0x20];
}

static void PREFIX80(_21h)(i8086_state *cpustate)
{
	// LXI H,nnnn
	I8080_HL = I8080_FETCH16(cpustate);
	ICOUNT -= I8080_CLK[0x21];
}

static void PREFIX80(_22h)(i8086_state *cpustate)
{
	// SHLD nnnn
	I8080_WM16(cpustate, I8080_FETCH16(cpustate), I8080_HL);
	ICOUNT -= I8080_CLK[0x22];
}

static void PREFIX80(_23h)(i8086_state *cpustate)
{
	// INX H
	I8080_HL++;
	ICOUNT -= I8080_CLK[0x23];
}

static void PREFIX80(_24h)(i8086_state *cpustate)
{
	// INR H
	I8080_INR(I8080_H);
	ICOUNT -= I8080_CLK[0x24];
}

static void PREFIX80(_25h)(i8086_state *cpustate)
{
	// DCR H
	I8080_DCR(I8080_H);
	ICOUNT -= I8080_CLK[0x25];
}

static void PREFIX80(_26h)(i8086_state *cpustate)
{
	// MVI H,nn
	I8080_MVI(I8080_H);
	ICOUNT -= I8080_CLK[0x26];
}

static void PREFIX80(_27h)(i8086_state *cpustate)
{
	// DAA
	UINT16 tmp16 = I8080_A;
	if(I8080_F & I8080_CF) tmp16 |= 0x100;
	if(I8080_F & I8080_HF) tmp16 |= 0x200;
	if(I8080_F & I8080_NF) tmp16 |= 0x400;
	I8080_AF = I8080_DAA[tmp16];
	I8080_F &= 0xd5;
	ICOUNT -= I8080_CLK[0x27];
}

static void PREFIX80(_28h)(i8086_state *cpustate)
{
	// NOP
	ICOUNT -= I8080_CLK[0x28];
}

static void PREFIX80(_29h)(i8086_state *cpustate)
{
	// DAD H
	I8080_DAD(I8080_HL);
	ICOUNT -= I8080_CLK[0x29];
}

static void PREFIX80(_2ah)(i8086_state *cpustate)
{
	// LHLD nnnn
	I8080_HL = I8080_RM16(cpustate, I8080_FETCH16(cpustate));
	ICOUNT -= I8080_CLK[0x2a];
}

static void PREFIX80(_2bh)(i8086_state *cpustate)
{
	// DCX H
	I8080_HL--;
	ICOUNT -= I8080_CLK[0x2b];
}

static void PREFIX80(_2ch)(i8086_state *cpustate)
{
	// INR L
	I8080_INR(I8080_L);
	ICOUNT -= I8080_CLK[0x2c];
}

static void PREFIX80(_2dh)(i8086_state *cpustate)
{
	// DCR L
	I8080_DCR(I8080_L);
	ICOUNT -= I8080_CLK[0x2d];
}

static void PREFIX80(_2eh)(i8086_state *cpustate)
{
	// MVI L,nn
	I8080_MVI(I8080_L);
	ICOUNT -= I8080_CLK[0x2e];
}

static void PREFIX80(_2fh)(i8086_state *cpustate)
{
	// CMA
	I8080_A ^= 0xff;
	ICOUNT -= I8080_CLK[0x2f];
}

static void PREFIX80(_30h)(i8086_state *cpustate)
{
	// NOP
	ICOUNT -= I8080_CLK[0x30];
}

static void PREFIX80(_31h)(i8086_state *cpustate)
{
	// LXI SP,nnnn
	I8080_SP = I8080_FETCH16(cpustate);
	ICOUNT -= I8080_CLK[0x31];
}

static void PREFIX80(_32h)(i8086_state *cpustate)
{
	// STA nnnn
	I8080_WM8(cpustate, I8080_FETCH16(cpustate), I8080_A);
	ICOUNT -= I8080_CLK[0x32];
}

static void PREFIX80(_33h)(i8086_state *cpustate)
{
	// INX SP
	I8080_SP++;
	ICOUNT -= I8080_CLK[0x33];
}

static void PREFIX80(_34h)(i8086_state *cpustate)
{
	// INR M
	UINT8 tmp8 = I8080_RM8(cpustate, I8080_HL);
	I8080_INR(tmp8);
	I8080_WM8(cpustate, I8080_HL, tmp8);
	ICOUNT -= I8080_CLK[0x34];
}

static void PREFIX80(_35h)(i8086_state *cpustate)
{
	// DCR M
	UINT8 tmp8 = I8080_RM8(cpustate, I8080_HL);
	I8080_DCR(tmp8);
	I8080_WM8(cpustate, I8080_HL, tmp8);
	ICOUNT -= I8080_CLK[0x35];
}

static void PREFIX80(_36h)(i8086_state *cpustate)
{
	// MVI M,nn
	I8080_WM8(cpustate, I8080_HL, I8080_FETCH8(cpustate));
	ICOUNT -= I8080_CLK[0x36];
}

static void PREFIX80(_37h)(i8086_state *cpustate)
{
	// STC
	I8080_F = (I8080_F & 0xfe) | I8080_CF;
	ICOUNT -= I8080_CLK[0x37];
}

static void PREFIX80(_38h)(i8086_state *cpustate)
{
	// LDES nn (NOP)
	ICOUNT -= I8080_CLK[0x38];
}

static void PREFIX80(_39h)(i8086_state *cpustate)
{
	// DAD SP
	I8080_DAD(I8080_SP);
	ICOUNT -= I8080_CLK[0x39];
}

static void PREFIX80(_3ah)(i8086_state *cpustate)
{
	// LDA nnnn
	I8080_A = I8080_RM8(cpustate, I8080_FETCH16(cpustate));
	ICOUNT -= I8080_CLK[0x3a];
}

static void PREFIX80(_3bh)(i8086_state *cpustate)
{
	// DCX SP
	I8080_SP--;
	ICOUNT -= I8080_CLK[0x3b];
}

static void PREFIX80(_3ch)(i8086_state *cpustate)
{
	// INR A
	I8080_INR(I8080_A);
	ICOUNT -= I8080_CLK[0x3c];
}

static void PREFIX80(_3dh)(i8086_state *cpustate)
{
	// DCR A
	I8080_DCR(I8080_A);
	ICOUNT -= I8080_CLK[0x3d];
}

static void PREFIX80(_3eh)(i8086_state *cpustate)
{
	// MVI A,nn
	I8080_MVI(I8080_A);
	ICOUNT -= I8080_CLK[0x3e];
}

static void PREFIX80(_3fh)(i8086_state *cpustate)
{
	// CMC
	I8080_F ^= I8080_CF;
	ICOUNT -= I8080_CLK[0x3f];
}

static void PREFIX80(_40h)(i8086_state *cpustate)
{
	// MOV B,B
	ICOUNT -= I8080_CLK[0x40];
}

static void PREFIX80(_41h)(i8086_state *cpustate)
{
	// MOV B,C
	I8080_B = I8080_C;
	ICOUNT -= I8080_CLK[0x41];
}

static void PREFIX80(_42h)(i8086_state *cpustate)
{
	// MOV B,D
	I8080_B = I8080_D;
	ICOUNT -= I8080_CLK[0x42];
}

static void PREFIX80(_43h)(i8086_state *cpustate)
{
	// MOV B,E
	I8080_B = I8080_E;
	ICOUNT -= I8080_CLK[0x43];
}

static void PREFIX80(_44h)(i8086_state *cpustate)
{
	// MOV B,H
	I8080_B = I8080_H;
	ICOUNT -= I8080_CLK[0x44];
}

static void PREFIX80(_45h)(i8086_state *cpustate)
{
	// MOV B,L
	I8080_B = I8080_L;
	ICOUNT -= I8080_CLK[0x45];
}

static void PREFIX80(_46h)(i8086_state *cpustate)
{
	// MOV B,M
	I8080_B = I8080_RM8(cpustate, I8080_HL);
	ICOUNT -= I8080_CLK[0x46];
}

static void PREFIX80(_47h)(i8086_state *cpustate)
{
	// MOV B,A
	I8080_B = I8080_A;
	ICOUNT -= I8080_CLK[0x47];
}

static void PREFIX80(_48h)(i8086_state *cpustate)
{
	// MOV C,B
	I8080_C = I8080_B;
	ICOUNT -= I8080_CLK[0x48];
}

static void PREFIX80(_49h)(i8086_state *cpustate)
{
	// MOV C,C
	ICOUNT -= I8080_CLK[0x49];
}

static void PREFIX80(_4ah)(i8086_state *cpustate)
{
	// MOV C,D
	I8080_C = I8080_D;
	ICOUNT -= I8080_CLK[0x4a];
}

static void PREFIX80(_4bh)(i8086_state *cpustate)
{
	// MOV C,E
	I8080_C = I8080_E;
	ICOUNT -= I8080_CLK[0x4b];
}

static void PREFIX80(_4ch)(i8086_state *cpustate)
{
	// MOV C,H
	I8080_C = I8080_H;
	ICOUNT -= I8080_CLK[0x4c];
}

static void PREFIX80(_4dh)(i8086_state *cpustate)
{
	// MOV C,L
	I8080_C = I8080_L;
	ICOUNT -= I8080_CLK[0x4d];
}

static void PREFIX80(_4eh)(i8086_state *cpustate)
{
	// MOV C,M
	I8080_C = I8080_RM8(cpustate, I8080_HL);
	ICOUNT -= I8080_CLK[0x4e];
}

static void PREFIX80(_4fh)(i8086_state *cpustate)
{
	// MOV C,A
	I8080_C = I8080_A;
	ICOUNT -= I8080_CLK[0x4f];
}

static void PREFIX80(_50h)(i8086_state *cpustate)
{
	// MOV D,B
	I8080_D = I8080_B;
	ICOUNT -= I8080_CLK[0x50];
}

static void PREFIX80(_51h)(i8086_state *cpustate)
{
	// MOV D,C
	I8080_D = I8080_C;
	ICOUNT -= I8080_CLK[0x51];
}

static void PREFIX80(_52h)(i8086_state *cpustate)
{
	// MOV D,D
	ICOUNT -= I8080_CLK[0x52];
}

static void PREFIX80(_53h)(i8086_state *cpustate)
{
	// MOV D,E
	I8080_D = I8080_E;
	ICOUNT -= I8080_CLK[0x53];
}

static void PREFIX80(_54h)(i8086_state *cpustate)
{
	// MOV D,H
	I8080_D = I8080_H;
	ICOUNT -= I8080_CLK[0x54];
}

static void PREFIX80(_55h)(i8086_state *cpustate)
{
	// MOV D,L
	I8080_D = I8080_L;
	ICOUNT -= I8080_CLK[0x55];
}

static void PREFIX80(_56h)(i8086_state *cpustate)
{
	// MOV D,M
	I8080_D = I8080_RM8(cpustate, I8080_HL);
	ICOUNT -= I8080_CLK[0x56];
}

static void PREFIX80(_57h)(i8086_state *cpustate)
{
	// MOV D,A
	I8080_D = I8080_A;
	ICOUNT -= I8080_CLK[0x57];
}

static void PREFIX80(_58h)(i8086_state *cpustate)
{
	// MOV E,B
	I8080_E = I8080_B;
	ICOUNT -= I8080_CLK[0x58];
}

static void PREFIX80(_59h)(i8086_state *cpustate)
{
	// MOV E,C
	I8080_E = I8080_C;
	ICOUNT -= I8080_CLK[0x59];
}

static void PREFIX80(_5ah)(i8086_state *cpustate)
{
	// MOV E,D
	I8080_E = I8080_D;
	ICOUNT -= I8080_CLK[0x5a];
}

static void PREFIX80(_5bh)(i8086_state *cpustate)
{
	// MOV E,E
	ICOUNT -= I8080_CLK[0x5b];
}

static void PREFIX80(_5ch)(i8086_state *cpustate)
{
	// MOV E,H
	I8080_E = I8080_H;
	ICOUNT -= I8080_CLK[0x5c];
}

static void PREFIX80(_5dh)(i8086_state *cpustate)
{
	// MOV E,L
	I8080_E = I8080_L;
	ICOUNT -= I8080_CLK[0x5d];
}

static void PREFIX80(_5eh)(i8086_state *cpustate)
{
	// MOV E,M
	I8080_E = I8080_RM8(cpustate, I8080_HL);
	ICOUNT -= I8080_CLK[0x5e];
}

static void PREFIX80(_5fh)(i8086_state *cpustate)
{
	// MOV E,A
	I8080_E = I8080_A;
	ICOUNT -= I8080_CLK[0x5f];
}

static void PREFIX80(_60h)(i8086_state *cpustate)
{
	// MOV H,B
	I8080_H = I8080_B;
	ICOUNT -= I8080_CLK[0x60];
}

static void PREFIX80(_61h)(i8086_state *cpustate)
{
	// MOV H,C
	I8080_H = I8080_C;
	ICOUNT -= I8080_CLK[0x61];
}

static void PREFIX80(_62h)(i8086_state *cpustate)
{
	// MOV H,D
	I8080_H = I8080_D;
	ICOUNT -= I8080_CLK[0x62];
}

static void PREFIX80(_63h)(i8086_state *cpustate)
{
	// MOV H,E
	I8080_H = I8080_E;
	ICOUNT -= I8080_CLK[0x63];
}

static void PREFIX80(_64h)(i8086_state *cpustate)
{
	// MOV H,H
	ICOUNT -= I8080_CLK[0x64];
}

static void PREFIX80(_65h)(i8086_state *cpustate)
{
	// MOV H,L
	I8080_H = I8080_L;
	ICOUNT -= I8080_CLK[0x65];
}

static void PREFIX80(_66h)(i8086_state *cpustate)
{
	// MOV H,M
	I8080_H = I8080_RM8(cpustate, I8080_HL);
	ICOUNT -= I8080_CLK[0x66];
}

static void PREFIX80(_67h)(i8086_state *cpustate)
{
	// MOV H,A
	I8080_H = I8080_A;
	ICOUNT -= I8080_CLK[0x67];
}

static void PREFIX80(_68h)(i8086_state *cpustate)
{
	// MOV L,B
	I8080_L = I8080_B;
	ICOUNT -= I8080_CLK[0x68];
}

static void PREFIX80(_69h)(i8086_state *cpustate)
{
	// MOV L,C
	I8080_L = I8080_C;
	ICOUNT -= I8080_CLK[0x69];
}

static void PREFIX80(_6ah)(i8086_state *cpustate)
{
	// MOV L,D
	I8080_L = I8080_D;
	ICOUNT -= I8080_CLK[0x6a];
}

static void PREFIX80(_6bh)(i8086_state *cpustate)
{
	// MOV L,E
	I8080_L = I8080_E;
	ICOUNT -= I8080_CLK[0x6b];
}

static void PREFIX80(_6ch)(i8086_state *cpustate)
{
	// MOV L,H
	I8080_L = I8080_H;
	ICOUNT -= I8080_CLK[0x6c];
}

static void PREFIX80(_6dh)(i8086_state *cpustate)
{
	// MOV L,L
	ICOUNT -= I8080_CLK[0x6d];
}

static void PREFIX80(_6eh)(i8086_state *cpustate)
{
	// MOV L,M
	I8080_L = I8080_RM8(cpustate, I8080_HL);
	ICOUNT -= I8080_CLK[0x6e];
}

static void PREFIX80(_6fh)(i8086_state *cpustate)
{
	// MOV L,A
	I8080_L = I8080_A;
	ICOUNT -= I8080_CLK[0x6f];
}

static void PREFIX80(_70h)(i8086_state *cpustate)
{
	// MOV M,B
	I8080_WM8(cpustate, I8080_HL, I8080_B);
	ICOUNT -= I8080_CLK[0x70];
}

static void PREFIX80(_71h)(i8086_state *cpustate)
{
	// MOV M,C
	I8080_WM8(cpustate, I8080_HL, I8080_C);
	ICOUNT -= I8080_CLK[0x71];
}

static void PREFIX80(_72h)(i8086_state *cpustate)
{
	// MOV M,D
	I8080_WM8(cpustate, I8080_HL, I8080_D);
	ICOUNT -= I8080_CLK[0x72];
}

static void PREFIX80(_73h)(i8086_state *cpustate)
{
	// MOV M,E
	I8080_WM8(cpustate, I8080_HL, I8080_E);
	ICOUNT -= I8080_CLK[0x73];
}

static void PREFIX80(_74h)(i8086_state *cpustate)
{
	// MOV M,H
	I8080_WM8(cpustate, I8080_HL, I8080_H);
	ICOUNT -= I8080_CLK[0x74];
}

static void PREFIX80(_75h)(i8086_state *cpustate)
{
	// MOV M,L
	I8080_WM8(cpustate, I8080_HL, I8080_L);
	ICOUNT -= I8080_CLK[0x75];
}

static void PREFIX80(_76h)(i8086_state *cpustate)
{
	// HLT
//	PREFIX86(_hlt)(cpustate);
	ICOUNT -= I8080_CLK[0x76];
}

static void PREFIX80(_77h)(i8086_state *cpustate)
{
	// MOV M,A
	I8080_WM8(cpustate, I8080_HL, I8080_A);
	ICOUNT -= I8080_CLK[0x77];
}

static void PREFIX80(_78h)(i8086_state *cpustate)
{
	// MOV A,B
	I8080_A = I8080_B;
	ICOUNT -= I8080_CLK[0x78];
}

static void PREFIX80(_79h)(i8086_state *cpustate)
{
	// MOV A,C
	I8080_A = I8080_C;
	ICOUNT -= I8080_CLK[0x79];
}

static void PREFIX80(_7ah)(i8086_state *cpustate)
{
	// MOV A,D
	I8080_A = I8080_D;
	ICOUNT -= I8080_CLK[0x7a];
}

static void PREFIX80(_7bh)(i8086_state *cpustate)
{
	// MOV A,E
	I8080_A = I8080_E;
	ICOUNT -= I8080_CLK[0x7b];
}

static void PREFIX80(_7ch)(i8086_state *cpustate)
{
	// MOV A,H
	I8080_A = I8080_H;
	ICOUNT -= I8080_CLK[0x7c];
}

static void PREFIX80(_7dh)(i8086_state *cpustate)
{
	// MOV A,L
	I8080_A = I8080_L;
	ICOUNT -= I8080_CLK[0x7d];
}

static void PREFIX80(_7eh)(i8086_state *cpustate)
{
	// MOV A,M
	I8080_A = I8080_RM8(cpustate, I8080_HL);
	ICOUNT -= I8080_CLK[0x7e];
}

static void PREFIX80(_7fh)(i8086_state *cpustate)
{
	// MOV A,A
	ICOUNT -= I8080_CLK[0x7f];
}

static void PREFIX80(_80h)(i8086_state *cpustate)
{
	// ADD B
	i8080_ADD(I8080_B);
	ICOUNT -= I8080_CLK[0x80];
}

static void PREFIX80(_81h)(i8086_state *cpustate)
{
	// ADD C
	i8080_ADD(I8080_C);
	ICOUNT -= I8080_CLK[0x81];
}

static void PREFIX80(_82h)(i8086_state *cpustate)
{
	// ADD D
	i8080_ADD(I8080_D);
	ICOUNT -= I8080_CLK[0x82];
}

static void PREFIX80(_83h)(i8086_state *cpustate)
{
	// ADD E
	i8080_ADD(I8080_E);
	ICOUNT -= I8080_CLK[0x83];
}

static void PREFIX80(_84h)(i8086_state *cpustate)
{
	// ADD H
	i8080_ADD(I8080_H);
	ICOUNT -= I8080_CLK[0x84];
}

static void PREFIX80(_85h)(i8086_state *cpustate)
{
	// ADD L
	i8080_ADD(I8080_L);
	ICOUNT -= I8080_CLK[0x85];
}

static void PREFIX80(_86h)(i8086_state *cpustate)
{
	// ADD M
	i8080_ADD(I8080_RM8(cpustate, I8080_HL));
	ICOUNT -= I8080_CLK[0x86];
}

static void PREFIX80(_87h)(i8086_state *cpustate)
{
	// ADD A
	i8080_ADD(I8080_A);
	ICOUNT -= I8080_CLK[0x87];
}

static void PREFIX80(_88h)(i8086_state *cpustate)
{
	// ADC B
	I8080_ADC(I8080_B);
	ICOUNT -= I8080_CLK[0x88];
}

static void PREFIX80(_89h)(i8086_state *cpustate)
{
	// ADC C
	I8080_ADC(I8080_C);
	ICOUNT -= I8080_CLK[0x89];
}

static void PREFIX80(_8ah)(i8086_state *cpustate)
{
	// ADC D
	I8080_ADC(I8080_D);
	ICOUNT -= I8080_CLK[0x8a];
}

static void PREFIX80(_8bh)(i8086_state *cpustate)
{
	// ADC E
	I8080_ADC(I8080_E);
	ICOUNT -= I8080_CLK[0x8b];
}

static void PREFIX80(_8ch)(i8086_state *cpustate)
{
	// ADC H
	I8080_ADC(I8080_H);
	ICOUNT -= I8080_CLK[0x8c];
}

static void PREFIX80(_8dh)(i8086_state *cpustate)
{
	// ADC L
	I8080_ADC(I8080_L);
	ICOUNT -= I8080_CLK[0x8d];
}

static void PREFIX80(_8eh)(i8086_state *cpustate)
{
	// ADC M
	I8080_ADC(I8080_RM8(cpustate, I8080_HL));
	ICOUNT -= I8080_CLK[0x8e];
}

static void PREFIX80(_8fh)(i8086_state *cpustate)
{
	// ADC A
	I8080_ADC(I8080_A);
	ICOUNT -= I8080_CLK[0x8f];
}

static void PREFIX80(_90h)(i8086_state *cpustate)
{
	// SUB B
	I8080_SUB(I8080_B);
	ICOUNT -= I8080_CLK[0x90];
}

static void PREFIX80(_91h)(i8086_state *cpustate)
{
	// SUB C
	I8080_SUB(I8080_C);
	ICOUNT -= I8080_CLK[0x91];
}

static void PREFIX80(_92h)(i8086_state *cpustate)
{
	// SUB D
	I8080_SUB(I8080_D);
	ICOUNT -= I8080_CLK[0x92];
}

static void PREFIX80(_93h)(i8086_state *cpustate)
{
	// SUB E
	I8080_SUB(I8080_E);
	ICOUNT -= I8080_CLK[0x93];
}

static void PREFIX80(_94h)(i8086_state *cpustate)
{
	// SUB H
	I8080_SUB(I8080_H);
	ICOUNT -= I8080_CLK[0x94];
}

static void PREFIX80(_95h)(i8086_state *cpustate)
{
	// SUB L
	I8080_SUB(I8080_L);
	ICOUNT -= I8080_CLK[0x95];
}

static void PREFIX80(_96h)(i8086_state *cpustate)
{
	// SUB M
	I8080_SUB(I8080_RM8(cpustate, I8080_HL));
	ICOUNT -= I8080_CLK[0x96];
}

static void PREFIX80(_97h)(i8086_state *cpustate)
{
	// SUB A
	I8080_SUB(I8080_A);
	ICOUNT -= I8080_CLK[0x97];
}

static void PREFIX80(_98h)(i8086_state *cpustate)
{
	// SBB B
	I8080_SBB(I8080_B);
	ICOUNT -= I8080_CLK[0x98];
}

static void PREFIX80(_99h)(i8086_state *cpustate)
{
	// SBB C
	I8080_SBB(I8080_C);
	ICOUNT -= I8080_CLK[0x99];
}

static void PREFIX80(_9ah)(i8086_state *cpustate)
{
	// SBB D
	I8080_SBB(I8080_D);
	ICOUNT -= I8080_CLK[0x9a];
}

static void PREFIX80(_9bh)(i8086_state *cpustate)
{
	// SBB E
	I8080_SBB(I8080_E);
	ICOUNT -= I8080_CLK[0x9b];
}

static void PREFIX80(_9ch)(i8086_state *cpustate)
{
	// SBB H
	I8080_SBB(I8080_H);
	ICOUNT -= I8080_CLK[0x9c];
}

static void PREFIX80(_9dh)(i8086_state *cpustate)
{
	// SBB L
	I8080_SBB(I8080_L);
	ICOUNT -= I8080_CLK[0x9d];
}

static void PREFIX80(_9eh)(i8086_state *cpustate)
{
	// SBB M
	I8080_SBB(I8080_RM8(cpustate, I8080_HL));
	ICOUNT -= I8080_CLK[0x9e];
}

static void PREFIX80(_9fh)(i8086_state *cpustate)
{
	// SBB A
	I8080_SBB(I8080_A);
	ICOUNT -= I8080_CLK[0x9f];
}

static void PREFIX80(_a0h)(i8086_state *cpustate)
{
	// ANA B
	I8080_ANA(I8080_B);
	ICOUNT -= I8080_CLK[0xa0];
}

static void PREFIX80(_a1h)(i8086_state *cpustate)
{
	// ANA C
	I8080_ANA(I8080_C);
	ICOUNT -= I8080_CLK[0xa1];
}

static void PREFIX80(_a2h)(i8086_state *cpustate)
{
	// ANA D
	I8080_ANA(I8080_D);
	ICOUNT -= I8080_CLK[0xa2];
}

static void PREFIX80(_a3h)(i8086_state *cpustate)
{
	// ANA E
	I8080_ANA(I8080_E);
	ICOUNT -= I8080_CLK[0xa3];
}

static void PREFIX80(_a4h)(i8086_state *cpustate)
{
	// ANA H
	I8080_ANA(I8080_H);
	ICOUNT -= I8080_CLK[0xa4];
}

static void PREFIX80(_a5h)(i8086_state *cpustate)
{
	// ANA L
	I8080_ANA(I8080_L);
	ICOUNT -= I8080_CLK[0xa5];
}

static void PREFIX80(_a6h)(i8086_state *cpustate)
{
	// ANA M
	I8080_ANA(I8080_RM8(cpustate, I8080_HL));
	ICOUNT -= I8080_CLK[0xa6];
}

static void PREFIX80(_a7h)(i8086_state *cpustate)
{
	// ANA A
	I8080_ANA(I8080_A);
	ICOUNT -= I8080_CLK[0xa7];
}

static void PREFIX80(_a8h)(i8086_state *cpustate)
{
	// XRA B
	I8080_XRA(I8080_B);
	ICOUNT -= I8080_CLK[0xa8];
}

static void PREFIX80(_a9h)(i8086_state *cpustate)
{
	// XRA C
	I8080_XRA(I8080_C);
	ICOUNT -= I8080_CLK[0xa9];
}

static void PREFIX80(_aah)(i8086_state *cpustate)
{
	// XRA D
	I8080_XRA(I8080_D);
	ICOUNT -= I8080_CLK[0xaa];
}

static void PREFIX80(_abh)(i8086_state *cpustate)
{
	// XRA E
	I8080_XRA(I8080_E);
	ICOUNT -= I8080_CLK[0xab];
}

static void PREFIX80(_ach)(i8086_state *cpustate)
{
	// XRA H
	I8080_XRA(I8080_H);
	ICOUNT -= I8080_CLK[0xac];
}

static void PREFIX80(_adh)(i8086_state *cpustate)
{
	// XRA L
	I8080_XRA(I8080_L);
	ICOUNT -= I8080_CLK[0xad];
}

static void PREFIX80(_aeh)(i8086_state *cpustate)
{
	// XRA M
	I8080_XRA(I8080_RM8(cpustate, I8080_HL));
	ICOUNT -= I8080_CLK[0xae];
}

static void PREFIX80(_afh)(i8086_state *cpustate)
{
	// XRA A
	I8080_XRA(I8080_A);
	ICOUNT -= I8080_CLK[0xaf];
}

static void PREFIX80(_b0h)(i8086_state *cpustate)
{
	// ORA B
	I8080_ORA(I8080_B);
	ICOUNT -= I8080_CLK[0xb0];
}

static void PREFIX80(_b1h)(i8086_state *cpustate)
{
	// ORA C
	I8080_ORA(I8080_C);
	ICOUNT -= I8080_CLK[0xb1];
}

static void PREFIX80(_b2h)(i8086_state *cpustate)
{
	// ORA D
	I8080_ORA(I8080_D);
	ICOUNT -= I8080_CLK[0xb2];
}

static void PREFIX80(_b3h)(i8086_state *cpustate)
{
	// ORA E
	I8080_ORA(I8080_E);
	ICOUNT -= I8080_CLK[0xb3];
}

static void PREFIX80(_b4h)(i8086_state *cpustate)
{
	// ORA H
	I8080_ORA(I8080_H);
	ICOUNT -= I8080_CLK[0xb4];
}

static void PREFIX80(_b5h)(i8086_state *cpustate)
{
	// ORA L
	I8080_ORA(I8080_L);
	ICOUNT -= I8080_CLK[0xb5];
}

static void PREFIX80(_b6h)(i8086_state *cpustate)
{
	// ORA M
	I8080_ORA(I8080_RM8(cpustate, I8080_HL));
	ICOUNT -= I8080_CLK[0xb6];
}

static void PREFIX80(_b7h)(i8086_state *cpustate)
{
	// ORA A
	I8080_ORA(I8080_A);
	ICOUNT -= I8080_CLK[0xb7];
}

static void PREFIX80(_b8h)(i8086_state *cpustate)
{
	// CMP B
	I8080_CMP(I8080_B);
	ICOUNT -= I8080_CLK[0xb8];
}

static void PREFIX80(_b9h)(i8086_state *cpustate)
{
	// CMP C
	I8080_CMP(I8080_C);
	ICOUNT -= I8080_CLK[0xb9];
}

static void PREFIX80(_bah)(i8086_state *cpustate)
{
	// CMP D
	I8080_CMP(I8080_D);
	ICOUNT -= I8080_CLK[0xba];
}

static void PREFIX80(_bbh)(i8086_state *cpustate)
{
	// CMP E
	I8080_CMP(I8080_E);
	ICOUNT -= I8080_CLK[0xbb];
}

static void PREFIX80(_bch)(i8086_state *cpustate)
{
	// CMP H
	I8080_CMP(I8080_H);
	ICOUNT -= I8080_CLK[0xbc];
}

static void PREFIX80(_bdh)(i8086_state *cpustate)
{
	// CMP L
	I8080_CMP(I8080_L);
	ICOUNT -= I8080_CLK[0xbd];
}

static void PREFIX80(_beh)(i8086_state *cpustate)
{
	// CMP M
	I8080_CMP(I8080_RM8(cpustate, I8080_HL));
	ICOUNT -= I8080_CLK[0xbe];
}

static void PREFIX80(_bfh)(i8086_state *cpustate)
{
	// CMP A
	I8080_CMP(I8080_A);
	ICOUNT -= I8080_CLK[0xbf];
}

static void PREFIX80(_c0h)(i8086_state *cpustate)
{
	// RNZ
	I8080_RET(!(I8080_F & I8080_ZF));
	ICOUNT -= I8080_CLK[0xc0];
}

static void PREFIX80(_c1h)(i8086_state *cpustate)
{
	// POP B
	I8080_BC = I8080_POP(cpustate);
	ICOUNT -= I8080_CLK[0xc1];
}

static void PREFIX80(_c2h)(i8086_state *cpustate)
{
	// JNZ nnnn
	I8080_JMP(!(I8080_F & I8080_ZF));
	ICOUNT -= I8080_CLK[0xc2];
}

static void PREFIX80(_c3h)(i8086_state *cpustate)
{
	// JMP nnnn
	I8080_JMP(1);
	ICOUNT -= I8080_CLK[0xc3];
}

static void PREFIX80(_c4h)(i8086_state *cpustate)
{
	// CNZ nnnn
	I8080_CALL(!(I8080_F & I8080_ZF));
	ICOUNT -= I8080_CLK[0xc4];
}

static void PREFIX80(_c5h)(i8086_state *cpustate)
{
	// PUSH B
	I8080_PUSH(cpustate, I8080_BC);
	ICOUNT -= I8080_CLK[0xc5];
}

static void PREFIX80(_c6h)(i8086_state *cpustate)
{
	// ADI nn
	UINT8 tmp8 = I8080_FETCH8(cpustate);
	i8080_ADD(tmp8);
	ICOUNT -= I8080_CLK[0xc6];
}

static void PREFIX80(_c7h)(i8086_state *cpustate)
{
	// RST 0
	RST(0);
	ICOUNT -= I8080_CLK[0xc7];
}

static void PREFIX80(_c8h)(i8086_state *cpustate)
{
	// RZ
	I8080_RET(I8080_F & I8080_ZF);
	ICOUNT -= I8080_CLK[0xc8];
}

static void PREFIX80(_c9h)(i8086_state *cpustate)
{
	// RET
	I8080_RET(1);
	ICOUNT -= I8080_CLK[0xc9];
}

static void PREFIX80(_cah)(i8086_state *cpustate)
{
	// JZ nnnn
	I8080_JMP(I8080_F & I8080_ZF);
	ICOUNT -= I8080_CLK[0xca];
}

static void PREFIX80(_cbh)(i8086_state *cpustate)
{
	// JMP nnnn
	I8080_JMP(1);
	ICOUNT -= I8080_CLK[0xcb];
}

static void PREFIX80(_cch)(i8086_state *cpustate)
{
	// CZ nnnn
	I8080_CALL(I8080_F & I8080_ZF);
	ICOUNT -= I8080_CLK[0xcc];
}

static void PREFIX80(_cdh)(i8086_state *cpustate)
{
	// CALL nnnn
	I8080_CALL(1);
	ICOUNT -= I8080_CLK[0xcd];
}

static void PREFIX80(_ceh)(i8086_state *cpustate)
{
	// ACI nn
	UINT8 tmp8 = I8080_FETCH8(cpustate);
	I8080_ADC(tmp8);
	ICOUNT -= I8080_CLK[0xce];
}

static void PREFIX80(_cfh)(i8086_state *cpustate)
{
	// RST 1
	RST(1);
	ICOUNT -= I8080_CLK[0xcf];
}

static void PREFIX80(_d0h)(i8086_state *cpustate)
{
	// RNC
	I8080_RET(!(I8080_F & I8080_CF));
	ICOUNT -= I8080_CLK[0xd0];
}

static void PREFIX80(_d1h)(i8086_state *cpustate)
{
	// POP D
	I8080_DE = I8080_POP(cpustate);
	ICOUNT -= I8080_CLK[0xd1];
}

static void PREFIX80(_d2h)(i8086_state *cpustate)
{
	// JNC nnnn
	I8080_JMP(!(I8080_F & I8080_CF));
	ICOUNT -= I8080_CLK[0xd2];
}

static void PREFIX80(_d3h)(i8086_state *cpustate)
{
	// OUT nn
	I8080_OUT8(cpustate, I8080_FETCH8(cpustate), I8080_A);
	ICOUNT -= I8080_CLK[0xd3];
}

static void PREFIX80(_d4h)(i8086_state *cpustate)
{
	// CNC nnnn
	I8080_CALL(!(I8080_F & I8080_CF));
	ICOUNT -= I8080_CLK[0xd4];
}

static void PREFIX80(_d5h)(i8086_state *cpustate)
{
	// PUSH D
	I8080_PUSH(cpustate, I8080_DE);
	ICOUNT -= I8080_CLK[0xd5];
}

static void PREFIX80(_d6h)(i8086_state *cpustate)
{
	// SUI nn
	UINT8 tmp8 = I8080_FETCH8(cpustate);
	I8080_SUB(tmp8);
	ICOUNT -= I8080_CLK[0xd6];
}

static void PREFIX80(_d7h)(i8086_state *cpustate)
{
	// RST 2
	RST(2);
	ICOUNT -= I8080_CLK[0xd7];
}

static void PREFIX80(_d8h)(i8086_state *cpustate)
{
	// RC
	I8080_RET(I8080_F & I8080_CF);
	ICOUNT -= I8080_CLK[0xd8];
}

static void PREFIX80(_d9h)(i8086_state *cpustate)
{
	// RET
	I8080_RET(1);
	ICOUNT -= I8080_CLK[0xd9];
}

static void PREFIX80(_dah)(i8086_state *cpustate)
{
	// JC nnnn
	I8080_JMP(I8080_F & I8080_CF);
	ICOUNT -= I8080_CLK[0xda];
}

static void PREFIX80(_dbh)(i8086_state *cpustate)
{
	// IN nn
	I8080_A = I8080_IN8(cpustate, I8080_FETCH8(cpustate));
	ICOUNT -= I8080_CLK[0xdb];
}

static void PREFIX80(_dch)(i8086_state *cpustate)
{
	// CC nnnn
	I8080_CALL(I8080_F & I8080_CF);
	ICOUNT -= I8080_CLK[0xdc];
}

static void PREFIX80(_ddh)(i8086_state *cpustate)
{
	// CALL nnnn
	I8080_CALL(1);
	ICOUNT -= I8080_CLK[0xdd];
}

static void PREFIX80(_deh)(i8086_state *cpustate)
{
	// SBI nn
	UINT8 tmp8 = I8080_FETCH8(cpustate);
	I8080_SBB(tmp8);
	ICOUNT -= I8080_CLK[0xde];
}

static void PREFIX80(_dfh)(i8086_state *cpustate)
{
	// RST 3
	RST(3);
	ICOUNT -= I8080_CLK[0xdf];
}

static void PREFIX80(_e0h)(i8086_state *cpustate)
{
	// RPO
	I8080_RET(!(I8080_F & I8080_VF));
	ICOUNT -= I8080_CLK[0xe0];
}

static void PREFIX80(_e1h)(i8086_state *cpustate)
{
	// POP H
	I8080_HL = I8080_POP(cpustate);
	ICOUNT -= I8080_CLK[0xe1];
}

static void PREFIX80(_e2h)(i8086_state *cpustate)
{
	// JPO nnnn
	I8080_JMP(!(I8080_F & I8080_VF));
	ICOUNT -= I8080_CLK[0xe2];
}

static void PREFIX80(_e3h)(i8086_state *cpustate)
{
	// XTHL
	UINT16 tmp16 = I8080_POP(cpustate);
	I8080_PUSH(cpustate, I8080_HL);
	I8080_HL = tmp16;
	ICOUNT -= I8080_CLK[0xe3];
}

static void PREFIX80(_e4h)(i8086_state *cpustate)
{
	// CPO nnnn
	I8080_CALL(!(I8080_F & I8080_VF));
	ICOUNT -= I8080_CLK[0xe4];
}

static void PREFIX80(_e5h)(i8086_state *cpustate)
{
	// PUSH H
	I8080_PUSH(cpustate, I8080_HL);
	ICOUNT -= I8080_CLK[0xe5];
}

static void PREFIX80(_e6h)(i8086_state *cpustate)
{
	// ANI nn
	UINT8 tmp8 = I8080_FETCH8(cpustate);
	I8080_ANA(tmp8);
	ICOUNT -= I8080_CLK[0xe6];
}

static void PREFIX80(_e7h)(i8086_state *cpustate)
{
	// RST 4
	RST(4);
	ICOUNT -= I8080_CLK[0xe7];
}

static void PREFIX80(_e8h)(i8086_state *cpustate)
{
	// RPE
	I8080_RET(I8080_F & I8080_VF);
	ICOUNT -= I8080_CLK[0xe8];
}

static void PREFIX80(_e9h)(i8086_state *cpustate)
{
	// PCHL
	cpustate->pc = cpustate->base[CS] + I8080_HL;
	ICOUNT -= I8080_CLK[0xe9];
}

static void PREFIX80(_eah)(i8086_state *cpustate)
{
	// JPE nnnn
	I8080_JMP(I8080_F & I8080_VF);
	ICOUNT -= I8080_CLK[0xea];
}

static void PREFIX80(_ebh)(i8086_state *cpustate)
{
	// XCHG
	UINT16 tmp16 = I8080_DE;
	I8080_DE = I8080_HL;
	I8080_HL = tmp16;
	ICOUNT -= I8080_CLK[0xeb];
}

static void PREFIX80(_ech)(i8086_state *cpustate)
{
	// CPE nnnn
	I8080_CALL(I8080_F & I8080_VF);
	ICOUNT -= I8080_CLK[0xec];
}

static void PREFIX80(_edh)(i8086_state *cpustate)
{
	UINT8 lo = I8080_FETCH8(cpustate);
	UINT8 hi = I8080_FETCH8(cpustate);
	UINT16 a = lo | (hi << 8);

	if(lo == 0xed) {
		// CALLN
		ICOUNT -= 38;
		PREFIX86(_interrupt)(cpustate, hi);
		/* keep MF write-enabled, so MF will be 0 and re-enter emulation mode when iret is done in native code */
		cpustate->MF = 1;
	} else if(lo == 0xfd) {
		// RETEM
		int icount = ICOUNT - 27;
		PREFIX86(_iret)(cpustate);
		cpustate->MF = 1;
		cpustate->MF_WriteDisabled = 1;
		ICOUNT = icount;
	} else {
		// CALL nnnn
		UINT16 ip = cpustate->pc - cpustate->base[CS];
		ICOUNT -= 6;
		I8080_PUSH(cpustate, ip);
		cpustate->pc = cpustate->base[CS] + a;
		ICOUNT -= I8080_CLK[0xed];
	}
}

static void PREFIX80(_eeh)(i8086_state *cpustate)
{
	// XRI nn
	UINT8 tmp8 = I8080_FETCH8(cpustate);
	I8080_XRA(tmp8);
	ICOUNT -= I8080_CLK[0xee];
}

static void PREFIX80(_efh)(i8086_state *cpustate)
{
	// RST 5
	RST(5);
	ICOUNT -= I8080_CLK[0xef];
}

static void PREFIX80(_f0h)(i8086_state *cpustate)
{
	// RP
	I8080_RET(!(I8080_F & I8080_SF));
	ICOUNT -= I8080_CLK[0xf0];
}

static void PREFIX80(_f1h)(i8086_state *cpustate)
{
	// POP A
	I8080_AF = I8080_POP(cpustate);
	ICOUNT -= I8080_CLK[0xf1];
}

static void PREFIX80(_f2h)(i8086_state *cpustate)
{
	// JP nnnn
	I8080_JMP(!(I8080_F & I8080_SF));
	ICOUNT -= I8080_CLK[0xf2];
}

static void PREFIX80(_f3h)(i8086_state *cpustate)
{
	// DI
	ICOUNT -= I8080_CLK[0xf3];
}

static void PREFIX80(_f4h)(i8086_state *cpustate)
{
	// CP nnnn
	I8080_CALL(!(I8080_F & I8080_SF));
	ICOUNT -= I8080_CLK[0xf4];
}

static void PREFIX80(_f5h)(i8086_state *cpustate)
{
	// PUSH A
	I8080_PUSH(cpustate, I8080_AF);
	ICOUNT -= I8080_CLK[0xf5];
}

static void PREFIX80(_f6h)(i8086_state *cpustate)
{
	// ORI nn
	UINT8 tmp8 = I8080_FETCH8(cpustate);
	I8080_ORA(tmp8);
	ICOUNT -= I8080_CLK[0xf6];
}

static void PREFIX80(_f7h)(i8086_state *cpustate)
{
	// RST 6
	RST(6);
	ICOUNT -= I8080_CLK[0xf7];
}

static void PREFIX80(_f8h)(i8086_state *cpustate)
{
	// RM
	I8080_RET(I8080_F & I8080_SF);
	ICOUNT -= I8080_CLK[0xf8];
}

static void PREFIX80(_f9h)(i8086_state *cpustate)
{
	// SPHL
	I8080_SP = I8080_HL;
	ICOUNT -= I8080_CLK[0xf9];
}

static void PREFIX80(_fah)(i8086_state *cpustate)
{
	// JM nnnn
	I8080_JMP(I8080_F & I8080_SF);
	ICOUNT -= I8080_CLK[0xfa];
}

static void PREFIX80(_fbh)(i8086_state *cpustate)
{
	// EI
	ICOUNT -= I8080_CLK[0xfb];
}

static void PREFIX80(_fch)(i8086_state *cpustate)
{
	// CM nnnn
	I8080_CALL(I8080_F & I8080_SF);
	ICOUNT -= I8080_CLK[0xfc];
}

static void PREFIX80(_fdh)(i8086_state *cpustate)
{
	// CALL nnnn
	I8080_CALL(1);
	ICOUNT -= I8080_CLK[0xfd];
}

static void PREFIX80(_feh)(i8086_state *cpustate)
{
	// CPI nn
	UINT8 tmp8 = I8080_FETCH8(cpustate);
	I8080_CMP(tmp8);
	ICOUNT -= I8080_CLK[0xfe];
}

static void PREFIX80(_ffh)(i8086_state *cpustate)
{
	// RST 7
	RST(7);
	ICOUNT -= I8080_CLK[0xff];
}
