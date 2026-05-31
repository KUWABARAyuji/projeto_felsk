# Testes realizados

O backend C++ foi testado no ambiente Linux com CMake e g++.

## Compilação

Comando executado:

```bash
cd back
cmake -S . -B build
cmake --build build -j2
```

Resultado: compilou com sucesso e gerou o executável `felsk_api`.

## Execução da API

Comando executado usando banco temporário para não alterar o arquivo original:

```bash
BANCO_TXT_CAMINHO=/tmp/felsk_test_banco.txt APP_PORT=8090 ./build/felsk_api
```

## Endpoints testados

```bash
curl http://localhost:8090/health
curl http://localhost:8090/clientes
curl http://localhost:8090/trabalhadores
curl -X POST http://localhost:8090/clientes -H "Content-Type: application/json" -d '{"nome":"Cliente Teste","telefone":"47999999999","cpf":"000","endereco":"Rua X"}'
curl -X POST http://localhost:8090/contratos -H "Content-Type: application/json" -d '{"idCliente":1,"idTrabalhador":1,"habilidade":"Eletricista","data":"2026-06-10","horas":2,"descricao":"Trocar tomada"}'
curl -X PATCH http://localhost:8090/contratos/1/confirmar
curl -X PATCH http://localhost:8090/contratos/1/concluir -H "Content-Type: application/json" -d '{"avaliacao":5}'
curl http://localhost:8090/contratos
```

Resultado: todos responderam com JSON compatível com o front Angular.

## Observação sobre o front

O front foi mantido com `apiUrl: 'http://localhost:8080'` e as rotas chamadas pelo serviço Angular foram implementadas no backend.

O build do Angular não foi executado neste ambiente porque a pasta enviada não tinha `node_modules` e o ambiente não tinha acesso à internet para executar `npm install`. No seu computador, rode:

```bash
cd front
npm install
npm start
```
