@.str.0 = private unnamed_addr constant [3 x i8] c"%d\00", align 4

define i32 @fibonacci(i32 noundef %0) #0 {
entry:
    %1 = alloca i32, align 4
    store i32 %0, i32* %1, align 4
    %2 = load i32, i32* %1, align 4
    %3 = icmp eq i32 %2, 1
    br i1 %3, label %4, label %5
4:
    ret i32 0
5:
    %6 = load i32, i32* %1, align 4
    %7 = icmp eq i32 %6, 2
    br i1 %7, label %8, label %9
8:
    ret i32 1
9:
    %10 = load i32, i32* %1, align 4
    %11 = sub nsw i32 %10, 1
    %12 = call i32 @fibonacci(i32 noundef %11)

    %13 = load i32, i32* %1, align 4
    %14 = sub nsw i32 %13, 2
    %15 = call i32 @fibonacci(i32 noundef %14)

    %16 = add nsw i32 %12, %15
    ret i32 %16

}

define i32 @main() #2 {
entry:
        %0 = alloca i32, align 4
        store i32 9, i32* %0, align 4
        %1 = load i32, i32* %0, align 4
        %2 = call i32 @fibonacci(i32 noundef %1)
        %3 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([3 x i8], [3 x i8]* @.str.0, i64 0, i64 0), i32 noundef %2)
        ret i32 0
}


declare i32 @printf(i8* noundef, ...) #1