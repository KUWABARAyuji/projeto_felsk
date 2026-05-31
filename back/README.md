# Backend C++ - Marido de Aluguel

Backend em C++ sem Crow, organizado em DDD, com DTOs e banco de dados em arquivo `.txt`.

## O que foi configurado

- API HTTP nativa em C++.
- Sem `crow.h`.
- Sem MySQL.
- Sem dependências externas obrigatórias.
- Banco em `data/banco_dados.txt`.
- DTOs em `include/application/dto`.
- Estrutura profissional com `include` e `src`.
- Rotas compatíveis com o front Angular enviado.

## Estrutura

```txt
back/
├── include/
│   ├── domain/model
│   ├── domain/repository
│   ├── application/dto
│   ├── application/service
│   ├── infrastructure/storage
│   ├── infrastructure/repository
│   └── presentation/http
├── src/
│   ├── domain/model
│   ├── application/dto
│   ├── application/service
│   ├── infrastructure/storage
│   ├── infrastructure/repository
│   ├── presentation/http
│   └── main.cpp
├── data/banco_dados.txt
└── CMakeLists.txt
```

## Por que existe `include` e `src`?

`include` guarda os arquivos `.hpp`, ou seja, a declaração das classes.

`src` guarda os arquivos `.cpp`, ou seja, a implementação dos métodos.

Essa separação deixa o projeto mais organizado e mais parecido com projetos C++ profissionais.

## Rotas usadas pelo front

```txt
GET   /clientes
POST  /clientes

GET   /trabalhadores
GET   /trabalhadores/disponiveis?data=2026-06-10

GET   /contratos
POST  /contratos

PATCH /contratos/{id}/confirmar
PATCH /contratos/{id}/concluir
PATCH /contratos/{id}/cancelar
```

Também existem rotas extras para manutenção:

```txt
GET    /health
GET    /clientes/{id}
PUT    /clientes/{id}
DELETE /clientes/{id}

GET    /trabalhadores/{id}
POST   /trabalhadores
PUT    /trabalhadores/{id}
DELETE /trabalhadores/{id}
POST   /trabalhadores/{id}/habilidades

GET    /contratos/{id}
POST   /orcamentos
```

## Como compilar

No terminal dentro da pasta `back`:

```bash
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

No Linux, pode usar:

```bash
cmake -S . -B build
cmake --build build
```

## Como rodar

```bash
./build/felsk_api
```

No Windows:

```powershell
.\build\felsk_api.exe
```

Ou dê duplo clique em:

```txt
rodar_backend.bat
```

## Testes rápidos

```bash
curl http://localhost:8080/health
curl http://localhost:8080/clientes
curl http://localhost:8080/trabalhadores
curl http://localhost:8080/contratos
```

Criar cliente:

```bash
curl -X POST http://localhost:8080/clientes \
  -H "Content-Type: application/json" \
  -d '{"nome":"Teste","telefone":"47999999999","cpf":"00000000000","endereco":"Rua Teste"}'
```

Criar contrato:

```bash
curl -X POST http://localhost:8080/contratos \
  -H "Content-Type: application/json" \
  -d '{"idCliente":1,"idTrabalhador":1,"habilidade":"Eletricista","data":"2026-06-10","horas":2,"descricao":"Trocar tomada"}'
```

Confirmar contrato:

```bash
curl -X PATCH http://localhost:8080/contratos/1/confirmar
```

Concluir contrato:

```bash
curl -X PATCH http://localhost:8080/contratos/1/concluir \
  -H "Content-Type: application/json" \
  -d '{"avaliacao":5}'
```

## Banco TXT

O banco fica em:

```txt
data/banco_dados.txt
```

Ele armazena:

- IDs automáticos.
- Clientes.
- Trabalhadores.
- Habilidades.
- Contratos.
- Status dos contratos.
- Avaliações.

Sempre que a API cria ou atualiza algo, esse arquivo é salvo novamente.

## Variáveis opcionais

Porta diferente:

```bash
APP_PORT=8081 ./build/felsk_api
```

Caminho diferente para o banco:

```bash
BANCO_TXT_CAMINHO=data/outro_banco.txt ./build/felsk_api
```
