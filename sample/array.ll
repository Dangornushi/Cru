@.str.0 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 4
@.str.1 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 4
@.str.2 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 4
@str.3 = private unnamed_addr constant [ 6x i8 ] c"Hello\00", align 1
@str.4 = private unnamed_addr constant [ 11x i8 ] c"dangomushi\00", align 1
@str.5 = private unnamed_addr constant [ 12x i8 ] c"warajimushi\00", align 1
@__const.main.x = private unnamed_addr constant [3 x i8*] [i8* getelementptr inbounds ([6 x i8], [6 x i8]* @str.3, i32 0, i32 0),i8* getelementptr inbounds ([11 x i8], [11 x i8]* @str.4, i32 0, i32 0),i8* getelementptr inbounds ([12 x i8], [12 x i8]* @str.5, i32 0, i32 0)], align 16

define i32 @sub(i8** noundef %0) #1 {
entry:
    %1 = alloca i8**, align 8
    store i8** %0, i8*** %1, align 8
    %2 = load i8**, i8*** %1, align 8
    %3 = getelementptr inbounds i8*, i8** %2, i64 0
    %4 = load i8*, i8** %3, align 8
    %5 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.0, i64 0, i64 0), i8* noundef %4)
    %6 = load i8**, i8*** %1, align 8
    %7 = getelementptr inbounds i8*, i8** %6, i64 1
    %8 = load i8*, i8** %7, align 8
    %9 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.1, i64 0, i64 0), i8* noundef %8)
    %10 = load i8**, i8*** %1, align 8
    %11 = getelementptr inbounds i8*, i8** %10, i64 2
    %12 = load i8*, i8** %11, align 8
    %13 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.2, i64 0, i64 0), i8* noundef %12)
    ret i32 0
}

define i32 @main() #2 {
entry:
    %0 = alloca [ 3 x i8* ], align 16
    %1 = bitcast [3 x i8*]* %0 to i8*
    call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %1, i8* align 16 bitcast ([3x i8*]* @__const.main.x to i8*), i64 24, i1 false)
        %2 = getelementptr inbounds [3x i8*], [3x i8*]*%0, i64 0, i64 0
    %3 = call i32 @sub(i8** noundef %2)

    ret i32 0
}


declare i32 @printf(i8* noundef, ...) #0
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #1
