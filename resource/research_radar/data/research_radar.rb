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
COLUMN_ANSWER_ID              = 0  # ��ID
COLUMN_ANSWER_ID_LAVEL        = 1  # ��ID���x����
COLUMN_ANSWER_STRING_ID_LAVEL = 2  # gmm�����x����
COLUMN_ANSWER_JPN             = 3  # �񓚕�����
COLUMN_ANSWER_JPN_KANJI       = 4  # �񓚕�����
COLUMN_COLOR                  = 5  # �F
# �s�C���f�b�N�X
ROW_FIRST_ANSWER = 1  # �擪�f�[�^�s

fileReader = TabSeparatedFileReader.new
fileReader.ReadFile( FILENAME_ANSWER_DATA )
ROW_FIRST_ANSWER.upto( fileReader.GetRowNum - 1 ) do |rowIdx|
  id              = fileReader.GetCell( rowIdx, COLUMN_ANSWER_ID ).to_i
  idLavel         = fileReader.GetCell( rowIdx, COLUMN_ANSWER_ID_LAVEL )
  stringIDLavel   = fileReader.GetCell( rowIdx, COLUMN_ANSWER_STRING_ID_LAVEL )
  stringJPN       = fileReader.GetCell( rowIdx, COLUMN_ANSWER_JPN )
  stringJPN_KANJI = fileReader.GetCell( rowIdx, COLUMN_ANSWER_JPN_KANJI )
  color           = fileReader.GetCell( rowIdx, COLUMN_COLOR )
  answer = Answer.new
  answer.SetID( id, idLavel )
  answer.SetStringIDLavel( stringIDLavel )
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
COLUMN_QUESTION_ID                = 0   # ����ID
COLUMN_QUESTION_ID_LAVEL          = 1   # ����ID (���x����)
COLUMN_QUESTION_TITLE_ID_LAVEL    = 2   # gmm�����x����
COLUMN_QUESTION_CAPTION_ID_LAVEL  = 3   # gmm�����x����
COLUMN_QUESTION_ANSWER_NUM        = 4   # �I��
COLUMN_QUESTION_BIT_COUNT         = 5   # bit��
COLUMN_QUESTION_TITLE_JPN         = 6   # �񓚕����� (����)
COLUMN_QUESTION_TITLE_JPN_KANJI   = 7   # �񓚕����� (����)
COLUMN_QUESTION_CAPTION_JPN       = 8   # �񓚕����� (����)
COLUMN_QUESTION_CAPTION_JPN_KANJI = 9   # �񓚕����� (����)
COLUMN_QUESTION_ANSWER_ID_0       = 10   # ��0
COLUMN_QUESTION_ANSWER_ID_1       = 12   # ��1
COLUMN_QUESTION_ANSWER_ID_2       = 14  # ��2 
COLUMN_QUESTION_ANSWER_ID_3       = 16  # ��3 
COLUMN_QUESTION_ANSWER_ID_4       = 18  # ��4 
COLUMN_QUESTION_ANSWER_ID_5       = 20  # ��5 
COLUMN_QUESTION_ANSWER_ID_6       = 22  # ��6 
COLUMN_QUESTION_ANSWER_ID_7       = 24  # ��7 
COLUMN_QUESTION_ANSWER_ID_8       = 26  # ��8 
COLUMN_QUESTION_ANSWER_ID_9       = 28  # ��9 
COLUMN_QUESTION_ANSWER_ID_10      = 30  # ��10
COLUMN_QUESTION_ANSWER_ID_11      = 32  # ��11
COLUMN_QUESTION_ANSWER_ID_12      = 34  # ��12
COLUMN_QUESTION_ANSWER_ID_13      = 36  # ��13
COLUMN_QUESTION_ANSWER_ID_14      = 38  # ��14
COLUMN_QUESTION_ANSWER_ID_15      = 40  # ��15
COLUMN_QUESTION_ANSWER_ID_16      = 42  # ��16
COLUMN_QUESTION_ANSWER_ID_17      = 44  # ��17
# �s�C���f�b�N�X
ROW_FIRST_QUESTION = 1  # �擪�f�[�^�s

