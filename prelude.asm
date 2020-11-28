extern bfcode
extern bfcode_size

section .stivalehdr

stivale_header:
  .stack:       dq stack.top
  .fbbpp:       dw 0
  .fbwidth:     dw 0
  .fbheight:    dw 0
  .flags:       dw 0
  .entry_point: dq 0

section .bss

stack:
    resb 4096
  .top:

section .text

global entry
entry:
    ; rdi+16 = memory_map_entries
    mov rcx, [rdi+16]
    ; rdi+8 = memory_map_addr
    mov rsi, [rdi+8]
    ; Keep highest base in rax
  .get_highest_usable_base:
    ; Check the type of the entry is usable
    cmp dword [rsi+16], 1
    jne .next_entry
    ; Check the entry is big enough for our purposes
    cmp qword [rsi+8], bfcode_size
    jb .next_entry
    ; Check if the entry is the highest up
    cmp qword [rsi], rdi
    jb .next_entry
    ; This is our guy, for now. Save its base in rdi and the address of the
    ; entry in rbx
    mov rdi, qword [rsi]
    mov rbx, rsi
  .next_entry:
    add rsi, 24
    loop .get_highest_usable_base
  .found_entry:
    ; Copy the bf code to entry base+length-bfcode.size and jump to it
    add rdi, qword [rbx+8]
    mov rcx, bfcode_size
    sub rdi, rcx
    mov rax, rdi
    mov rsi, bfcode
    rep movsb

    mov rbx, 0x100000
    jmp rax
