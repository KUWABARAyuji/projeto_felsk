cmake_minimum_required(VERSION 3.20)

project(projeto_felsk_backend_txt_ddd LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(felsk_api
    src/main.cpp

    src/domain/model/Data.cpp
    src/domain/model/StatusContrato.cpp
    src/domain/model/Habilidade.cpp
    src/domain/model/Pessoa.cpp
    src/domain/model/Cliente.cpp
    src/domain/model/Trabalhador.cpp
    src/domain/model/Contrato.cpp

    src/application/dto/ClienteDtos.cpp
    src/application/dto/HabilidadeDtos.cpp
    src/application/dto/TrabalhadorDtos.cpp
    src/application/dto/ContratoDtos.cpp
    src/application/dto/DtoMapper.cpp

    src/application/service/ClienteService.cpp
    src/application/service/TrabalhadorService.cpp
    src/application/service/ContratoService.cpp

    src/infrastructure/storage/TextDatabase.cpp
    src/infrastructure/repository/TxtClienteRepository.cpp
    src/infrastructure/repository/TxtTrabalhadorRepository.cpp
    src/infrastructure/repository/TxtContratoRepository.cpp

    src/presentation/http/JsonUtil.cpp
    src/presentation/http/HttpServer.cpp
)

target_include_directories(felsk_api PRIVATE ${PROJECT_SOURCE_DIR}/include)

if(WIN32)
    target_link_libraries(felsk_api PRIVATE ws2_32)
endif()
