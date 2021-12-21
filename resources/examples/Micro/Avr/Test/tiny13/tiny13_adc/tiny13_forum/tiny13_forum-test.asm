;Program compiled by Great Cow BASIC (0.98.02 2018-05-16 (Linux))
;Need help? See the GCBASIC forums at http://sourceforge.net/projects/gcbasic/forums,
;check the documentation or email w_cholmondeley at users dot sourceforge dot net.

;********************************************************************************

;Chip Model: TINY13A
;Assembler header file
.INCLUDE "tn13adef.inc"

;SREG bit names (for AVR Assembler compatibility, GCBASIC uses different names)
#define C 0
#define H 5
#define I 7
#define N 2
#define S 4
#define T 6
#define V 3
#define Z 1

;********************************************************************************

;Set aside memory locations for variables
.EQU	ADREADPORT=96
.EQU	LDR=97
.EQU	MYFLAG=98
.EQU	READAD=99
.EQU	SAVESREG=100
.EQU	SAVESYSVALUECOPY=101
.EQU	SYSINTSTATESAVE0=102
.EQU	TIMEOUT=103

;********************************************************************************

;Register variables
.DEF	DELAYTEMP=r25
.DEF	DELAYTEMP2=r26
.DEF	SYSBITTEST=r5
.DEF	SYSCALCTEMPA=r22
.DEF	SYSCALCTEMPB=r28
.DEF	SYSVALUECOPY=r21
.DEF	SYSWAITTEMP10US=r27
.DEF	SYSWAITTEMPMS=r29
.DEF	SYSWAITTEMPMS_H=r30
.DEF	SYSTEMP1=r0

;********************************************************************************

;Alias variables
#define	SYSREADADBYTE	99

;********************************************************************************

;Vectors
;Interrupt vectors
.ORG	0
	rjmp	BASPROGRAMSTART ;Reset
.ORG	1
	reti	;INT0
.ORG	2
	reti	;PCINT0
.ORG	3
	reti	;TIM0_OVF
.ORG	4
	reti	;EE_RDY
.ORG	5
	reti	;ANA_COMP
.ORG	6
	reti	;TIM0_COMPA
.ORG	7
	reti	;TIM0_COMPB
.ORG	8
	rjmp	IntWDT ;WDT
.ORG	9
	reti	;ADC

;********************************************************************************

;Start of program memory page 0
.ORG	11
BASPROGRAMSTART:
;Initialise stack
	ldi	SysValueCopy,low(RAMEND)
	out	SPL, SysValueCopy
;Call initialisation routines
	rcall	INITSYS
;Enable interrupts
	sei

;Start of the main program
;Source:F1L3S0I3
;Source:F1L4S0I4
;Source:F1L5S0I5
;Source:F1L6S0I6
;Source:F1L7S0I7
;Source:F1L8S0I8
;Source:F1L10S0I10
	sbi	DDRB,0
;Source:F1L11S0I11
	cbi	DDRB,2
;Source:F1L13S0I13
;Source:F1L14S0I14
	rcall	WATCHDOG_OFF
;Source:F1L17S0I17
;Source:F1L18S0I18
;Source:F1L19S0I19
;Source:F1L20S0I20
;Source:F1L22S0I22
	ldi	SysValueCopy,1
	sts	TIMEOUT,SysValueCopy
;Source:F1L23S0I23
	in	SysValueCopy,WDTCR
	sbr	SysValueCopy,1<<WDTIE
	out	WDTCR,SysValueCopy
;Source:F1L25S0I25
SysDoLoop_S1:
;Source:F1L26S0I26
	rcall	WDT_SLEEP
;Source:F1L27S0I27
	lds	SysTemp1,TIMEOUT
	dec	SysTemp1
	sts	TIMEOUT,SysTemp1
;Source:F1L28S0I28
	lds	SysCalcTempA,TIMEOUT
	tst	SysCalcTempA
	brne	ENDIF1
