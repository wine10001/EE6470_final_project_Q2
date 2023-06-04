## multiple accelerator PEs with a multi-core riscv-vp platform - Handwritten digit recognition


## Usage
1. Clone the file to ./ee6470 folder (Git Bash)
```properties
git clone https://github.com/wine10001/EE6470_final_project_Q2.git riscv-vp
```
2. Open Windows Powershell
```properties
docker run -h ubuntu --rm --cap-add SYS_ADMIN -it -v "C:\\Users\\<Your Username>:/home/user" ee6470/ubuntu-ee6470:latest
```
3. Login
```properties
/usr/local/bin/entrypoint.sh
```
4. Move to build directory
```properties
cd ~/ee6470
```
```properties
source bashrc
```
5. Build the "basic-acc" platform of riscv-vp
```properties
cd riscv-vp/vp/build
```
```properties
cmake ..
```
```properties
make install
```
6. Build neural network software
```properties
cd ~/ee6470
```
```properties
cd riscv-vp/sw/basic-nn
```
```properties
make
```
```properties
make sim
```