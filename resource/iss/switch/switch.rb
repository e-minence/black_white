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
# @brief �w�肵���t���O�ԍ��̃g���b�N�r�b�g���擾����
# @param flag_state �e�g���b�N�̃t���O�ԍ���ێ�����z��
# @param flag_no    �g���b�N�r�b�g�����߂�t���O�ԍ�
# @return �w�肵���t���O�ԍ��𑀍삷�邽�߂̃g���b�N�r�b�g
#----------------------------------------------------------------------------------
def CalcTrackBit( flag_state, flag_no )

  track_bit = 0

  # �S�g���b�N�̏�Ԃ�����
  0.upto(flag_state.size-1) do |i|
    if flag_state[i]==flag_no then 
      track_bit = track_bit | (1 << i)
    end
  end
  return track_bit
end

#---------------------------------------------------------------------------------- 
# @brief �]�[��������, �]�[��ID���擾����
# @param str_zone_id �]�[��ID��\��������
# @return �w�肵���]�[��ID�̒l
#---------------------------------------------------------------------------------- 
def GetZoneID( str_zone_id )

	# �t�@�C�����J��
	file_def = File.open( "../../../prog/arc/fieldmap/zone_id.h" )

	# 1�s�����Ă���
	file_def.each do |line|
		if line.index(str_zone_id)!=nil then
			i0 = line.index("(") + 1
			i1 = line.index(")") - 1
			def_val = line[i0..i1].to_i
			return def_val
		end
	end
	
	# �t�@�C�������
	file_def.close

  abort( "error: �w�肳�ꂽ�]�[��ID[# str_zone_id}]�͒�`����Ă��܂���B" );
end


#----------------------------------------------------------------------------------
# @brief main
# @param ARGV[0] �R���o�[�g�Ώۃt�@�C����(�G�N�Z���̃^�u��؂�f�[�^)
# @param ARGV[1] �o�C�i���f�[�^�̏o�͐�f�B���N�g��
#----------------------------------------------------------------------------------

# �f�[�^�E�C���f�b�N�X
ROW_SEQ_LABEL     = 0   # �V�[�P���X���x��
ROW_FLAG_NO_TR01  = 1   # �t���O�ԍ�(�g���b�N1)
ROW_FLAG_NO_TR02  = 2   # �t���O�ԍ�(�g���b�N2)
ROW_FLAG_NO_TR03  = 3   # �t���O�ԍ�(�g���b�N3)
ROW_FLAG_NO_TR04  = 4   # �t���O�ԍ�(�g���b�N4)
ROW_FLAG_NO_TR05  = 5   # �t���O�ԍ�(�g���b�N5)
ROW_FLAG_NO_TR06  = 6   # �t���O�ԍ�(�g���b�N6)
ROW_FLAG_NO_TR07  = 7   # �t���O�ԍ�(�g���b�N7)
ROW_FLAG_NO_TR08  = 8   # �t���O�ԍ�(�g���b�N8)
ROW_FLAG_NO_TR09  = 9   # �t���O�ԍ�(�g���b�N9)
ROW_FLAG_NO_TR10  = 10  # �t���O�ԍ�(�g���b�N10)
ROW_FLAG_NO_TR11  = 11  # �t���O�ԍ�(�g���b�N11)
ROW_FLAG_NO_TR12  = 12  # �t���O�ԍ�(�g���b�N12)
ROW_FLAG_NO_TR13  = 13  # �t���O�ԍ�(�g���b�N13)
ROW_FLAG_NO_TR14  = 14  # �t���O�ԍ�(�g���b�N14)
ROW_FLAG_NO_TR15  = 15  # �t���O�ԍ�(�g���b�N15)
ROW_FLAG_NO_TR16  = 16  # �t���O�ԍ�(�g���b�N16)
ROW_FADE_FRAME    = 17  # �t�F�[�h����
ROW_FIRST_ZONE_ID = 18  # �擪�]�[��ID

# �o�̓t�@�C�����̃��X�g
bin_file_list = Array.new

# �t�@�C���f�[�^��ǂݍ���
file = File.open( ARGV[0], "r" )
file_lines = file.readlines
file.close