;Source:F1L29S0I29
	ldi	SysValueCopy,1
	sts	ADREADPORT,SysValueCopy
	rcall	FN_READAD6
	lds	SysValueCopy,SYSREADADBYTE
	sts	LDR,SysValueCopy
;Source:F1L30S0I30
	ldi	SysCalcTempA,60
	lds	SysCalcTempB,LDR
	cp	SysCalcTempA,SysCalcTempB
	brlo	ELSE2_1
;Source:F1L31S0I31
	sbi	PORTB,0
;Source:F1L32S0I32
	rjmp	ENDIF2
ELSE2_1:
;Source:F1L33S0I33
	cbi	PORTB,0
;Source:F1L34S0I34
ENDIF2:
;Source:F1L36S0I36
	ldi	SysValueCopy,1
	sts	TIMEOUT,SysValueCopy
;Source:F1L37S0I37
ENDIF1:
;Source:F1L38S0I38
	rjmp	SysDoLoop_S1
SysDoLoop_E1:
;Source:F2L146S0I146
;Source:F2L147S0I147
;Source:F2L151S0I151
;Source:F2L157S0I157
;Source:F2L162S0I162
;Source:F2L164S0I164
;Source:F2L165S0I165
;Source:F2L166S0I166
;Source:F2L167S0I167
;Source:F2L169S0I169
;Source:F2L172S0I172
;Source:F2L173S0I173
;Source:F2L174S0I174
;Source:F2L175S0I175
;Source:F2L179S0I179
;Source:F2L180S0I180
;Source:F2L181S0I181
;Source:F2L182S0I182
;Source:F2L183S0I183
;Source:F2L184S0I184
;Source:F2L185S0I185
;Source:F2L186S0I186
;Source:F2L187S0I187
;Source:F2L188S0I188
;Source:F2L189S0I189
;Source:F2L190S0I190
;Source:F2L191S0I191
;Source:F2L192S0I192
;Source:F2L193S0I193
;Source:F2L194S0I194
;Source:F2L195S0I195
;Source:F2L196S0I196
;Source:F2L197S0I197
;Source:F2L198S0I198
;Source:F2L199S0I199
;Source:F2L200S0I200
;Source:F2L201S0I201
;Source:F2L202S0I202
;Source:F2L203S0I203
;Source:F2L204S0I204
;Source:F2L205S0I205
;Source:F2L206S0I206
;Source:F2L207S0I207
;Source:F2L208S0I208
;Source:F2L209S0I209
;Source:F2L210S0I210
;Source:F2L211S0I211
;Source:F2L212S0I212
;Source:F2L213S0I213
;Source:F2L215S0I215
;Source:F2L216S0I216
;Source:F2L217S0I217
;Source:F2L218S0I218
;Source:F2L219S0I219
;Source:F2L220S0I220
;Source:F2L221S0I221
;Source:F2L222S0I222
;Source:F2L223S0I223
;Source:F2L224S0I224
;Source:F2L225S0I225
;Source:F2L226S0I226
;Source:F2L227S0I227
;Source:F2L228S0I228
;Source:F2L229S0I229
;Source:F2L230S0I230
;Source:F2L231S0I231
;Source:F2L232S0I232
;Source:F2L233S0I233
;Source:F2L234S0I234
;Source:F2L235S0I235
;Source:F2L236S0I236
;Source:F2L237S0I237
;Source:F2L238S0I238
;Source:F2L239S0I239
;Source:F2L240S0I240
;Source:F2L241S0I241
;Source:F2L242S0I242
;Source:F2L243S0I243
;Source:F2L244S0I244
;Source:F2L245S0I245
;Source:F2L246S0I246
;Source:F2L247S0I247
;Source:F2L248S0I248
;Source:F2L249S0I249
;Source:F2L252S0I252
;Source:F2L253S0I253
;Source:F2L254S0I254
;Source:F2L255S0I255
;Source:F2L256S0I256
;Source:F2L257S0I257
;Source:F2L258S0I258
;Source:F2L259S0I259
;Source:F2L260S0I260
;Source:F2L261S0I261
;Source:F2L262S0I262
;Source:F2L263S0I263
;Source:F2L264S0I264
;Source:F2L265S0I265
;Source:F2L267S0I267
;Source:F2L268S0I268
;Source:F2L269S0I269
;Source:F2L270S0I270
;Source:F2L271S0I271
;Source:F2L272S0I272
;Source:F2L273S0I273
;Source:F2L274S0I274
;Source:F2L275S0I275
;Source:F2L276S0I276
;Source:F2L277S0I277
;Source:F2L278S0I278
;Source:F2L279S0I279
;Source:F2L280S0I280
;Source:F2L281S0I281
;Source:F2L282S0I282
;Source:F2L283S0I283
;Source:F2L284S0I284
;Source:F2L285S0I285
;Source:F2L286S0I286
;Source:F2L287S0I287
;Source:F2L288S0I288
;Source:F2L289S0I289
;Source:F2L290S0I290
;Source:F2L291S0I291
;Source:F2L292S0I292
;Source:F2L293S0I293
;Source:F2L294S0I294
;Source:F2L295S0I295
;Source:F2L296S0I296
;Source:F2L297S0I297
;Source:F2L298S0I298
;Source:F2L299S0I299
;Source:F2L300S0I300
;Source:F2L301S0I301
;Source:F2L302S0I302
;Source:F2L305S0I305
;Source:F2L306S0I306
;Source:F2L307S0I307
;Source:F2L308S0I308
;Source:F2L309S0I309
;Source:F2L310S0I310
;Source:F2L311S0I311
;Source:F2L312S0I312
;Source:F2L313S0I313
;Source:F2L314S0I314
;Source:F2L315S0I315
;Source:F2L316S0I316
;Source:F2L317S0I317
;Source:F2L318S0I318
;Source:F2L319S0I319
;Source:F2L320S0I320
;Source:F2L321S0I321
;Source:F2L322S0I322
;Source:F2L323S0I323
;Source:F2L324S0I324
;Source:F2L325S0I325
;Source:F2L326S0I326
;Source:F2L327S0I327
;Source:F2L328S0I328
;Source:F2L329S0I329
;Source:F2L330S0I330
;Source:F2L331S0I331
;Source:F2L332S0I332
;Source:F2L333S0I333
;Source:F2L334S0I334
;Source:F2L335S0I335
;Source:F2L336S0I336
;Source:F2L337S0I337
;Source:F2L338S0I338
;Source:F2L339S0I339
;Source:F2L2063S0I38
;Source:F2L2064S0I39
;Source:F2L2065S0I40
;Source:F2L2066S0I41
;Source:F2L2068S0I43
;Source:F2L2069S0I44
;Source:F3L77S0I77
;Source:F3L78S0I78
;Source:F3L79S0I79
;Source:F3L80S0I80
;Source:F3L82S0I82
;Source:F3L1841S0I1706
;Source:F3L1974S0I51
;Source:F3L4037S0I160
;Source:F4L58S0I58
;Source:F4L59S0I59
;Source:F4L60S0I60
;Source:F4L63S0I63
;Source:F4L64S0I64
;Source:F4L67S0I67
;Source:F4L69S0I69
;Source:F4L118S0I118
;Source:F5L149S0I84
;Source:F6L25S0I25
;Source:F6L26S0I26
;Source:F6L54S0I23
;Source:F7L41S0I41
;Source:F7L42S0I42
;Source:F7L43S0I43
;Source:F7L44S0I44
;Source:F7L45S0I45
;Source:F7L46S0I46
;Source:F7L47S0I47
;Source:F7L49S0I49
;Source:F7L52S0I52
;Source:F7L53S0I53
;Source:F7L54S0I54
;Source:F7L247S0I21
;Source:F9L149S0I149
;Source:F9L152S0I152
;Source:F9L153S0I153
;Source:F9L156S0I156
;Source:F9L157S0I157
;Source:F9L159S0I159
;Source:F9L160S0I160
;Source:F9L162S0I162
;Source:F9L164S0I164
;Source:F9L165S0I165
;Source:F9L166S0I166
;Source:F9L167S0I167
;Source:F9L169S0I169
;Source:F9L170S0I170
;Source:F9L171S0I171
;Source:F9L173S0I173
;Source:F9L177S0I177
;Source:F9L179S0I179
;Source:F9L180S0I180
;Source:F9L181S0I181
;Source:F9L182S0I182
;Source:F9L185S0I185
;Source:F9L186S0I186
;Source:F9L188S0I188
;Source:F9L189S0I189
;Source:F9L190S0I190
;Source:F9L192S0I192
;Source:F9L193S0I193
;Source:F9L195S0I195
;Source:F9L196S0I196
;Source:F9L199S0I199
;Source:F9L200S0I200
;Source:F9L203S0I203
;Source:F9L207S0I207
;Source:F9L208S0I208
;Source:F9L328S0I9
;Source:F9L329S0I10
;Source:F10L34S0I34
;Source:F10L35S0I35
;Source:F10L36S0I36
;Source:F10L37S0I37
;Source:F10L38S0I38
;Source:F10L39S0I39
;Source:F10L40S0I40
;Source:F10L41S0I41
;Source:F11L202S0I202
;Source:F11L262S0I262
;Source:F11L263S0I263
;Source:F11L264S0I264
;Source:F11L318S0I318
;Source:F11L319S0I319
;Source:F11L320S0I320
;Source:F11L321S0I321
;Source:F11L322S0I322
;Source:F11L323S0I323
;Source:F11L325S0I325
;Source:F11L326S0I326
;Source:F11L327S0I327
;Source:F11L328S0I328
;Source:F11L329S0I329
;Source:F11L330S0I330
;Source:F11L332S0I332
;Source:F11L333S0I333
;Source:F11L334S0I334
;Source:F11L335S0I335
;Source:F11L336S0I336
;Source:F11L337S0I337
;Source:F11L339S0I339
;Source:F11L340S0I340
;Source:F11L341S0I341
;Source:F11L342S0I342
;Source:F11L343S0I343
;Source:F11L344S0I344
;Source:F11L346S0I346
;Source:F11L347S0I347
;Source:F11L348S0I348
;Source:F11L349S0I349
;Source:F11L350S0I350
;Source:F11L351S0I351
;Source:F11L353S0I353
;Source:F11L354S0I354
;Source:F11L355S0I355
;Source:F11L356S0I356
;Source:F11L357S0I357
;Source:F11L358S0I358
;Source:F11L363S0I363
;Source:F11L364S0I364
;Source:F11L365S0I365
;Source:F11L367S0I367
;Source:F11L368S0I368
;Source:F11L369S0I369
;Source:F11L370S0I370
;Source:F11L372S0I372
;Source:F11L374S0I374
;Source:F11L376S0I376
;Source:F11L377S0I377
;Source:F11L378S0I378
;Source:F11L379S0I379
;Source:F11L380S0I380
;Source:F11L381S0I381
;Source:F11L382S0I382
;Source:F11L383S0I383
;Source:F11L384S0I384
;Source:F11L385S0I385
;Source:F11L386S0I386
;Source:F11L387S0I387
;Source:F11L388S0I388
;Source:F11L389S0I389
;Source:F11L390S0I390
;Source:F11L391S0I391
;Source:F11L393S0I393
;Source:F11L394S0I394
;Source:F11L395S0I395
;Source:F11L396S0I396
;Source:F11L397S0I397
;Source:F11L398S0I398
;Source:F11L399S0I399
;Source:F11L400S0I400
;Source:F11L401S0I401
;Source:F11L402S0I402
;Source:F11L403S0I403
;Source:F11L404S0I404
;Source:F11L405S0I405
;Source:F11L406S0I406
;Source:F11L407S0I407
;Source:F11L408S0I408
;Source:F11L412S0I412
;Source:F11L413S0I413
;Source:F11L414S0I414
;Source:F11L415S0I415
;Source:F11L416S0I416
;Source:F11L417S0I417
;Source:F11L418S0I418
;Source:F11L419S0I419
;Source:F11L420S0I420
;Source:F11L421S0I421
;Source:F11L422S0I422
;Source:F11L423S0I423
;Source:F11L424S0I424
;Source:F11L425S0I425
;Source:F11L426S0I426
;Source:F11L427S0I427
;Source:F11L431S0I431
;Source:F11L432S0I432
;Source:F11L433S0I433
;Source:F11L434S0I434
;Source:F11L435S0I435
;Source:F11L436S0I436
;Source:F11L437S0I437
;Source:F11L438S0I438
;Source:F11L439S0I439
;Source:F11L440S0I440
;Source:F11L441S0I441
;Source:F11L442S0I442
;Source:F11L443S0I443
;Source:F11L503S0I503
;Source:F11L504S0I504
;Source:F11L506S0I506
;Source:F11L507S0I507
;Source:F11L509S0I509
;Source:F11L510S0I510
;Source:F11L512S0I512
;Source:F11L513S0I513
;Source:F11L515S0I515
;Source:F11L516S0I516
;Source:F11L518S0I518
;Source:F11L519S0I519
;Source:F11L531S0I531
;Source:F11L532S0I532
;Source:F11L533S0I533
;Source:F11L534S0I534
;Source:F11L535S0I535
;Source:F11L536S0I536
;Source:F11L537S0I537
;Source:F11L538S0I538
;Source:F11L541S0I541
;Source:F11L542S0I542
;Source:F11L543S0I543
;Source:F11L544S0I544
;Source:F11L547S0I547
;Source:F11L548S0I548
;Source:F11L549S0I549
;Source:F11L550S0I550
;Source:F11L553S0I553
;Source:F11L554S0I554
;Source:F11L555S0I555
;Source:F11L556S0I556
;Source:F11L559S0I559
;Source:F11L560S0I560
;Source:F11L561S0I561
;Source:F11L562S0I562
;Source:F11L673S0I673
;Source:F11L674S0I674
;Source:F11L675S0I675
;Source:F11L676S0I676
;Source:F11L679S0I679
;Source:F11L680S0I680
;Source:F11L681S0I681
;Source:F11L682S0I682
;Source:F12L57S0I57
;Source:F12L58S0I58
;Source:F12L59S0I59
;Source:F12L60S0I60
;Source:F12L63S0I63
;Source:F12L64S0I64
;Source:F12L65S0I65
;Source:F12L66S0I66
;Source:F12L67S0I67
;Source:F12L70S0I70
;Source:F13L42S0I42
;Source:F13L43S0I43
;Source:F13L44S0I44
;Source:F13L45S0I45
;Source:F13L46S0I46
;Source:F13L50S0I50
;Source:F13L51S0I51
;Source:F13L52S0I52
;Source:F13L53S0I53
;Source:F13L56S0I56
;Source:F13L57S0I57
;Source:F13L58S0I58
;Source:F13L59S0I59
;Source:F13L60S0I60
;Source:F13L61S0I61
;Source:F13L62S0I62
;Source:F13L63S0I63
;Source:F13L64S0I64
;Source:F13L66S0I66
;Source:F13L495S0I11
;Source:F14L57S0I57
;Source:F14L64S0I64
;Source:F14L66S0I66
;Source:F14L67S0I67
;Source:F14L68S0I68
;Source:F14L69S0I69
;Source:F14L70S0I70
;Source:F14L71S0I71
;Source:F14L72S0I72
;Source:F15L22S0I22
;Source:F15L25S0I25
;Source:F16L298S0I60
;Source:F17L88S0I88
;Source:F17L91S0I91
;Source:F17L94S0I94
;Source:F18L136S0I136
;Source:F18L137S0I137
;Source:F18L138S0I138
;Source:F18L139S0I139
;Source:F18L143S0I143
;Source:F18L144S0I144
;Source:F18L145S0I145
;Source:F18L146S0I146
;Source:F18L148S0I148
;Source:F18L149S0I149
;Source:F18L150S0I150
;Source:F18L155S0I155
;Source:F18L157S0I157
;Source:F18L158S0I158
;Source:F19L61S0I61
;Source:F19L89S0I89
;Source:F19L92S0I92
;Source:F19L93S0I93
;Source:F19L95S0I95
;Source:F19L96S0I96
;Source:F19L97S0I97
;Source:F19L98S0I98
;Source:F19L99S0I99
;Source:F19L101S0I101
;Source:F19L102S0I102
;Source:F19L103S0I103
;Source:F19L104S0I104
;Source:F19L384S0I19
;Source:F19L470S0I56
;Source:F19L686S0I16
;Source:F19L687S0I17
;Source:F19L688S0I18
;Source:F19L690S0I20
;Source:F19L691S0I21
;Source:F19L692S0I22
;Source:F19L693S0I23
;Source:F19L694S0I24
;Source:F19L695S0I25
;Source:F19L696S0I26
;Source:F19L697S0I27
;Source:F19L698S0I28
;Source:F19L700S0I30
;Source:F19L701S0I31
;Source:F19L702S0I32
;Source:F19L703S0I33
;Source:F19L704S0I34
;Source:F19L705S0I35
;Source:F19L706S0I36
;Source:F19L707S0I37
;Source:F19L708S0I38
;Source:F19L709S0I39
;Source:F19L711S0I41
;Source:F19L713S0I43
;Source:F19L714S0I44
;Source:F19L715S0I45
;Source:F19L717S0I47
;Source:F20L42S0I42
;Source:F20L43S0I43
;Source:F20L44S0I44
;Source:F20L45S0I45
;Source:F20L46S0I46
;Source:F20L47S0I47
;Source:F20L48S0I48
;Source:F20L49S0I49
;Source:F20L50S0I50
;Source:F20L51S0I51
;Source:F20L52S0I52
;Source:F20L53S0I53
;Source:F20L57S0I57
;Source:F20L58S0I58
BASPROGRAMEND:
	sleep
	rjmp	BASPROGRAMEND

