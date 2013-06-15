.model flat
.code
_asmrecurse proc
push esi
push edi
push ecx
push edx
mov ecx, dword ptr [esp + 14h]
mov edx, dword ptr [esp + 18h]
call arec
pop edx
pop ecx
pop edi
pop esi
ret
_asmrecurse endp
arec proc
push ebx
push edx
movzx ebx, word ptr [edx + 8]
test ebx, ebx
jz skip
mov eax, dword ptr [edx + 4]
mov esi, dword ptr [ecx + 4]
lea edi, [eax + 4*eax]
lea edx, [esi + 4*edi]
rloop:
call arec
add edx, 14h
dec ebx
jnz rloop
skip:
pop edx
pop ebx
ret
arec endp
end