; Autor reseni: Simona Jánošíková xjanos19

; Projekt 2 - INP 2022
; Vernamova sifra na architekture MIPS64
; xjanos19-r5-r3-r30-r7-r0-r4
; DATA SEGMENT
                .data
login:          .asciiz "xjanos19"  ; sem doplnte vas login
cipher:         .space  17  ; misto pro zapis sifrovaneho loginu

params_sys5:    .space  8   ; misto pro ulozeni adresy pocatku
                            ; retezce pro vypis pomoci syscall 5
                            ; (viz nize "funkce" print_string)

; CODE SEGMENT
                .text

                ; ZDE NAHRADTE KOD VASIM RESENIM
main:

l:
        LB R5,login(R7)
        SLTI R30,R5,97
        BNE R30,R0,e
        ADDI R5,R5,10
        MOVZ R3,R5,R0
        SLTI R3,R5,123 
        BNE  R3,R0,z1
        ADDI R5,R5,-26
           
z1:
        SB R5,cipher(R7)
        ADDI R7,R7,1

n:
        LB R5,login(R7)
        SLTI R30,R5,97
        BNE R30,R0,e
        ADDI R5,R5,-1
        MOVZ R3,R5,R0
        SLTI R3,R5,97
        BEQ  R3,R0,z2
        ADDI R5,R5,26

z2:
        SB R5,cipher(R7)
        ADDI R7,R7,1
        J l

e:
        NOP
        SB R0,cipher(R7)
        daddi   r4, r0, cipher   ; vozrovy vypis: adresa login: do r4
        jal     print_string    ; vypis pomoci print_string - viz nize
        syscall 0   ; halt

print_string:   ; adresa retezce se ocekava v r4
        sw      r4, params_sys5(r0)
        daddi   r14, r0, params_sys5    ; adr pro syscall 5 musi do r14
        syscall 5   ; systemova procedura - vypis retezce na terminal
        jr      r31 ; return - r31 je urcen na return address
