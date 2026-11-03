* DOS _IOCTRL(MD=0)が、ハンドル番号ではなくハンドルの実体で答えることの確認
*
* 標準ハンドル0～2の値を表示し、_DUP(1)で複製したハンドルの値を表示する。
* 次に自分自身(ioctrl_dup2.s)を開いて_DUP2でハンドル0に付け替え、ハンドル0の
* 値と、そこから_READした最初のバイトを表示する。
* 期待する出力は expected.txt。ioctrl_dup2.x はHAS060.XとHLKで作成した:
*   run68 HAS060.X -w ioctrl_dup2.s
*   run68 hlk.x ioctrl_dup2.o

	.include	doscall.mac
	.text
start:
	moveq	#0,d7
@@:	bsr	ioctrl
	addq.w	#1,d7
	cmpi.w	#3,d7
	bne	@b
	move.w	#1,-(sp)
	DOS	_DUP
	addq.l	#2,sp
	move.w	d0,d7
	bsr	ioctrl
	clr.w	-(sp)
	pea	self(pc)
	DOS	_OPEN
	addq.l	#6,sp
	move.w	d0,d6
	clr.w	-(sp)
	move.w	d6,-(sp)
	DOS	_DUP2
	addq.l	#4,sp
	move.w	d6,-(sp)
	DOS	_CLOSE
	addq.l	#2,sp
	moveq	#0,d7
	bsr	ioctrl
	move.l	#1,-(sp)
	pea	buf(pc)
	clr.w	-(sp)
	DOS	_READ
	lea	10(sp),sp
	bsr	hex
	moveq	#0,d0
	move.b	buf(pc),d0
	bsr	hex
	pea	crlf(pc)
	DOS	_PRINT
	addq.l	#4,sp
	DOS	_EXIT

* ハンドルd7の番号と_IOCTRL(MD=0)の値を1行に表示する
ioctrl:	moveq	#0,d0
	move.w	d7,d0
	bsr	hex
	move.w	d7,-(sp)
	clr.w	-(sp)
	DOS	_IOCTRL
	addq.l	#4,sp
	bsr	hex
	pea	crlf(pc)
	DOS	_PRINT
	addq.l	#4,sp
	rts

hex:	movem.l	d0-d2,-(sp)
	moveq	#8-1,d2
	move.l	d0,d1
@@:	rol.l	#4,d1
	moveq	#$0f,d0
	and.b	d1,d0
	move.b	digits(pc,d0.w),d0
	move.w	d0,-(sp)
	DOS	_PUTCHAR
	addq.l	#2,sp
	dbra	d2,@b
	move.w	#' ',-(sp)
	DOS	_PUTCHAR
	addq.l	#2,sp
	movem.l	(sp)+,d0-d2
	rts
digits:	.dc.b	'0123456789ABCDEF'

	.data
self:	.dc.b	'ioctrl_dup2.s',0
crlf:	.dc.b	13,10,0
	.bss
buf:	.ds.b	2
	.end	start
