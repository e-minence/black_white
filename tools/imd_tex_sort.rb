#################################################################################
# 
# @brief  imd�t�@�C����<tex_image>�v�f����R���o�[�^
# @file   imd_tex_sort.rb
# @author obata
# @date   2009.08.31
#
# <tex_image>�v�f��name�������L�[�ɂ��ĕ��בւ��܂��B
# name������ *.no �ł��邱�Ƃ�z�肵�Ă��܂��B
# "no"�̕����ɔԍ�������܂��B
# "no"�̏����ɕ��בւ��܂��B
#
# "no"��1�I���W����z�肵�Ă��܂��B
# index��0�I���W���ł��B
#
# [�g�p�@]
# ruby imd_tex_sort.rb "�R���o�[�g�Ώۃt�@�C����" "�o�͐�ւ̃p�X/�ۑ��t�@�C����"
#
#################################################################################

#----------------------------------------------------------------------
# @brief �e�N�X�`���̔ԍ����擾����
#
# @param tex_image <tex_image>�v�f��\��������
#
# @return �e�N�X�`�����Ɋ܂܂��ԍ�
#----------------------------------------------------------------------
def GetTexIndex( tex_image )

  tex_image.each do |line|

    # "name=" �𔭌�
    if line.index( "name=" )!=nil then
      i0 = line.index( "name=" )
      i1 = line.index( "\"", i0+1 ) + 1
      i2 = line.index( "\"", i1+1 ) - 1
      tex_name = line[ i1..i2 ]
      i3 = tex_name.index( "." ) + 1
      tex_no = tex_name[ i3..tex_name.length ]
      no = tex_no.to_i
      return no
    end

  end

end

#----------------------------------------------------------------------
# @brief <tex_image> �� index������ύX����
#
# @param tex_image index������ύX���镶����
# @param index     �ݒ肷��index�̒l
#
# @return index������ύX����������
#----------------------------------------------------------------------
def SetIndexAttribute( tex_image, index )

  # index�����ύX���<tex_image>
  new_tex_image = ""

  tex_image.each do |line|

    # "index=" �𔭌�
    if line.index( "index=" )!=nil then 
      i0 = line.index( "index=" )
      i1 = line.index( "\"", i0+1 )
      i2 = line.index( "\"", i1+1 )
      new_tex_image += line[0..i1]
      new_tex_image += index.to_s
      new_tex_image += line[i2..line.length]
    else
      new_tex_image += line
    end

  end

  return new_tex_image

end


#----------------------------------------------------------------------
# @brief <tex_image>�v�f�𒊏o����
#
# @param filename ���o�Ώۂ�imd�t�@�C����
#
# @return <tex_image>�v�f�̔z��
#----------------------------------------------------------------------
def ExtractTexImage( filename )

  cnv_flag     = false      # <tex_image_array>�����Ȃ�true
  extract_flag = false      # <tex_image>�����Ȃ�true
  tex_image    = Array.new  # <tex_image>�z��
  imd_file     = File.open( filename, "r" )

  # <tex_image>���o�o�b�t�@
  str_tex_image = ""

  # �S�Ă�<tex_image>�v�f�𒊏o
  imd_file.each do |line|

    # "tex_image_array" �𔭌�
    if line.index( "tex_image_array" )!=nil then
      cnv_flag = !cnv_flag
      next
    end

    # "<tex_image" �𔭌�
    if cnv_flag==true && line.index( "<tex_image" )!=nil then
      extract_flag = true
    end

    # <tex_image> �v�f�𒊏o
    if extract_flag==true then
      str_tex_image += line
    end

    # "</tex_image" �𔭌�
    if cnv_flag==true && line.index( "</tex_image" )!=nil then
      extract_flag = true
      tex_image << str_tex_image
      str_tex_image = ""
    end

  end


  # ���o����<tex_image>�v�f��Ԃ�
  imd_file.close
  return tex_image

end



#----------------------------------------------------------------------
# @brief �w�肵��imd�t�@�C����<tex_image>�v�f���e�N�X�`�����Ő��񂷂�
#
# @param src_filename �����Ώۂ�imd�t�@�C����
# @param dst_filename �o�͐�t�@�C����
#----------------------------------------------------------------------
def SortTexImageArrayByTexName( src_filename, dst_filename )

  # <tex_image> �v�f�𒊏o
  tex_image = ExtractTexImage( src_filename )

  # <tex_image>�v�f����1�ȉ��Ȃ炻�̂܂܏o�͂���
  if tex_image.length <= 1 then
    src_file = File.open( src_filename, "r" )
    dst_file = File.open( dst_filename, "w" )
    dst_file.write( src_file.read )
    dst_file.close
    src_file.close
    return
  end

  # <tex_image> �v�f���e�N�X�`���ԍ��ŕ��בւ���
  tex_image_arranged = Array.new( tex_image.length )
  tex_image.each do |str|
    tex_index = GetTexIndex( str ) 
    tex_image_arranged[ tex_index-1 ] = SetIndexAttribute( str, tex_index-1 )
  end


  # �f�[�^������������
  data = ""
  file = File.open( src_filename, "r" ) 
  copy_flag = true
  file.each do |line|

    # "</tex_image_array" ���� ==> �R�s�[�J�n
    if line.index( "</tex_image_array" )!=nil then
      copy_flag = true
    end

    # �R�s�[
    if copy_flag==true then
      data << line
    end

    # "<tex_image_array" ���� ==> �R�s�[��~ & ���בւ���<tex_image>���R�s�[
    if line.index( "<tex_image_array" )!=nil then
      copy_flag = false
      tex_image_arranged.each do |str|
        data << str
      end
    end
  end
  file.close


  # �����������f�[�^���t�@�C���ɏo��
  file = File.open( dst_filename, "w" )
  file.write( data )
  file.close

end

#----------------------------------------------------------------------
# @brief ���C��
#
# ARGV[0] = �R���o�[�g�Ώ�imd�t�@�C����
# ARGV[1] = �o�̓t�@�C����
#----------------------------------------------------------------------
SortTexImageArrayByTexName( ARGV[0], ARGV[1] )
