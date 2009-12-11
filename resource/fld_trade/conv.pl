#
#			�����|�P�����f�[�^�@�R���o�[�^�[
#			�f�[�^��13�Œ�
#

@FLD_TRADE_FILE = undef;	#�f�[�^�t�@��
@MONSNO_H_FILE	= undef;	#�����X�^�[�i���o�[�f�t�@�C��
@TOKUSYU_H_FILE	= undef;	#����i���o�[�f�t�@�C��
@SEIKAKU_H_FILE = undef;	#���i�i���o�[�f�t�@�C��
@ITEMSYM_H_FILE = undef;	#�A�C�e���i���o�[�f�t�@�C��
@PMVER_H_FILE = undef;		#���R�[�h�i���o�[�f�t�@�C��
@OUTPUT0_FILE = undef;		#�����o���f�[�^
@OUTPUT1_FILE = undef;		#�����o���f�[�^
@OUTPUT2_FILE = undef;		#�����o���f�[�^
@OUTPUT3_FILE = undef;		#�����o���f�[�^
@OUTPUT4_FILE = undef;		#�����o���f�[�^
@OUTPUT5_FILE = undef;		#�����o���f�[�^
@OUTPUT6_FILE = undef;		#�����o���f�[�^
@OUTPUT7_FILE = undef;		#�����o���f�[�^
@OUTPUT7_FILE = undef;		#�����o���f�[�^
@OUTPUT8_FILE = undef;		#�����o���f�[�^
@OUTPUT9_FILE = undef;		#�����o���f�[�^
@OUTPUT10_FILE = undef;		#�����o���f�[�^
@OUTPUT11_FILE = undef;		#�����o���f�[�^
@OUTPUT12_FILE = undef;		#�����o���f�[�^

$FILE_NUM	= 12

# ��C���f�b�N�X
@ROW_INDEX=( 1, 4, 7, 10, 13, 16, 19, 22, 25, 28, 31, 34, 37,);

# �s�C���f�b�N�X
$LINE_MONSNO        = 1;   # �����X�^�[�i���o�[ 
$LINE_FORM          = 3;   # �t�H�[���i���o�[
$LINE_LEVEL         = 4;   # ���x��
$LINE_HP_RND        = 5;   # HP����
$LINE_AT_RND        = 6;   # �U������
$LINE_DF_RND        = 7;   # �h�䗐��
$LINE_AG_RND        = 8;   # ��������
$LINE_SA_RND        = 9;   # ���U����
$LINE_SD_RND        = 10;  # ���h����
$LINE_TOKUSEI       = 11;  # ����\��
$LINE_SEIKAKU       = 12;  # ���i
$LINE_SEX           = 13;  # ����
$LINE_ID            = 14;  # ID
$LINE_STYPE         = 15;  # �������悳
$LINE_BEAUTIFUL     = 16;  # ��������
$LINE_CUTE          = 17;  # ���킢��
$LINE_CLEVER        = 18;  # ��������
$LINE_STRONG        = 19;  # �����܂���
$LINE_ITEM          = 20;  # �A�C�e��
$LINE_OYA_SEX       = 22;  # �e����
$LINE_FUR           = 23;  # �щ�
$LINE_COUNTRY       = 24;  # �e�̍��R�[�h
$LINE_CHANGE_MONSNO = 25;  # �������郂���X�^�[�i���o�[
$LINE_CHANGE_SEX    = 26;  # ��������|�P�����̐���

#############################################################
#
#	���C������
#
#############################################################

#�t�@�C���ǂݍ���
&file_open();