fileReader = TabSeparatedFileReader.new
fileReader.ReadFile( FILENAME_QUESTION_DATA )
ROW_FIRST_QUESTION.upto( fileReader.GetRowNum - 1 ) do |rowIdx|
  id               = fileReader.GetCell( rowIdx, COLUMN_QUESTION_ID ).to_i
  idLavel          = fileReader.GetCell( rowIdx, COLUMN_QUESTION_ID_LAVEL )
  titleIDLavel     = fileReader.GetCell( rowIdx, COLUMN_QUESTION_TITLE_ID_LAVEL )
  captionIDLavel   = fileReader.GetCell( rowIdx, COLUMN_QUESTION_CAPTION_ID_LAVEL )
  answerNum        = fileReader.GetCell( rowIdx, COLUMN_QUESTION_ANSWER_NUM ).to_i
  bitCount         = fileReader.GetCell( rowIdx, COLUMN_QUESTION_BIT_COUNT ).to_i
  titleJPN         = fileReader.GetCell( rowIdx, COLUMN_QUESTION_TITLE_JPN )
  titleJPN_KANJI   = fileReader.GetCell( rowIdx, COLUMN_QUESTION_TITLE_JPN_KANJI )
  captionJPN       = fileReader.GetCell( rowIdx, COLUMN_QUESTION_CAPTION_JPN )
  captionJPN_KANJI = fileReader.GetCell( rowIdx, COLUMN_QUESTION_CAPTION_JPN_KANJI )
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
  question.SetID( id, idLavel )
  question.SetStringIDLavel( titleIDLavel, captionIDLavel )
  question.SetTitleString( titleJPN, titleJPN_KANJI )
  question.SetCaptionString( captionJPN, captionJPN_KANJI )
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
COLUMN_TOPIC_ID                      = 0   # ����ID
COLUMN_TOPIC_ID_LAVEL                = 1   # ����ID (���x����)
COLUMN_TOPIC_TITLE_STRING_ID_LAVEL   = 2   # gmm�����x���� (�薼)
COLUMN_TOPIC_CAPTION_STRING_ID_LAVEL = 3   # gmm�����x���� (�⑫)
COLUMN_TOPIC_TITLE_JPN               = 4   # �薼 (����)
COLUMN_TOPIC_TITLE_JPN_KANJI         = 5   # �薼 (����)
COLUMN_TOPIC_CAPTION_JPN             = 6   # �⑫ (����)
COLUMN_TOPIC_CAPTION_JPN_KANJI       = 7   # �⑫ (����)
COLUMN_TOPIC_QUESTION_ID_0           = 8   # ����0
COLUMN_TOPIC_QUESTION_ID_1           = 10  # ����1
COLUMN_TOPIC_QUESTION_ID_2           = 12  # ����2 
# �s�C���f�b�N�X
ROW_FIRST_TOPIC = 1  # �擪�f�[�^�s

fileReader = TabSeparatedFileReader.new
fileReader.ReadFile( FILENAME_TOPIC_DATA ) 
ROW_FIRST_TOPIC.upto( fileReader.GetRowNum - 1 ) do |rowIdx|
  id                    = fileReader.GetCell( rowIdx, COLUMN_TOPIC_ID ).to_i
  idLavel               = fileReader.GetCell( rowIdx, COLUMN_TOPIC_ID_LAVEL )
  titleStringID_lavel   = fileReader.GetCell( rowIdx, COLUMN_TOPIC_TITLE_STRING_ID_LAVEL )
  captionStringID_lavel = fileReader.GetCell( rowIdx, COLUMN_TOPIC_CAPTION_STRING_ID_LAVEL )
  titleJPN              = fileReader.GetCell( rowIdx, COLUMN_TOPIC_TITLE_JPN )
  titleJPN_KANJI        = fileReader.GetCell( rowIdx, COLUMN_TOPIC_TITLE_JPN_KANJI )
  captionJPN            = fileReader.GetCell( rowIdx, COLUMN_TOPIC_CAPTION_JPN )
  captionJPN_KANJI      = fileReader.GetCell( rowIdx, COLUMN_TOPIC_CAPTION_JPN_KANJI )
  questionID  = Array.new
  questionID << fileReader.GetCell( rowIdx, COLUMN_TOPIC_QUESTION_ID_0  ).to_i
  questionID << fileReader.GetCell( rowIdx, COLUMN_TOPIC_QUESTION_ID_1  ).to_i
  questionID << fileReader.GetCell( rowIdx, COLUMN_TOPIC_QUESTION_ID_2  ).to_i
  topic  = Topic.new
  topic.SetID( id, idLavel )
  topic.SetStringIDLavel( titleStringID_lavel, captionStringID_lavel )
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
COLUMN_HOBBY_ID        = 0  # �ID
COLUMN_HOBBY_ID_LAVEL  = 1  # �IO���x����
COLUMN_HOBBY_JPN       = 3  # ������� (����)
COLUMN_HOBBY_JPN_KANJI = 4  # ������� (����)
# �s�C���f�b�N�X
ROW_FIRST_HOBBY = 2  # �擪�f�[�^

