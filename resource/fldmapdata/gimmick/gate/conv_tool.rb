######################################################################################
# @brief �R���o�[�g���ʃ��W���[��
# @file  conv_tool.rb
# @autor obata
# @date  2009.12.14
######################################################################################

#-------------------------------------------------------------------------------------
# @brief �]�[��ID���擾����
# @param zone �]�[��ID��\��������
# @return �w�肵���]�[��ID�̒l
#-------------------------------------------------------------------------------------
def GetZoneID( zone )

  # ��`�t�@�C�����J��
  filename = ENV["PROJECT_RSCDIR"] + "fldmapdata/zonetable/zone_id.h"
  file = File.open( filename, "r" )

  # �w�肳�ꂽ�]�[��ID������
  file.each do |line|
    if line.index(zone) != nil then
      i0 = line.index("(") + 1
      i1 = line.index(")") - 1
      val = line[i0..i1].to_i
      return val
    end
  end
  file.close

  # �w�肳�ꂽ�]�[��ID����`����Ă��Ȃ��ꍇ
  abort("�]�[��ID:#{zone}�͒�`����Ă��܂���")

end

#-------------------------------------------------------------------------------------
# @brief �o�[�W�������擾����
# @param version "WB" or "W" or "B"
# @return "W"=>VERSION_WHITE, "B"=>VERSION_BLACK, "WB"=>0
#-------------------------------------------------------------------------------------
def GetVersion( version )

  # "WB"���w�肳�ꂽ
  if version == "WB" then return 0 end

  # �n�b�V���e�[�u���쐬
  hash_table = Hash::new
  hash_table["W"] = "VERSION_WHITE"
  hash_table["B"] = "VERSION_BLACK" 
  # ���݂��Ȃ��L�[���w�肳�ꂽ
  if hash_table.has_key?(version) == false then
    abort("�o�[�W�����w��[#{version}]�͑Ή����Ă��܂���")
  end 
  # �w�肳�ꂽ�o�[�W�����ɂ���, �������[�h������
  target_word = hash_table[ version ]

  # ��`�t�@�C�����J��
  filename = ENV["PROJECT_PROGDIR"] + "include/pm_version.h"
  file = File.open( filename, "r" )
  file_lines = file.readlines
  file.close

  # �������[�h�̒�`�l���擾
  file_lines.each do |line|
    if (line =~ /#define\s*#{target_word}\s*(\d+)/) != nil then return $1.to_i end
  end

  # �w�肳�ꂽ�o�[�W��������`����Ă��Ȃ��ꍇ
  abort("�o�[�W����:#{target_word}�͒�`����Ă��܂���")
end

#-------------------------------------------------------------------------------------
# @brief ���b�Z�[�WID���擾����
# @param gmm_filename gmm�t�@�C����
# @param msg_id       ���b�Z�[�WID��\��������
# @return �w�肵�����b�Z�[�WID�̒l
#-------------------------------------------------------------------------------------
def GetMsgID( gmm_filename, msg_id )

  # ��`�t�@�C�����J��
  def_filename = 
    ENV["PROJECT_PROGDIR"] + "include/msg/msg_" + File.basename(gmm_filename, ".gmm") + ".h"
  file = File.open( def_filename, "r" )

  # �w�肳�ꂽ���b�Z�[�WID������
  file.each do |line|
    if line.index(msg_id) != nil then
      i0 = line.index("(") + 1
      i1 = line.index(")") - 1
      val = line[i0..i1].to_i
      return val
    end
  end
  file.close

  # �w�肳�ꂽ���b�Z�[�WID����`����Ă��Ȃ��ꍇ
  abort("���b�Z�[�WID:#{msg_id}�͒�`����Ă��܂���")

end

#-------------------------------------------------------------------------------------
# @brief �w�肵�������ɑΉ�����l���擾����
# @param dir ������\��������(DIR_xxxx)
# @return �w�肵��������\���l
#-------------------------------------------------------------------------------------
def GetDir( dir )

  # ��`�t�@�C�����J��
  def_filename = ENV["PROJECT_PROGDIR"] + "include/field/field_dir.h"
  file = File.open( def_filename, "r" )

  # �w�肳�ꂽ����������
  file.each do |line|
    if line.index(dir) != nil then
      i0 = line.index("(") + 1
      i1 = line.index(")") - 1
      val = line[i0..i1].to_i
      return val
    end
  end
  file.close

  # �w�肳�ꂽ��������`����Ă��Ȃ��ꍇ
  abort("����:#{dir}�͒�`����Ă��܂���")
end

#-------------------------------------------------------------------------------------
# @brief �t���OID���擾����
# @param flag �t���OID��\��������
# @return �w�肵���t���OID�̒l
#-------------------------------------------------------------------------------------
def GetFlagID( flag )

  # ��`�t�@�C�����J��
  filename = ENV["PROJECT_RSCDIR"] + "fldmapdata/flagwork/flag_define.h"
  file = File.open( filename, "r" )

  # �w�肳�ꂽ�t���OID������
  file.each do |line|
		if line.index(/#define.*#{flag}\s*(\d*)\s*/) != nil then 
			return $1.to_i
		end
  end
  file.close

  # �w�肳�ꂽ�t���OID����`����Ă��Ȃ��ꍇ
  abort("�t���OID:#{flag}�͒�`����Ă��܂���")

end
