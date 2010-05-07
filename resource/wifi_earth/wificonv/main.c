//-------------------------------------------------------------------
// WifiConv.exe
//
// DP�̎��ɍ�����n���V���W��荞�݃c�[�����ēx���p
//
// 2010.05.04:�����X�g�̃G�N�Z���\�����ς�����̂ň����u3�v�̎������ύX
//
//
// AkitoMori GameFreak inc.
//-------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// �o�C�i���ɂ���Ƃ��Ɏg���\����
typedef struct{
  short x;
  short y;
}POS_XY;

typedef struct{
  short type;
  short x;
  short y;
}POS_FLAG_XY;


//------------------------------------------------------------------
/**
 * $brief   XY�`���Ŏ�荞��
 *
 * @param   fp    
 * @param   buf   
 * @param   str[][200]    
 * @param   posxy   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int GetTextInfo0( FILE *fp, char *buf, char str[][200], POS_XY *posxy)
{
  int count=0;
  int  no,  x, y;

  // �t�@�C�����I���܂�
  while(fgets(buf,1000,fp)!=NULL){

    // �����񂩂��荞�݁i�����E�����E���l�E���l�j
    if(sscanf(buf,"%[^,],%[^,],%x,%x" ,str[0],str[1],&x,&y)==EOF){
      printf("���̍s�œ��̓G���[���N���܂���\n%s\n",buf);
      exit(1);
    }
    printf("%s, %s,  %x, %x, \n",str[0],str[1],x,y);

    // �\���̂Ɋi�[
    posxy[count].x = x;
    posxy[count].y = y;

    // ��荞�񂾉񐔂��{�P
    count++;
    
  }
  return count;
}

//------------------------------------------------------------------
/**
 * $brief   �^�C�v�{XY�Ŏ�荞��
 *
 * @param   fp    
 * @param   buf   
 * @param   str[][200]    
 * @param   posfxy    
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int GetTextInfo1( FILE *fp, char *buf, char str[][200], POS_FLAG_XY *posfxy)
{
  int count=0;
  int  no, x, y, d1,d2,flag;

  // �t�@�C�����I���܂�
  while(fgets(buf,1000,fp)!=NULL){

    // �����񂩂��荞�݁i�����E�����E�����E���l�E���l��2�ڂ̐��l��10�i�ŕϊ��j
    if(sscanf(buf,"%[^,],%[^,],%[^,],%d,%x,%x,%d,%d" ,str[0],str[1],str[2],&d1,&x,&y,&d2,&flag)==EOF){
      printf("���̍s�œ��̓G���[���N���܂���\n%s\n",buf);
      exit(1);
    }
    printf("%s, %s, %s, %x, %x, %d\n",str[0],str[1],str[2],x,y,flag);

    // �\���̂Ɋi�[
    posfxy[count].type = flag;
    posfxy[count].x    = x;
    posfxy[count].y    = y;

    // ��荞�񂾉񐔂��{�P
    count++;
    
  }
  return count;
}


//==============================================================================
/**
 * $brief   ���C��
 *
 * @param   argc    
 * @param   argv    
 *
 * @retval  int   
 */
//==============================================================================
int main(int argc, char **argv)
{
  char  buf[1000];    //����͓K��
  char  str[3][200];    //����͓K��
  POS_XY      posxy[10000];   //����͓K��
  POS_FLAG_XY posfxy[10000];    //����͓K��
  int  count;
  int convert_mode = 0;
  FILE *fp,*outfp;
  
  // �I�v�V�����̐��𐔂���
  if(argc!=4){
    printf("argc = %d\n",argc);
    printf("�t�@�C���w�肵�Ă�������\n");
    printf("earthconv.exe <0,1> input output\n");
    exit(1);
  }
  
  if(argv[1][0]=='2' || argv[1][0]=='3'){
    convert_mode = argv[1][0]-'2';
  }else{
    printf("��P�����́u2�v���u3�v���w�肵�Ă�������\n�o�̓f�[�^���Q��(XY)���R��(TYPE,XY)���Ƃ����Ӗ��ł�\n");
    exit(1);
  }
  
  // ���̓t�@�C���I�[�v��
  if((fp=fopen(argv[2],"r"))==NULL){
    printf("%s���ǂݍ��߂܂���\n",argv[2]);
    exit(1);
  }

  // �o�̓t�@�C���I�[�v��
  if((outfp=fopen(argv[3],"wb"))==NULL){
    printf("%s�����܂���\n",argv[3]);
    exit(1);
  }


  // 2�s�ǂݎ̂Ă�
  fgets(buf,1000,fp);
  fgets(buf,1000,fp);
  

  // �R���o�[�g�J�n
  if(convert_mode==0){

    // XY�̂ݎ��o�����[�h
    count = GetTextInfo0(fp, buf, str, posxy);

    printf("��荞�񂾍s��=%d �o�̓o�C�g��=%d\n", count, count*sizeof(POS_XY));

    // �o��
    fwrite((void*)&posxy, sizeof(POS_XY)*count, 1, outfp);

  }else{
    // TYPE�EXY�����o�����[�h
    count = GetTextInfo1(fp, buf, str, posfxy);

    printf("��荞�񂾍s��=%d �o�̓o�C�g��=%d\n", count, count*sizeof(POS_FLAG_XY));

    // �o��
    fwrite((void*)&posfxy, sizeof(POS_FLAG_XY)*count, 1, outfp);
  }


  
  return 0;
}
