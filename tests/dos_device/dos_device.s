* DOS _OPENによるデバイスのオープンとDOS _IOCTRL(MD=0)の動作確認
*
* 開いた結果、_IOCTRL(0)の値、NULからの_READのバイト数を16進数で表示する。
* 期待する出力は expected.txt。dos_device.x はHAS060.XとHLKで作成した:
*   run68 HAS060.X -w dos_device.s
*   run68 hlk.x dos_device.o

	.include	doscall.mac
	.text
start:
	lea	names(pc),a3
loop:	tst.b	(a3)
	beq	fds
	move.l	a3,-(sp)
	DOS	_PRINT
	addq.l	#4,sp
	pea	sep(pc)
	DOS	_PRINT
	addq.l	#4,sp
	clr.w	-(sp)
	move.l	a3,-(sp)
	DOS	_OPEN
	addq.l	#6,sp
	move.l	d0,d7
	bsr	hex
	tst.l	d7
	bmi	next
	move.w	d7,-(sp)
	clr.w	-(sp)
	DOS	_IOCTRL
	addq.l	#4,sp
	bsr	hex
	move.l	#$100,-(sp)	* read 256 bytes (only for NUL)
	pea	buf(pc)
	move.w	d7,-(sp)
	cmpi.b	#'N',(a3)
	bne	1f
	DOS	_READ
	bsr	hex
1:	lea	10(sp),sp
	move.w	d7,-(sp)
	DOS	_CLOSE
	addq.l	#2,sp
next:	pea	crlf(pc)
	DOS	_PRINT
	addq.l	#4,sp
@@:	tst.b	(a3)+
	bne	@b
	bra	loop
fds:	lea	fdlist(pc),a3
fdl:	move.w	(a3)+,d7
	bmi	wcon
	move.w	d7,d0
	ext.l	d0
	bsr	hex
	move.w	d7,-(sp)
	clr.w	-(sp)
	DOS	_IOCTRL
	addq.l	#4,sp
	bsr	hex
	pea	crlf(pc)
	DOS	_PRINT
	addq.l	#4,sp
	bra	fdl
wcon:	move.w	#1,-(sp)
	pea	con(pc)
	DOS	_OPEN
	addq.l	#6,sp
	move.l	d0,d7
	move.l	#msgend-msg,-(sp)
	pea	msg(pc)
	move.w	d7,-(sp)
	DOS	_WRITE
	lea	10(sp),sp
	DOS	_EXIT

hex:	movem.l	d0-d2,-(sp)
	moveq	#8-1,d2
	move.l	d0,d1
@@:	rol.l	#4,d1
	move.b	d1,d0
	andi.w	#$f,d0
	move.b	hexd(pc,d0.w),d0
	move.w	d0,-(sp)
	DOS	_PUTCHAR
	addq.l	#2,sp
	dbra	d2,@b
	move.w	#' ',-(sp)
	DOS	_PUTCHAR
	addq.l	#2,sp
	movem.l	(sp)+,d0-d2
	rts
hexd:	.dc.b	'0123456789ABCDEF'

	.data
names:	.dc.b	'NUL',0,'nul',0,'A:\nodir\NUL',0,'nul.txt',0,'CON',0,'AUX',0,'PRN',0,'CLOCK',0,'NULL',0,'CONFIG',0,'dos_device.s',0,0
con:	.dc.b	'con',0
sep:	.dc.b	': ',0
crlf:	.dc.b	13,10,0
msg:	.dc.b	'written to CON',13,10
msgend:
	.even
fdlist:	.dc.w	0,1,2,3,4,5,7,19,20,99,-1
	.bss
buf:	.ds.b	256
