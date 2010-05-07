#==================================================
# wifiearth_gmm.rb
# �G�N�Z���t�@�C������擾���������E�n�於��
# gmm�t�@�C���ɕϊ�����X�N���v�g
#
# 2010/05/04 mori
#
# �����P�F�Q�Ƃ���CSV�t�@�C��
# �����Q�F�o�͂���GMM�t�@�C��
# �����t�H���_�Ɂutemplate.gmm�v�t�@�C�������݂��Ă���K�v������܂��B
#
# �g�p��j
# ruby wifiearth_gmm.rb world.csv wifi_earth_msg_world.gmm
#
#==================================================
require "../../tools/gmm_make/gmm_make"   # gmm�����N���X
require "csv"

#GMM�N���X������
gmm = GMM::new
gmm.open_gmm( "template.gmm", ARGV[1] )

#csv�t�@�C���I�[�v��
csv = CSV.open(ARGV[0],"r")
csv.each{|row|
	# ��s���uID�v�̍s�͏o�͂��Ȃ��悤�ɂ���
	if (row[0]!=nil && row[0].to_a.index("ID")==nil) then
		gmm.make_row_kanji( row[0], row[1], row[1] )
#		printf("%s, %s\n", row[0].to_a, row[1].to_a)
	end
}

#GMM�o��
gmm.close_gmm
