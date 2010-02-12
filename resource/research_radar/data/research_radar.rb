####################################################################################
#
# ��brief:  �������[�_�[ �f�[�^�R���o�[�^
# ��file:   research_radar.rb
# ��author: obata
# ��date:   2010.02.10
#
# ��param: ARGV[0] �R���o�[�g ���[�g�f�B���N�g��
# ��param: ARGV[1] �񓚃f�[�^
# ��param: ARGV[2] ����f�[�^
# ��param: ARGV[3] �������ڃf�[�^
# ��param: ARGV[4] ��f�[�^
# ��param: ARGV[5] gmm�t�@�C���̏o�͐�
# ��param: ARGV[6] �f�o�b�O�f�[�^�̏o�͐�
#
####################################################################################
# �J�����g�f�B���N�g����ύX
Dir::chdir( ARGV[0] )

require "tab_separated_file_reader.rb"  # for TabSeparatedFileReader
require "answer.rb"                     # for Answer
require "question.rb"                   # for Question
require "topic.rb"                      # for Topic
require "hobby.rb"                      # for Hobby
require "gmm_generator.rb"              # for GenerateGMM

FILENAME_ANSWER_DATA   = ARGV[1]  # �񓚃f�[�^
FILENAME_QUESTION_DATA = ARGV[2]  # ����f�[�^
FILENAME_TOPIC_DATA    = ARGV[3]  # �������ڃf�[�^
FILENAME_HOBBY_DATA    = ARGV[4]  # ��f�[�^
GMM_OUTPUT_DEST        = ARGV[5]  # gmm�t�@�C���̏o�͐�
DEBUG_OUTPUT_DEST      = ARGV[6]  # �f�o�b�O�f�[�^�̏o�͐�

answers   = Array.new  # �S�񓚃f�[�^
questions = Array.new  # �S����f�[�^
topics    = Array.new  # �S�������ڃf�[�^
hobbies   = Array.new  # ����X�g


#===================================================================================
# ��main
# ���S�񓚂̎擾
#===================================================================================
# ��C���f�b�N�X
COLUMN_ANSWER_ID        = 0  # ��ID
COLUMN_ANSWER_JPN       = 1  # �񓚕�����
COLUMN_ANSWER_JPN_KANJI = 2  # �񓚕�����
COLUMN_COLOR            = 3  # �F
# �s�C���f�b�N�X
ROW_FIRST_ANSWER = 1  # �擪�f�[�^�s

fileReader = TabSeparatedFileReader.new
fileReader.ReadFile( FILENAME_ANSWER_DATA )
ROW_FIRST_ANSWER.upto( fileReader.GetRowNum - 1 ) do |rowIdx|
  id              = fileReader.GetCell( rowIdx, COLUMN_ANSWER_ID ).to_i
  stringJPN       = fileReader.GetCell( rowIdx, COLUMN_ANSWER_JPN )
  stringJPN_KANJI = fileReader.GetCell( rowIdx, COLUMN_ANSWER_JPN_KANJI )
  color           = fileReader.GetCell( rowIdx, COLUMN_COLOR )
  answer = Answer.new
  answer.SetID( id )
  answer.SetString( stringJPN, stringJPN_KANJI )
  answer.SetColor( color )
  answer.OutputDebug( DEBUG_OUTPUT_DEST )
  answers << answer
end


#===================================================================================
# ��main
# ���S����̎擾
#===================================================================================
# ��C���f�b�N�X
COLUMN_QUESTION_ID           = 0   # ��ID
COLUMN_QUESTION_JPN          = 1   # �񓚕����� (����)
COLUMN_QUESTION_JPN_KANJI    = 2   # �񓚕����� (����)
COLUMN_QUESTION_ANSWER_NUM   = 3   # �I��
COLUMN_QUESTION_BIT_COUNT    = 4   # bit��
COLUMN_QUESTION_ANSWER_ID_0  = 5   # ��0
COLUMN_QUESTION_ANSWER_ID_1  = 7   # ��1
COLUMN_QUESTION_ANSWER_ID_2  = 9   # ��2 
COLUMN_QUESTION_ANSWER_ID_3  = 11  # ��3 
COLUMN_QUESTION_ANSWER_ID_4  = 13  # ��4 
COLUMN_QUESTION_ANSWER_ID_5  = 15  # ��5 
COLUMN_QUESTION_ANSWER_ID_6  = 17  # ��6 
COLUMN_QUESTION_ANSWER_ID_7  = 19  # ��7 
COLUMN_QUESTION_ANSWER_ID_8  = 21  # ��8 
COLUMN_QUESTION_ANSWER_ID_9  = 23  # ��9 
COLUMN_QUESTION_ANSWER_ID_10 = 25  # ��10
COLUMN_QUESTION_ANSWER_ID_11 = 27  # ��11
COLUMN_QUESTION_ANSWER_ID_12 = 29  # ��12
COLUMN_QUESTION_ANSWER_ID_13 = 31  # ��13
COLUMN_QUESTION_ANSWER_ID_14 = 33  # ��14
COLUMN_QUESTION_ANSWER_ID_15 = 35  # ��15
COLUMN_QUESTION_ANSWER_ID_16 = 37  # ��16
COLUMN_QUESTION_ANSWER_ID_17 = 39  # ��17
# �s�C���f�b�N�X
ROW_FIRST_QUESTION = 1  # �擪�f�[�^�s