;********************************************************************************

Delay_10US:
D10US_START:
	ldi	DELAYTEMP,15
DelayUS0:
	dec	DELAYTEMP
	brne	DelayUS0
	dec	SysWaitTemp10US
	brne	D10US_START
	ret

;********************************************************************************

Delay_MS:
	inc	SysWaitTempMS_H
DMS_START:
	ldi	DELAYTEMP2,123
DMS_OUTER:
	ldi	DELAYTEMP,12
DMS_INNER:
	dec	DELAYTEMP
	brne	DMS_INNER
	dec	DELAYTEMP2
	brne	DMS_OUTER
	dec	SysWaitTempMS
	brne	DMS_START
	dec	SysWaitTempMS_H
	brne	DMS_START
	ret

;********************************************************************************

INITSYS:
;Source:F12L909S168I818
	ldi	SysValueCopy,0
	out	PORTB,SysValueCopy
	ret

;********************************************************************************

IntWDT:
	rcall	SysIntContextSave
	rcall	WDT_WAKEUP
	in	SysValueCopy,TIFR0
	cbr	SysValueCopy,1<<TOV0
	out	TIFR0,SysValueCopy
	rjmp	SysIntContextRestore

;********************************************************************************

;Overloaded signature: BYTE:
FN_READAD6:
;Source:F2L1385S6I37
;Source:F2L1250S5I908
	lds	SysValueCopy,ADREADPORT
	out	ADMUX,SysValueCopy
