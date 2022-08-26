@str.0 = private unnamed_addr constant [ 6x i8 ] c"Hello\00", align 1
@str.1 = private unnamed_addr constant [ 11x i8 ] c"dangomushi\00", align 1
@str.2 = private unnamed_addr constant [ 12x i8 ] c"warajimushi\00", align 1
@__const.main.x = private unnamed_addr constant [3 x i8*] [i8* getelementptr inbounds ([6 x i8], [6 x i8]* @str.0, i32 0, i32 0),i8* getelementptr inbounds ([11 x i8], [11 x i8]* @str.1, i32 0, i32 0),i8* getelementptr inbounds ([12 x i8], [12 x i8]* @str.2, i32 0, i32 0)], align 16
@.str.3 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 4
@.str.4 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 4
@.str.5 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 4

define i32 @main() #1 {
entry:
    %0 = alloca [ 3 x i8* ], align 16
    %1 = bitcast [3 x i8*]* %0 to i8*
    call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %1, i8* align 16 bitcast ([3x i8*]* @__const.main.x to i8*), i64 24, i1 false)
    %2 = getelementptr inbounds [3 x i8*], [3 x i8*]*%0, i64 0, i64 0
    %3 = load i8*, i8** %2, align 8
    %4 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.3, i64 0, i64 0), i8* noundef %3)
    %5 = getelementptr inbounds [3 x i8*], [3 x i8*]*%0, i64 0, i64 1
    %6 = load i8*, i8** %5, align 8
    %7 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.4, i64 0, i64 0), i8* noundef %6)
    %8 = getelementptr inbounds [3 x i8*], [3 x i8*]*%0, i64 0, i64 2
    %9 = load i8*, i8** %8, align 8
    %10 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.5, i64 0, i64 0), i8* noundef %9)
    ret i32 0
}


declare i32 @printf(i8* noundef, ...) #0
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #1
