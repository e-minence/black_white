#include "message_dl.naix"
#define MB_ERROR_MSG_FILE (NARC_message_dl_net_err_dat)
#define MB_BACKUP_ERROR_MSG_FILE (NARC_message_dl_backup_err_dat)

#include "../prog/src/system/net_err.c"
#include "../prog/src/system/save_error_warning.c"

//net_err�̖��O�̂܂܂��ƁAnarc�t�@�C����*.o�t�@�C���Ƃ���ROM��
//�u�����߁Anet_err.o��2���݂��Ă��܂��Ƃ����󋵂ɂȂ�B
//�Ȃ̂ŁA�\�[�X�Ń\�[�X��include���ăt�@�C������ς��邱�ƂőΏ����Ă���E�E�E