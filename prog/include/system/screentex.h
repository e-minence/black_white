#pragma once

typedef enum {
	SCRNTEX_VRAM_A = 0,
	SCRNTEX_VRAM_B,
	SCRNTEX_VRAM_C,
	SCRNTEX_VRAM_D,
}SCRNTEX_VRAM;

GFL_G3D_RES*	GFL_SCRNTEX_CreateG3DresTex(
	HEAPID				heapID,			// ���\�[�X�����Ώۃq�[�vID 
	SCRNTEX_VRAM	texVram			// �e�N�X�`���Ƃ��Ďg�p����Ώ�VRAM(�v0x20000: 256*256*2)
);
void					GFL_SCRNTEX_Load(
	SCRNTEX_VRAM	capVram,		// �L���v�`���[�Ŏg�p����Ώ�VRAM(�v0x20000: ���ۂ�256*192*2)
	SCRNTEX_VRAM	texVram			// �L���v�`���[�摜�]��VRAM(�]���s�v�̏ꍇ�� capVram�Ɠ��l�ɂ���)
);

