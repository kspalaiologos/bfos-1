nav r1
#emit("<<<")

#times("raw .<", 0x100000 - 0xb8000)

mov r1, .h
raw .>
raw .>
mov r1, .e
raw .>
raw .>
mov r1, .l
raw .>
raw .>
mov r1, .l
raw .>
raw .>
mov r1, .o

#times("raw .>", 0x100000 - 0xb8000 - 8)
