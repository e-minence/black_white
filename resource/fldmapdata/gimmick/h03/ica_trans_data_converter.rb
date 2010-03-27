###############################################################################################
# @brief  ica�t�@�C���� <node_translate_data> �ɃI�t�Z�b�g�����Z����R���o�[�^
# @file   ica_trans_data_converter.rb
# @author obata
# @date   2009.11.20
#
# <�R���o�[�^�쐬�̗��R>
#  H01�̃M�~�b�N����A�j���[�V�������Đ������Ƃ���, �Ӑ}�����ʒu�ɕ\��������Ȃ��������ߍ쐬�B
#  ica�f�[�^��, H01�̃}�b�v�����ɒu���ꂽ���f���ɑ΂��č쐬���ꂽ���߂ɋN�����Ǝv����B
#  (�}�b�v���S�܂ł̃I�t�Z�b�g���K�v)
###############################################################################################


#----------------------------------------------------------------------------------------------
# @brief �w�肵�������l���擾����
# @param filename �t�@�C����
# @param tag      �^�O��
# @param element  ������
# @return �w�肵���^�O�̑����l
#
# [�O�����]
#  �^�O���Ƒ������͓���̍s�ɑ��݂��邱�ƁB
#  �����l�� element = "val" �̌`���ł��邱�ƁB
#----------------------------------------------------------------------------------------------
def GetElement( filename, tag, element )

  # �t�@�C����ǂݍ���
  file = File.open( filename, "r" )
  file_lines = file.readlines
  file.close

  # �^�O������
  file_lines.each do |line|
    if line.index(tag)!=nil && line.index(element)!=nil then
      i0 = line.index(element)
      i1 = line.index("\"", i0) + 1
      i2 = line.index("\"", i1) - 1
      val = line[i1..i2].to_i
      return val
    end
  end

  # ������Ȃ������ꍇ
  abort( "ERROR: �^�O<#{tag}> ����#{element} ��������܂���B" )
end

#----------------------------------------------------------------------------------------------
# @brief �w�肵���^�O�̒l��S�Ď擾����
# @param filename �t�@�C����
# @param tag      �擾����f�[�^�̃^�O��
# @return �擾�����S�f�[�^�̔z��
#
# [�O�����]
#  �f�[�^�J�n�^�O��, �f�[�^�����Ӗ����鑮�� size �������ƁB
#  �J�n�^�O�E�I���^�O�Ɠ���s�ɂ̓f�[�^���Ȃ����ƁB
#  �f�[�^�̓X�y�[�X��؂�ł��邱�ƁB
#  �]�v�ȋ󔒂���s�͑��݂��Ȃ����ƁB
#----------------------------------------------------------------------------------------------
def GetData( filename, tag )

  # �t�@�C����ǂݍ���
  file = File.open( filename, "r" )
  file_lines = file.readlines
  file.close

  # �f�[�^�z��
  value = Array.new

  # �J�n�^�O�����������ǂ���
  tag_open = false

  file_lines.each do |line|
    # �J�n�^�O���J���Ă���ꍇ
    if tag_open==true then
      # �I���^�O�𔭌�
      if line.index(tag)!=nil then break end
      # �f�[�^���擾
      line.strip!  # �O��̋󔒂��폜
      data = line.split(/\s/)
      data.each do |item|
        value << item.to_f
      end
    # �J�n�^�O���J���Ă��Ȃ��ꍇ
    else
      # �J�n�^�O�𔭌�
      if line.index(tag)!=nil then
        tag_open = true 
      end
    end
  end

  # �擾�����f�[�^�����`�F�b�N
  size = GetElement(filename, tag, "size")
  if value.size != size then
    abort("ERROR: ���f�[�^�擾�Ɏ��s")
  end
  return value 
end