;Source:F2L1268S5I926
;Source:F2L1269S5I927
	sbi	ADMUX,ADLAR
;Source:F2L1270S5I928
;Source:F2L1271S5I929
;Source:F2L1280S5I938
;Source:F2L1282S5I940
;
;Source:F2L1283S5I941
	cbi	ADMUX,REFS0
;Source:F2L1296S5I954
;Source:F2L1297S5I955
;Source:F2L1320S5I978
	sbi	ADCSRA,ADPS2
;Source:F2L1321S5I979
	cbi	ADCSRA,ADPS1
;Source:F2L1330S5I988
	ldi	SysWaitTemp10US,2
	rcall	Delay_10US
;Source:F2L1333S5I991
	sbi	ADCSRA,ADEN
;Source:F2L1335S5I993
;Source:F2L1338S5I996
	sbi	ADCSRA,ADSC
;Source:F2L1339S5I997
SysWaitLoop2:
	sbic	ADCSRA,ADSC
	rjmp	SysWaitLoop2
;Source:F2L1340S5I998
	cbi	ADCSRA,ADEN
;Source:F2L1448S6I100
	in	SysValueCopy,ADCH
	sts	READAD,SysValueCopy
	ret

;********************************************************************************

SysIntContextRestore:
;Restore registers
;Restore SREG
	lds	SysValueCopy,SaveSREG
	out	SREG,SysValueCopy
