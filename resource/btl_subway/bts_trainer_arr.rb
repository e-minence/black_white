#=====================================================
#
# �o�g���T�u�E�F�C�p�g���[�i�[���z��̐���
# 2010.03.29  tamada
# 
# tools/btl_subway/trainer.txt�̃g���[�i�[�������̂܂܂�
# �����ɂ����z��𐶐�����
#
#
# �o�g���^���[���[�J�[�̏o�͂ɂ����āA�g���[�i�[��ʂ�
# tools/btl_subwa/trainer.txt���̃g���[�i�[���ւ̃C���f�b�N�X���l��
# �o�͂���Ă���B
# �����tools/hash/trtype_hash.rb�������ďo�͂ł���悤��
# �ϊ�����K�v������A�b��ō쐬�����B
#
#
#=====================================================
puts "\$bts_trainername_array = [\n"

count = 0
File.open(ARGV[0]){|file|
  file.each{|line|
    printf("\"%s\", # %d\n", line.chomp, count )
    count += 1
  }
}

puts "]\n"
