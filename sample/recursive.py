
def recursive (num) :
    if num==0:
        return 1
    return num*recursive(num-1)
def main () :
    x = 6
    print(recursive(x))
    return 0




main()