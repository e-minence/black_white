##-----------------------------------------------------------------------------
#
#
#		�^�}�����f�[�^�R���o�[�^
#
#
##-----------------------------------------------------------------------------

##-----------------------------------------------------------------------------
#	�t�@�C���|�C���^
##-----------------------------------------------------------------------------
@FILE_IN	= undef;

##-----------------------------------------------------------------------------
#	�f�[�^�Q�ƒ萔
##-----------------------------------------------------------------------------
$FILE_DATA_NUM	= 15;		#�f�[�^����
$FILE_START_IDX	= 3;		#�f�[�^�J�n�s��
$FILE_TYPE_ST	= 1;		#�f�[�^�^�C�v�J�n��
$FILE_TYPE_NUM	= 6;		#�f�[�^�^�C�v��




#�܂��̓G�N�Z���R���o�[�g
#�R���o�[�g��̃t�@�C����ǂݍ���
system( "../../pokemon_pl/convert/exceltool/ExcelSeetConv.exe -o bucket_data.txt -p 0 -s csv bucket_data.xls" );
open( FILEIN, "bucket_data.txt" );
@FILE_IN = <FILEIN>;
close( FILEIN );


#�����o���悤�̃t�@�C�����I�[�v��
#�o�C�i�����[�h�ɐݒ�
open( FIELOUT, ">bucket_data.bin" );
binmode( FIELOUT );


#�f�[�^�̏����o��
for( $i=0; $i<$FILE_DATA_NUM; $i++ ){
	@file_data = split( /,/, $FILE_IN[ $i+$FILE_START_IDX ] );
	for( $j=0; $j<$FILE_TYPE_NUM; $j++ ){

		if( $file_data[ $j+$FILE_TYPE_ST ] eq "MAX" ){
			print( FIELOUT pack("I", 100000 ) );
		}else{
			print( FIELOUT pack("I", $file_data[ $j+$FILE_TYPE_ST ] ) );
		}
	}
}

#�I��
close( FIELOUT );

#��������j��
system( "rm bucket_data.txt" );

exit(0);
