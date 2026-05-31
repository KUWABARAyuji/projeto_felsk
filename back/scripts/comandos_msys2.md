# Comandos rápidos no MSYS2/MinGW

```bash
cd back
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
./build/felsk_api.exe
```

Se usar PowerShell:

```powershell
cd back
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
.\build\felsk_api.exe
```
