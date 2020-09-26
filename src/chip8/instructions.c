#include "chip8/instructions.h"

void instruct_op(CHIP8* chip8, uint16_t opcode) {
  debug_print("opcode: 0x%04x", opcode);

  uint16_t x;
  uint16_t y;
  uint16_t n;
  uint16_t nn;
  uint16_t nnn;
  uint16_t i;
  int key_status;

  x = (opcode & 0x0f00) >> 8;
  y = (opcode & 0x00f0) >> 4;
  n = opcode & 0x000f;
  nn = opcode & 0x00ff;
  nnn = opcode & 0x0fff;

  switch (opcode & 0xf000) {
    case 0x0000:
      switch (opcode & 0x00ff) {
        case 0x00e0:
          // CLS 00E0	Display	disp_clear()
          display_clear(chip8);
          chip8->cpu.pc += 2;
          break;
        case 0x00ee:
          // RET 00EE	Flow	return;
          chip8->cpu.sp--;
          chip8->cpu.pc = chip8->stack[chip8->cpu.sp];
          chip8->cpu.pc += 2;
          break;
        default:
          // SYS 0NNN	Call
          // ignore
          break;
      }
      break;
    case 0x1000:
      // JP 1NNN	Flow	goto NNN;
      chip8->cpu.pc = nnn;
      break;
    case 0x2000:
      // CALL 2NNN	Flow	*(0xNNN)()
      chip8->stack[chip8->cpu.sp] = chip8->cpu.pc;
      chip8->cpu.sp++;
      chip8->cpu.pc = nnn;
      break;
    case 0x3000:
      // SE 3XNN	Cond	if(Vx==NN)
      if (chip8->cpu.v[x] == nn) chip8->cpu.pc += 2;
      chip8->cpu.pc += 2;
      break;
    case 0x4000:
      // SNE 4XNN	Cond	if(Vx!=NN)
      if (chip8->cpu.v[x] != nn) chip8->cpu.pc += 2;
      chip8->cpu.pc += 2;
      break;
    case 0x5000:
      // SE 5XY0	Cond	if(Vx==Vy)
      if (chip8->cpu.v[x] == chip8->cpu.v[y]) chip8->cpu.pc += 2;
      chip8->cpu.pc += 2;
      break;
    case 0x6000:
      // LD 6XNN	Const	Vx = NN
      chip8->cpu.v[x] = nn;
      chip8->cpu.pc += 2;
      break;
    case 0x7000:
      // ADD 7XNN	Const	Vx += NN
      chip8->cpu.v[x] += nn;
      chip8->cpu.pc += 2;
      break;
    case 0x8000:
      switch (opcode & 0x00f) {
        case 0x0000:
          // LD 8XY0	Assign	Vx=Vy
          chip8->cpu.v[x] = chip8->cpu.v[y];
          break;
        case 0x0001:
          // OR 8XY1	BitOp	Vx=Vx|Vy
          chip8->cpu.v[x] = chip8->cpu.v[x] | chip8->cpu.v[y];
          break;
        case 0x0002:
          // AND 8XY2	BitOp	Vx=Vx&Vy
          chip8->cpu.v[x] = chip8->cpu.v[x] & chip8->cpu.v[y];
          break;
        case 0x0003:
          // XOR 8XY3	BitOp	Vx=Vx^Vy
          chip8->cpu.v[x] = chip8->cpu.v[x] ^ chip8->cpu.v[y];
          break;
        case 0x0004:
          // ADD 8XY4	Math	Vx += Vy
          chip8->cpu.v[0xF] = (chip8->cpu.v[x] > 255);
          chip8->cpu.v[x] += chip8->cpu.v[y];
          break;
        case 0x0005:
          // SUB 8XY5	Math	Vx -= Vy
          chip8->cpu.v[0xF] = (chip8->cpu.v[x] > chip8->cpu.v[y]);
          chip8->cpu.v[x] -= chip8->cpu.v[y];
          break;
        case 0x0006:
          // SHR 8XY6	BitOp	Vx>>=1
          chip8->cpu.v[0xF] = (chip8->cpu.v[x] & 0x0001);
          chip8->cpu.v[x] >>= 1;
          break;
        case 0x0007:
          // SUBN 8XY7	Math	Vx=Vy-Vx
          chip8->cpu.v[0xF] = (chip8->cpu.v[y] > chip8->cpu.v[x]);
          chip8->cpu.v[x] = chip8->cpu.v[y] - chip8->cpu.v[x];
          break;
        case 0x000E:
          // SHL 8XYE	BitOp	Vx<<=1
          chip8->cpu.v[0xF] = (chip8->cpu.v[x] & 0x8000);
          chip8->cpu.v[x] <<= 1;
          break;
        default:
          error("Invalid operation: %x.", chip8->cpu.v[opcode & 0xf00f]);
          break;
      }
      chip8->cpu.pc += 2;
      break;
    case 0x9000:
      // SNE 9XY0	Cond	if(Vx!=Vy)
      if (chip8->cpu.v[x] != chip8->cpu.v[y]) chip8->cpu.pc += 2;
      chip8->cpu.pc += 2;
      break;
    case 0xa000:
      // LD ANNN	MEM	I = NNN
      chip8->cpu.i = nnn;
      chip8->cpu.pc += 2;
      break;
    case 0xb000:
      // JP BNNN	Flow	PC=V0+NNN
      chip8->cpu.pc = chip8->cpu.v[0] + nnn;
      break;
    case 0xc000:
      // RND CXNN	Rand	Vx=rand()&NN
      chip8->cpu.v[x] = (rand() % 256) & nn;
      chip8->cpu.pc += 2;
      break;
    case 0xd000:
      // DRW DXYN	Disp	draw(Vx,Vy,N)
      display_draw(chip8, chip8->cpu.v[x], chip8->cpu.v[y], n);
      chip8->cpu.pc += 2;
      break;
    case 0xe000:
      switch (opcode & 0x00ff) {
        case 0x009e:
          // SKP EX9E	KeyOp	if(key()==Vx)
          if (chip8->key_inputs[chip8->cpu.v[x]] == 1) chip8->cpu.pc += 2;
          break;
        case 0x00A1:
          // SKNP EXA1	KeyOp	if(key()!=Vx)
          if (chip8->key_inputs[chip8->cpu.v[x]] == 0) chip8->cpu.pc += 2;
          break;
        default:
          error("Invalid operation: %x.", chip8->cpu.v[opcode & 0xf00f]);
          break;
      }
      chip8->cpu.pc += 2;
      break;
    case 0xf000:
      switch (opcode & 0x00ff) {
        case 0x0007:
          // LD FX07	Timer	Vx = get_delay()
          chip8->cpu.v[x] = chip8->cpu.delay_timer;
          chip8->cpu.pc += 2;
          break;
        case 0x000A:
          // LD FX0A	KeyOp	Vx = get_key()
          key_status = -1;
          for (i = 0; i < KEY_COUNT; i++) {
            if (chip8->key_inputs[i]) {
              chip8->cpu.v[x] = i;
              key_status = i;
            }
          }

          if (key_status != -1) chip8->cpu.pc += 2;
          break;
        case 0x0015:
          // LD FX15	Timer	delay_timer(Vx)
          chip8->cpu.delay_timer = chip8->cpu.v[x];
          chip8->cpu.pc += 2;
          break;
        case 0x0018:
          // LD FX18	Sound	sound_timer(Vx)
          chip8->cpu.sound_timer = chip8->cpu.v[x];
          chip8->cpu.pc += 2;
          break;
        case 0x001E:
          // ADD FX1E	MEM	I += Vx
          chip8->cpu.v[0xF] = (chip8->cpu.i + chip8->cpu.v[x] > 0xfff) ? 1 : 0;
          chip8->cpu.i += chip8->cpu.v[x];
          chip8->cpu.pc += 2;
          break;
        case 0x0029:
          // LD FX29	MEM	I=sprite_addr[Vx]
          chip8->cpu.i = FONT_ADDRESS + (FONT_BYTES_PER_CHAR * chip8->cpu.v[x]);
          chip8->cpu.pc += 2;
          break;
        case 0x0033:
          // LD FX33	BCD	set_BCD(Vx)
          // *(I+0)=BCD(3);
          // *(I+1)=BCD(2);
          // *(I+2)=BCD(1);
          chip8->memory[chip8->cpu.i] = (chip8->cpu.v[x] / 100) % 10;
          chip8->memory[chip8->cpu.i + 1] = (chip8->cpu.v[x] / 10) % 10;
          chip8->memory[chip8->cpu.i + 2] = chip8->cpu.v[x] % 10;
          chip8->cpu.pc += 2;
          break;
        case 0x0055:
          // LD FX55	MEM	reg_dump(Vx,&I)
          for (i = 0; i <= x; i++) {
            chip8->memory[chip8->cpu.i + i] = chip8->cpu.v[i];
          }
          chip8->cpu.i += x + 1;
          chip8->cpu.pc += 2;
          break;
        case 0x0065:
          // LD FX65	MEM	reg_load(Vx,&I)
          for (i = 0; i <= x; i++) {
            chip8->cpu.v[i] = chip8->memory[chip8->cpu.i + i];
          }
          chip8->cpu.i += x + 1;
          chip8->cpu.pc += 2;
          break;
        default:
          error("Unsupported operation: 0x%x.", opcode);
          break;
      }
      break;
    default:
      error("Unknown operation code.");
  }
}
