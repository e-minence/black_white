#################################################################################
# @brief h01.xls�R���o�[�^
# @file h01.rb
# @author obata
# @date 2009.11.19
#################################################################################

#--------------------------------------------------------------------------------
# @brief main
# @param ARGV[0] �R���o�[�g�Ώۃt�@�C���̃p�X
# @param ARGV[1] �R���o�[�g��̏o�͐�̃p�X
#--------------------------------------------------------------------------------
# �f�[�^�C���f�b�N�X(�s)
ROW_WIND      = 1  # ��
ROW_SHIP      = 2  # �D
ROW_TRAILER_L_HEAD = 3  # �g���[���[(��/�O)
ROW_TRAILER_R_HEAD = 4  # �g���[���[(�E/�O)
ROW_TRAILER_L_TAIL = 5  # �g���[���[(��/��)
ROW_TRAILER_R_TAIL = 6  # �g���[���[(�E/��)
# �f�[�^�C���f�b�N�X(��)
COLUMN_TRACK_NO        = 1  # �g���b�NNo.
COLUMN_3DS_RANGE       = 2  # �����͂�����
COLUMN_WIND_MIN_HEIGHT = 3  # ��������o������
COLUMN_WIND_MAX_HEIGHT = 4  # �������ő�ɂȂ鍂��
COLUMN_3DS_VOLUME      = 5  # ����
COLUMN_MIN_WAIT        = 6  # �ŒZ�ҋ@����
COLUMN_MAX_WAIT        = 7  # �Œ��ҋ@����

# ���̓f�[�^�ǂݍ���
file = File.open( ARGV[0], "r" )
file_data = file.readlines
file.close

# ���f�[�^
in_data = file_data[ROW_WIND].split(/\s/)
out_data = Array.new
out_data << (1<<(in_data[COLUMN_TRACK_NO].to_i - 1))
out_data << 0  # padding
out_data << in_data[COLUMN_WIND_MIN_HEIGHT].to_f
out_data << in_data[COLUMN_WIND_MAX_HEIGHT].to_f
file = File.open( ARGV[1]+"/wind_data.bin", "wb" )
file.write( out_data.pack("SSff") )
file.close 
# �D(3DS)�f�[�^
in_data = file_data[ROW_SHIP].split(/\s/)
out_data = Array.new
out_data << in_data[COLUMN_TRACK_NO].to_i
out_data << in_data[COLUMN_3DS_RANGE].to_f
out_data << in_data[COLUMN_3DS_VOLUME].to_i
file = File.open( ARGV[1]+"/ship_3dsu_data.bin", "wb" )
file.write( out_data.pack("ifi") )
file.close 
# �D(�҂�)�f�[�^
in_data = file_data[ROW_SHIP].split(/\s/)
out_data = Array.new
out_data << in_data[COLUMN_MIN_WAIT].to_i
out_data << in_data[COLUMN_MAX_WAIT].to_i
file = File.open( ARGV[1]+"/ship_wait_data.bin", "wb" )
file.write( out_data.pack("II") )
file.close 
# ���O�g���[��(3DS)�f�[�^
in_data = file_data[ROW_TRAILER_L_HEAD].split(/\s/)
out_data = Array.new
out_data << in_data[COLUMN_TRACK_NO].to_i
out_data << in_data[COLUMN_3DS_RANGE].to_f
out_data << in_data[COLUMN_3DS_VOLUME].to_i
file = File.open( ARGV[1]+"/trailer1_head_3dsu_data.bin", "wb" )
file.write( out_data.pack("ifi") )
file.close
# ���O�g���[��(�҂�)�f�[�^
in_data = file_data[ROW_TRAILER_L_HEAD].split(/\s/)
out_data = Array.new
out_data << in_data[COLUMN_MIN_WAIT].to_i
out_data << in_data[COLUMN_MAX_WAIT].to_i
file = File.open( ARGV[1]+"/trailer1_head_wait_data.bin", "wb" )
file.write( out_data.pack("II") )
file.close 
# �E�O�g���[��(3DS)�f�[�^
in_data = file_data[ROW_TRAILER_R_HEAD].split(/\s/)
out_data = Array.new
out_data << in_data[COLUMN_TRACK_NO].to_i
out_data << in_data[COLUMN_3DS_RANGE].to_f
out_data << in_data[COLUMN_3DS_VOLUME].to_i
file = File.open( ARGV[1]+"/trailer2_head_3dsu_data.bin", "wb" )
file.write( out_data.pack("ifi") )
file.close
# �E�O�g���[��(�҂�)�f�[�^
in_data = file_data[ROW_TRAILER_R_HEAD].split(/\s/)
out_data = Array.new
out_data << in_data[COLUMN_MIN_WAIT].to_i
out_data << in_data[COLUMN_MAX_WAIT].to_i
file = File.open( ARGV[1]+"/trailer2_head_wait_data.bin", "wb" )
file.write( out_data.pack("II") )
file.close 
# ����g���[��(3DS)�f�[�^
in_data = file_data[ROW_TRAILER_L_TAIL].split(/\s/)
out_data = Array.new
out_data << in_data[COLUMN_TRACK_NO].to_i
out_data << in_data[COLUMN_3DS_RANGE].to_f
out_data << in_data[COLUMN_3DS_VOLUME].to_i
file = File.open( ARGV[1]+"/trailer1_tail_3dsu_data.bin", "wb" )
file.write( out_data.pack("ifi") )
file.close
# ����g���[��(�҂�)�f�[�^
in_data = file_data[ROW_TRAILER_L_TAIL].split(/\s/)
out_data = Array.new
out_data << in_data[COLUMN_MIN_WAIT].to_i
out_data << in_data[COLUMN_MAX_WAIT].to_i
file = File.open( ARGV[1]+"/trailer1_tail_wait_data.bin", "wb" )
file.write( out_data.pack("II") )
file.close 
# �E��g���[��(3DS)�f�[�^
in_data = file_data[ROW_TRAILER_R_TAIL].split(/\s/)
out_data = Array.new
out_data << in_data[COLUMN_TRACK_NO].to_i
out_data << in_data[COLUMN_3DS_RANGE].to_f
out_data << in_data[COLUMN_3DS_VOLUME].to_i
file = File.open( ARGV[1]+"/trailer2_tail_3dsu_data.bin", "wb" )
file.write( out_data.pack("ifi") )
file.close
# �E��g���[��(�҂�)�f�[�^
in_data = file_data[ROW_TRAILER_R_TAIL].split(/\s/)
out_data = Array.new
out_data << in_data[COLUMN_MIN_WAIT].to_i
out_data << in_data[COLUMN_MAX_WAIT].to_i
file = File.open( ARGV[1]+"/trailer2_tail_wait_data.bin", "wb" )
file.write( out_data.pack("II") )
file.close 
