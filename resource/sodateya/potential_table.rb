require "kowaza_table"

#===================================================================================
# �萔
#===================================================================================
# �^�}�S�O���[�v�̐�
EGG_GROUP_NUM = 15


#===================================================================================
# ��c�\���e�[�u��
#===================================================================================
# ������
$potential_table = Array.new( EGG_GROUP_NUM+1 )
0.upto( EGG_GROUP_NUM ) do |i|
  $potential_table[i] = Array.new( EGG_GROUP_NUM+1 )
  0.upto( EGG_GROUP_NUM ) do |j|
    $potential_table[i][j] = false
  end
end


# ��c�\���e�[�u�����쐬
1.upto( EGG_GROUP_NUM ) do |my_group|
  # �`�F�b�N���X�g
  check = Array.new( EGG_GROUP_NUM+1 )
  0.upto( EGG_GROUP_NUM ) do |i|
    check[i] = false
  end 
  
  # ���O���[�v����`�F�b�N���J�n����
  $potential_table[my_group][my_group] = true
  check_group = my_group

  # �`�F�b�N�ΏۃO���[�v���Ȃ��Ȃ�܂ŌJ��Ԃ�
  while check_group!=nil
    # �`�F�b�N�ΏۃO���[�v�����|�P���������X�g�A�b�v
    poke_list = GetPokemonListByEggGroup( check_group ) 
    # ���X�g�A�b�v�����|�P�����̎��O���[�v���c�e�[�u���ɓo�^
    poke_list.each do |poke_name|
      monsno = GetMonsNo( poke_name )
      data = $kowaza_table_data[ monsno ].split(/\s/)
      g1 = data[DATA_INDEX_EGG_GROUP_1].to_i
      g2 = data[DATA_INDEX_EGG_GROUP_2].to_i
      $potential_table[my_group][g1] = true
      $potential_table[my_group][g2] = true
    end
    # �`�F�b�N���X�g���X�V
    check[check_group] = true
    # ���̃`�F�b�N�ΏۃO���[�v��T��
    check_group = nil
    1.upto( EGG_GROUP_NUM ) do |g|
      if $potential_table[my_group][g]==true &&
         check[g]==false then
        check_group = g
        break
      end
    end
  end

  puts "��c�\���e�[�u���쐬: �q���O���[�v[#{my_group}]����"
end


# ��c�\���e�[�u����\��
1.upto( EGG_GROUP_NUM ) do |my_group|
  1.upto( EGG_GROUP_NUM ) do |your_group|
    if $potential_table[my_group][your_group]==true then
      print "��"
    else
      print "�~"
    end
  end
  puts
end