;Restore SysValueCopy
	lds	SysValueCopy,SaveSysValueCopy
	reti

;********************************************************************************

SysIntContextSave:
;Store SysValueCopy
	sts	SaveSysValueCopy,SysValueCopy
;Store SREG
	in	SysValueCopy,SREG
	sts	SaveSREG,SysValueCopy
;Store registers
	ret

;********************************************************************************

WATCHDOG_OFF:
;Source:F1L41S1I1
	lds	SysValueCopy,SYSINTSTATESAVE0
	cbr	SysValueCopy,1<<0
	brbc	I,PC + 2
	sbr	SysValueCopy,1<<0
	sts	SYSINTSTATESAVE0,SysValueCopy
	cli
;Source:F1L42S1I2
	wdr
;Source:F1L43S1I3
	ldi	SysValueCopy,0
	out	MCUSR,SysValueCopy
;Source:F1L44S1I4
	ldi	SysValueCopy,24
	out	WDTCR,SysValueCopy
;Source:F1L45S1I5
	ldi	SysValueCopy,0
	out	WDTCR,SysValueCopy
;Source:F1L46S1I6
	lds	SysBitTest,SYSINTSTATESAVE0
	sbrc	SysBitTest,0
	sei
	ret

;********************************************************************************

WATCHDOG_ON:
;Source:F1L50S2I1
	lds	SysValueCopy,SYSINTSTATESAVE0
	cbr	SysValueCopy,1<<1
	brbc	I,PC + 2
	sbr	SysValueCopy,1<<1
	sts	SYSINTSTATESAVE0,SysValueCopy
	cli
