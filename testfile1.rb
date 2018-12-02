# Program 1: Vypocet faktorialu (iterativne)
print "Zadejte cislo pro vypocet faktorialu: "
a = inputf()
if a < 1 then
    print( "\nFaktorial nelze spocitat \xFF \t \n")
else
    vysl = 1
    while a > 0 do
        vysl = vysl * a
        a = a - 1
    end
    print "\nVysledek je:", a, "\n"
end