# �S�Ă̕�������擾
fileReader = TabSeparatedFileReader.new
fileReader.ReadFile( FILENAME_HOBBY_DATA )
ROW_FIRST_HOBBY.upto( fileReader.GetRowNum - 1 ) do |rowIdx|
  id              = fileReader.GetCell( rowIdx, COLUMN_HOBBY_ID ).to_i
  idLavel         = fileReader.GetCell( rowIdx, COLUMN_HOBBY_ID_LAVEL ).strip
  stringJPN       = fileReader.GetCell( rowIdx, COLUMN_HOBBY_JPN ).strip
  stringJPN_KANJI = fileReader.GetCell( rowIdx, COLUMN_HOBBY_JPN_KANJI ).strip
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
  answerNumOffset[ questionIdx ] = answerNumOffset[ questionIdx - 1 ] + ( questions[ questionIdx - 1 ].answerNum - 1 ) # "����"����������1������
  answerBitOffset[ questionIdx ] = answerBitOffset[ questionIdx - 1 ] + questions[ questionIdx - 1 ].bitCount
end

# �o�̓f�[�^�쐬
outData = Array.new
outData << "// �R���o�[�^�ɂ�萶��"
outData << "static const QUESTIONNAIRE_INDEX QuestionnaireIndex[] = "
outData << "{"
0.upto( questions.size - 1 ) do |qIdx|
  outData << "  { #{answerNum[qIdx]}, #{answerBitCount[qIdx]}, #{answerNumOffset[qIdx]}, #{answerBitOffset[qIdx]} },"
end
outData << "};"

# �o��
file = File.open( "questionnaire_index.cdat", "w" )
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
  stringLavel     << topic.titleStringID_lavel
  stringJPN       << topic.titleJPN
  stringJPN_KANJI << topic.titleJPN_KANJI
end

# �������� (�⑫)
topics.each do |topic|
  stringLavel     << topic.captionStringID_lavel
  stringJPN       << topic.captionJPN
  stringJPN_KANJI << topic.captionJPN_KANJI
end

# ����
questions.each do |question|
  stringLavel     << question.titleStringID_lavel
  stringJPN       << question.titleStringJPN
  stringJPN_KANJI << question.titleStringJPN_KANJI
end

# ���� (�⑫)
questions.each do |question|
  stringLavel     << question.captionStringID_lavel
  stringJPN       << question.captionStringJPN
  stringJPN_KANJI << question.captionStringJPN_KANJI
end

# ��
answers.each do |answer|
  stringLavel     << answer.stringID_lavel
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
  outData << "#define #{hobby.ID_lavel} (#{hobby.ID}) //�u#{hobby.stringJPN}�v"
end
outData << "#define HOBBY_ID_NUM   (#{hobbies.size})   // �ID�̐�"
outData << "#define HOBBY_ID_MAX   (HOBBY_ID_NUM - 1)  // �ID�̍ő�l"
outData << "#define HOBBY_ID_DUMMY (0xff)              // �ID�̃_�~�[�l"

# �o��
file = File.open( "hobby_id.h", "w" )
file.puts( outData )
file.close


#=================================================================================== 
# ��main
# ����ID �t�@�C���o��
#===================================================================================
# �o�̓f�[�^�쐬
outData = Array.new
outData << "// �R���o�[�^�ɂ�萶��"
answers.each do |answer|
  outData << "#define #{answer.ID_lavel} (#{answer.ID}) //�u#{answer.stringJPN}�v"
end
outData << "#define ANSWER_ID_NUM   (#{answers.size})    // ��ID�̐�"
outData << "#define ANSWER_ID_MAX   (ANSWER_ID_NUM - 1)  // ��ID�̍ő�l"
outData << "#define ANSWER_ID_DUMMY (0xffff)             // ��ID�̃_�~�[�l"

# �o��
file = File.open( "answer_id.h", "w" )
file.puts( outData )
file.close


