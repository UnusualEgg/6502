  .org $0000
  .org $8000
  lda #$ee
  sta $4000
  lda #$ff
  .byte $02

  .org $fffc
  .word $8000