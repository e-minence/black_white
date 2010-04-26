#=============================================================================================
#
#	�X�^�b�t���[���F�R�}���h�f�[�^�쐬
#	2010/04/23 by nakahiro
#
#=============================================================================================

#---------------------------------------------------------------------------------------------
#	�萔��`
#---------------------------------------------------------------------------------------------


#---------------------------------------------------------------------------------------------
#	�O���[�o���ϐ�
#---------------------------------------------------------------------------------------------
$CSV_FILE = $ARGV[0];		# ���t�@�C��
$CNV_FILE = 0;				# �쐬�t�@�C��

# ���x���e�[�u��
@LabelName = (
	"�ꊇ",
	"�N���A",
	"�X�N���[���J�n",
	"�X�N���[����~",
	"�I��",
);
$LABEL_NAME_MAX = 5;

# ���x���ϊ��l
@LabelConv = (
	1,		# ITEMLIST_LABEL_STR_PUT
	2,		# ITEMLIST_LABEL_STR_CLEAR
	3,		# ITEMLIST_LABEL_SCROLL_START
	4,		# ITEMLIST_LABEL_SCROLL_STOP
	5,		# ITEMLIST_LABEL_END
);

# �\�����[�h�e�[�u��
@PutMode = (
	"��",
	"�E",
	"����",
);
$PUT_MODE_MAX = 3;

# �t�H���g�e�[�u��
@FontType = (
	"��",
	"��",
);
$FONT_TYPE_MAX = 2;

# �t�H���g�J���[�e�[�u��
@FontColor = (
	"��",
	"��",
	"��",
	"��",
	"��",
	"�I�����W",
	"�s���N",
);
$FONT_COLOR_MAX = 7;


#---------------------------------------------------------------------------------------------
#	���C������
#---------------------------------------------------------------------------------------------

&SUB_MakeCnvFileName();	# �R���o�[�g�t�@�C�����쐬
&SUB_DataConv();		# �f�[�^�R���o�[�g

exit;


#---------------------------------------------------------------------------------------------
#	�R���o�[�g�t�@�C�����쐬
#---------------------------------------------------------------------------------------------
sub SUB_MakeCnvFileName {
	$CNV_FILE = $CSV_FILE;
	$CNV_FILE =~ s/.csv/.dat/;
}


#---------------------------------------------------------------------------------------------
#	�f�[�^�R���o�[�g
#---------------------------------------------------------------------------------------------

sub SUB_DataConv {
#	my( $write, $val );

	# ���t�@�C���I�[�v��
	open( FP_CSV, "< " . $CSV_FILE );
	$fp_csv = <FP_CSV>;					# �_�~�[
	@fp_csv = <FP_CSV>;
	close( FP_CSV );

	# �쐬�t�@�C���I�[�v��
	open( FP_CNV, "> " . $CNV_FILE );
	binmode( FP_CNV );

	$msgIdx = 0;

	$cnt = 0;
	foreach $one ( @fp_csv ){
		$one =~ s/\r/\n/g;				# ���s��\r�̂��̂�\n�̂��̂�����̂ŁA\r��\n�ɒu������
		$one =~ s/\n/,\n/g;				# "\n"��",\n"�ɕύX
		@line = split( ",", $one );		# ","�ŕ���

# typedef struct {
#	u32	msgIdx;
#
#	u16	wait;
#	u8	label;
#	u8	labelType;
#
#	u8	color;
#	u8	font;
#	u16	putMode;
#
#	s16	px;
#	s16	offs_x;
# }ITEMLIST_DATA;

		# ������ [0]
		if( $line[0] eq "" ){
			$val = 0xffff + 0;
#			print( "[0] ". $val . ", " );
			$write = pack "V", $val;
			print( FP_CNV $write );
		}else{
			$val = $msgIdx + 0;
#			print( "[0] ". $val . ", " );
			$write = pack "V", $val;
			print( FP_CNV $write );
			$msgIdx++;
		}

		# �E�F�C�g / �x���W [3]
		if( $line[3] eq "" ){
			$val = 0 + 0;
#			print( "[1] ". $val . ", " );
			$write = pack "v", $val;
			print( FP_CNV $write );
		}else{
			$val = $line[3] + 0;
#			print( "[1] ". $val . ", " );
			$write = pack "v", $val;
			print( FP_CNV $write );
		}

		# ���x�� [1]
		if( $line[1] eq "" ){
			$val = 0 + 0;
#			print( "[2] ". $val . ", " );
			$write = pack "C", $val;
			print( FP_CNV $write );
		}else{
			for( $i=0; $i<$LABEL_NAME_MAX; $i++ ){
				if( $line[1] eq $LabelName[$i] ){
					$val = $LabelConv[$i] + 0;
#					print( "[2] ". $val . ", " );
					$write = pack "C", $val;
					print( FP_CNV $write );
					last;
				}
			}
		}

		# ���x���^�C�v [2]
		if( $line[2] eq "" ){
			$val = 0 + 0;
#			print( "[3] ". $val . ", " );
			$write = pack "C", $val;
			print( FP_CNV $write );
		}else{
			$val = $line[2] + 0;
#			print( "[3] ". $val . ", " );
			$write = pack "C", $val;
			print( FP_CNV $write );
		}

		# �J���[ [8]
		if( $line[8] eq "" ){
			$val = 0 + 0;
#			print( "[4] ". $val . ", " );
			$write = pack "C", $val;
			print( FP_CNV $write );
		}else{
			for( $i=0; $i<$FONT_COLOR_MAX; $i++ ){
				if( $line[8] eq $FontColor[$i] ){
					$val = $i + 0;
#					print( "[4] ". $val . ", " );
					$write = pack "C", $val;
					print( FP_CNV $write );
					last;
				}
			}
		}

		# �t�H���g [7]
		if( $line[7] eq "" ){
			$val = 0 + 0;
#			print( "[5] ". $val . ", " );
			$write = pack "C", $val;
			print( FP_CNV $write );
		}else{
			for( $i=0; $i<$FONT_TYPE_MAX; $i++ ){
				if( $line[7] eq $FontType[$i] ){
					$val = $i + 0;
#					print( "[5] ". $val . ", " );
					$write = pack "C", $val;
					print( FP_CNV $write );
					last;
				}
			}
		}

		# �\�����[�h [4]
		if( $line[4] eq "" ){
			$val = 0 + 0;
#			print( "[6] ". $val . ", " );
			$write = pack "v", $val;
			print( FP_CNV $write );
		}else{
			for( $i=0; $i<$PUT_MODE_MAX; $i++ ){
				if( $line[4] eq $PutMode[$i] ){
					$val = $i + 0;
#					print( "[6] ". $val . ", " );
					$write = pack "v", $val;
					print( FP_CNV $write );
					last;
				}
			}
		}

		# �\����w���W [5]
		if( $line[5] eq "" ){
			$val = 0 + 0;
#			print( "[7] ". $val . ", " );
			$write = pack "v", $val;
			print( FP_CNV $write );
		}else{
			$val = $line[5] + 0;
#			print( "[7] ". $val . ", " );
			$write = pack "v", $val;
			print( FP_CNV $write );
		}

		# �w���W�I�t�Z�b�g [6]
		if( $line[6] eq "" ){
			$val = 0 + 0;
#			print( "[8] ". $val . "\n" );
			$write = pack "v", $val;
			print( FP_CNV $write );
		}else{
			$val = $line[6] + 0;
#			print( "[8] ". $val . "\n" );
			$write = pack "v", $val;
			print( FP_CNV $write );
		}

#		print( $line[1] . "\n" );
	}

	close( FP_CNV );
}
