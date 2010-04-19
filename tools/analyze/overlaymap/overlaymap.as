;--------------------------------------------------------------------------
; xMap�t�@�C������͂��ăI�[�o�[���C�z�u����}������c�[��
; 
; Akito Mori 2008/01/01
; 
; HSP 2.6�ō쐬
; ���s�ɂ�hspda.dll��imgctl.hpi���K�v�i�����t�H���_�ɂ��̃t�@�C��������΂悢�j
; 
; �O���t�쐬�������PNG�Ƃ��ďo�́B
; graphviz�p��dot�t�@�C�����o�͂��܂��B
;--------------------------------------------------------------------------
#include "llmod.as"
#include "misc.as"
#include "hspda.as"
#include "imgctl.as"
;--------------------------------------------
; �֐����
;--------------------------------------------
#module
#deffunc PrintOverlayName str,int,int
	mref name, 32
	mref x,1
	mref y,2

;	dialog "name="+name+" x="+x+" y="+y,0

	color 255,255,255
	pos x-1,y
	print name
	pos x+1,y
	print name
	pos x,y+1
	print name
	pos x,y-1
	print name

	color 0,0,0
	pos x,y
	print name
	
	return

#global


;--------------------------------------------
;--------------------------------------------
; ���C��
;--------------------------------------------
	gsel 0,-1
	cls 1
	width 150,60
	title "Overlay Map"

;	screen 0,150,60,4,,,150,60
	gsel 0,1

	pos 0,0
	objsize 150,30
	button "open",*openfile
	pos 0,30
	button "window off",*window_off

	
	OVERLAY_MAX = 400
	sdim overlayname,64,OVERLAY_MAX		;�I�[�o�[���C�̈�̖��O
	dim  overlayinfo,OVERLAY_MAX,5		;�e�L�X�g��񂩂��荞�񂾐��l
	dim  overlaylink,OVERLAY_MAX,100	;�I�[�o�[���C�J�n�A�h���X�ƕʃI�[�o�[���C�̔z�u��Ԃ̊֌W���i�[
	dim  overlaylinkcount,OVERLAY_MAX	;���̃I�[�o�[���C���Q�Ƃ���Ă����
	dim  stack,500			;
	sdim temp,  512
	sdim temp2, 256
	sdim temp3, 512
	sdim temp4, 256
	sdim mapfile, 1000000		;�}�b�v�t�@�C���̓ǂݍ��݃o�b�t�@
	sdim xmapfile,20000000
	sdim dotbuf,  1000000		; Graphviz�p�����o�b�t�@
	sdim filename, 512

	OVERLAY_NAME_SIZE = 40
	IMG_WIDTH         = 7000
	IMG_HEIGHT        = 1300

	screen 2,IMG_WIDTH,IMG_HEIGHT,2,,,1024,900
	font "Arial",10
	
	gsel 0

	;----------------------------------------------
	; �������w�肳��Ă�����
	;----------------------------------------------
	filename = cmdline
	automode = 0
	strlen cmdline_length,filename
	if cmdline_length!=0 {
		automode=1
		exist filename
		if strsize=-1{
			dialog ""+filename+"�͑��݂��܂���",0
			end
		}
		goto *readfile
	}
;--------------------------------------------
;�@���[�v
;--------------------------------------------
*main
	wait 30
	if automode=1{
;		dialog "�I��",0
		end
	}
	goto main



;--------------------------------------------
;�@�����N�v�Z
;--------------------------------------------
*linkcalc
	linkcount = 0
	repeat lines
		linkp = cnt
		repeat lines

		; ���W���[���̂��K���ʂȃ��W���[���̃g�b�v�ɂȂ��Ă��邩�H
		
		if (overlayinfo.linkp.4 = overlayinfo.cnt.0) & (linkp!=cnt) {
			p = overlaylinkcount.linkp
			overlaylink.linkp.p = cnt
			overlaylinkcount.linkp+
			linkcount+
		}
		loop
	loop
	

	return


;--------------------------------------------
;�@�����N��ԃe�X�g�v�����g
;--------------------------------------------
*linkprint
	gsel 2,-1
	printpos = 0
	repeat lines
		if overlaylinkcount.cnt != 0{
			pos 100, printpos*11
			color 0,0,0
			print ""+cnt+": "+overlayname.cnt+" linkcount="+overlaylinkcount.cnt
			printpos+
		}
	loop

	font "�l�r�޼��",20
	gsel 2
	pos 400,10
	print "�����N������������="+linkcount
	font "Arial",10

	return

;--------------------------------------------
;�@�ċN�����N��ԃJ�E���g�擾
;--------------------------------------------
*linkcountget
	searchtop = 0
	stackmax  = 0
	stacknow = 0
	searchcount = 0
	searchnow   = searchtop
	repeat
;		dialog "searchnow="+searchnow+" count="+overlaylinkcount.searchnow+" name="+overlayname.searchnow,0
		if overlaylinkcount.searchnow != 0 {
			repeat overlaylinkcount.searchnow
				stack.stackmax = overlaylink.searchnow.cnt
				stackmax+
			loop
		}
 		else{
			if stacknow=stackmax{
				break
			}
		}
		searchnow = stack.stacknow
		stacknow+
	loop

;	dialog "stack="+stackmax,0

	return

;--------------------------------------------
;�@�l�`�o���\�[�g
;--------------------------------------------
*sortinfo
	repeat lines-1
		src = cnt+1
		repeat lines-1
			des = cnt+1
			if overlayinfo.src.0 < overlayinfo.des.0{
				t0 = overlayinfo.des.0
				t1 = overlayinfo.des.1
				t2 = overlayinfo.des.2
				t3 = overlayinfo.des.3
				t4 = overlayinfo.des.4
				temp = overlayname.des

				overlayinfo.des.0 = overlayinfo.src.0
				overlayinfo.des.1 = overlayinfo.src.1
				overlayinfo.des.2 = overlayinfo.src.2
				overlayinfo.des.3 = overlayinfo.src.3
				overlayinfo.des.4 = overlayinfo.src.4
				overlayname.des   = overlayname.src

				overlayinfo.src.0 = t0
				overlayinfo.src.1 = t1
				overlayinfo.src.2 = t2
				overlayinfo.src.3 = t3
				overlayinfo.src.4 = t4
				overlayname.src   = temp
			}
		loop
	loop
	return

;--------------------------------------------
;�@���[�N������
;--------------------------------------------
*initwork

	;�Q�Ə�񏉊���
	repeat OVERLAY_MAX
		linkp = cnt
		repeat 100
			overlaylink.linkp.cnt = -1
		loop
	loop

	

	return

;--------------------------------------------
; map�t�@�C������f�[�^�擾
;--------------------------------------------
*mapfile2addr

	notemax lines

	infono=0

	;�Q�s�����Ɏ擾���A�����I�[�o�[���C�̈���w���Ă���ꍇ�͊i�[����
	;�܂��A�擪�A�h���X��00000000�̏ꍇ�����O����
	readline = startline+1
	repeat 
		readline = readline + 1

		;1�s�ڂ̎擾
		noteget temp,readline

		;2�s�ڂ̎擾
		noteget temp3,(readline+1)

		; �擾�����s�ɕ��͂��Ȃ�������I�[�o�[���C��ԏI���Ƃ݂Ȃ�
		strlen length,temp3
		if length < 5 {
			break
		}

		; 2�s���̃I�[�o�[���C�����擾
		strmid temp2, temp,29,OVERLAY_NAME_SIZE
		strmid temp4, temp3,29,OVERLAY_NAME_SIZE
		
		;��v���邩��r����
		strlen length1, temp2
		strlen length2, temp4
		if length1=length2 {
			instr flag, temp2,temp4,0
			if flag=-1{
				continue
			}else{
;				dialog ""+readline+"�s�ڂƎ��̍s�͈�v���܂���["+temp2+"]",0
			}
		}else{
			continue
		}
		
		;��v���Ă��A�h���X��00000000�̏ꍇ�͔�΂�
		strmid temp2,temp,2,8
		strtoint temp2,16
		if stat=0 {
			continue
		}

		

		;�����܂Œʂ��Ă�����i�[����
		strmid temp2, temp,2,8		;�X�^�[�g�ʒu�擾
		strtoint temp2,16
		overlayinfo.infono.0 = stat

		strmid temp2, temp,20,8		;�T�C�Y�擾
		strtoint temp2,16
		overlayinfo.infono.1 = stat

		strmid temp2, temp3,2,8		;BSS�X�^�[�g�ʒu�擾
		strtoint temp2,16
		overlayinfo.infono.2 = stat

		strmid temp2, temp3,20,8	;BSS�T�C�Y�擾
		strtoint temp2,16
		overlayinfo.infono.3 = stat

		;���T�C�Y�擾
		overlayinfo.infono.4 = overlayinfo.infono.0+overlayinfo.infono.1+overlayinfo.infono.3

		;�I�[�o�[���C���擾
		strmid temp2, temp, 29,OVERLAY_NAME_SIZE
		overlayname.infono = temp2
		
		infono+
	loop

	lines = infono

	return



;--------------------------------------------
; map�t�@�C������f�[�^�擾����BMP�ɕ`��
;--------------------------------------------
*openfile
	if openfileflag=1{
		dialog "����܂���B2��͓ǂݍ��߂܂ւ�",0
		goto *main
	}

	openfileflag =1

	dialog "xMap",16,"map�t�@�C��"
	if stat=0 : goto *main
	exist refstr
	if stat=0 : goto *main

	filename = refstr
*readfile
	bload filename, xmapfile
;	dialog "�t�@�C���T�C�Y"+strsize,0

	count=0

	notesel xmapfile
	notemax lines

;	dialog "�s���T�C�Y"+lines,0
	hitflag = 0

	repeat lines
		noteget temp,lines-cnt-1

		strlen length,temp
		if length>10{
			instr flag, temp, "# Memory map",0
			if flag!=-1{
				startline = lines-cnt-1
				hitflag = 1
;				dialog "�uMemory map�v��"+startline+"�ł݂�������",0
				break

			}
			count = count + 1
			if count=500{
				break
			}
		}
	loop
	if hitflag=0{
		title ""+temp
		dialog "������500�s�T�������ǂ݂���Ȃ�������",0
		goto *main
	}

	;--------�t�@�C���ǂݍ���-------------
;	bload "map.txt",mapfile
;	notesel mapfile
	


	; ---- �����N���񏉊��� ----
	gosub initwork


	;-----mapfile����I�[�o�[���C�̃A�h���X�E�T�C�Y�擾	-------
	gosub mapfile2addr

	;-----�A�h���X�̏��Ń\�[�g-----------
	gosub sortinfo

	;-----�����N�Q�Ƃ�����----------
	gosub linkcalc

	;-----���ʕ\��----------
	gosub linkprint

	;-----�탊���N�񐔂��J�E���g----------
	gosub linkcountget


	;--------------------------------------------
	;���\��
	;--------------------------------------------

;	repeat lines
;		pos 5, cnt*11
;		print overlayname.cnt +": start="+overlayinfo.cnt.0+" size="+overlayinfo.cnt.1+" bss="+overlayinfo.cnt.2+" bsssize="+overlayinfo.cnt.3+" end="+overlayinfo.cnt.4
;		calcresult = overlayinfo.cnt.4-33554432
;		size = overlayinfo.cnt.4 - overlayinfo.cnt.0
;		print overlayname.cnt +": end="+calcresult+" size="+size
;	loop

;	dialog "stop",0

	gWidth  = IMG_WIDTH
	gHeight = IMG_HEIGHT
;	DIV_RATE = 540704	; fieldmap����ԑ傫���݂���
	DIV_RATE = 900000
;	DIV_RATE = 2040704	; main������

;	DIV_RATE  = 3040704	; �v���O�����̍Ō�܂ł͂���
	DIV_NUM   = 250		; �_�O���t����ʓ��ŉ��������ĕ`�悷�邩
	BAR_WIDTH = (gWidth/DIV_NUM)

	sizemax=0

	

	;--------------------------------------------
	;�O���t�`��
	;--------------------------------------------

	repeat lines
		size = overlayinfo.cnt.4 - overlayinfo.cnt.0
		if size>sizemax {
			sizemax = size
		}
	loop

	pos 200,40
	print "sizemax="+sizemax

	;---------���g���`��--------------
	repeat 20
		color 200,200,200
		y1 = gHeight - (((cnt*(65536))*gHeight)/DIV_RATE) - 11
		line 0,y1,gWidth,y1
		pos  5,y1,

		;���g���̏��0x10000�����l����������
		color 0,0,0
		hexstr = cnt*65536
		str hexstr,22
		print "0x"+hexstr
	loop

	repeat lines
		size = overlayinfo.cnt.4 - overlayinfo.cnt.0
		top  = overlayinfo.cnt.0 - overlayinfo.0.4
		x1 = (gWidth*cnt)/DIV_NUM
		ys = (top*gHeight/DIV_RATE)
		y1 = gHeight - (((size*gHeight/DIV_RATE)+ys)  ) - 11
		y2 = gHeight - ys -11

		rnd r1,256
		rnd g1,256
		rnd b1,256
		
		color r1,g1,b1
		boxf x1,y1,(x1+BAR_WIDTH),y2
		line x1,y1,x1,0

;		if cnt<30{
;			dialog "size="+size+" top="+top+" x1="+x1+" y1="+y1+" y2="+y2+" main="+overlayinfo.0.0+" ntop="+overlayinfo.cnt.0,0
;		}

	loop	


	;--------------------------------------------
	;���O�\��
	;--------------------------------------------
	diffcount = 0
	repeat lines
		size = overlayinfo.cnt.4 - overlayinfo.cnt.0
		top  = overlayinfo.cnt.0 - overlayinfo.0.4
		diff = size-nsize
		if diff < 0 : diff = diff*-1
		if diff > (DIV_RATE/20) : diffcount=0

		x1 = (gWidth*cnt)/DIV_NUM
		ys = (top*gHeight/DIV_RATE)
		y1 = gHeight - (((size*gHeight/DIV_RATE)+ys)  ) - 11 -(diffcount*15)

		PrintOverlayName overlayname.cnt, x1, y1

		diffcount+
		if diffcount=10 : diffcount=0

		nsize = size
	loop

;	dialog "lines="+lines

	;------------------------------------------------
	;�`�悵���O���t��png�ŏo��
	;------------------------------------------------
;	bmpsave "overlay_graph.bmp"
	ic_DCtoDIB 2, 0, 0, IMG_WIDTH, IMG_HEIGHT, hdib
	if (stat != 0) { goto *main }	// �G���[

	ic_DIBtoPNG "overlay_graph.png", hdib

	; DIB�f�[�^���
	ic_DeleteDIB hdib


	;------------------------------------------------
	;�v�Z���������N����Graphviz�p�ɐ��`���ďo��
	;------------------------------------------------
	notesel dotbuf
	noteadd "digraph G {",-1
	noteadd "\tgraph [rankdir = RL]\;",-1
	noteadd "\tnode [shape = box]\;",-1

	;�I�[�o�[���C�̌����܂킷
	repeat lines
		nowpoint = cnt

		;�����N����Ă���񐔕��܂킷
		repeat overlaylinkcount.nowpoint
			link = overlaylink.nowpoint.cnt
;			dialog ""+overlayname.link+"->"+overlayname.nowpoint,0
			noteadd "\t"+overlayname.link+"->"+overlayname.nowpoint+";", -1
		loop
	loop

	noteadd "}",-1
	notesave "overlaymap.dot"

	gsel 2,1

	;----------�����-------------------
	goto *main

;--------------------------------------------
; overlay��͌��ʃE�C���h�EOFF
;--------------------------------------------
*window_off
	gsel 2,-1
	goto *main


