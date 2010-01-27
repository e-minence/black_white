#---------------------------------------------------------------------------------- 
# @brief �V�[�P���X���x������, �V�[�P���X�ԍ����擾����
# @param seq_label �V�[�P���X���x��
# @return �w�肵�����x���̃V�[�P���X�ԍ�
#---------------------------------------------------------------------------------- 
def GetSeqNo( seq_label )

	# �t�@�C�����J��
	file_def = File.open( "../../../prog/include/sound/wb_sound_data.sadl" )

	# 1�s�����Ă���
	file_def.each do |line|
		if line.index(seq_label)!=nil then
			i0 = line.index(seq_label) + seq_label.length
			i1 = line.length - 1
			def_val = line[i0..i1].to_i
			return def_val
		end
	end
	
	# �t�@�C�������
	file_def.close

  # ���x���������炸
  abort( "error: �w��V�[�P���X���x��[#{seq_label}]�͒�`����Ă��܂���B" );
end

#----------------------------------------------------------------------------------
# @brief �g���b�N�r�b�g���쐬����
# @param �e�g���b�N�̊J����Ԃ�ێ�����z��
# @return �w�肵���z�񂩂狁�߂��J���g���b�N�r�b�g
#----------------------------------------------------------------------------------
def CalcTrackBit( track_state )

  track_bit = 0

  # �S�g���b�N�̏�Ԃ�����
  0.upto(track_state.size-1) do |i|
    if track_state[i]=="��" then 
      track_bit = track_bit | (1 << i)
    end
  end
  return track_bit
end


#----------------------------------------------------------------------------------
# @brief main
# @param ARGV[0] �R���o�[�g�Ώۃt�@�C����(�G�N�Z���̃^�u��؂�f�[�^)
# @param ARGV[1] �o�C�i���f�[�^�̏o�͐�f�B���N�g��
#----------------------------------------------------------------------------------

#�s�C���f�b�N�X
LINE_FIRST_DATA = 2  # �擪�f�[�^

# �f�[�^�E�C���f�b�N�X
ROW_SOUND_IDX  = 0   # �]�[��ID
ROW_TRACK_1    = 1   # ���삷��g���b�N��On/Off (�g���b�N1)
ROW_TRACK_2    = 2   # ���삷��g���b�N��On/Off (�g���b�N2)
ROW_TRACK_3    = 3   # ���삷��g���b�N��On/Off (�g���b�N3)
ROW_TRACK_4    = 4   # ���삷��g���b�N��On/Off (�g���b�N4)
ROW_TRACK_5    = 5   # ���삷��g���b�N��On/Off (�g���b�N5)
ROW_TRACK_6    = 6   # ���삷��g���b�N��On/Off (�g���b�N6)
ROW_TRACK_7    = 7   # ���삷��g���b�N��On/Off (�g���b�N7)
ROW_TRACK_8    = 8   # ���삷��g���b�N��On/Off (�g���b�N8)
ROW_TRACK_9    = 9   # ���삷��g���b�N��On/Off (�g���b�N9)
ROW_TRACK_10   = 10  # ���삷��g���b�N��On/Off (�g���b�N10)
ROW_TRACK_11   = 11  # ���삷��g���b�N��On/Off (�g���b�N11)
ROW_TRACK_12   = 12  # ���삷��g���b�N��On/Off (�g���b�N12)
ROW_TRACK_13   = 13  # ���삷��g���b�N��On/Off (�g���b�N13)
ROW_TRACK_14   = 14  # ���삷��g���b�N��On/Off (�g���b�N14)
ROW_TRACK_15   = 15  # ���삷��g���b�N��On/Off (�g���b�N15)
ROW_TRACK_16   = 16  # ���삷��g���b�N��On/Off (�g���b�N16)

# �o�̓t�@�C�����̃��X�g
bin_file_list = Array.new

# �t�@�C���f�[�^��ǂݍ���
file = File.open( ARGV[0], "r" )
file_lines = file.readlines
file.close

# 3�s�ڈȍ~�̑S���C�����R���o�[�g
LINE_FIRST_DATA.upto( file_lines.size - 1 ) do |i|
  # �R���o�[�g�Ώۂ̃f�[�^���擾
  line = file_lines[i]
  in_data = line.split(/\s/)
  if in_data[0]==nil then break end # ��f�[�^�𔭌�==>�I��
  # �g���b�N��Ԕz����쐬
  track_state = Array.new
  track_state << in_data[ROW_TRACK_1]
  track_state << in_data[ROW_TRACK_2]
  track_state << in_data[ROW_TRACK_3]
  track_state << in_data[ROW_TRACK_4]
  track_state << in_data[ROW_TRACK_5]
  track_state << in_data[ROW_TRACK_6]
  track_state << in_data[ROW_TRACK_7]
  track_state << in_data[ROW_TRACK_8]
  track_state << in_data[ROW_TRACK_9]
  track_state << in_data[ROW_TRACK_10]
  track_state << in_data[ROW_TRACK_11]
  track_state << in_data[ROW_TRACK_12]
  track_state << in_data[ROW_TRACK_13]
  track_state << in_data[ROW_TRACK_14]
  track_state << in_data[ROW_TRACK_15]
  track_state << in_data[ROW_TRACK_16]
  # �o�̓f�[�^���쐬
  out_data = Array.new
  out_data << GetSeqNo( in_data[ROW_SOUND_IDX] )
  out_data << CalcTrackBit( track_state )
  out_data << 0  # padding
  # �o�C�i���f�[�^���o��
  filename = ARGV[1] + "/iss_bridge_data_" + in_data[ROW_SOUND_IDX].downcase + ".bin"
  file = File.open( filename, "wb" )
  file.write( out_data.pack("ISS") )
  file.close
  # �o�̓t�@�C�������L��
  bin_file_list << filename
end

# �o�͂����o�C�i���t�@�C������\��
bin_file_list.each do |filename|
  puts "-output: #{filename}"
end 
