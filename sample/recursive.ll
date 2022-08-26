@.str.0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 4

define i32 @recursive(i32* noundef %0) #0 {
entry:
    %1 = alloca i32*, align 8
    store i32* %0, i32** %1, align 8
    %2 = load i32*, i32** %1, align 8
    %3 = load i32, i32* %2, align 4
    %4 = icmp eq i32 %3, 0
    br i1 %4, label %5, label %6
5:
    ret i32 1
6:
    %7 = load i32*, i32** %1, align 8
    %8 = load i32, i32* %7, align 4
    %9 = load i32*, i32** %1, align 8
    %10 = load i32, i32* %9, align 4
    %11 = sub nsw i32 %10, 1

    %12 = alloca i32, align 8
    store  i32 %11, i32* %12, align 8
    %13 = call i32 @recursive(i32* noundef %12)

    %14 = mul nsw i32 %8, %13

    ret i32 %14

}

define i32 @main() #2 {
entry:
    %0 = alloca i32*, align 8
    %1 = alloca i32, align 4
    store i32* %1, i32** %0, align 8
    store i32 6, i32* %1, align 4

    %2 = load i32*, i32** %0 , align 8
    %3 = call i32 @recursive(i32* noundef %2)
    %4 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.0, i64 0, i64 0), i32 noundef %3)
    ret i32 0
}


declare i32 @printf(i8* noundef, ...) #1
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #1
