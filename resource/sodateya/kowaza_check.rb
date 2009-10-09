#! ruby -Ks

###################################################################################
#
# @brief  �q�Z�e�[�u���̐������`�F�b�N
# @file   kowaza_check.rb
# @auther obata
# @date   2009.10.09
#
###################################################################################

# �n�b�V���̎�荞��
require "C:/home/pokemon_wb/tools/hash/monsno_hash"   # �����X�^�[No.
require "C:/home/pokemon_wb/tools/hash/wazaoboe_hash" # �Z�o��
require "kowaza_table"
require "potential_table"


#-----------------------------------------------------------------------------------
# ���C��
#-----------------------------------------------------------------------------------

# �G���[���X�g
error_list = Array.new

# �S�|�P�������`�F�b�N
1.upto( $kowaza_table_data.size-1 ) do |i|
  data = $kowaza_table_data[i].split(/\s/)
  no   = data[DATA_INDEX_MONS_NO].to_i
  name = data[DATA_INDEX_MONS_NAME]
  group1 = data[DATA_INDEX_EGG_GROUP_1].to_i
  group2 = data[DATA_INDEX_EGG_GROUP_2].to_i
  if 0<no then
    puts "�q�Z�`�F�b�N: #{name}"
    # �q�Z���X�g���擾
    kowaza_list = GetKowazaList( name )
    # �S�Ă̎q�Z���`�F�b�N
    kowaza_list.each do |waza|
      print "-#{waza}"
      # �`�F�b�N�Ώۂ̎q�Z�����͂Ŋo����|�P���������X�g�A�b�v
      poke_list = GetPokemonListByWaza( waza )
      # ���X�g�A�b�v�����|�P�����̒�����, �`�F�b�N�Ώۃ|�P�����̐�c�ƂȂ蓾��|�P������T��
      ok = false
      poke_list.each do |father_name|
        father_monsno = $monsno_hash[father_name]
        father_data = $kowaza_table_data[father_monsno].split(/\s/)
        g1 = father_data[DATA_INDEX_EGG_GROUP_1].to_i
        g2 = father_data[DATA_INDEX_EGG_GROUP_2].to_i
        if $potential_table[group1][g1]==true ||
           $potential_table[group1][g2]==true ||
           $potential_table[group2][g1]==true ||
           $potential_table[group2][g2]==true then
          ok = true
        end
      end
      if ok==true then
        puts "��"
      else
        puts "�~"
        if group1!=15 && group2!=15 then  # �����B�O���[�v(15)�̌x���͏o���Ȃ��I
          error_list << "#{name} �� #{waza} ���o�����܂���!!"
        end
      end
    end
  end
end

# �G���[���X�g��\��
error_list.each do |message|
  puts message
end
