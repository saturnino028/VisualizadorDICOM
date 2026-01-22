# ==============================================================================
# Visualizador DICOM - Script de Configuração do CMake
# ==============================================================================
# Este arquivo configura o processo de compilação do projeto, gerenciando:
# 1. Padrões da linguagem C++ (C++ 17)
# 2. Integração com o framework Qt5 (Widgets)
# 3. Integração com a biblioteca DCMTK (incluindo codecs de compressão)
# 4. Linkagem de bibliotecas do sistema Windows necessárias
# ==============================================================================

cmake_minimum_required(VERSION 3.10)

# Definição do Projeto e Versão
project(VisualizadorDICOM VERSION 1.2.0 LANGUAGES CXX)

# ------------------------------------------------------------------------------
# Configurações do Compilador (C++17)
# ------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------------------------
# Configurações do Qt (Automação)
# ------------------------------------------------------------------------------
# Estas variáveis instruem o CMake a chamar automaticamente as ferramentas do Qt:
# - MOC (Meta-Object Compiler): Para sinais e slots
# - UIC (User Interface Compiler): Para arquivos .ui (se houver)
# - RCC (Resource Compiler): Para arquivos de recursos .qrc
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ------------------------------------------------------------------------------
# Caminhos de Busca (Environment)
# ------------------------------------------------------------------------------
# bibliotecas MSYS2 / UCRT64
set(CMAKE_PREFIX_PATH "C:/msys64/ucrt64")

# ------------------------------------------------------------------------------
# Dependências Externas
# ------------------------------------------------------------------------------
# DCMTK: Carregamos os componentes essenciais e os de compressão.
# - dcmjpeg: Suporte a imagens JPEG padrão
# - dcmjpls: Suporte a JPEG-LS
# - dcmdata, dcmimgle, dcmimage: Manipulação de tags e pixels
# - ofstd: Biblioteca padrão de suporte da DCMTK
find_package(DCMTK REQUIRED COMPONENTS dcmjpeg dcmjpls dcmdata dcmimgle dcmimage ofstd)

# Qt5: Apenas o módulo de Widgets é necessário para a GUI
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# ------------------------------------------------------------------------------
# Definição do Executável
# ------------------------------------------------------------------------------
add_executable(${PROJECT_NAME}
    main.cpp
    DicomManager.cpp
    DicomManager.h
)

# ------------------------------------------------------------------------------
# Configuração de Includes e Linkagem
# ------------------------------------------------------------------------------
# Adiciona os diretórios de cabeçalho da DCMTK ao projeto
target_include_directories(${PROJECT_NAME} PRIVATE ${DCMTK_INCLUDE_DIRS})

# Linka as bibliotecas ao executável
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Widgets        # Framework Gráfico
    ${DCMTK_LIBRARIES}  # Todas as libs encontradas da DCMTK
    
    # Bibliotecas de sistema do Windows exigidas pela DCMTK (winsock, netapi, etc)
    ws2_32 
    netapi32 
    wsock32 
    iphlpapi
)