#----------------------------------------------------------------------------------------------
# @brief �w�肵���S�f�[�^�ɃI�t�Z�b�g�����Z
# @param data      ���f�[�^�z��
# @param offset    �I�t�Z�b�g�z��(x, y, z)
# @param data_size �f�[�^���z��(x, y, z)
# @param data_head �f�[�^�擪�ʒu�z��(x, y, z)
# @return �I�t�Z�b�g���Z��̃f�[�^�z��
#----------------------------------------------------------------------------------------------
def AddOffset( data, offset, data_size, data_head )

  # ���Z��̃f�[�^�z��
  conv_data = Array.new

  0.upto(data.size - 1) do |idx|
    # �����Ώۂ�x, y, z��, �ǂ̒l�Ȃ̂��𔻒f���ĉ��Z����
    0.upto(2) do |type|
      if data_head[type]<=idx && idx<data_head[type]+data_size[type] then
        conv_data << data[idx] + offset[type]
      end
    end
  end

  # �f�[�^���`�F�b�N
  if data.size != conv_data.size then
    abort("ERROR: �I�t�Z�b�g���Z�Ɏ��s")
  end
  return conv_data
end

#----------------------------------------------------------------------------------------------
# @brief �R���o�[�g��̃f�[�^������ ica �t�@�C�����o�͂���
# @param filename_org ���f�[�^�̃t�@�C����
# @param filename_cnv �o�͐�t�@�C����
# @param conv_data    �R���o�[�g��̃f�[�^
# @param data_tag     �f�[�^�̃^�O��
#----------------------------------------------------------------------------------------------
def OutputConvertData( filename_org, filename_cnv, conv_data, data_tag )

  # ���t�@�C����ǂݍ���
  file = File.open( filename_org, "r" )
  file_lines = file.readlines
  file.close

  # �R���o�[�g��̃f�[�^���o�͂������ǂ���
  data_out = false

  # �^�O���J���Ă��邩�ǂ���
  tag_open = false

  # �R���o�[�g�f�[�^���o��
  file = File.open( filename_cnv, "w" )
  file_lines.each do |line_org|
    # �^�O���J���Ă��Ȃ��ꍇ
    if tag_open == false then
      # ���f�[�^���o��
      file.puts(line_org)
    end
    # �^�O�𔭌�
    if line_org.index(data_tag) != nil then
      # �^�O�󋵔��]
      tag_open = !tag_open
      # �f�[�^�o��
      if data_out == false then
        data_out = true
        0.upto(conv_data.size - 1) do |idx|
          file.write("#{conv_data[idx]} ")
          if idx%10==0 then
            file.write("\n")
          end
        end
        file.write("\n")
        # �^�O�����
        file.puts("</#{data_tag}>")
      end
    end
  end

  # �R���o�[�g�������
  file.puts("<converted!! original=""#{filename_org}""/>")
end

#----------------------------------------------------------------------------------------------
# @brief main
# @param ARGV[0] �R���o�[�g�Ώۂ�ica�t�@�C�����w��
# @param ARGV[1] �R���o�[�g��̃t�@�C����
# @param ARGV[2] ica�f�[�^�ɉ��Z����x�I�t�Z�b�g�l
# @param ARGV[3] ica�f�[�^�ɉ��Z����y�I�t�Z�b�g�l
# @param ARGV[4] ica�f�[�^�ɉ��Z����z�I�t�Z�b�g�l
#----------------------------------------------------------------------------------------------

# �f�[�^�����擾
data_size_tx = GetElement(ARGV[0], "translate_x", "data_size")
data_size_ty = GetElement(ARGV[0], "translate_y", "data_size")
data_size_tz = GetElement(ARGV[0], "translate_z", "data_size")
data_size = Array.new
data_size << data_size_tx
data_size << data_size_ty
data_size << data_size_tz
# �f�[�^�J�n�ʒu���擾
data_head_tx = GetElement(ARGV[0], "translate_x", "data_head")
data_head_ty = GetElement(ARGV[0], "translate_y", "data_head")
data_head_tz = GetElement(ARGV[0], "translate_z", "data_head")
data_head = Array.new
data_head << data_head_tx
data_head << data_head_ty
data_head << data_head_tz 
# �I�t�Z�b�g�l���擾
offset = Array.new
offset << ARGV[2].to_f
offset << ARGV[3].to_f
offset << ARGV[4].to_f

# ���f�[�^���擾
data = GetData(ARGV[0], "node_translate_data")

# ���f�[�^�ɃI�t�Z�b�g�����Z
conv_data = AddOffset( data, offset, data_size, data_head )

# �R���o�[�g��̃f�[�^���o��
OutputConvertData( ARGV[0], ARGV[1], conv_data, "node_translate_data" )