#=================================================================================== 
# ��main
# ������ID �t�@�C���o��
#===================================================================================
# �o�̓f�[�^�쐬
outData = Array.new
outData << "// �R���o�[�^�ɂ�萶��"
questions.each do |question|
  outData << "#define #{question.ID_lavel} (#{question.ID}) //�u#{question.titleStringJPN}�v"
end
outData << "#define QUESTION_ID_NUM   (#{questions.size})    // ����ID�̐�"
outData << "#define QUESTION_ID_MAX   (QUESTION_ID_NUM - 1)  // ����ID�̍ő�l"
outData << "#define QUESTION_ID_DUMMY (0xff)                 // ����ID�̃_�~�[�l"

# �o��
file = File.open( "question_id.h", "w" )
file.puts( outData )
file.close


#=================================================================================== 
# ��main
# ����������ID �t�@�C���o��
#===================================================================================
# �o�̓f�[�^�쐬
outData = Array.new
outData << "// �R���o�[�^�ɂ�萶��"
topics.each do |topic|
  outData << "#define #{topic.ID_lavel} (#{topic.ID}) //�u#{topic.titleJPN}�v"
end
outData << "#define TOPIC_ID_NUM   (#{topics.size})    // ��������ID�̐�"
outData << "#define TOPIC_ID_MAX   (TOPIC_ID_NUM - 1)  // ��������ID�̍ő�l"
outData << "#define TOPIC_ID_DUMMY (0xff)              // ��������ID�̃_�~�[�l"

# �o��
file = File.open( "topic_id.h", "w" )
file.puts( outData )
file.close


#=================================================================================== 
# ��main
# ����ID �� ������ �e�[�u���o��
#===================================================================================
# �o�̓f�[�^�쐬
outData = Array.new
outData << "// �R���o�[�^�ɂ�萶��"
outData << "static const u32 StringID_answer[ ANSWER_ID_NUM ] = "
outData << "{"
answers.each do |answer|
  outData << "  #{answer.stringID_lavel},  // [#{answer.ID_lavel}]"
end
outData << "};"

# �o��
file = File.open( "string_id_answer.cdat", "w" )
file.puts( outData )
file.close


#=================================================================================== 
# ��main
# ����ID �� �\���J���[(R) �e�[�u���o��
#===================================================================================
# �o�̓f�[�^�쐬
outData = Array.new
outData << "// �R���o�[�^�ɂ�萶��"
outData << "static const u8 ColorR_answer[ ANSWER_ID_NUM ] = "
outData << "{"
answers.each do |answer|
  outData << "  #{answer.colorR},  // [#{answer.ID_lavel}]"
end
outData << "};"

# �o��
file = File.open( "color_r_answer.cdat", "w" )
file.puts( outData )
file.close


#=================================================================================== 
# ��main
# ����ID �� �\���J���[(G) �e�[�u���o��
#===================================================================================
# �o�̓f�[�^�쐬
outData = Array.new
outData << "// �R���o�[�^�ɂ�萶��"
outData << "static const u8 ColorG_answer[ ANSWER_ID_NUM ] = "
outData << "{"
answers.each do |answer|
  outData << "  #{answer.colorG},  // [#{answer.ID_lavel}]"
end
outData << "};"

# �o��
file = File.open( "color_g_answer.cdat", "w" )
file.puts( outData )
file.close


#=================================================================================== 
# ��main
# ����ID �� �\���J���[(B) �e�[�u���o��
#===================================================================================
# �o�̓f�[�^�쐬
outData = Array.new
outData << "// �R���o�[�^�ɂ�萶��"
outData << "static const u8 ColorB_answer[ ANSWER_ID_NUM ] = "
outData << "{"
answers.each do |answer|
  outData << "  #{answer.colorB},  // [#{answer.ID_lavel}]"
end
outData << "};"

# �o��
file = File.open( "color_b_answer.cdat", "w" )
file.puts( outData )
file.close


#=================================================================================== 
# ��main
# ������ID �� ������ �e�[�u���o��
#===================================================================================
# �o�̓f�[�^�쐬
outData = Array.new
outData << "// �R���o�[�^�ɂ�萶��"
outData << "static const u32 StringID_question[ QUESTION_ID_NUM ] = "
outData << "{"
questions.each do |question|
  outData << "  #{question.titleStringID_lavel},  // [#{question.ID_lavel}]"
end
outData << "};"

