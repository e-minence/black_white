##############################################################
#
#
#	�}�b�v�}�g���b�N�X�p�ȈՃR���o�[�^
#	2009.04.13	tamada
#
#	resource/fldmapdata/map_matrix/map_matrix.xls��
#	�^�u��؂�e�L�X�g�ɕϊ��������̂��󂯎��A
#	���L�̌`���̒�`�t�@�C���𐶐�����
#
#	#define MAPMATRIX_�`	���l
#
#
##############################################################

#�擪�s�ǂݔ�΂�
gets

count = 0
while(line = gets)
	words = line.split
	if words[0] =~ /\#END/ then break end
	printf( "\#define MATRIX_ID_%-16s %3d\n", words[0].upcase, count )
	count += 1
end
