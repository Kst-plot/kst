# http://clang.llvm.org/docs/UsersManual.html#controlling-code-generation

list(APPEND clflg -fno-omit-frame-pointer)
list(APPEND clflg -fno-optimize-sibling-calls)

list(APPEND clflg -fsanitize=address)
list(APPEND clflg -fsanitize=init-order)
list(APPEND clflg -fsanitize=address-full)
list(APPEND clflg -fsanitize=integer)

#list(APPEND clflg -fsanitize=thread)
#list(APPEND clflg -fsanitize=memory)
#list(APPEND clflg -fsanitize=undefined) # Qt uses reinterpret_cast<T>(0)->staticMetaObject
list(APPEND clflg -fsanitize=undefined-trap)
#list(APPEND clflg -fsanitize=alignment)
list(APPEND clflg -fsanitize=bool)
list(APPEND clflg -fsanitize=bounds)
list(APPEND clflg -fsanitize=enum)
list(APPEND clflg -fsanitize=float-cast-overflow)
list(APPEND clflg -fsanitize=float-divide-by-zero)
list(APPEND clflg -fsanitize=integer-divide-by-zero)
list(APPEND clflg -fsanitize=null)
list(APPEND clflg -fsanitize=object-size)
list(APPEND clflg -fsanitize=return)
list(APPEND clflg -fsanitize=shift)
list(APPEND clflg -fsanitize=signed-integer-overflow)
list(APPEND clflg -fsanitize=unreachable)
list(APPEND clflg -fsanitize=unsigned-integer-overflow)
list(APPEND clflg -fsanitize=vla-bound)
#list(APPEND clflg -fsanitize=vptr)
list(APPEND clflg -fsanitize=use-after-return)
list(APPEND clflg -fsanitize=use-after-scope)


message(STATUS "Used sanitize flags:")
set(clang_flags)
foreach(it ${clflg})
    message(STATUS "    ${it}")
    set(clang_flags "${clang_flags} ${it}")
endforeach()