# �o��
file = File.open( "string_id_question.cdat", "w" )
file.puts( outData )
file.close

#=================================================================================== 
# ��main
# ������ID �� �⑫������ �e�[�u���o��
#===================================================================================
# �o�̓f�[�^�쐬
outData = Array.new
outData << "// �R���o�[�^�ɂ�萶��"
outData << "static const u32 CaptionStringID_question[ QUESTION_ID_NUM ] = "
outData << "{"
questions.each do |question|
  outData << "  #{question.captionStringID_lavel},  // [#{question.ID_lavel}]"
end
outData << "};"

# �o��
file = File.open( "caption_string_id_question.cdat", "w" )
file.puts( outData )
file.close


#=================================================================================== 
# ��main
# ������ID �� ������ �e�[�u���o��
#===================================================================================
# �o�̓f�[�^�쐬
outData = Array.new
outData << "// �R���o�[�^�ɂ�萶��"
outData << "static const u8 AnswerNum_question[ QUESTION_ID_NUM ] = "
outData << "{"
questions.each do |question|
  outData << "  #{question.answers.size},  // [#{question.ID_lavel}]"
end
outData << "};"

# �o��
file = File.open( "answer_num_question.cdat", "w" )
file.puts( outData )
file.close


#=================================================================================== 
# ��main
# ������ID �� ��ID �e�[�u���o��
#===================================================================================
# �o�̓f�[�^�쐬
outData = Array.new
outData << "// �R���o�[�^�ɂ�萶��"
outData << "static const u32 AnswerID_question[ QUESTION_ID_NUM ][17] = "
outData << "{"
questions.each do |question|
  data = String.new
  data += "  { "
  0.upto( 16 ) do |answerIdx|
    answer = question.answers[ answerIdx ]
    if answer == nil then 
      data += "0, "
    else 
      data += "#{answer.ID}, "
    end
  end
  data += "},"
  outData << data
end
outData << "};"

# �o��
file = File.open( "answer_id_question.cdat", "w" )
file.puts( outData )
file.close


#=================================================================================== 
# ��main
# ����������ID �� �薼������ �e�[�u���o��
#===================================================================================
# �o�̓f�[�^�쐬
outData = Array.new
outData << "// �R���o�[�^�ɂ�萶��"
outData << "static const u32 StringID_topicTitle[ TOPIC_ID_NUM ] = "
outData << "{"
topics.each do |topic|
  outData << "  #{topic.titleStringID_lavel},  // [#{topic.ID_lavel}]"
end
outData << "};"

# �o��
file = File.open( "string_id_topic_title.cdat", "w" )
file.puts( outData )
file.close


#=================================================================================== 
# ��main
# ����������ID �� �⑫������ �e�[�u���o��
#===================================================================================
# �o�̓f�[�^�쐬
outData = Array.new
outData << "// �R���o�[�^�ɂ�萶��"
outData << "static const u32 StringID_topicCaption[ TOPIC_ID_NUM ] = "
outData << "{"
topics.each do |topic|
  outData << "  #{topic.captionStringID_lavel},  // [#{topic.ID_lavel}]"
end
outData << "};"

# �o��
file = File.open( "string_id_topic_caption.cdat", "w" )
file.puts( outData )
file.close


#=================================================================================== 
# ��main
# ����������ID �� ����ID �e�[�u���o��
#===================================================================================
# �o�̓f�[�^�쐬
outData = Array.new
outData << "// �R���o�[�^�ɂ�萶��"
outData << "static const u8 Question1_topic[ TOPIC_ID_NUM ] = "
outData << "{"
topics.each do |topic|
  question1 = topic.questions[0]
  outData << "  #{question1.ID_lavel},  // [#{topic.ID_lavel}]"
end
outData << "};"

outData << "static const u8 Question2_topic[ TOPIC_ID_NUM ] = "
outData << "{"
topics.each do |topic|
  question2 = topic.questions[1]
  outData << "  #{question2.ID_lavel},  // [#{topic.ID_lavel}]"
end
outData << "};"

outData << "static const u8 Question3_topic[ TOPIC_ID_NUM ] = "
outData << "{"
topics.each do |topic|
  question3 = topic.questions[2]
  outData << "  #{question3.ID_lavel},  // [#{topic.ID_lavel}]"
end
outData << "};"

# �o��
file = File.open( "question_id_topic.cdat", "w" )
file.puts( outData )
file.close