;Source:F1L51S2I2
	wdr
;Source:F1L53S2I4
	ldi	SysValueCopy,24
	out	WDTCR,SysValueCopy
;Source:F1L54S2I5
	ldi	SysValueCopy,105
	out	WDTCR,SysValueCopy
;Source:F1L55S2I6
	lds	SysBitTest,SYSINTSTATESAVE0
	sbrc	SysBitTest,1
	sei
	ret

;********************************************************************************

WDT_SLEEP:
;Source:F1L59S3I1
	ldi	SysValueCopy,3
	out	PRR,SysValueCopy
;Source:F1L60S3I2
	ldi	SysValueCopy,63
	out	DIDR0,SysValueCopy
;Source:F1L61S3I3
	rcall	WATCHDOG_ON
;Source:F1L62S3I4
	ldi	SysValueCopy,1
	sts	MYFLAG,SysValueCopy
;Source:F1L63S3I5
SysWaitLoop1:
	lds	SysCalcTempA,MYFLAG
	cpi	SysCalcTempA,1
	breq	SysWaitLoop1
	ret

;********************************************************************************

WDT_WAKEUP:
;Source:F1L69S4I1
	ldi	SysValueCopy,0
	out	MCUCR,SysValueCopy
;Source:F1L70S4I2
	ldi	SysValueCopy,0
	out	PRR,SysValueCopy
;Source:F1L71S4I3
	rcall	WATCHDOG_OFF
;Source:F1L72S4I4
	ldi	SysValueCopy,0
	sts	MYFLAG,SysValueCopy
	ret

;********************************************************************************

