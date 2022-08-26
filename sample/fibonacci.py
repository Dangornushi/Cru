
def fibonacci (num) :
    if num==1:
        return 0
    if num==2:
        return 1
    return fibonacci(num-1)+fibonacci(num-2)
def main () :
    x = 9
    print(fibonacci(x))
    return 0




main()