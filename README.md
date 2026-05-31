# Projeto Felsk - Marido de Aluguel

## Integrantes

- Enzo Yuji Kuwabara
- Lucas Cavalcante
- Davi Lorenzo

## Objetivo

O Projeto é uma aplicação web para contratação e gerenciamento de serviços de marido de aluguel. O sistema permite cadastrar clientes, visualizar profissionais disponíveis, registrar solicitações de serviço, gerar contratos e acompanhar o andamento dos atendimentos.

A proposta do projeto é organizar, em uma única plataforma, a comunicação entre clientes que precisam de pequenos reparos residenciais e profissionais que oferecem serviços como elétrica, hidráulica, jardinagem, pintura, montagem de móveis, manutenção e outros serviços domésticos.

## Funcionalidades

- Cadastro de clientes.
- Listagem de profissionais.
- Cadastro de habilidades dos profissionais.
- Consulta de profissionais disponíveis.
- Registro de solicitações de serviço.
- Geração de contratos.
- Alteração de status dos contratos.
- Conclusão de contratos com avaliação do profissional.
- Armazenamento local das informações em arquivo `.txt`.

## Tecnologias utilizadas

### Front-end

- Angular 20.3.x
- Angular CLI 20.3.x
- TypeScript 5.9.x
- RxJS 7.8.x
- Zone.js 0.15.x
- HTML5
- SCSS

### Back-end

- C++17
- CMake 3.20 ou superior
- Ninja
- API HTTP em C++
- Arquivo `.txt` como armazenamento local
- Organização em camadas
- DTOs para entrada e saída de dados

## Estrutura do projeto

```txt
projeto_felsk/
├── front/
│   ├── src/
│   ├── angular.json
│   ├── package.json
│   └── tsconfig.json
│
├── back/
│   ├── include/
│   │   ├── domain/
│   │   ├── application/
│   │   ├── infrastructure/
│   │   └── presentation/
│   │
│   ├── src/
│   │   ├── domain/
│   │   ├── application/
│   │   ├── infrastructure/
│   │   ├── presentation/
│   │   └── main.cpp
│   │
│   ├── data/
│   │   └── banco_dados.txt
│   │
│   ├── CMakeLists.txt
│   └── rodar_backend.bat
│
└── README.md
```

## Organização do back-end

O back-end foi separado em camadas para facilitar manutenção e evolução do projeto.

```txt
back/include/domain
```

Contém as entidades principais do sistema, como Cliente, Trabalhador, Habilidade, Contrato e Data.

```txt
back/include/application
```

Contém os serviços da aplicação e os DTOs usados para transportar dados entre a API e as regras do sistema.

```txt
back/include/infrastructure
```

Contém a implementação do armazenamento em arquivo `.txt`.

```txt
back/include/presentation
```

Contém a camada HTTP responsável por receber as requisições do front-end.

## Banco de dados local

O projeto usa um arquivo `.txt` como armazenamento local:

```txt
back/data/banco_dados.txt
```

Esse arquivo guarda os dados de clientes, trabalhadores, habilidades, contratos e os contadores de IDs. Quando um cadastro é criado ou um contrato é atualizado, o arquivo é salvo novamente.

## Rotas principais da API

Por padrão, o back-end roda em:

```txt
http://localhost:8080
```

Rotas disponíveis:

```txt
GET    /health
GET    /clientes
POST   /clientes
GET    /trabalhadores
GET    /trabalhadores/disponiveis?data=2026-06-10
GET    /contratos
POST   /contratos
PATCH  /contratos/{id}/confirmar
PATCH  /contratos/{id}/concluir
PATCH  /contratos/{id}/cancelar
```

## Requisitos para rodar o projeto

Antes de executar o projeto, instale as ferramentas abaixo.

### Node.js e npm

O front-end precisa do Node.js e do npm.

Verifique a instalação:

```bash
node -v
npm -v
```

### Angular CLI

Instale o Angular CLI:

```bash
npm install -g @angular/cli
```

Verifique:

```bash
ng version
```

### CMake

Verifique se o CMake está instalado:

```bash
cmake --version
```

### Ninja

O projeto está configurado para ser compilado com Ninja.

Verifique se o Ninja está instalado:

```bash
ninja --version
```

### Compilador C++

Verifique se o compilador C++ está disponível no terminal:

```bash
g++ --version
```

Caso esteja usando Windows com MSYS2, abra o terminal correto do ambiente em que o `g++`, o `cmake` e o `ninja` estejam funcionando.

## Como rodar o back-end

Abra um terminal na pasta do back-end:

```bash
cd back
```

Apague a pasta `build`, caso ela já exista de uma compilação anterior:

### PowerShell

```powershell
Remove-Item -Recurse -Force build
```

### Linux, macOS ou MSYS2

```bash
rm -rf build
```

Gere os arquivos de compilação com Ninja:

```bash
cmake -S . -B build -G "Ninja"
```

Compile o projeto:

```bash
cmake --build build
```

Execute o servidor:

### PowerShell

```powershell
.\build\felsk_api.exe
```

### Linux, macOS ou MSYS2

```bash
./build/felsk_api
```

Após iniciar, o back-end ficará disponível em:

```txt
http://localhost:8080
```

Também é possível rodar o back-end pelo arquivo:

```txt
back/rodar_backend.bat
```

## Como rodar o front-end

Abra outro terminal na pasta do front-end:

```bash
cd front
```

Instale as dependências:

```bash
npm install
```

Inicie o servidor Angular:

```bash
npm start
```

Acesse no navegador:

```txt
http://localhost:4200
```

## Ordem recomendada de execução

1. Abrir um terminal na pasta `back`.
2. Compilar e iniciar o back-end.
3. Abrir outro terminal na pasta `front`.
4. Rodar `npm install`, caso as dependências ainda não estejam instaladas.
5. Rodar `npm start`.
6. Acessar `http://localhost:4200` no navegador.

## Arquivos que não devem ser enviados para o GitHub

Não é necessário enviar arquivos gerados automaticamente. O `.gitignore` deve manter fora do repositório:

```txt
front/node_modules/
front/dist/
front/.angular/
back/build/
*.exe
```

## Observações

- O back-end precisa estar rodando antes de usar as telas que consultam ou cadastram dados.
- O arquivo `back/data/banco_dados.txt` é o armazenamento local do sistema.
- A pasta `back/build` pode ser apagada e gerada novamente sempre que necessário.
- O front-end se comunica com a API em `http://localhost:8080`.
