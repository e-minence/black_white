������������������������������������������������������������������������������
��                                                                          ��
��  Readme-NitroSDK-4_2-patch-plus3-081006.txt                              ��
��                                                                          ��
��  Plus Patch for NITRO-SDK 4.2                                            ��
��                                                                          ��
��  Oct. 06, 2008                                                           ��
��                                                                          ��
������������������������������������������������������������������������������


�y �͂��߂� �z

    ���̃p�b�`�́ANITRO-SDK 4.2 �̃����[�X�ȍ~�ɔ������ꂽ�s����C������
    ���߂̂��̂ł��B�C���X�g�[�����邽�߂ɂ́A��������Ă���t�@�C����
    NITRO-SDK 4.2 (071210)���C���X�g�[������Ă���f�B���N�g���ɏ㏑���R�s�[
    ���Ă��������B

�y ���� �z

�@�@�@NitroSDK �̃��C�u�������r���h������������ꍇ�A�{�p�b�`�𓖂Ă���������
�@�@�L���b�V���̉e���Ńv���O�������r���h����Ƃ��ɃG���[����������\����
�@�@����܂��B
�@�@�@�����r���h���ɃG���[�����������ꍇ�́ANitroSDK �̃��[�g�f�B���N�g����
�@�@�umake clobber�v�����s���邱�ƂŃL���b�V�����폜����܂��B
�@�@���̌�A���� NitroSDK �̃��[�g�f�B���N�g���Łumake�v���邱�ƂŁA
�@�@���̏Ǐ�͉��P����܂��B

�y �C�����e �z 
    

    4_2-patch-plus3-081006 �ŏC�����ꂽ���e
    --------------------------------------------------------------------------

    �E(MIC) �}�C�N DC �I�t�Z�b�g�̎����␳�����̒ǉ�

    �}�C�N�f�o�C�X��p���ăT���v�����O���s���ہA�������̃I�t�Z�b�g�l��
    �n�[�h�E�F�A�̌̍��ɂ��΂��������܂��B
    ���̖��ւ̑Ώ����ȕւɂ��邽�߂ɁA�}�C�N���색�C�u������
    �I�t�Z�b�g�l�̎����␳�@�\��ǉ����܂����B
    �}�C�N���C�u������p���ăT���v�����O���s���ƁA
    �������ɂ��傤�ǒ��Ԃ̃f�[�^�l�ɂȂ�悤�ɃT���v�����O�f�[�^��
    �����I�ɕ␳�����悤�ɂȂ�܂��B

    �E���ӎ���

    �{�p�b�`�p�b�P�[�W�ɂ� CARD ���C�u�����V�K�o�b�N�A�b�v�f�o�C�X��
    �]���p�p�����[�^���ǉ�����Ă��܂����A���ɓ���ɕύX�͂���܂���B

    4_2-patch-plus2-080513 �ŏC�����ꂽ���e
    --------------------------------------------------------------------------

    �E(OS) OS_ResetSystem �֐��̏C��

    OS_ResetSystem �֐�������̏����� ARM7 �� ARM9 �� PXI �ʐM���s���Ă���ꍇ�A
    �����܂�ɒ�~����Ƃ����s�������܂������A������C�����܂����B

    4_2-patch-plus-080118 �ŏC�����ꂽ���e
    --------------------------------------------------------------------------

    �E(OS) OS ���C�u�����̃��t�@�����X�Ɋ܂܂��X���b�h�T�v�̍X�V

    OS ���C�u�����̃��t�@�����X���̃X���b�h�T�v�ɁA
    �u�X���b�h��Ԃ��O������ω�������ꍇ�̒��Ӂv��ǋL���܂����B

    �E(TP) TP_WaitRawResult�ATP_WaitCalibratedResult �֐��̏C��

    TP_WaitRawResult�ATP_WaitCalibratedResult �֐��̓T���v�����O����̊�����҂���
    �f�[�^���擾����֐��ł����A���ɃT���v�����O���삪�������Ă���ꍇ�Ɍ�����
    �����݂��֎~������Ԃł̌Ăяo�����T�|�[�g���Ă��܂��B
    �������f�o�b�O�r���h�ł͈Ӑ}�ʂ蓮�삵�Ȃ��Ƃ����s�������܂����B
    ���̕s����C�����܂����B

    �E(WM) WM_EnableForListening �֐��� blink �����̉��߂̏C��

    WM_EnableForListening �֐��� blink �����̐^�U�l���t�ɉ��߂���s���
    ����܂����̂ŁA������C�����܂����B

    �E(WM) ���M�T�C�Y�`�F�b�N�̕s��C��

    �uNote 50-16: (WM) �q�@���M�e�ʂ̐����̊ɘa�v�̕ύX�ɋN�����āA
    MP �ʐM�Őe�@���M�e�ʂ�510�o�C�g�ȏ�ɐݒ肷��A��������
    �q�@���M�e�ʂ�512�o�C�g�ɐݒ肵���ꍇ�ɁA�ꍇ�ɂ���ăf�[�^��
    ���M�ł��Ȃ��Ȃ�s����������Ă��܂��Ă��܂����B
    ������C�����܂����B


�y �t�@�C�����X�g �z

���\�[�X�t�@�C��
	/NitroSDK/build/demos/spi/pm-1/src/main.c
	/NitroSDK/build/libraries/card/common/src/card_spi.c
	/NitroSDK/build/libraries/mi/common/src/mi_dma.c
	/NitroSDK/build/libraries/os/common/src/os_reset.c
	/NitroSDK/build/libraries/os/common/src/os_thread.c
	/NitroSDK/build/libraries/spi/ARM9/src/pm.c
	/NitroSDK/build/libraries/spi/ARM9/src/tp.c
	/NitroSDK/build/libraries/wm/ARM9/src/wm_standard.c

���w�b�_�t�@�C��
	/NitroSDK/include/nitro/mi/dma.h
	/NitroSDK/include/nitro/os/common/interrupt.h
	/NitroSDK/include/nitro/spi/ARM9/pm.h
	/NitroSDK/include/nitro/spi/common/pm_common.h
	/NitroSDK/include/nitro/spi/common/type.h
	/NitroSDK/include/nitro/version.h

�����C�u�����t�@�C��
	/NitroSDK/lib/ARM9-TS/Debug/crt0.o
	/NitroSDK/lib/ARM9-TS/Debug/libcard.a
	/NitroSDK/lib/ARM9-TS/Debug/libcard.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libctrdg.a
	/NitroSDK/lib/ARM9-TS/Debug/libctrdg.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libmi.a
	/NitroSDK/lib/ARM9-TS/Debug/libmi.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libos.a
	/NitroSDK/lib/ARM9-TS/Debug/libos.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libos_TS0.a
	/NitroSDK/lib/ARM9-TS/Debug/libos_TS0.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libos_TS100.a
	/NitroSDK/lib/ARM9-TS/Debug/libos_TS100.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libos_TS200.a
	/NitroSDK/lib/ARM9-TS/Debug/libos_TS200.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi_TS0.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi_TS0.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi_TS100.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi_TS100.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi_TS200.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi_TS200.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libwm.a
	/NitroSDK/lib/ARM9-TS/Debug/libwm.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/crt0.o
	/NitroSDK/lib/ARM9-TS/Release/libcard.a
	/NitroSDK/lib/ARM9-TS/Release/libcard.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libctrdg.a
	/NitroSDK/lib/ARM9-TS/Release/libctrdg.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libmi.a
	/NitroSDK/lib/ARM9-TS/Release/libmi.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libos.a
	/NitroSDK/lib/ARM9-TS/Release/libos.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libos_TS0.a
	/NitroSDK/lib/ARM9-TS/Release/libos_TS0.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libos_TS100.a
	/NitroSDK/lib/ARM9-TS/Release/libos_TS100.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libos_TS200.a
	/NitroSDK/lib/ARM9-TS/Release/libos_TS200.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libspi.a
	/NitroSDK/lib/ARM9-TS/Release/libspi.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libspi_TS0.a
	/NitroSDK/lib/ARM9-TS/Release/libspi_TS0.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libspi_TS100.a
	/NitroSDK/lib/ARM9-TS/Release/libspi_TS100.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libspi_TS200.a
	/NitroSDK/lib/ARM9-TS/Release/libspi_TS200.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libwm.a
	/NitroSDK/lib/ARM9-TS/Release/libwm.thumb.a
	/NitroSDK/lib/ARM9-TS/Rom/crt0.o
	/NitroSDK/lib/ARM9-TS/Rom/libcard.a
	/NitroSDK/lib/ARM9-TS/Rom/libcard.thumb.a
	/NitroSDK/lib/ARM9-TS/Rom/libctrdg.a
	/NitroSDK/lib/ARM9-TS/Rom/libctrdg.thumb.a
	/NitroSDK/lib/ARM9-TS/Rom/libmi.a
	/NitroSDK/lib/ARM9-TS/Rom/libmi.thumb.a
	/NitroSDK/lib/ARM9-TS/Rom/libos.a
	/NitroSDK/lib/ARM9-TS/Rom/libos.thumb.a
	/NitroSDK/lib/ARM9-TS/Rom/libos_TS0.a
	/NitroSDK/lib/ARM9-TS/Rom/libos_TS0.thumb.a
	/NitroSDK/lib/ARM9-TS/Rom/libos_TS100.a
	/NitroSDK/lib/ARM9-TS/Rom/libos_TS100.thumb.a
	/NitroSDK/lib/ARM9-TS/Rom/libos_TS200.a
	/NitroSDK/lib/ARM9-TS/Rom/libos_TS200.thumb.a
	/NitroSDK/lib/ARM9-TS/Rom/libspi.a
	/NitroSDK/lib/ARM9-TS/Rom/libspi.thumb.a
	/NitroSDK/lib/ARM9-TS/Rom/libspi_TS0.a
	/NitroSDK/lib/ARM9-TS/Rom/libspi_TS0.thumb.a
	/NitroSDK/lib/ARM9-TS/Rom/libspi_TS100.a
	/NitroSDK/lib/ARM9-TS/Rom/libspi_TS100.thumb.a
	/NitroSDK/lib/ARM9-TS/Rom/libspi_TS200.a
	/NitroSDK/lib/ARM9-TS/Rom/libspi_TS200.thumb.a
	/NitroSDK/lib/ARM9-TS/Rom/libwm.a
	/NitroSDK/lib/ARM9-TS/Rom/libwm.thumb.a

���R���|�[�l���g�t�@�C��
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS0.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS100.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS200.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS0.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS100.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS200.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub_TS0.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub_TS100.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub_TS200.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS0.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS100.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS200.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS0.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS100.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS200.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub_TS0.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub_TS100.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub_TS200.nef
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub.nef
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS0.nef
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS100.nef
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS200.nef
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub.nef
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS0.nef
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS100.nef
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS200.nef
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub.nef
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub_TS0.nef
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub_TS100.nef
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub_TS200.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS0.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS100.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS200.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS0.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS100.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS200.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub_TS0.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub_TS100.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub_TS200.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS0.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS0_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS100.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS100_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS200.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS200_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS0.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS0_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS100.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS100_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS200.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS200_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub_TS0.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub_TS0_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub_TS100.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub_TS100_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub_TS200.sbin
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub_TS200_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS0.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS0_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS100.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS100_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS200.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS200_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS0.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS0_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS100.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS100_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS200.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS200_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub_TS0.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub_TS0_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub_TS100.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub_TS100_defs.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub_TS200.sbin
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub_TS200_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS0.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS0_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS100.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS100_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS200.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS200_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS0.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS0_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS100.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS100_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS200.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS200_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub_TS0.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub_TS0_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub_TS100.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub_TS100_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub_TS200.sbin
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub_TS200_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS0.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS0_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS100.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS100_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS200.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS200_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS0.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS0_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS100.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS100_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS200.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS200_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub_TS0.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub_TS0_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub_TS100.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub_TS100_defs.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub_TS200.sbin
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub_TS200_defs.sbin

���h�L�������g�E���̑�
	/NitroSDK/man/ja_JP/os/thread/about_Thread.html

�ȏ�
