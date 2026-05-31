# Projeto Felsk - Front Angular + Backend C++ TXT

Este pacote foi configurado para o front Angular funcionar com o backend C++.

## Pastas

```txt
projeto_felsk_profissional_txt_ddd/
├── front/  # Front Angular enviado
└── back/   # API C++ sem Crow, com DDD, DTOs e banco TXT
```

## Rodar o backend

```bash
cd back
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
./build/felsk_api.exe
```

No PowerShell:

```powershell
cd back
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
.\build\felsk_api.exe
```

Teste:

```txt
http://localhost:8080/health
http://localhost:8080/clientes
http://localhost:8080/trabalhadores
http://localhost:8080/contratos
```

## Rodar o front

Em outro terminal:

```bash
cd front
npm install
npm start
```

Abra:

```txt
http://localhost:4200
```

## Integração front-back

O front já usa:

```ts
apiUrl: 'http://localhost:8080'
```

O backend responde exatamente as rotas que o serviço Angular chama:

```txt
GET   /clientes
POST  /clientes
GET   /trabalhadores
GET   /trabalhadores/disponiveis?data=AAAA-MM-DD
GET   /contratos
POST  /contratos
PATCH /contratos/{id}/confirmar
PATCH /contratos/{id}/concluir
PATCH /contratos/{id}/cancelar
```

## Banco

O banco é um arquivo TXT:

```txt
back/data/banco_dados.txt
```

Ele é criado/atualizado automaticamente pelo backend.
