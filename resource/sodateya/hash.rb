#! ruby -Ks
require "C:/home/pokemon_wb/tools/hash/monsno_hash"   # �����X�^�[No.
require "C:/home/pokemon_wb/tools/hash/wazaoboe_hash" # �Z�o��

#####################################################################################
#
# @brief  �e��n�b�V���� ���̃��[�e�B���e�B�[�֐��Q
# @file   hash.rb
# @author obata
# @date   2009.10.09
#
####################################################################################

#===================================================================================
# ���Z�ԍ��n�b�V�� 
# $wazano_hash[�Z��]�ŋZ�ԍ����擾
#===================================================================================
$wazano_hash = Hash::new

# �Z�n�b�V���쐬
file = File.open( "../../prog/include/waza_tool/wazano_def.h", "r" )
name = String::new
file.each do |line|
  if line.index("WAZANO_MAX")!=nil then break end
  if line.index("//")!=nil then
    i0 = line.index("//") + 2
    i1 = line.length - 1
    name = line[i0..i1]
    name.strip!
  end
  if line.index("define")!=nil then
    i0 = line.index("(") + 1
    i1 = line.index(")") - 1
    str = line[i0..i1]
    no = str.to_i
    $wazano_hash[name] = no
  end
end
file.close


#-----------------------------------------------------------------------------------
# @brief �Z�i���o�[���擾����
# @param name �Z��
# @return �Z�i���o�[
#-----------------------------------------------------------------------------------
def GetWazaNo( name )
  no = $wazano_hash[name]
  if no==nil then
    abort( "[#{name}]�͋Z�n�b�V���ɓo�^����Ă��܂���B" )
  end
  return no
end



#===================================================================================
# �������X�^�[��, �����X�^�[�ԍ�
#===================================================================================

#-----------------------------------------------------------------------------------
# @brief �����X�^�[���������X�^�[�i���o�[�ɕϊ�����
# @param name �����X�^�[��
# @return �����X�^�[�i���o�[
#-----------------------------------------------------------------------------------
def GetMonsNo( name )
  monsno = $monsno_hash[ name ]
  if monsno==nil then
    abort( "�����X�^�[�l�[��[#{name}]�̓n�b�V���ɓo�^����Ă��܂���B" )
  end
  return monsno
end

#-----------------------------------------------------------------------------------
# @brief �����X�^�[�i���o�[�̍ő�l���擾����
# @return �|�P������
#-----------------------------------------------------------------------------------
def GetMaxMonsNo()
  file = File.open( "../personal/monsno_def.h", "r" )
  file.each do |line|
    if line.index("MONSNO_END")!=nil then
      i0 = line.index("(") + 1
      i1 = line.index(")") - 1
      str = line[i0..i1]
      no = str.to_i
      return no
    end
  end
  file.close
  return 0
end
