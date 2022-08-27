@.str.0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 4

define i32 @main() #1 {
entry:
    %0 = alloca i32*, align 8
    %1 = alloca i32, align 4
    store i32* %1, i32** %0, align 8
    store i32 12, i32* %1, align 4

    %2 = load i32*, i32** %0, align 8
    %3 = load i32, i32* %2, align 4
    %4 = add nsw i32 %3, 2

    %5 = load i32*, i32** %0, align 8
    %6 = load i32, i32* %5, align 4
    %7 = add nsw i32 %4, %6

    %8 = add nsw i32 %7, 4

    %9 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.0, i64 0, i64 0), i32 noundef %8)
    ret i32 0
}


declare i32 @printf(i8* noundef, ...) #0
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #1
