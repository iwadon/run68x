* プログラムが横取りしたtrap #9の呼び出し
*
* DOS _INTVCSでtrap #9を横取りし、trap #9を2回実行する。ハンドラは呼ばれた
* 回数を数えてrteで戻る。最後にベクタを元に戻し、回数と、rteで戻った後の
* モード(ユーザーならU、スーパーバイザならS)を表示する。
* 期待する出力は expected.txt。trap9.x はHAS060.XとHLKで作成した:
*   run68 HAS060.X -w trap9.s
*   run68 hlk.x trap9.o

	.include	doscall.mac
	.text
start:
	pea	handler(pc)
	move.w	#$29,-(sp)
	DOS	_INTVCS
	addq.l	#6,sp
	move.l	d0,old
	trap	#9
	trap	#9
	move.w	sr,d0
	btst	#13,d0
	beq	user
	move.b	#'S',mode
user:
	move.l	old,-(sp)
	move.w	#$29,-(sp)
	DOS	_INTVCS
	addq.l	#6,sp
	move.w	count,d0
	add.b	#'0',d0
	move.b	d0,digit
	pea	message(pc)
	DOS	_PRINT
	addq.l	#4,sp
	DOS	_EXIT

handler:
	addq.w	#1,count
	rte

	.data
message:	.dc.b	'trap #9: '
digit:		.dc.b	'?',' '
mode:		.dc.b	'U',13,10,0
	.even
count:		.dc.w	0
old:		.dc.l	0
	.end	start
