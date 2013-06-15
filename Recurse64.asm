code
asmrecurse proc
push rbx
push rdx
movzx ebx, word ptr [rdx + 8]
test ebx, ebx
jz skip
mov eax, dword ptr [rdx + 4]
mov r8, qword ptr [rcx + 8]
lea r9, [rax + 4*rax]
lea rdx, [r8 + 4*r9]
rloop:
call asmrecurse
add rdx, 14h
dec rbx
jnz rloop
skip:
pop rdx
pop rbx
ret
asmrecurse endp
end