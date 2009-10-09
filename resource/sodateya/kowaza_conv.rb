#! ruby -Ks

###################################################################################
#
# @brief  �q�Z�e�[�u�����o�C�i���փR���o�[�g����
# @file   kowaza_conv.rb
# @auther obata
# @date   2009.10.09
#
# �g�p���@
#  ruby kowaza_conv.rb �o�C�i���o�͐�̃p�X
#
###################################################################################

# �n�b�V���̎�荞��
require "C:/home/pokemon_wb/tools/hash/monsno_hash"   # �����X�^�[No.
require "C:/home/pokemon_wb/tools/hash/wazaoboe_hash" # �Z�o��
require "kowaza_table"
require "wazano_hash.rb" 



#-----------------------------------------------------------------------------------
# ���C��
#-----------------------------------------------------------------------------------

# �o�C�i���t�@�C�����X�g
file_list = Array.new

# �ő僂���X�^�[�i���o�[���擾
monsno_max = GetMaxMonsNo()

# �����X�^�[�i���o�[0�p�Ƀ_�~�[���o��
filename = "./#{ARGV[0]}/kowaza_data0.bin"
file = File.open( filename, "wb" )
file.close
file_list << filename

# �S�|�P�����̎q�Z�f�[�^���o��
1.upto( monsno_max ) do |monsno|

  # �q�Z���X�g���擾
  kowaza_list = GetKowazaList_byMonsNo( monsno )

  # �o�C�i���f�[�^���o��
  data = Array.new
  data << kowaza_list.size
  kowaza_list.each do |waza_name|
    data << GetWazaNo( waza_name )
  end
  filename = "./#{ARGV[0]}/kowaza_data#{monsno}.bin"
  file = File.open( filename, "wb" )
  file.print( data.pack("S*") )
  file.close
  file_list << filename
  puts "�q�Z�o�C�i���f�[�^�o��: #{filename}"
end


# �o�C�i���t�@�C�����X�g���o��
file = File.open( "conv_data.list", "w" ) 
file.print( "CONVERTDATA = " )
file_list.each do |filename|
  file.print( filename )
  file.print( " " )
end
file.close
