#! ruby -Ks
#############################################################################################
#
# @brief  �q�Z���K���\���ǂ��������؂���
# @file   kowaza_inspect.rb
# @author obata
# @date   2009.10.09
#
# �T�v
#  �w�肵���|�P������, �w�肵���Z���q�Z�Ƃ��Čp��������̂��𒲂ׂ�
#
# �g�p���@
#  ruby kowaza_inspect.rb �|�P������ �Z��
#
############################################################################################# 
require "C:/home/pokemon_wb/tools/hash/monsno_hash"   # �����X�^�[No.
require "C:/home/pokemon_wb/tools/hash/wazaoboe_hash" # �Z�o��
require "kowaza_table"
require "potential_table"


#============================================================================================
# ���`�F�b�N�ς݃��X�g
#  check[�����X�^�[�i���o�[]==true �Ȃ�, ���̃����X�^�[�̓`�F�b�N�ς�
#============================================================================================
$check = Array.new( GetMaxMonsNo() )
0.upto( $check.size-1 ) do |i|
  $check[i] = false
end


#============================================================================================
# ���ƌn�}
#============================================================================================
$family_tree = Array.new

#--------------------------------------------------------------------------------------------
# @brief �ƌn�}��\������
#--------------------------------------------------------------------------------------------
def DispFamilyTree
  $family_tree.each do |name|
    print "#{name} ==> "
  end
  puts ""
end


#--------------------------------------------------------------------------------------------
# @brief �w�肵���Z���o���������X�^�[�̉ƌn�}��T��
#        �������ʂ� $family_tree �Ɋi�[����
# @param monsname �����X�^�[��
# @param wazaname �Z��
# @return �w�肵���Z�����͂Ŋo����c�悪���݂���ꍇ true
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
# @breif ���C��
# @param ARGV[0] �����X�^�[��
# @param ARGV[1] �Z��
#--------------------------------------------------------------------------------------------
result = SearchOrigin( ARGV[0], ARGV[1] )
if result==true then 
  puts "-----------------------------------------------------"
  puts "#{ARGV[1]} ���o���� #{ARGV[0]} �̐�c�𔭌����܂����B"
  puts "-----------------------------------------------------"
  DispFamilyTree() 
elsif
  puts "-------------------------------------------------------------"
  puts "#{ARGV[1]} ���o���� #{ARGV[0]} �̐�c�𔭌��ł��܂���ł����B"
  puts "-------------------------------------------------------------"
end
