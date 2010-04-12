###################################################################################
# ��brief:  gmm�����֐�
# ��file    gmm_generator.rb
# ��author: obata
# ��date:   2010.02.12
################################################################################### 
require 'kconv'  # for String.toutf8


RETURN_CODE = "\n" # �u���Ώۂ̉��s�R�[�h
SPACE_CODE  = "�@" # �u���Ώۂ̋󔒃R�[�h
DUMMY_RETURN_CODE  = "@"   # ���s�R�[�h�Ƃ��Ė��ߍ��ރ_�~�[�R�[�h
DUMMY_SPACE_CODE   = "��"  # �󔒃R�[�h�Ƃ��Ė��ߍ��ރ_�~�[�R�[�h


#==================================================================================
# ��brief:  gmm�w�b�_�����擾����
# ��return: gmm�w�b�_���̕�����
#==================================================================================
def GetGMMHeader

  header = ' 

<?xml version="1.0" encoding="UTF-8" ?>
<?xml-stylesheet type="text/xsl" href="" ?>

<gmml version="4.5">

<head>
  <create user="obata" host="W00207" date="2010-02-12T11:11:29"/>
  <generator name="ResearchRadarConverter" version="4.9.2.5"/>
</head>

<body language="japanese">
  <color>
    <list entry="0" r="00" g="00" b="00" a="FF"/>
    <list entry="1" r="FF" g="00" b="00" a="FF"/>
    <list entry="2" r="00" g="00" b="FF" a="FF"/>
    <list entry="3" r="00" g="8C" b="00" a="FF"/>
  </color>

  <tag-table>
    <group entry="0" japanese="�^�O�O���[�v" english="�^�O�O���[�v">
    </group>

    <group entry="1" japanese="�|�P�����v�a�i�P��j" english="PokemonDP" color="#c0c0c0">
      <tag entry="00" japanese="�g���[�i�[��" english="�g���[�i�[��" view="EP" width="60" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
      </tag>
      <tag entry="01" japanese="�|�P�����푰��" english="�|�P�����푰��" view="EP" width="60" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�|�P�����̖��O�B