# 3�s�ڈȍ~�̑S���C�����R���o�[�g
2.upto( file_lines.size - 1 ) do |i|
  # �R���o�[�g�Ώۂ̃f�[�^���擾
  line = file_lines[i]
  in_data = line.split(/\s/)
  if in_data[0]==nil then break end # ��f�[�^�𔭌�==>�I��
  # �t���O�ԍ��z����쐬
  flag_state = Array.new
  flag_state << in_data[ROW_FLAG_NO_TR01].to_i
  flag_state << in_data[ROW_FLAG_NO_TR02].to_i
  flag_state << in_data[ROW_FLAG_NO_TR03].to_i
  flag_state << in_data[ROW_FLAG_NO_TR04].to_i
  flag_state << in_data[ROW_FLAG_NO_TR05].to_i
  flag_state << in_data[ROW_FLAG_NO_TR06].to_i
  flag_state << in_data[ROW_FLAG_NO_TR07].to_i
  flag_state << in_data[ROW_FLAG_NO_TR08].to_i
  flag_state << in_data[ROW_FLAG_NO_TR09].to_i
  flag_state << in_data[ROW_FLAG_NO_TR10].to_i
  flag_state << in_data[ROW_FLAG_NO_TR11].to_i
  flag_state << in_data[ROW_FLAG_NO_TR12].to_i
  flag_state << in_data[ROW_FLAG_NO_TR13].to_i
  flag_state << in_data[ROW_FLAG_NO_TR14].to_i
  flag_state << in_data[ROW_FLAG_NO_TR15].to_i
  flag_state << in_data[ROW_FLAG_NO_TR16].to_i
  # �o�̓f�[�^���쐬
  out_data = Array.new
  out_data << GetSeqNo(in_data[ROW_SEQ_LABEL])        # �V�[�P���X�ԍ�
  out_data << CalcTrackBit(flag_state, 0)             # �X�C�b�`0�̑���r�b�g
  out_data << CalcTrackBit(flag_state, 1)             # �X�C�b�`1�̑���r�b�g
  out_data << CalcTrackBit(flag_state, 2)             # �X�C�b�`2�̑���r�b�g
  out_data << CalcTrackBit(flag_state, 3)             # �X�C�b�`3�̑���r�b�g
  out_data << CalcTrackBit(flag_state, 4)             # �X�C�b�`4�̑���r�b�g
  out_data << CalcTrackBit(flag_state, 5)             # �X�C�b�`5�̑���r�b�g
  out_data << CalcTrackBit(flag_state, 6)             # �X�C�b�`6�̑���r�b�g
  out_data << CalcTrackBit(flag_state, 7)             # �X�C�b�`7�̑���r�b�g
  out_data << CalcTrackBit(flag_state, 8)             # �X�C�b�`8�̑���r�b�g
  out_data << in_data[ROW_FADE_FRAME].to_i            # �t�F�[�h �t���[����
  out_data << in_data.size - ROW_FIRST_ZONE_ID        # �L���]�[����
  ROW_FIRST_ZONE_ID.upto( in_data.size - 1 ) do |i|
    out_data << GetZoneID( "ZONE_ID_" + in_data[i] )  # �L���]�[��
  end

  # �f�o�b�O�f�[�^���o��
  filename = ARGV[1] + "/iss_switch_data_" + in_data[ROW_SEQ_LABEL].downcase + ".txt"
  file = File.open( filename, "w" )
  file.puts ( GetSeqNo(in_data[ROW_SEQ_LABEL]) )       # �V�[�P���X�ԍ�
  file.puts ( CalcTrackBit(flag_state, 0)      )       # �X�C�b�`0�̑���r�b�g
  file.puts ( CalcTrackBit(flag_state, 1)      )       # �X�C�b�`1�̑���r�b�g
  file.puts ( CalcTrackBit(flag_state, 2)      )       # �X�C�b�`2�̑���r�b�g
  file.puts ( CalcTrackBit(flag_state, 3)      )       # �X�C�b�`3�̑���r�b�g
  file.puts ( CalcTrackBit(flag_state, 4)      )       # �X�C�b�`4�̑���r�b�g
  file.puts ( CalcTrackBit(flag_state, 5)      )       # �X�C�b�`5�̑���r�b�g
  file.puts ( CalcTrackBit(flag_state, 6)      )       # �X�C�b�`6�̑���r�b�g
  file.puts ( CalcTrackBit(flag_state, 7)      )       # �X�C�b�`7�̑���r�b�g
  file.puts ( CalcTrackBit(flag_state, 8)      )       # �X�C�b�`8�̑���r�b�g
  file.puts ( in_data[ROW_FADE_FRAME].to_i     )       # �t�F�[�h �t���[����
  file.puts ( in_data.size - ROW_FIRST_ZONE_ID )       # �L���]�[����
  ROW_FIRST_ZONE_ID.upto( in_data.size - 1 ) do |i|
    file.puts ( GetZoneID( "ZONE_ID_" + in_data[i] ) )  # �L���]�[��
  end
  file.close

  # �o�C�i���f�[�^���o��
  filename = ARGV[1] + "/iss_switch_data_" + in_data[ROW_SEQ_LABEL].downcase + ".bin"
  file = File.open( filename, "wb" )
  file.write( out_data.pack("ISSSSSSSSSSCS*") )
  file.close

  # �o�̓t�@�C�������L��
  bin_file_list << filename
end

# �o�͂����o�C�i���t�@�C������\��
bin_file_list.each do |filename|
  puts "-output: #{filename}"
end 