fileReader = TabSeparatedFileReader.new
fileReader.ReadFile( FILENAME_QUESTION_DATA )
ROW_FIRST_QUESTION.upto( fileReader.GetRowNum - 1 ) do |rowIdx|
  id              = fileReader.GetCell( rowIdx, COLUMN_QUESTION_ID ).to_i
  stringJPN       = fileReader.GetCell( rowIdx, COLUMN_QUESTION_JPN )
  stringJPN_KANJI = fileReader.GetCell( rowIdx, COLUMN_QUESTION_JPN_KANJI )
  answerNum       = fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_NUM ).to_i
  bitCount        = fileReader.GetCell( rowIdx, COLUMN_QUESTION_BIT_COUNT ).to_i
  answerID = Array.new
  answerID << fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_ID_0  ).to_i
  answerID << fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_ID_1  ).to_i
  answerID << fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_ID_2  ).to_i
  answerID << fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_ID_3  ).to_i
  answerID << fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_ID_4  ).to_i
  answerID << fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_ID_5  ).to_i
  answerID << fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_ID_6  ).to_i
  answerID << fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_ID_7  ).to_i
  answerID << fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_ID_8  ).to_i
  answerID << fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_ID_9  ).to_i
  answerID << fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_ID_10 ).to_i
  answerID << fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_ID_11 ).to_i
  answerID << fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_ID_12 ).to_i
  answerID << fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_ID_13 ).to_i
  answerID << fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_ID_14 ).to_i
  answerID << fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_ID_15 ).to_i
  answerID << fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_ID_16 ).to_i
  answerID << fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_ID_17 ).to_i
  question  = Question.new
  question.SetID( id )
  question.SetString( stringJPN, stringJPN_KANJI )
  question.SetAnswerNum( answerNum )
  question.SetBitCount( bitCount )
  answerID.each do |ansID|
    question.AddAnswer( answers[ ansID ] )
  end
  question.OutputDebug( DEBUG_OUTPUT_DEST )
  questions << question
end


#===================================================================================
# ��main
# ���S�������ڂ̎擾
#===================================================================================
# ��C���f�b�N�X
COLUMN_TOPIC_ID                = 0   # ����ID
COLUMN_TOPIC_TITLE_JPN         = 1   # �薼 (����)
COLUMN_TOPIC_TITLE_JPN_KANJI   = 2   # �薼 (����)
COLUMN_TOPIC_CAPTION_JPN       = 3   # �⑫ (����)
COLUMN_TOPIC_CAPTION_JPN_KANJI = 4   # �⑫ (����)
COLUMN_TOPIC_QUESTION_ID_0     = 5   # ����0
COLUMN_TOPIC_QUESTION_ID_1     = 7   # ����1
COLUMN_TOPIC_QUESTION_ID_2     = 9   # ����2 
# �s�C���f�b�N�X
ROW_FIRST_TOPIC = 1  # �擪�f�[�^�s

fileReader = TabSeparatedFileReader.new
fileReader.ReadFile( FILENAME_TOPIC_DATA ) 
ROW_FIRST_TOPIC.upto( fileReader.GetRowNum - 1 ) do |rowIdx|
  id               = fileReader.GetCell( rowIdx, COLUMN_TOPIC_ID ).to_i
  titleJPN         = fileReader.GetCell( rowIdx, COLUMN_TOPIC_TITLE_JPN )
  titleJPN_KANJI   = fileReader.GetCell( rowIdx, COLUMN_TOPIC_TITLE_JPN_KANJI )
  captionJPN       = fileReader.GetCell( rowIdx, COLUMN_TOPIC_CAPTION_JPN )
  captionJPN_KANJI = fileReader.GetCell( rowIdx, COLUMN_TOPIC_CAPTION_JPN_KANJI )
  questionID  = Array.new
  questionID << fileReader.GetCell( rowIdx, COLUMN_TOPIC_QUESTION_ID_0  ).to_i
  questionID << fileReader.GetCell( rowIdx, COLUMN_TOPIC_QUESTION_ID_1  ).to_i
  questionID << fileReader.GetCell( rowIdx, COLUMN_TOPIC_QUESTION_ID_2  ).to_i
  topic  = Topic.new
  topic.SetID( id )
  topic.SetTitle( titleJPN, titleJPN_KANJI )
  topic.SetCaption( captionJPN, captionJPN_KANJI )
  questionID.each do |qID|
    topic.AddQuestion( questions[ qID ] )
  end
  topic.OutputDebug( DEBUG_OUTPUT_DEST )
  topics << topic
end


#===================================================================================
# ��main
# ������X�g�̎擾
#=================================================================================== 
# ��C���f�b�N�X
COLUMN_HOBBY_JPN       = 1  # ������� (����)
COLUMN_HOBBY_JPN_KANJI = 2  # ������� (����)
COLUMN_HOBBY_ID_LAVEL  = 3  # �IO���x����
# �s�C���f�b�N�X
ROW_FIRST_HOBBY = 2  # �擪�f�[�^

