# ComputerNetworkPJ
## 开发环境: Linux
## 工具
```bash
    sudo apt install gcc-multilib
```
## 运行客户端
```bash
    make ftp
    ./ftp <IP> <port>
```
## 运行服务端
```bash
    make ftp-server
    ./ftp-server <port>
```
## 测试
```bash
    sudo apt-get install gcc-multilib
    make
    Terminal1:~$ .server/ftp-server <port>
    Terminal2:~$ ./ftp <IP> <port>
    Terminal2:~$ ftp>put test/<file name>
```
test/test1.txt: 文本文件  
test/test: 可执行文件   
test/test3.jpg: 图片  
