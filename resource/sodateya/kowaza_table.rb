# �n�b�V���̎�荞��
require "C:/home/pokemon_wb/tools/hash/monsno_hash"   # �����X�^�[No.
require "C:/home/pokemon_wb/tools/hash/wazaoboe_hash" # �Z�o��


#===================================================================================
# �q�Z�e�[�u��
#===================================================================================
DATA_INDEX_MONS_NO      = 0   # �����X�^�[�i���o�[
DATA_INDEX_MONS_NAME    = 2   # �����X�^�[��
DATA_INDEX_EGG_GROUP_1  = 5   # �q���O���[�v1
DATA_INDEX_EGG_GROUP_2  = 6   # �q���O���[�v2
DATA_INDEX_KOWAZA_START = 7   # �q�Z���X�g�J�n�ʒu

file = File.open( "kowaza_table.txt", "r" )
$kowaza_table_data = file.readlines
file.close 


#-----------------------------------------------------------------------------------
# @brief �q�Z���X�g���擾����
# @param mons_no �����X�^�[�i���o�[
# @return �w�胂���X�^�[�̎q�Z���X�g
#-----------------------------------------------------------------------------------
def GetKowazaList_byMonsNo( mons_no )
  # �z����쐬
  list = Array.new

  # �Y������s�f�[�^���擾
  line = $kowaza_table_data[mons_no]
  if line==nil then return list end
  data = line.split(/\s/)

  # ���ׂĂ̎q�Z�����X�g�ɒǉ�
  if data!=nil then
    DATA_INDEX_KOWAZA_START.upto( data.size-1 ) do |index|
      waza = data[index]
      if waza!=nil then
        list << waza
      end
    end
  end
  return list
end


#-----------------------------------------------------------------------------------
# @brief �q�Z���X�g���擾����
# @param mons_name �����X�^�[��
# @return �w�胂���X�^�[�̎q�Z���X�g
#-----------------------------------------------------------------------------------
def GetKowazaList( mons_name )

  # �����X�^�[�ԍ����擾
  mons_no = GetMonsNo( mons_name )

  # �����X�^�[�ԍ����烊�X�g���擾
  return GetKowazaList_byMonsNo( mons_no ) 
end


#-----------------------------------------------------------------------------------
# @brief �w�肵���q���O���[�v�����|�P�����������X�g�A�b�v����
# @param group �q���O���[�v1���w��
# @return �q���O���[�v group �����|�P�������z��
#-----------------------------------------------------------------------------------
def GetPokemonListByEggGroup( group )

  # �z����쐬
  list = Array.new

  # �q�Z�e�[�u��������
  1.upto( $kowaza_table_data.size - 1 ) do |i|
    data = $kowaza_table_data[i].split(/\s/)
    # �w��O���[�v�����|�P��������o�^
    if data[DATA_INDEX_MONS_NAME]!=nil then
       if data[DATA_INDEX_EGG_GROUP_1].to_i==group ||
          data[DATA_INDEX_EGG_GROUP_2].to_i==group then
         list << data[DATA_INDEX_MONS_NAME]
       end
    end
  end 
  return list
end


#-----------------------------------------------------------------------------------
# @brief �w�肵���Z���o����|�P���������X�g�A�b�v����
# @param waza_name �Z��
# @return �w�肵���Z�����x���A�b�v���Z�}�V���Ŋo����|�P�������̔z��
#-----------------------------------------------------------------------------------
def GetPokemonListByWaza( waza_name ) 

  # �z����쐬
  list = Array.new

  # �w��Z���o����|�P����������
  1.upto( $kowaza_table_data.size - 1 ) do |i|
    data = $kowaza_table_data[i].split(/\s/)
    name = data[DATA_INDEX_MONS_NAME]
    waza_list = $wazaoboe_hash[name]
    # �o���Z���X�g����w��Z������
    if waza_list!=nil then
      waza_list.each do |waza|
        if waza==waza_name then
          list << name
        end
      end
    end
  end 

  return list
end




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