# �S�Ă̕�������擾
fileReader = TabSeparatedFileReader.new
fileReader.ReadFile( FILENAME_HOBBY_DATA )
ROW_FIRST_HOBBY.upto( fileReader.GetRowNum - 1 ) do |rowIdx|
  id              = rowIdx - ROW_FIRST_HOBBY
  stringJPN       = fileReader.GetCell( rowIdx, COLUMN_HOBBY_JPN ).strip
  stringJPN_KANJI = fileReader.GetCell( rowIdx, COLUMN_HOBBY_JPN_KANJI ).strip
  idLavel         = fileReader.GetCell( rowIdx, COLUMN_HOBBY_ID_LAVEL ).strip
  hobby = Hobby.new
  hobby.SetID( id, idLavel )
  hobby.SetString( stringJPN, stringJPN_KANJI )
  hobby.OutputDebug( DEBUG_OUTPUT_DEST )
  hobbies << hobby
end


#===================================================================================
# ��main
# ���A���P�[�g�񓚃C���f�b�N�X �f�[�^�o��
#===================================================================================
answerNum       = Array.new  # ���ڐ�
answerBitCount  = Array.new  # bit��
answerNumOffset = Array.new  # ���ڐ��I�t�Z�b�g
answerBitOffset = Array.new  # bit���I�t�Z�b�g

# �f�[�^�W�v
answerNum[0]       = questions[0].answerNum
answerBitCount[0]  = questions[0].bitCount
answerNumOffset[0] = 0
answerBitOffset[0] = 0

1.upto( questions.size - 1 ) do |questionIdx|
  answerNum      [ questionIdx ] = questions[ questionIdx ].answerNum
  answerBitCount [ questionIdx ] = questions[ questionIdx ].bitCount
  answerNumOffset[ questionIdx ] = answerNumOffset[ questionIdx - 1 ] + questions[ questionIdx - 1 ].answerNum
  answerBitOffset[ questionIdx ] = answerBitOffset[ questionIdx - 1 ] + questions[ questionIdx - 1 ].bitCount
end

# �o�̓f�[�^�쐬
outData = Array.new
outData << "// �R���o�[�^�ɂ�萶��"
outData << "QUESTIONNAIRE_ANSWER_INDEX QuestionnaireAnswerIndex[] = "
outData << "{"
0.upto( questions.size - 1 ) do |qIdx|
  outData << "  { #{answerNum[qIdx]}, #{answerBitCount[qIdx]}, #{answerNumOffset[qIdx]}, #{answerBitOffset[qIdx]} },"
end
outData << "};"

# �o��
file = File.open( "questionnaire_answer_index.cdat", "w" )
file.puts( outData )
file.close


#===================================================================================
# ��main
# ��gmm �o�� ( questionnaire.gmm )
#===================================================================================
directory       = GMM_OUTPUT_DEST
fileName        = "questionnaire.gmm"
stringLavel     = Array.new
stringJPN       = Array.new
stringJPN_KANJI = Array.new

# �������� (�^�C�g��)
topics.each do |topic|
  stringLavel     << "title_%d" % topic.ID
  stringJPN       << topic.titleJPN
  stringJPN_KANJI << topic.titleJPN_KANJI
end

# �������� (�⑫)
topics.each do |topic|
  stringLavel     << "caption_%d" % topic.ID
  stringJPN       << topic.captionJPN
  stringJPN_KANJI << topic.captionJPN_KANJI
end

# ����
questions.each do |question|
  stringLavel     << "question_%d" % question.ID
  stringJPN       << question.stringJPN
  stringJPN_KANJI << question.stringJPN_KANJI
end

# ��
answers.each do |answer|
  stringLavel     << "answer_%d" % answer.ID
  stringJPN       << answer.stringJPN
  stringJPN_KANJI << answer.stringJPN_KANJI
end

# gmm ����
GenerateGMM( directory, fileName, stringLavel, stringJPN, stringJPN_KANJI )


#===================================================================================
# ��main
# ��gmm �o�� ( hobby.gmm )
#=================================================================================== 
directory       = GMM_OUTPUT_DEST
fileName        = "hobby.gmm"
stringLavel     = Array.new
stringJPN       = Array.new
stringJPN_KANJI = Array.new

# �
hobbies.each do |hobby|
  stringLavel     << "hobby_%d" % hobby.ID
  stringJPN       << hobby.stringJPN
  stringJPN_KANJI << hobby.stringJPN_KANJI
end

# gmm ����
GenerateGMM( directory, fileName, stringLavel, stringJPN, stringJPN_KANJI )


#=================================================================================== 
# ��main
# ���ID �t�@�C���o��
#=================================================================================== 
# �o�̓f�[�^�쐬
outData = Array.new
outData << "// �R���o�[�^�ɂ�萶��"
hobbies.each do |hobby|
  outData << "#define #{hobby.ID_lavel} (#{hobby.ID})"
end

# �o��
file = File.open( "hobby_id.h", "w" )
file.puts( outData )
file.close
