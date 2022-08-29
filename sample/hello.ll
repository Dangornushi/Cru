@.str.0 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 4
@.str.a = private unnamed_addr constant [19 x i8] c"Hello, Dangmushi!!\00", align 1
@.str.1 = private unnamed_addr constant [ 11 x i8 ] c"Test, OK.\0A\00", align 1

define i32 @sub(i8* noundef %0) #1 {
entry:
    %1 = alloca i8*, align 8
    store i8* %0, i8** %1, align 8
    %2 = load i8*, i8** %1, align 8
    %3 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.0, i64 0, i64 0), i8* noundef %2)
    ret i32 0
}

define i32 @main() #2 {
entry:
    %0 = alloca i8*, align 8
    store i8* getelementptr inbounds ([19 x i8], [19 x i8]* @.str.a, i64 0, i64 0), i8** %0, align 8
        %1 = load i8*, i8** %0 , align 8
    %2 = call i32 @sub(i8* noundef %1)

    %3 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([ 11 x i8 ], [ 11 x i8 ]* @.str.1, i64 0, i64 0))
    ret i32 0
}


declare i32 @printf(i8* noundef, ...) #0
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #1
