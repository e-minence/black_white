################################################################################# 
#
# @brief  .ica�t�@�C���R���o�[�^
# @file   ica_converter.rb
# @author obata
# @date   2009.09.15
#
# [�T�v]
# ica�t�@�C�����̃f�[�^���o�C�i���t�@�C���ɏo�͂��܂��B
# �I�v�V�����w��ɂ��, �K�v�ȃf�[�^�݂̂����o�����Ƃ��o���܂��B
#
# [�g�p�@]
# ruby ica_converter.rb [�I�v�V�����w��] [ica�t�@�C����] [bin�t�@�C����]
#
# [�I�v�V�����w��]
# �ȉ��̃I�v�V�������w�肷�邱�ƂŁA
# bin�t�@�C���ɏo�͂���f�[�^�̑g�ݍ��킹��I�����邱�Ƃ��o���܂��B
# -s   scale �̂�
# -r   rotate �̂�
# -t   translate �̂�
# -sr  scale�Erotate
# -st  scale�Etranslate
# -rt  rotate�Etranslate
# -srt scale�Erotate�Etranslate
#
# [�o�C�i���f�[�^�t�H�[�}�b�g (�I�v�V������ -srt ���w�肵���ꍇ)]
# [ 0]: �A�j���[�V�����t���[���� (int)
# [ 4]: scale x (float)         |
# [ 8]: scale y (float)         |
# [12]: scale z (float)         | 
# [16]: rotate x (float)        |
# [20]: rotate y (float)        |------1�t���[���ڂ̃f�[�^
# [24]: rotate z (float)        |
# [28]: translate x (float)     |
# [32]: translate y (float)     |
# [36]: translate z (float)     |
# �ȉ��A�S�t���[�����̃f�[�^������
# �܂��A�I�v�V�����w��ɊY�����Ȃ��f�[�^�͍팸����܂��B
# 
#
#################################################################################

# ��̓v���O������荞��
lib_path = File.dirname(__FILE__).gsub(/\\/,"/") + "/ica_parser"
require lib_path

#================================================================================
# @brief ���C��
#
# @param ARGV[0] �I�v�V�����w��
# @param ARGV[1] �R���o�[�g�Ώ�ica�t�@�C����
# @param ARGV[2] �o��bin�t�@�C����
#================================================================================
# �o�̓f�[�^�̑I��
output_scale     = false
output_rotate    = false
output_translate = false

#--------------------
# �R�}���h�`�F�b�N
#--------------------
# �����̐��`�F�b�N
if ARGV.size!=3 then
	abort( "error: �����Ɍ�肪����܂��B" )
end

# �I�v�V�����`�F�b�N
if ARGV[0].index( "s" )!=nil then 
	output_scale = true
end
if ARGV[0].index( "r" )!=nil then 
	output_rotate = true
end
if ARGV[0].index( "t" )!=nil then 
	output_translate = true
end

# �R���o�[�g�Ώۃt�@�C���̑��݃`�F�b�N
if File.exist?(ARGV[1])==false then
	abort( "error: �w�肳�ꂽica�t�@�C���͑��݂��܂���B(#{ARGV[1]})" )
end

# �R���o�[�g�Ώۃt�@�C���̊g���q�`�F�b�N
if File::extname(ARGV[1])!=".ica" then
	abort( "error: �R���o�[�g�Ώۂ�ica�t�@�C�����w�肵�Ă��������B(#{ARGV[1]})" )
end

#----------------------
# ica�t�@�C���̓ǂݍ���
#----------------------
ica = Ica.new
ica.read( ARGV[1] )

#--------------------
# �o�C�i���f�[�^�o��
#--------------------
# �o�̓t�@�C���I�[�v��
file = File.open( ARGV[2], "wb" )

# �A�j���[�V�����t���[�������o��
file.write( [ica.frameSize].pack( "I" ) )

# �S�t���[�����̃A�j���[�V�����f�[�^���o��
0.upto( ica.frameSize-1 ) do |i|
	# scale
	if output_scale==true then
		file.write( ica.GetScale(i).pack( "fff" ) )
	end
	# rotate
	if output_rotate==true then
		file.write( ica.GetRotate(i).pack( "fff" ) )
	end
	# translate
	if output_translate==true then
		file.write( ica.GetTranslate(i).pack( "fff" ) )
	end
end

# �o�̓t�@�C���N���[�Y
file.close
