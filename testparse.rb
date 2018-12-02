def fun1(c)
    id = fun2
    print("Fun 1: ", id, "\n")
end

def fun2()
    print("Fun 2:\n")
end

a = fun2

fun1(a)

print a
