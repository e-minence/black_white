//���샂�f���Ǘ��\�Ŏg�p����V���{���S
//define��`���\�ł��B���l��10�i��������16�i�\�L���L���ł��B
//�s��"//"�ȍ~�̕�������R�����g�Ɣ��f���܂��B

//DRAWTYPE �\���^�C�v�Ŏw�肷��V���{���ł��B
#define DRAWTYPE_NON 0 //�\������
#define DRAWTYPE_BLACT 1 //�r���{�[�h�A�N�^�[
#define DRAWTYPE_MDL 2 //�|���S�����f��

//PROC �����֐��Ŏw�肷��V���{���ł��B
#define PROC_NON 0 //����
#define PROC_HERO 1 //���@��p
#define PROC_BLACT 2 //�ėp�r���{�[�h�A�N�^�[
#define PROC_CYCLEHERO 3 //���]�Ԏ��@��p
#define PROC_SWIMHERO 4 //�g��莩�@��p
#define PROC_BLACT_ALANM16 5 //�ėp�r���{�[�h�A�N�^�[���16�t���[���ړ��A�j��
#define PROC_HERO_ITEMGET 6 //���@�A�C�e���Q�b�g
#define PROC_HERO_PCAZUKE 7 //���@PC�a��
#define PROC_BLACT_ONELOOP 8 //��p�^�[�����[�v�A�j��
#define PROC_PCWOMAN 9 //PC�o
#define PROC_POKE 10 //�A������|�P����
#define PROC_POKE_FLY 11 //�A������|�P���� ���V���o
#define PROC_MODEL 12 //�|���S�����f��
#define PROC_FISHINGHERO 13 //�ނ莩�@��p
#define PROC_YUREHERO 14 //�����h�ꎩ�@
#define PROC_BLACT_ALANM32 15 //�ėp�r���{�[�h�A�N�^�[���32�t���[���ړ��A�j��

//FOOTMARK ���Վ�ނŎw�肷��V���{���ł��B
#define FOOTMARK_NON 0 //���Ֆ���
#define FOOTMARK_HUMAN 1 //�񑫂̑�
#define FOOTMARK_CYCLE 2 //���]�ԑ���

//MDLSIZE ���f���T�C�Y�Ŏw�肷��V���{���ł��B
#define MDLSIZE_32x32 0 //32x32
#define MDLSIZE_16x16 1 //16x16
#define MDLSIZE_64x64 2 //64x64

//TEXSIZE �e�N�X�`���T�C�Y�Ŏw�肷��V���{���ł��B
#define TEXSIZE_8x8 0
#define TEXSIZE_8x16 1
#define TEXSIZE_8x32 2
#define TEXSIZE_8x64 3
#define TEXSIZE_8x128 4
#define TEXSIZE_8x256 5
#define TEXSIZE_8x512 6
#define TEXSIZE_8x1024 7
#define TEXSIZE_16x8 8
#define TEXSIZE_16x16 9
#define TEXSIZE_16x32 10
#define TEXSIZE_16x64 11
#define TEXSIZE_16x128 12
#define TEXSIZE_16x256 13
#define TEXSIZE_16x512 14
#define TEXSIZE_16x1024 15
#define TEXSIZE_32x8 16
#define TEXSIZE_32x16 17
#define TEXSIZE_32x32 18
#define TEXSIZE_32x64 19
#define TEXSIZE_32x128 20
#define TEXSIZE_32x256 21
#define TEXSIZE_32x512 22
#define TEXSIZE_32x1024 23
#define TEXSIZE_64x8 24 
#define TEXSIZE_64x16 25
#define TEXSIZE_64x32 26
#define TEXSIZE_64x64 27
#define TEXSIZE_64x128 28
#define TEXSIZE_64x256 29
#define TEXSIZE_64x512 30
#define TEXSIZE_64x1024 31
#define TEXSIZE_128x8 32
#define TEXSIZE_128x16 33
#define TEXSIZE_128x32 34
#define TEXSIZE_128x64 35
#define TEXSIZE_128x128 36
#define TEXSIZE_128x256 37
#define TEXSIZE_128x512 38
#define TEXSIZE_128x1024 39
#define TEXSIZE_256x8 40
#define TEXSIZE_256x16 41
#define TEXSIZE_256x32 42
#define TEXSIZE_256x64 43
#define TEXSIZE_256x128 44
#define TEXSIZE_256x256 45
#define TEXSIZE_256x512 46
#define TEXSIZE_256x1024 47
#define TEXSIZE_512x8 48
#define TEXSIZE_512x16 49
#define TEXSIZE_512x32 50
#define TEXSIZE_512x64 51
#define TEXSIZE_512x128 52
#define TEXSIZE_512x256 53
#define TEXSIZE_512x512 54
#define TEXSIZE_512x1024 55
#define TEXSIZE_1024x8 56
#define TEXSIZE_1024x16 57
#define TEXSIZE_1024x32 58
#define TEXSIZE_1024x64 59
#define TEXSIZE_1024x128 60
#define TEXSIZE_1024x256 61
#define TEXSIZE_1024x512 62
#define TEXSIZE_1024x1024 63

//ANMID �A�j��ID�Ŏw�肷��V���{���ł��B
#define ANMID_NON 0 //�A�j������
#define ANMID_HERO 1 //���@��p
#define ANMID_BLACT 2 //�ėp�r���{�[�h�A�N�^�[�p ��
#define ANMID_CYCLEHERO_OLD 3 //���]�Ԏ��@��p ��
#define ANMID_SWIMHERO 4 //�g��莩�@��p
#define ANMID_BLACT_FLIP 5 //�l���ėp ���]�g�p
#define ANMID_BLACT_NONFLIP 6 //�l���ėp ���]���g�p
#define ANMID_CYCLEHERO 7 //���]�Ԏ��@��p
#define ANMID_ONECELL 8 //��p�^�[���G��p
#define ANMID_ITEMGET 9 //���@�A�C�e���Q�b�g
#define ANMID_PCAZUKE 10 //���@�|�P�Z���a��
#define ANMID_SAVE 11 //���@���|�[�g
#define ANMID_PCWOMAN 12 //PC�o
#define ANMID_POKE_FLIP 13 //�|�P�����@���]�g�p
#define ANMID_POKE_NONFLIP 14 //�|�P�����@���]���g�p
#define ANMID_FISHINGHERO 15 //���@�ނ�
#define ANMID_CUTINHERO 16 //�J�b�g�C�����@
#define ANMID_YUREHERO 17 //�����h�ꎩ�@

//SEX ���ʂŎw�肷��V���{���ł��B
#define SEX_MALE 0
#define SEX_FEMALE 1
#define SEX_NON 2
