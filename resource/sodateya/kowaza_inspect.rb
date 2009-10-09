#! ruby -Ks
#############################################################################################
#
# @brief  �q�Z���K���\���ǂ��������؂���
# @file   kowaza_inspect.rb
# @author obata
# @date   2009.10.09
#
############################################################################################# 
require "C:/home/pokemon_wb/tools/hash/monsno_hash"   # �����X�^�[No.
require "C:/home/pokemon_wb/tools/hash/wazaoboe_hash" # �Z�o��
require "kowaza_table"
require "potential_table"

#============================================================================================
# �ƌv�}
#============================================================================================
$family_tree = Array.new

#--------------------------------------------------------------------------------------------
# �ƌn�}��\������
#--------------------------------------------------------------------------------------------
def DispFamilyTree
  $family_tree.each do |name|
    print "#{name}--"
  end
  puts ""
end


#============================================================================================
# �`�F�b�N�ς݃��X�g
#============================================================================================
$check = Array.new( GetMaxMonsNo() )
0.upto( $check.size-1 ) do |i|
  $check[i] = false
end


#--------------------------------------------------------------------------------------------
# �w�肵���Z���o���������X�^�[�̉ƌn�}��T��
#--------------------------------------------------------------------------------------------
def SearchOrigin( monsname, wazaname ) 
  monsno = GetMonsNo( monsname )
  data = $kowaza_table_data[monsno].split(/\s/)
  group1 = data[DATA_INDEX_EGG_GROUP_1].to_i
  group2 = data[DATA_INDEX_EGG_GROUP_2].to_i

  $family_tree.push( monsname )
  $check[monsno] = true
  DispFamilyTree()

  # ���ɂȂ�\���̂���S�Ẵ|�P����������
  father_list = GetPokemonListByEggGroup(group1) | GetPokemonListByEggGroup(group2)
  father_list.each do |father|
    if $check[ GetMonsNo(father) ]==false then
      waza_list = $wazaoboe_hash[father]
      waza_list.each do |waza|
        if wazaname==waza then  # ���͂Ŋo���镃���𔭌�
          $family_tree.push( father )
          return true
        end
      end
    end
  end

  # �����Ɏ��͂Ŋo������̂����Ȃ�������, �c�^�T�����s��
  father_list.each do |father|
    if $check[ GetMonsNo(father) ]==false then
      result = SearchOrigin( father, wazaname )
      if result==true then
        return true
      end
    end
  end

  # ������Ȃ������ꍇ
  file_tree.pop
  return false 
end


#--------------------------------------------------------------------------------------------
# ���C��
#--------------------------------------------------------------------------------------------
result = SearchOrigin( ARGV[0], ARGV[1] )
puts "================================================="
if result==true then DispFamilyTree() end
