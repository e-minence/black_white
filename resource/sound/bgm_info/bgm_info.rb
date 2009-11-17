###############################################################################
#
# @brief  [BGM�̃V�[�P���X�ԍ� : ISS�^�C�v] �̃e�[�u�����쐬����
# @author obata_tosihiro
# @date   2009.07.17
# 
# [�o�̓o�C�i���E�f�[�^�̃t�H�[�}�b�g]
# [0] �f�[�^��
# [1] �V�[�P���X�ԍ�
# [2]
# [3] ISS�^�C�v
# �ȉ��A[1]-[3]���f�[�^�̐���������
###############################################################################


###############################################################################
#
# @brief �w�肳�ꂽ�����񂪋󂩂ǂ����𔻒肷��
#
# @param str ����Ώۂ̕�����
# 
###############################################################################
def IsEmpty( str )

	# �󔒕������폜
	str = str.gsub( /\s/, "" )

	# ���炩�̕��������݂����, ��ł͂Ȃ�
	if str != "" then
		return false;
	end

	# ��
	return true;
	
end


###############################################################################
#
# @brief ������ɊY������V�[�P���X�ԍ����擾����
#
# @param seq_name �V�[�P���X�ԍ��� define ��`��
# @param file     �V�[�P���X�ԍ�����`����Ă���t�@�C��
# 
###############################################################################
def GetSeqIndex( seq_name, file )

	# �t�@�C���擪�ʒu�փV�[�N
	file.seek( 0, IO::SEEK_SET )

	# ���ׂĂ̍s����������
	file.each do | line |
		# ����
		if( line.index( seq_name ) != nil ) then
			items = line.split( "\t" )
			return items[1].to_i
		end
	end

	# ������Ȃ������ꍇ
	return -1
end


###############################################################################
#
# @brief ������ɊY������ISS�^�C�v�ԍ����擾����
#
# @param iss_type ISS�^�C�v( "�ʏ�", "�X", "���H" �Ȃ� )
# @param file     ISS�^�C�v����`����Ă���t�@�C��
# 
###############################################################################
def GetISSType( iss_type, file )

	# �t�@�C���擪�ʒu�փV�[�N
	file.seek( 0, IO::SEEK_SET )

	# ���ׂĂ̍s����������
	file.each do | line |
		# ����
		if( line.index( iss_type ) != nil ) then

			# "(", ")" �̊Ԃɂ��鐔�l���擾
			i0 = line.index( "(" ) + 1
			i1 = line.index( ")" ) - 1
			str = line[ i0..i1 ]
			return str.to_i
		end
	end

	# "�ʏ�"��������Ȃ������ꍇ ==> iss_type.def �Ɍ�肪����!! ==> ������Ԃ�
	if( iss_type == "�ʏ�" ) then
		return -1
	end

	# "�ʏ�"�ȊO��������Ȃ������ꍇ, �f�t�H���g�� "�ʏ�" ��Ԃ�
	return GetISSType( "�ʏ�", file );
end


###############################################################################
#
# @brief ���C��
# 
###############################################################################
# �o�̓f�[�^
data_num = 0	     # �f�[�^��
table    = Array.new # �e�[�u���f�[�^[�V�[�P���X�ԍ�, ISS�^�C�v]

#------------------------------------------------------------
# �e�[�u���쐬
#------------------------------------------------------------

# �e�t�@�C����
SRC_FILENAME      = ARGV[0]
DEF_SEQ_FILENAME  = "../wb_sound_data.sadl"
DEF_TYPE_FILENAME = "iss_type.h"
DST_FILENAME      = "bgm_info.bin"

# �t�@�C�����J��
src_file      = File.open( SRC_FILENAME )
def_seq_file  = File.open( DEF_SEQ_FILENAME )
def_type_file = File.open( DEF_TYPE_FILENAME )

# 1,2�s�ڂ��X�L�b�v
src_file.gets
src_file.gets

# 3�s�ڈȍ~�����ׂČ���
src_file.each do | line |

	# ���ڕʂɕ���
	items = line.split( "\t" )

	# ISS�^�C�v���擾
	iss_type = items[5];

	# ISS�^�C�v���ݒ肳��Ă���ꍇ, �V�[�P���X�ԍ���ISS�^�C�v���e�[�u���ɓo�^
	if iss_type!=nil && ( IsEmpty( iss_type ) != true ) then
		table[ data_num ] = [ GetSeqIndex( items[2], def_seq_file ), GetISSType( iss_type, def_type_file ) ];
		data_num          = data_num + 1;
	end
	
end

# �t�@�C�������
src_file.close
def_seq_file.close
def_type_file.close


#------------------------------------------------------------
# �f�[�^���o��
#------------------------------------------------------------
# �t�@�C�����J��
dst_file = File.open( DST_FILENAME, "wb" )

# �f�[�^�����o��
dst_file.write( [ data_num ].pack( "C" ) )

# �e�[�u�����o��
table.each { |items|
	dst_file.write( items.pack( "SC" ) )
}

# �t�@�C�������
dst_file.close


#=begin
#------------------------------------------------------------
# �f�o�b�O�o��
#------------------------------------------------------------
# �t�@�C�����J��
dst_file = File.open( "debug.txt", "w" )

# �f�[�^�����o��
str = "data_num = " + data_num.to_s() + "\n"
dst_file.write( str )

# �e�[�u�����o��
table.each { |items|
	str = "seq = " + items[0].to_s() + "  type = " + items[1].to_s() + "\n"
	dst_file.write( str )
}

# �t�@�C�������
dst_file.close

#=end