�s�J�`���E�Ƃ��t�V�M�_�l�Ƃ��B</comment>
        <comment language="english">�|�P�����̖��O�B
�s�J�`���E�Ƃ��t�V�M�_�l�Ƃ��B</comment>
      </tag>
      <tag entry="02" japanese="�|�P�����j�b�N�l�[��" english="�|�P�����j�b�N�l�[��" view="EP" width="60" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�|�P�����̃j�b�N�l�[��</comment>
        <comment language="english">�|�P�����̃j�b�N�l�[��</comment>
      </tag>
      <tag entry="03" japanese="�|�P�����^�C�v" english="�|�P�����^�C�v" view="EP" width="48" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�݂��A�����A�ق̂��c�Ƃ��B</comment>
        <comment language="english">�݂��A�����A�ق̂��c�Ƃ��B</comment>
      </tag>
      <tag entry="04" japanese="�|�P��������" english="�|�P��������" view="EP" width="60" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�uDNA�|�P�����v�ȂǁB</comment>
        <comment language="english">�uDNA�|�P�����v�ȂǁB</comment>
      </tag>
      <tag entry="05" japanese="�}�b�v��" english="�}�b�v��" view="EP" width="120" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
      </tag>
      <tag entry="06" japanese="�Ƃ�������" english="�Ƃ�������" view="EP" width="84" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
      </tag>
      <tag entry="07" japanese="�킴��" english="�킴��" view="EP" width="84" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
      </tag>
      <tag entry="08" japanese="��������" english="��������" view="EP" width="60" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
      </tag>
      <tag entry="09" japanese="�ǂ�����" english="�ǂ�����" view="EP" width="96" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
      </tag>
      <tag entry="10" japanese="�O�b�Y��" english="�O�b�Y��" view="EP" length="8" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
      </tag>
      <tag entry="11" japanese="�{�b�N�X��" english="�{�b�N�X��" view="EP" width="96" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
      </tag>
      <tag entry="13" japanese="�X�e�[�^�X��" english="�X�e�[�^�X��" view="EP" width="74" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">������傭�A����������傭�A�ڂ������傭�A�Ƃ������A�Ƃ��ڂ��A���΂₳�A�����Ђ�A�߂����イ</comment>
        <comment language="english">������傭�A����������傭�A�ڂ������傭�A�Ƃ������A�Ƃ��ڂ��A���΂₳�A�����Ђ�A�߂����イ</comment>
      </tag>
      <tag entry="14" japanese="�g���[�i�[��ʖ�" english="�g���[�i�[��ʖ�" view="EP" width="120" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�u����ς񂱂����v�u�G���[�g�g���[�i�[�v�Ȃ�</comment>
        <comment language="english">�u����ς񂱂����v�u�G���[�g�g���[�i�[�v�Ȃ�</comment>
      </tag>
      <tag entry="17" japanese="�����Ԗ�" english="�����Ԗ�" view="EP" width="48" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�|�P�����̓����Ԃł��B</comment>
        <comment language="english">�|�P�����̓����Ԃł��B</comment>
      </tag>
      <tag entry="18" japanese="�o�b�O�̃|�P�b�g��" english="�o�b�O�̃|�P�b�g��" view="EP" width="132" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�o�b�O�̃|�P�b�g�̖��O�B�u�|�P�b�g�v�̕������܂ށB</comment>
        <comment language="english">�o�b�O�̃|�P�b�g�̖��O�B�u�|�P�b�g�v�̕������܂ށB</comment>
      </tag>
      <tag entry="26" japanese="�J�b�v��" english="�J�b�v��" view="EP" width="72" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�ΐ�̃��M�����[�V������</comment>
        <comment language="english">�ΐ�̃��M�����[�V������</comment>
      </tag>
      <tag entry="28" japanese="�ȈՉ�b�P��" english="�ȈՉ�b�P��" view="EP" width="84" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
      </tag>
      <tag entry="31" japanese="�A�N�Z�T���[��" english="�A�N�Z�T���[��" view="EP" width="96" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�|�P�����ɂ���A�N�Z�T���[</comment>
        <comment language="english">�|�P�����ɂ���A�N�Z�T���[</comment>
      </tag>
      <tag entry="32" japanese="�W����" english="�W����" view="EP" width="72" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
      </tag>
      <tag entry="33" japanese="���ԑ�" english="���ԑ�" view="EP" width="48" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�ڂ�����m�[�g�ȂǂŎg��</comment>
        <comment language="english">�ڂ�����m�[�g�ȂǂŎg��</comment>
      </tag>
      <tag entry="34" japanese="�R���e�X�g����" english="�R���e�X�g����" view="EP" width="120" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�R���e�X�g�̃W������
�u�����������R���e�X�g�v</comment>
        <comment language="english">�R���e�X�g�̃W������
�u�����������R���e�X�g�v</comment>
      </tag>
      <tag entry="35" japanese="�R���e�X�g�����N" english="�R���e�X�g�����N" view="EP" width="84" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�������������N
�u�����N�v���܂�</comment>
        <comment language="english">�������������N
�u�����N�v���܂�</comment>
      </tag>
      <tag entry="36" japanese="����" english="����" view="EP" width="216" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�v���C���[�����͂���A�������Z��ł��鍑��</comment>
        <comment language="english">�v���C���[�����͂���A�������Z��ł��鍑��</comment>
      </tag>
      <tag entry="37" japanese="�n�於" english="�n�於" view="EP" width="216" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�v���C���[�����͂���A�������Z��ł���n�於</comment>
        <comment language="english">�v���C���[�����͂���A�������Z��ł���n�於</comment>
      </tag>
      <tag entry="39" japanese="���{���̖��O" english="���{���̖��O" view="EP" width="168" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">���{���̖��O</comment>
        <comment language="english">���{���̖��O</comment>
      </tag>
      <tag entry="40" japanese="�L��~�j�Q�[��" english="�L��~�j�Q�[��" view="EP" width="120" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">WiFi�L��̃~�j�Q�[����</comment>
        <comment language="english">WiFi�L��̃~�j�Q�[����</comment>
      </tag>
      <tag entry="41" japanese="�L��C�x���g��" english="�L��C�x���g��" view="EP" width="60" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">WiFi�L��C�x���g��</comment>
        <comment language="english">WiFi�L��C�x���g��</comment>
      </tag>
      <tag entry="42" japanese="�L��A�C�e����" english="�L��A�C�e����" view="EP" width="120" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">WiFi�L��̃A�C�e����</comment>
        <comment language="english">WiFi�L��̃A�C�e����</comment>
      </tag>
      <tag entry="43" japanese="���{�̈��A" english="���{�̈��A" view="EP" width="143" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">���{�̈��A</comment>
        <comment language="english">���{�̈��A</comment>
      </tag>
      <tag entry="44" japanese="�p��̈��A" english="�p��̈��A" view="EP" width="143" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�p��̈��A</comment>
        <comment language="english">�p��̈��A</comment>
      </tag>
      <tag entry="45" japanese="�t�����X��̈��A" english="�t�����X��̈��A" view="EP" width="143" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�t�����X��̈��A</comment>
        <comment language="english">�t�����X��̈��A</comment>
      </tag>
      <tag entry="46" japanese="�h�C�c��̈��A" english="�h�C�c��̈��A" view="EP" width="143" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�h�C�c��̈��A</comment>
        <comment language="english">�h�C�c��̈��A</comment>
      </tag>
      <tag entry="47" japanese="�C�^���A��̈��A" english="�C�^���A��̈��A" view="EP" width="143" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�C�^���A��̈��A</comment>
        <comment language="english">�C�^���A��̈��A</comment>
      </tag>
      <tag entry="48" japanese="�X�y�C����̈��A" english="�X�y�C����̈��A" view="EP" width="143" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">�X�y�C����̈��A</comment>
        <comment language="english">�X�y�C����̈��A</comment>
      </tag>
    </group>

    <group entry="2" japanese="�|�P�����v�a�i���l�j" english="�|�P�����v�a�i���l�j" color="#c0c0c0">
      <tag entry="00" japanese="�P��" english="�P��" view="EP" width="8" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
      </tag>
      <tag entry="01" japanese="�Q��" english="�Q��" view="EP" width="16" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
      </tag>
      <tag entry="02" japanese="�R��" english="�R��" view="EP" width="24" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
      </tag>
      <tag entry="03" japanese="�S��" english="�S��" view="EP" width="32" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
      </tag>
      <tag entry="04" japanese="�T��" english="�T��" view="EP" width="40" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
      </tag>
      <tag entry="05" japanese="�U��" english="�U��" view="EP" width="48" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
        <comment language="japanese">
</comment>
        <comment language="english">
</comment>
      </tag>
      <tag entry="06" japanese="�V��" english="�V��" view="EP" width="56" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
      </tag>
      <tag entry="07" japanese="�W��" english="�W��" view="EP" width="64" line="1">
        <parameter number="0" japanese="�ԍ�" english="�ԍ�" type="int8"/>
      </tag>
    </group>

    <group entry="189" japanese="�ėp�R���g���[��" english="�ėp�R���g���[��" color="#c0c0c0">
      <tag entry="00" japanese="�J���[�ύX" english="�J���[�ύX" view="EP" length="0" line="1">
        <parameter number="0" japanese="�����F�ԍ�" english="�����F�ԍ�" type="int8"/>
        <parameter number="1" japanese="�e�F�ԍ�" english="�e�F�ԍ�" type="int8"/>
        <parameter number="2" japanese="�w�i�F�ԍ�" english="�w�i�F�ԍ�" type="int8"/>
        <comment language="japanese">�V�X�e���̃J���[�^�O���ׂ����ݒ肪�\�Ȓ჌�x���ŁB
�قƂ�ǂ̏ꍇ�A�V�X�e���^�O���g���Ζ��Ȃ��Ǝv���܂��B</comment>
        <comment language="english">�V�X�e���̃J���[�^�O���ׂ����ݒ肪�\�Ȓ჌�x���ŁB
�قƂ�ǂ̏ꍇ�A�V�X�e���^�O���g���Ζ��Ȃ��Ǝv���܂��B</comment>
      </tag>
      <tag entry="01" japanese="�J���[�ύX���Z�b�g" english="�J���[�ύX���Z�b�g" view="EP" length="0" line="1"/>
      <tag entry="02" japanese="�w�Z���^�����O" english="�w�Z���^�����O" view="EP" length="0" line="1"/>
      <tag entry="03" japanese="�w�E��" english="�w�E��" view="EP" length="0" line="1">
        <parameter number="0" japanese="�E�X�y�[�X" english="�E�X�y�[�X" type="int8"/>
        <comment language="japanese">���Y�s�̕������
�E�B���h�E���ŉE�񂹂��܂��B</comment>
        <comment language="english">���Y�s�̕������
�E�B���h�E���ŉE�񂹂��܂��B</comment>
      </tag>
    </group>

    <group entry="190" japanese="���I�R���g���[��" english="Word" color="#c0c0c0">
      <tag entry="00" japanese="�y�[�W�؂�ւ��i�s����j" english="�y�[�W�؂�ւ��i�s����j" view="EP" length="0" line="1"/>
      <tag entry="01" japanese="�y�[�W�؂�ւ��i�N���A�j" english="�y�[�W�؂�ւ��i�N���A�j" view="EP" length="0" line="1"/>
      <tag entry="02" japanese="�E�F�C�g�ύX�i�P���j" english="�E�F�C�g�ύX�i�P���j" view="EP" length="0" line="1">
        <parameter number="0" japanese="�t���[����" english="�t���[����" type="int16"/>
      </tag>
      <tag entry="03" japanese="�E�F�C�g�ύX�i�p���j" english="�E�F�C�g�ύX�i�p���j" view="EP" length="0" line="1">
        <parameter number="0" japanese="�t���[����" english="�t���[����" type="int16"/>
      </tag>
      <tag entry="04" japanese="�E�F�C�g���Z�b�g" english="�E�F�C�g���Z�b�g" view="EP" length="0" line="1"/>
      <tag entry="05" japanese="�R�[���o�b�N�����ύX�i�P���j" english="�R�[���o�b�N�����ύX�i�P���j" view="EP" length="0" line="1">
        <parameter number="0" japanese="����" english="����" type="int16"/>
      </tag>
      <tag entry="06" japanese="�R�[���o�b�N�����ύX�i�p���j" english="�R�[���o�b�N�����ύX�i�p���j" view="EP" length="0" line="1">
        <parameter number="0" japanese="����" english="����" type="int16"/>
      </tag>
      <tag entry="07" japanese="�R�[���o�b�N�������Z�b�g" english="�R�[���o�b�N�������Z�b�g" view="EP" length="0" line="1"/>
      <tag entry="08" japanese="�`��̈�N���A" english="�`��̈�N���A" view="EP" length="0" line="1"/>
    </group>

    <group entry="254" japanese="������W�J" english="Word">
    </group>

    <set japanese="500%" english="500%" value="FF:01:�T�C�Y:500"/>
    <set japanese="400%" english="400%" value="FF:01:�T�C�Y:400"/>
    <set japanese="300%" english="300%" value="FF:01:�T�C�Y:300"/>
    <set japanese="200%" english="200%" value="FF:01:�T�C�Y:200"/>
    <set japanese="150%" english="150%" value="FF:01:�T�C�Y:150"/>
    <set japanese="100%" english="100%" value="FF:01:�T�C�Y:100"/>
    <set japanese="75%" english="75%" value="FF:01:�T�C�Y:75"/>
    <set japanese="50%" english="50%" value="FF:01:�T�C�Y:50"/>
    <set japanese="�J���[�f�t�H���g" english="Black" value="FF:00:�F:0"/>
    <set japanese="�J���[�P" english="Red" value="FF:00:�F:1"/>
    <set japanese="�J���[�Q" english="Green" value="FF:00:�F:2"/>
    <set japanese="�J���[�R" english="Blue" value="FF:00:�F:3"/>
  </tag-table>

  <columns>
    <id position="0" width="151"/>
    <group default=" 0:�f�t�H���g" position="-1" width="100">
      <list entry="0" japanese="�f�t�H���g" english="Default" colorID="0"/>
    </group>
    <number position="-1" width="100"/>
    <comment position="3" width="288"/>
    <erase position="-1" width="100"/>
    <language japanese="JPN" english="JPN">
      <message position="1" width="274"/>
      <font default=" 0:large" position="4" width="109">
        <list entry="0" file="W:\font\large.nftr"/>
        <list entry="1" file="W:\font\small.nftr"/>
      </font>
      <size position="-1" width="100"/>
      <space position="-1" width="100"/>
      <width position="6" width="70"/>
      <line position="-1" width="100"/>
    </language>
    <language japanese="JPN_KANJI" english="JPN_KANJI">
      <message position="2" width="246"/>
      <font default=" 0:large" position="5" width="106">
        <list entry="0" file="W:\font\large.nftr"/>
        <list entry="1" file="W:\font\small.nftr"/>
      </font>
      <size position="-1" width="100"/>
      <space position="-1" width="100"/>
      <width position="7" width="75"/>
      <line position="-1" width="100"/>
    </language>
  </columns>

  <dialog left="32" top="207" right="684" bottom="827" maximized="no" rayout="Vertical2">
    <comment-input left="733" top="589" right="1080" bottom="739" visible="no"/>
    <tag-palette left="736" top="133" right="1209" bottom="531" visible="no"/>
    <band id="0" index="0" style="0000" width="477"/>
    <band id="1" index="1" style="0000" width="165"/>
    <band id="2" index="2" style="0000" width="644"/>
    <band id="3" index="3" style="0000" width="644"/>
    <edit entry="0" language="JPN_KANJI" mode="Edit" spell-check="-1"/>
    <edit entry="1" language="JPN" mode="Preview" spell-check="-1"/>
    <edit entry="2" language="JPN" mode="Source" spell-check="-1"/>
    <edit entry="3" language="JPN" mode="Source" spell-check="-1"/>
  </dialog>

  <output default="�f�t�H���g">
    <target japanese="�f�t�H���g" english="default" endian="big" encoding="Shift_JIS" letter-list-encoding="Shift_JIS" header-file-encoding="Shift_JIS" language="JPN">
      <struct name="JMSMesgEntry" id="" message="mesgOffset">
      </struct>
      <entry sort="id" id-block="no" flowlabel-block="no"/>
      <header divide="no" id-prefix="JMS_" query-prefix="JMS_QUERY_" flownode-prefix="JMS_FLOWNODE_" id="" struct="" query=""/>
    </target>
  </output>

'

  # �擪�̋󔒕������폜���ĕԂ�
  return header.lstrip

end


#==================================================================================
# ��brief:  gmm�t�b�^�����擾����
# ��return: gmm�t�b�^���̕�����
#==================================================================================
def GetGMMFooter

  footer = '

  <lock>
    <operation add_message="no" header_setting="no" tag_setting="no" output_setting="no" display="no" sort="no" flowchart_window="no"/>
  </lock>

  <flowchart-group-list>
    <flowchart-group japanese="root" english="root" group-id="0" parent-id="-1"/>
  </flowchart-group-list>
</body>

</gmml> 

'

  # �����̋󔒂��폜���ĕԂ�
  return footer.rstrip

end


#==================================================================================
# ��brief: gmm�t�@�C�����o�͂���
# ��param: directory       �o�͐�f�B���N�g��
# ��param: fileName        �o�̓t�@�C����
# ��param: stringLavel     �e�L�X�g�̃��x�����z��
# ��param: stringJPN       ������f�[�^�z�� ( �Ђ炪�� )
# ��param: stringJPN_KANJI ������f�[�^�z�� ( ���� )
#==================================================================================
def GenerateGMM( directory, fileName, stringLavel, stringJPN, stringJPN_KANJI )

  # �����̕�����𐮌`
  0.upto( stringLavel.size - 1 ) do |strIdx|
    # ���[�̋󔒂��폜
    stringLavel    [ strIdx ].strip!
    stringJPN      [ strIdx ].strip!
    stringJPN_KANJI[ strIdx ].strip!
    # ���s�_�~�[�R�[�h�𕜌�
    if stringJPN[ strIdx ].include?( DUMMY_RETURN_CODE ) then
      stringJPN[ strIdx ].gsub!( DUMMY_RETURN_CODE, RETURN_CODE )
    end
    if stringJPN_KANJI[ strIdx ].include?( DUMMY_RETURN_CODE ) then
      stringJPN_KANJI[ strIdx ].gsub!( DUMMY_RETURN_CODE, RETURN_CODE )
    end
    # �󔒃_�~�[�R�[�h�𕜌�
    if stringJPN[ strIdx ].include?( DUMMY_SPACE_CODE ) then
      stringJPN[ strIdx ].gsub!( DUMMY_SPACE_CODE, SPACE_CODE )
    end
    if stringJPN_KANJI[ strIdx ].include?( DUMMY_SPACE_CODE ) then
      stringJPN_KANJI[ strIdx ].gsub!( DUMMY_SPACE_CODE, SPACE_CODE )
    end
  end

  # gmm �e�L�X�g�񋓕������쐬
  gmmRows = ""
  0.upto( stringLavel.size - 1 ) do |strIdx|
    row = '
      <row id="%s">
        <language name="JPN" width="256">%s</language>
        <language name="JPN_KANJI" width="256">%s</language>
      </row>
      ' % [ stringLavel[strIdx], stringJPN[strIdx], stringJPN_KANJI[strIdx] ]
      row.strip!
      gmmRows += row
  end

  # gmm �t�@�C�����\�z
  gmmHeader = GetGMMHeader()
  gmmFooter = GetGMMFooter()
  gmmData   = gmmHeader + gmmRows + gmmFooter

  # gmm �t�@�C���o��
  file = File.open( directory + "/" + fileName, "w" )
  file.puts( gmmData.toutf8 )  # UTF-8 �ɕϊ����ďo��
  file.close

end 
