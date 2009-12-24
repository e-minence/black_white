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

  puts( "===============================================================" );
  puts( "error: �w�肳�ꂽ�]�[��ID[# str_zone_id}]�͒�`����Ă��܂���B" );
  puts( "===============================================================" );
end

#----------------------------------------------------------------------------------
# @brief �L�[�̒l���v���O�����Ŏg�p����l�ɕϊ�����
# @param key �ϊ�����l
# @return �ϊ���̒l
#----------------------------------------------------------------------------------
def ConvertKey( key )

  # �L�[�̒l��64�{����(64=1����)
  return key * 64
end

#----------------------------------------------------------------------------------
# @brief �e���|�̒l���v���O�����Ŏg�p����l�ɕϊ�����
# @param tempo �ϊ�����l
# @return �ϊ���̒l
#----------------------------------------------------------------------------------
def ConvertTempo( tempo )

  # 256 �����{�ɂȂ�悤�ɕϊ�
  return tempo * 256
end

#----------------------------------------------------------------------------------
# @brief ���o�[�u�̒l���v���O�����Ŏg�p����l�ɕϊ�����
# @param reverb �ϊ�����l
# @return �ϊ���̒l
#----------------------------------------------------------------------------------
def ConvertReverb( reverb )

  # ���o�[�u�̒l��64�{����
  return reverb * 64
end


#----------------------------------------------------------------------------------
# @brief main
# @param ARGV[0] �R���o�[�g�Ώۃt�@�C����(�G�N�Z���̃^�u��؂�f�[�^)
# @param ARGV[1] �o�C�i���f�[�^�̏o�͐�f�B���N�g��
#----------------------------------------------------------------------------------

# �f�[�^�E�C���f�b�N�X
ROW_ZONE          = 0   # �]�[��ID
ROW_KEY_SPRING    = 1   # �L�[(�t)
ROW_KEY_SUMMER    = 2   # �L�[(��)
ROW_KEY_AUTUMN    = 3   # �L�[(�H)
ROW_KEY_WINTER    = 4   # �L�[(�~)
ROW_TEMPO_SPRING  = 5   # �e���|(�t)
ROW_TEMPO_SUMMER  = 6   # �e���|(��)
ROW_TEMPO_AUTUMN  = 7   # �e���|(�H)
ROW_TEMPO_WINTER  = 8   # �e���|(�~)
ROW_REVERB_SPRING = 9   # ���o�[�u(�t)
ROW_REVERB_SUMMER = 10  # ���o�[�u(��)
ROW_REVERB_AUTUMN = 11  # ���o�[�u(�H)
ROW_REVERB_WINTER = 12  # ���o�[�u(�~)

# �o�̓t�@�C�����̃��X�g
bin_file_list = Array.new

# �t�@�C���f�[�^��ǂݍ���
file = File.open( ARGV[0], "r" )
file_lines = file.readlines
file.close

# 3�s�ڈȍ~�̑S���C�����R���o�[�g
2.upto( file_lines.size - 1 ) do |i|
  # �R���o�[�g�Ώۂ̃f�[�^���擾
  line    = file_lines[i]
  in_data = line.split(/\s/)
  # ��f�[�^�𔭌�==>�I��
  if in_data[0]==nil then break end 
  # �o�̓f�[�^���쐬
  out_data = Array.new
  out_data << GetZoneID("ZONE_ID_"+in_data[ROW_ZONE])  # �]�[��ID
  out_data << 0   # padding
  0.upto(3) do |season|
    out_data << ConvertKey(in_data[ROW_KEY_SPRING+season].to_i)  # �L�[
  end
  0.upto(3) do |season|
    out_data << ConvertTempo(in_data[ROW_TEMPO_SPRING+season].to_f)  # �e���|
  end
  0.upto(3) do |season|
    out_data << ConvertReverb(in_data[ROW_REVERB_SPRING+season].to_i)  # ���o�[�u
  end
  # �o�C�i���f�[�^���o��
  filename = ARGV[1] + "/iss_dungeon_data_" + in_data[ROW_ZONE].downcase + ".bin"
  file = File.open( filename, "wb" )
  file.write( out_data.pack("SSssssSSSSSSSS") )
  file.close
  # �o�̓t�@�C�������L��
  bin_file_list << filename
end

# �o�͂����o�C�i���t�@�C������\��
bin_file_list.each do |filename|
  puts "-output: #{filename}"
end 