$line_idx = 0;  # �s��
$data_idx = 0;  # �f�[�^��
foreach $one ( @FLD_TRADE_FILE ){

	#0���ڂ̓_�~�[�f�[�^
	if( $line_idx > 0 ){
		@line = split( "\t", $one );

		if( $line_idx == $LINE_MONSNO ){		#�����X�^�[�i���o�[
			$OUTPUT0_FILE[@data_idx]  = &get_monsno($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[@data_idx]  = &get_monsno($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[@data_idx]  = &get_monsno($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[@data_idx]  = &get_monsno($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[@data_idx]  = &get_monsno($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[@data_idx]  = &get_monsno($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[@data_idx]  = &get_monsno($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[@data_idx]  = &get_monsno($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[@data_idx]  = &get_monsno($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[@data_idx]  = &get_monsno($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[@data_idx] = &get_monsno($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[@data_idx] = &get_monsno($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[@data_idx] = &get_monsno($line[$ROW_INDEX[12]] );
      $data_idx++; 
    }elsif( $line_idx == $LINE_FORM ){  #�t�H�[���i���o�[
			$OUTPUT0_FILE[@data_idx]  = &get_formno($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[@data_idx]  = &get_formno($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[@data_idx]  = &get_formno($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[@data_idx]  = &get_formno($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[@data_idx]  = &get_formno($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[@data_idx]  = &get_formno($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[@data_idx]  = &get_formno($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[@data_idx]  = &get_formno($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[@data_idx]  = &get_formno($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[@data_idx]  = &get_formno($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[@data_idx] = &get_formno($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[@data_idx] = &get_formno($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[@data_idx] = &get_formno($line[$ROW_INDEX[12]] );
      $data_idx++;
    }elsif( $line_idx == $LINE_LEVEL ){  #���x��
			$OUTPUT0_FILE[@data_idx]  = ($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[@data_idx]  = ($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[@data_idx]  = ($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[@data_idx]  = ($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[@data_idx]  = ($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[@data_idx]  = ($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[@data_idx]  = ($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[@data_idx]  = ($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[@data_idx]  = ($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[@data_idx]  = ($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[@data_idx] = ($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[@data_idx] = ($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[@data_idx] = ($line[$ROW_INDEX[12]] );
      $data_idx++;
    }elsif( $line_idx == $LINE_HP_RND ){  # HP����
			$OUTPUT0_FILE[@data_idx]  = ($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[@data_idx]  = ($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[@data_idx]  = ($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[@data_idx]  = ($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[@data_idx]  = ($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[@data_idx]  = ($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[@data_idx]  = ($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[@data_idx]  = ($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[@data_idx]  = ($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[@data_idx]  = ($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[@data_idx] = ($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[@data_idx] = ($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[@data_idx] = ($line[$ROW_INDEX[12]] );
      $data_idx++;
    }elsif( $line_idx == $LINE_AT_RND ){  # AT����
			$OUTPUT0_FILE[@data_idx]  = ($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[@data_idx]  = ($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[@data_idx]  = ($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[@data_idx]  = ($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[@data_idx]  = ($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[@data_idx]  = ($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[@data_idx]  = ($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[@data_idx]  = ($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[@data_idx]  = ($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[@data_idx]  = ($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[@data_idx] = ($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[@data_idx] = ($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[@data_idx] = ($line[$ROW_INDEX[12]] );
      $data_idx++;
    }elsif( $line_idx == $LINE_DF_RND ){  # DF����
			$OUTPUT0_FILE[@data_idx]  = ($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[@data_idx]  = ($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[@data_idx]  = ($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[@data_idx]  = ($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[@data_idx]  = ($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[@data_idx]  = ($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[@data_idx]  = ($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[@data_idx]  = ($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[@data_idx]  = ($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[@data_idx]  = ($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[@data_idx] = ($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[@data_idx] = ($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[@data_idx] = ($line[$ROW_INDEX[12]] );
      $data_idx++;
    }elsif( $line_idx == $LINE_AG_RND ){  # AG����
			$OUTPUT0_FILE[@data_idx]  = ($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[@data_idx]  = ($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[@data_idx]  = ($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[@data_idx]  = ($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[@data_idx]  = ($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[@data_idx]  = ($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[@data_idx]  = ($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[@data_idx]  = ($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[@data_idx]  = ($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[@data_idx]  = ($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[@data_idx] = ($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[@data_idx] = ($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[@data_idx] = ($line[$ROW_INDEX[12]] );
      $data_idx++;
    }elsif( $line_idx == $LINE_SA_RND ){  # SA����
			$OUTPUT0_FILE[@data_idx]  = ($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[@data_idx]  = ($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[@data_idx]  = ($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[@data_idx]  = ($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[@data_idx]  = ($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[@data_idx]  = ($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[@data_idx]  = ($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[@data_idx]  = ($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[@data_idx]  = ($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[@data_idx]  = ($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[@data_idx] = ($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[@data_idx] = ($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[@data_idx] = ($line[$ROW_INDEX[12]] );
      $data_idx++;
    }elsif( $line_idx == $LINE_SD_RND ){  # SD����
			$OUTPUT0_FILE[@data_idx]  = ($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[@data_idx]  = ($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[@data_idx]  = ($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[@data_idx]  = ($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[@data_idx]  = ($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[@data_idx]  = ($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[@data_idx]  = ($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[@data_idx]  = ($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[@data_idx]  = ($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[@data_idx]  = ($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[@data_idx] = ($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[@data_idx] = ($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[@data_idx] = ($line[$ROW_INDEX[12]] );
      $data_idx++;
		}elsif( $line_idx == $LINE_TOKUSEI ){	#����
			$OUTPUT0_FILE[@data_idx]  = &get_tokusyu($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[@data_idx]  = &get_tokusyu($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[@data_idx]  = &get_tokusyu($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[@data_idx]  = &get_tokusyu($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[@data_idx]  = &get_tokusyu($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[@data_idx]  = &get_tokusyu($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[@data_idx]  = &get_tokusyu($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[@data_idx]  = &get_tokusyu($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[@data_idx]  = &get_tokusyu($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[@data_idx]  = &get_tokusyu($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[@data_idx] = &get_tokusyu($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[@data_idx] = &get_tokusyu($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[@data_idx] = &get_tokusyu($line[$ROW_INDEX[12]] );
      $data_idx++;
		}elsif( $line_idx == $LINE_SEIKAKU ){	#���i
			$OUTPUT0_FILE[@data_idx]  = &get_seikaku($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[@data_idx]  = &get_seikaku($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[@data_idx]  = &get_seikaku($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[@data_idx]  = &get_seikaku($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[@data_idx]  = &get_seikaku($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[@data_idx]  = &get_seikaku($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[@data_idx]  = &get_seikaku($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[@data_idx]  = &get_seikaku($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[@data_idx]  = &get_seikaku($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[@data_idx]  = &get_seikaku($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[@data_idx] = &get_seikaku($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[@data_idx] = &get_seikaku($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[@data_idx] = &get_seikaku($line[$ROW_INDEX[12]] );
      $data_idx++;
		}elsif( $line_idx == $LINE_SEX ){	#����
			$OUTPUT0_FILE[$data_idx]  = ($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[$data_idx]  = ($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[$data_idx]  = ($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[$data_idx]  = ($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[$data_idx]  = ($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[$data_idx]  = ($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[$data_idx]  = ($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[$data_idx]  = ($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[$data_idx]  = ($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[$data_idx]  = ($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[$data_idx] = ($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[$data_idx] = ($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[$data_idx] = ($line[$ROW_INDEX[12]] );
      $data_idx++;
		}elsif( $line_idx == $LINE_ID ){	#ID
			$OUTPUT0_FILE[$data_idx]  = ($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[$data_idx]  = ($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[$data_idx]  = ($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[$data_idx]  = ($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[$data_idx]  = ($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[$data_idx]  = ($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[$data_idx]  = ($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[$data_idx]  = ($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[$data_idx]  = ($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[$data_idx]  = ($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[$data_idx] = ($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[$data_idx] = ($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[$data_idx] = ($line[$ROW_INDEX[12]] );
      $data_idx++;
		}elsif( $line_idx == $LINE_STYLE ){	#�������悳
			$OUTPUT0_FILE[$data_idx]  = ($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[$data_idx]  = ($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[$data_idx]  = ($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[$data_idx]  = ($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[$data_idx]  = ($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[$data_idx]  = ($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[$data_idx]  = ($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[$data_idx]  = ($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[$data_idx]  = ($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[$data_idx]  = ($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[$data_idx] = ($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[$data_idx] = ($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[$data_idx] = ($line[$ROW_INDEX[12]] );
      $data_idx++;
		}elsif( $line_idx == $LINE_BEAUTIFUL ){	#��������
			$OUTPUT0_FILE[$data_idx]  = ($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[$data_idx]  = ($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[$data_idx]  = ($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[$data_idx]  = ($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[$data_idx]  = ($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[$data_idx]  = ($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[$data_idx]  = ($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[$data_idx]  = ($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[$data_idx]  = ($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[$data_idx]  = ($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[$data_idx] = ($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[$data_idx] = ($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[$data_idx] = ($line[$ROW_INDEX[12]] );
      $data_idx++;
		}elsif( $line_idx == $LINE_CUTE ){	#���킢��
			$OUTPUT0_FILE[$data_idx]  = ($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[$data_idx]  = ($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[$data_idx]  = ($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[$data_idx]  = ($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[$data_idx]  = ($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[$data_idx]  = ($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[$data_idx]  = ($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[$data_idx]  = ($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[$data_idx]  = ($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[$data_idx]  = ($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[$data_idx] = ($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[$data_idx] = ($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[$data_idx] = ($line[$ROW_INDEX[12]] );
      $data_idx++;
		}elsif( $line_idx == $LINE_CLEVER ){	#��������
			$OUTPUT0_FILE[$data_idx]  = ($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[$data_idx]  = ($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[$data_idx]  = ($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[$data_idx]  = ($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[$data_idx]  = ($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[$data_idx]  = ($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[$data_idx]  = ($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[$data_idx]  = ($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[$data_idx]  = ($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[$data_idx]  = ($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[$data_idx] = ($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[$data_idx] = ($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[$data_idx] = ($line[$ROW_INDEX[12]] );
      $data_idx++;
		}elsif( $line_idx == $LINE_STRONG ){	#�����܂���
			$OUTPUT0_FILE[$data_idx]  = ($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[$data_idx]  = ($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[$data_idx]  = ($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[$data_idx]  = ($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[$data_idx]  = ($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[$data_idx]  = ($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[$data_idx]  = ($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[$data_idx]  = ($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[$data_idx]  = ($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[$data_idx]  = ($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[$data_idx] = ($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[$data_idx] = ($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[$data_idx] = ($line[$ROW_INDEX[12]] );
      $data_idx++;
		}elsif( $line_idx == $LINE_ITEM ){	#�A�C�e��
			$OUTPUT0_FILE[$data_idx]  = &get_item($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[$data_idx]  = &get_item($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[$data_idx]  = &get_item($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[$data_idx]  = &get_item($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[$data_idx]  = &get_item($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[$data_idx]  = &get_item($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[$data_idx]  = &get_item($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[$data_idx]  = &get_item($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[$data_idx]  = &get_item($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[$data_idx]  = &get_item($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[$data_idx] = &get_item($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[$data_idx] = &get_item($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[$data_idx] = &get_item($line[$ROW_INDEX[12]] );
      $data_idx++;
		}elsif( $line_idx == $LINE_OYA_SEX ){	#�e����
			$OUTPUT0_FILE[$data_idx]  = ($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[$data_idx]  = ($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[$data_idx]  = ($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[$data_idx]  = ($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[$data_idx]  = ($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[$data_idx]  = ($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[$data_idx]  = ($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[$data_idx]  = ($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[$data_idx]  = ($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[$data_idx]  = ($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[$data_idx] = ($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[$data_idx] = ($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[$data_idx] = ($line[$ROW_INDEX[12]] );
      $data_idx++;
		}elsif( $line_idx == $LINE_FUR ){	#�щ�
			$OUTPUT0_FILE[$data_idx]  = ($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[$data_idx]  = ($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[$data_idx]  = ($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[$data_idx]  = ($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[$data_idx]  = ($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[$data_idx]  = ($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[$data_idx]  = ($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[$data_idx]  = ($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[$data_idx]  = ($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[$data_idx]  = ($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[$data_idx] = ($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[$data_idx] = ($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[$data_idx] = ($line[$ROW_INDEX[12]] );
      $data_idx++;
		}elsif( $line_idx == $LINE_COUNTRY){		#���R�[�h
			$OUTPUT0_FILE[$data_idx]  = &get_pmver($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[$data_idx]  = &get_pmver($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[$data_idx]  = &get_pmver($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[$data_idx]  = &get_pmver($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[$data_idx]  = &get_pmver($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[$data_idx]  = &get_pmver($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[$data_idx]  = &get_pmver($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[$data_idx]  = &get_pmver($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[$data_idx]  = &get_pmver($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[$data_idx]  = &get_pmver($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[$data_idx] = &get_pmver($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[$data_idx] = &get_pmver($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[$data_idx] = &get_pmver($line[$ROW_INDEX[12]] );
      $data_idx++;
		}elsif( $line_idx == $LINE_CHANGE_MONSNO ){		#��������|�P����
			$OUTPUT0_FILE[$data_idx]  = ($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[$data_idx]  = ($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[$data_idx]  = ($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[$data_idx]  = ($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[$data_idx]  = ($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[$data_idx]  = ($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[$data_idx]  = ($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[$data_idx]  = ($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[$data_idx]  = ($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[$data_idx]  = ($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[$data_idx] = ($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[$data_idx] = ($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[$data_idx] = ($line[$ROW_INDEX[12]] );
      $data_idx++;
		}elsif( $line_idx == $LINE_CHANGE_SEX ){		#��������|�P�����̐���
			$OUTPUT0_FILE[$data_idx]  = ($line[$ROW_INDEX[0]] );
			$OUTPUT1_FILE[$data_idx]  = ($line[$ROW_INDEX[1]] );
			$OUTPUT2_FILE[$data_idx]  = ($line[$ROW_INDEX[2]] );
			$OUTPUT3_FILE[$data_idx]  = ($line[$ROW_INDEX[3]] );
			$OUTPUT4_FILE[$data_idx]  = ($line[$ROW_INDEX[4]] );
			$OUTPUT5_FILE[$data_idx]  = ($line[$ROW_INDEX[5]] );
			$OUTPUT6_FILE[$data_idx]  = ($line[$ROW_INDEX[6]] );
			$OUTPUT7_FILE[$data_idx]  = ($line[$ROW_INDEX[7]] );
			$OUTPUT8_FILE[$data_idx]  = ($line[$ROW_INDEX[8]] );
			$OUTPUT9_FILE[$data_idx]  = ($line[$ROW_INDEX[9]] );
			$OUTPUT10_FILE[$data_idx] = ($line[$ROW_INDEX[10]] );
			$OUTPUT11_FILE[$data_idx] = ($line[$ROW_INDEX[11]] );
			$OUTPUT12_FILE[$data_idx] = ($line[$ROW_INDEX[12]] );
      $data_idx++;
		}
	}
	$line_idx ++;
}


#�����o��
open( FILEOUT_0, ">fld_trade_00.dat" );
open( FILEOUT_0_T, ">fld_trade_00.txt" );
binmode( FILEOUT_0 );

open( FILEOUT_1, ">fld_trade_01.dat" );
open( FILEOUT_1_T, ">fld_trade_01.txt" );
binmode( FILEOUT_1 );

open( FILEOUT_2, ">fld_trade_02.dat" );
open( FILEOUT_2_T, ">fld_trade_02.txt" );
binmode( FILEOUT_2 );

open( FILEOUT_3, ">fld_trade_03.dat" );
open( FILEOUT_3_T, ">fld_trade_03.txt" );
binmode( FILEOUT_3 );

open( FILEOUT_4, ">fld_trade_04.dat" );
open( FILEOUT_4_T, ">fld_trade_04.txt" );
binmode( FILEOUT_4 );

open( FILEOUT_5, ">fld_trade_05.dat" );
open( FILEOUT_5_T, ">fld_trade_05.txt" );
binmode( FILEOUT_5 );

open( FILEOUT_6, ">fld_trade_06.dat" );
open( FILEOUT_6_T, ">fld_trade_06.txt" );
binmode( FILEOUT_6 );

open( FILEOUT_7, ">fld_trade_07.dat" );
open( FILEOUT_7_T, ">fld_trade_07.txt" );
binmode( FILEOUT_7 );

open( FILEOUT_8, ">fld_trade_08.dat" );
open( FILEOUT_8_T, ">fld_trade_08.txt" );
binmode( FILEOUT_8 );

open( FILEOUT_9, ">fld_trade_09.dat" );
open( FILEOUT_9_T, ">fld_trade_09.txt" );
binmode( FILEOUT_9 );

open( FILEOUT_10, ">fld_trade_10.dat" );
open( FILEOUT_10_T, ">fld_trade_10.txt" );
binmode( FILEOUT_10 );

open( FILEOUT_11, ">fld_trade_11.dat" );
open( FILEOUT_11_T, ">fld_trade_11.txt" );
binmode( FILEOUT_11 );

open( FILEOUT_12, ">fld_trade_12.dat" );
open( FILEOUT_12_T, ">fld_trade_12.txt" );
binmode( FILEOUT_12 );

for( $i=0; $i<24; $i++ ){
	print( FILEOUT_0 pack("I", $OUTPUT0_FILE[$i]) );
	print( FILEOUT_1 pack("I", $OUTPUT1_FILE[$i]) );
	print( FILEOUT_2 pack("I", $OUTPUT2_FILE[$i]) );
	print( FILEOUT_3 pack("I", $OUTPUT3_FILE[$i]) );
	print( FILEOUT_4 pack("I", $OUTPUT4_FILE[$i]) );
	print( FILEOUT_5 pack("I", $OUTPUT5_FILE[$i]) );
	print( FILEOUT_6 pack("I", $OUTPUT6_FILE[$i]) );
	print( FILEOUT_7 pack("I", $OUTPUT7_FILE[$i]) );
	print( FILEOUT_8 pack("I", $OUTPUT8_FILE[$i]) );
	print( FILEOUT_9 pack("I", $OUTPUT9_FILE[$i]) );
	print( FILEOUT_10 pack("I", $OUTPUT10_FILE[$i]) );
	print( FILEOUT_11 pack("I", $OUTPUT11_FILE[$i]) );
	print( FILEOUT_12 pack("I", $OUTPUT12_FILE[$i]) );

	print( FILEOUT_0_T $OUTPUT0_FILE[$i]."\r\n" );
	print( FILEOUT_1_T $OUTPUT1_FILE[$i]."\r\n" );
	print( FILEOUT_2_T $OUTPUT2_FILE[$i]."\r\n" );
	print( FILEOUT_3_T $OUTPUT3_FILE[$i]."\r\n" );
	print( FILEOUT_4_T $OUTPUT4_FILE[$i]."\r\n" );
	print( FILEOUT_5_T $OUTPUT5_FILE[$i]."\r\n" );
	print( FILEOUT_6_T $OUTPUT6_FILE[$i]."\r\n" );
	print( FILEOUT_7_T $OUTPUT7_FILE[$i]."\r\n" );
	print( FILEOUT_8_T $OUTPUT8_FILE[$i]."\r\n" );
	print( FILEOUT_9_T $OUTPUT9_FILE[$i]."\r\n" );
	print( FILEOUT_10_T $OUTPUT10_FILE[$i]."\r\n" );
	print( FILEOUT_11_T $OUTPUT11_FILE[$i]."\r\n" );
	print( FILEOUT_12_T $OUTPUT12_FILE[$i]."\r\n" );
}

close( FILEOUT_0 );
close( FILEOUT_1 );
close( FILEOUT_2 );
close( FILEOUT_3 );
close( FILEOUT_4 );
close( FILEOUT_5 );
close( FILEOUT_6 );
close( FILEOUT_7 );
close( FILEOUT_8 );
close( FILEOUT_9 );
close( FILEOUT_10 );
close( FILEOUT_11 );
close( FILEOUT_12 );

close( FILEOUT_0_T );
close( FILEOUT_1_T );
close( FILEOUT_2_T );
close( FILEOUT_3_T );
close( FILEOUT_4_T );
close( FILEOUT_5_T );
close( FILEOUT_6_T );
close( FILEOUT_7_T );
close( FILEOUT_8_T );
close( FILEOUT_9_T );
close( FILEOUT_10_T );
close( FILEOUT_11_T );
close( FILEOUT_12_T );

exit(0);


sub file_open{
	system( "../../tools/exceltool/ExcelSeetConv -s tab fld_trade_poke.xls" );
	open( FILEIN, "fld_trade_poke.txt" );
	@FLD_TRADE_FILE = <FILEIN>;
	close( FILEIN );

	open( FILEIN, "monsno_def.h" );
	@MONSNO_H_FILE = <FILEIN>;
	close( FILEIN );

	open( FILEIN, "tokusyu_def.h" );
	@TOKUSYU_H_FILE = <FILEIN>;
	close( FILEIN );

	open( FILEIN, "poke_tool.h" );
	@SEIKAKU_H_FILE = <FILEIN>;
	close( FILEIN );

	open( FILEIN, "itemsym.h" );
	@ITEMSYM_H_FILE = <FILEIN>;
	close( FILEIN );

	open( FILEIN, "pm_version.h" );
	@PMVER_H_FILE = <FILEIN>;
	close( FILEIN );
}
sub get_monsno{
	my( $key )  = @_;
	
  print "$key = ";
	foreach $one ( @MONSNO_H_FILE ){

		if( $one =~ /#define\t*$key\t*\( ([0-9]*) \)/ ){
      print "$1\n";
			return $1;	#�q�b�g�����i���o�[��Ԃ�
		}
	}
  print "�~\n";
	return 0;
}
sub get_formno{
	my( $key )  = @_;
	
  print "$key = ";
	foreach $one ( @MONSNO_H_FILE ){

		if( $one =~ /#define\t*$key\t*\( ([0-9]*) \)/ ){
      print "$1\n";
			return $1;	#�q�b�g�����i���o�[��Ԃ�
		}
	}
  print "�~\n";
	return 0;
}
sub get_tokusyu{
	my( $key )  = @_;
	
  print "$key = ";
	foreach $one ( @TOKUSYU_H_FILE ){

		if( $one =~ /#define\t*$key\t*\(([0-9]*)\)/ ){
      print "$1\n";
			return $1;	#�q�b�g�����i���o�[��Ԃ�
		}
	}
  print "�~\n";
	return 0; 
}
sub get_seikaku{
	my( $key )  = @_;
	
  print "$key = ";
	foreach $one ( @SEIKAKU_H_FILE ){

		if( $one =~ /#define\s*$key\s*\( ([0-9]*) \)/ ){
      print "$1\n";
			return $1;	#�q�b�g�����i���o�[��Ԃ�
		}
	}
  print "�~\n";
	return 0; 
}
sub get_item{
	my( $key )  = @_;
	
  print "$key\n";
	foreach $one ( @ITEMSYM_H_FILE ){

		if( $one =~ /#define $key\t*\( ([0-9]*) \)/ ){
      print "$1\n";
			return $1;	#�q�b�g�����i���o�[��Ԃ�
		}
	}
  print "�~\n";
	return 0;
}
sub get_pmver{
	my( $key )  = @_;
	
  print "$key = ";
	foreach $one ( @PMVER_H_FILE ){

		if( $one =~ /#define\t*$key\t*([0-9]*)/ ){
      print "$1\n";
			return $1;	#�q�b�g�����i���o�[��Ԃ�
		}
	}
  print "�~\n";
	return 0;
}
