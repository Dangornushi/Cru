@.str.0 = private unnamed_addr constant [3 x i8] c"%d\00", align 4

define i32 @recursive(i32 noundef %0) #0 {
entry:
    %1 = alloca i32, align 4
    store i32 %0, i32* %1, align 4
    %2 = load i32, i32* %1, align 4
    %3 = icmp eq i32 %2, 0
    br i1 %3, label %4, label %5
4:
    ret i32 1
5:
    %6 = load i32, i32* %1, align 4
    %7 = load i32, i32* %1, align 4
    %8 = sub nsw i32 %7, 1
    %9 = call i32 @recursive(i32 noundef %8)

    %10 = mul nsw i32 %6, %9
    ret i32 %10

}

define i32 @main() #2 {
entry:
        %0 = alloca i32, align 4
        store i32 6, i32* %0, align 4
        %1 = load i32, i32* %0, align 4
        %2 = call i32 @recursive(i32 noundef %1)
        %3 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([3 x i8], [3 x i8]* @.str.0, i64 0, i64 0), i32 noundef %2)
        ret i32 0
}


declare i32 @printf(i8* noundef, ...) #1