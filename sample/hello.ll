@.__const.main.hello = private unnamed_addr constant [14 x i8] c"Hello, World!!", align 1
@.str.0 = private unnamed_addr constant [3 x i8] c"%s\00", align 4

define i32 @main() #1 {
entry:
    %0 = alloca [14 x i8], align 1
    %1 = bitcast [14 x i8]* %0 to i8*
    call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 1 %1, i8* align 1 getelementptr inbounds ([14 x i8], [14 x i8]* @.__const.main.hello, i32 0, i32 0), i64 14, i1 false)
    %2 = getelementptr inbounds [14 x i8], [14 x i8]* %0, i64 0, i64 0
    %3 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([3 x i8], [3 x i8]* @.str.0, i64 0, i64 0), i8* noundef %2)
    ret i32 0
}


declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #1declare i32 @printf(i8* noundef, ...) #0