<p align="center">
  <img src="https://user-images.githubusercontent.com/74484618/174520473-17ce38cc-004e-4056-8790-33cb450bdbb8.png" />
</p>

# > What is this??

Original language to C trancpiler.

# ./DEMO

<img width="400" src="https://user-images.githubusercontent.com/74484618/185827769-eda96111-ba2b-43e7-af03-4c3e0eda1662.png"> 

# > Features

既存のフレームワークを一切使用せずに実装され、多言語に対応したトランスパイラ。

手続き型／関数型／オブジェクト指向に対応。

ガーベージコレクションの代替として、所有権システムを実装。

現段階ではC/C++, Python, LLVM IR　に変換可能。

直接的で単純な変換により既存のフレームワークやプロジェクトにそのまま使用可能。

Rust, Python, C++などに似た直感的で美しい文法を採用。

# > Requirement

```
> Macintosh ver # 開発環境

                 -/+:.          
                :++++.          OS: 64bit Mac OS X 11.5.1 20G80
               /+++/.           Kernel: x86_64 Darwin 20.6.0
       .:-::- .+/:-``.::-       Uptime: 3h 15m
    .:/++++++/::::/++++++/:`    Packages: 296
  .:///////////////////////:`   Shell: fish 3.4.1
  ////////////////////////`     Resolution: 3360x2100 ,1360x768
 -+++++++++++++++++++++++`      DE: Aqua
 /++++++++++++++++++++++/       WM: Quartz Compositor
 /sssssssssssssssssssssss.      WM Theme: Blue (Dark)
 :ssssssssssssssssssssssss-     Disk: 245G
  osssssssssssssssssssssssso/`  CPU: Apple M1
  `syyyyyyyyyyyyyyyyyyyyyyyy+`  GPU: Apple M1
   `ossssssssssssssssssssss/    RAM: 8192MiB
     :ooooooooooooooooooo+.
      `:+oo+/:-..-:/+o+/-

> clang -v
    Homebrew clang version 14.0.6
    Target: x86_64-apple-darwin20.6.0
    Thread model: posix

> cmake --version
    cmake version 3.23.2

    CMake suite maintained and supported by Kitware (kitware.com/cmake).

❯ python3 --version                                                                                                                                                                                                                                              ─╯
    Python 3.10.4

```

# > cmake build

```
~/cru > mkdir build

~/cru > cd build

~/build > cmake ..
```

# > Usage

Run test script.

```
~/cru > cd sample 

~/sample > python3 test.py
```

# > Note

```
~/cru > ls

CMakeLists.txt         -  cmake buile file
LICENSE                -  LICENSE
README.md              -  作品説明補足&作品の保存場所
compile_commands.json  -  clang format setting
build/                 -  プログラムファイル一式
    CMakeCache.txt     -  cmake cache
    CMakeFiles/        -  cmake settings
    Makefile           -  Makefile
    cmake_install.cmake-  cmake settings
    cru*               <- プログラムファイル

sample/                -  cru source sample
src/                   -  ソースコード
std.h                  -  CRU C コンパイルオプション用のヘッダファイル
```

# > ./License

```
Copyright (c) 2022  Dangomushi
This software is released under the MIT License, see LICENSE, see LICENSE.
This content is released under the CC BY 4.0 License, see LICENSE.
```

