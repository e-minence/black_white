

# �f�[�^�̃C���f�b�N�X
LINE_TRAIN_1 = 1  # �d��1
LINE_TRAIN_2 = 2  # �d��2

ROW_TRACK    = 1  # �g���b�NNo.
ROW_DISTANCE = 2  # �����͂�����
ROW_VOLUME   = 5  # ����
ROW_WAIT_MIN = 6  # �ŒZ�ҋ@����
ROW_WAIT_MAX = 7  # �Œ��ҋ@����


# �t�@�C���ǂݍ���
file = File.open( "h03.txt", "r" )
file_lines = file.readlines
file.close

# �f�[�^�擾(�d��1)
line = file_lines[LINE_TRAIN_1]
item = line.split(/\s/)
train1_track    = item[ROW_TRACK].to_i
train1_distance = item[ROW_DISTANCE].to_f
train1_volume   = item[ROW_VOLUME].to_i
train1_wait_min = item[ROW_WAIT_MIN].to_i
train1_wait_max = item[ROW_WAIT_MAX].to_i

# �f�[�^�擾(�d��2)
line = file_lines[LINE_TRAIN_2]
item = line.split(/\s/)
train2_track    = item[ROW_TRACK].to_i
train2_distance = item[ROW_DISTANCE].to_f
train2_volume   = item[ROW_VOLUME].to_i
train2_wait_min = item[ROW_WAIT_MIN].to_i
train2_wait_max = item[ROW_WAIT_MAX].to_i

# �f�[�^�o��(�d��1��3D�T�E���h���j�b�g�f�[�^)
file = File.open( "train1_3ds_unit_data.bin", "wb" )
data = Array.new
data << train1_track
data << train1_distance
data << train1_volume
file.write( data.pack("IfI") )
file.close

# �f�[�^�o��(�d��1�̑ҋ@���ԃf�[�^)
file = File.open( "train1_wait_data.bin", "wb" )
data = Array.new
data << train1_wait_min
data << train1_wait_max
file.write( data.pack("II") )
file.close

# �f�[�^�o��(�d��2��3D�T�E���h���j�b�g�f�[�^)
file = File.open( "train2_3ds_unit_data.bin", "wb" )
data = Array.new
data << train2_track
data << train2_distance
data << train2_volume
file.write( data.pack("IfI") )
file.close

# �f�[�^�o��(�d��2�̑ҋ@���ԃf�[�^)
file = File.open( "train2_wait_data.bin", "wb" )
data = Array.new
data << train2_wait_min
data << train2_wait_max
file.write( data.pack("II") )
file.close


