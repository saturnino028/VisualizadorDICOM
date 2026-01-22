# 🩻 Saturnino.eng View

### Visualizador DICOM Desktop

O **Saturnino.eng View** é uma aplicação desktop desenvolvida em **C++ (C++17)** para visualização de imagens médicas no formato **DICOM** (`.dcm`).

O projeto foi concebido com foco em **alta performance**, **robustez no processamento de imagens médicas** e **boa experiência do usuário**, utilizando o framework **Qt 5** para a interface gráfica e a biblioteca **DCMTK** para o manuseio confiável de arquivos DICOM.

---

## 🚀 Funcionalidades

### ✔️ Funcionalidades Essenciais

* **Carregamento de arquivos DICOM:** Suporte a arquivos DICOM padrão via sistema de arquivos.
* **Visualização em escala de cinza:** Renderização correta de imagens médicas em **8 bits**.
* **Tratamento de erros:** Identificação e notificação de arquivos inválidos, corrompidos ou incompatíveis.

### ⭐ Diferenciais Implementados

* **Window Level / Window Width Automático:**
  Leitura inteligente das tags DICOM de janelamento para ajuste automático de contraste e brilho, garantindo visualização correta para diferentes modalidades (ex.: **mamografia**).
* **Suporte a imagens comprimidas:**
  Integração completa com codecs DICOM, incluindo:

  * JPEG
  * **JPEG-LS**
  * RLE
* **Zoom e Pan interativos:**
  Navegação fluida utilizando o **Qt Graphics View Framework**, permitindo zoom in/out e movimentação da imagem com o mouse.
* **Interface moderna e intuitiva:**
  Tela de boas-vindas (*Landing Page*) e layout limpo, priorizando usabilidade.

---

## 🛠️ Tecnologias Utilizadas

* **Linguagem:** C++17
* **Interface Gráfica:** Qt 5 (Widgets Module)
* **Processamento DICOM:** DCMTK (DICOM Toolkit)
* **Sistema de Build:** CMake (≥ 3.10) + Ninja
* **Ambiente de Desenvolvimento:** MSYS2 (UCRT64) + Visual Studio Code

---

## ⚙️ Compilação e Execução (Windows)

O projeto foi configurado para o ambiente **MSYS2 UCRT64**, garantindo compatibilidade com bibliotecas atualizadas e um fluxo de build moderno.

### 1️⃣ Pré-requisitos

Instale o [MSYS2](https://www.msys2.org/) e, no terminal **UCRT64**, execute:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-cmake
pacman -S mingw-w64-ucrt-x86_64-ninja
pacman -S mingw-w64-ucrt-x86_64-qt5
pacman -S mingw-w64-ucrt-x86_64-dcmtk
```

---

### 2️⃣ Compilação

No terminal **MSYS2 UCRT64** (ou no VS Code configurado para esse ambiente), navegue até a raiz do projeto e execute:

```bash
# 1. Configura o projeto e gera os arquivos de build
cmake -S . -B build -G "Ninja"

# 2. Compila o executável
cmake --build build
```

---

### 3️⃣ Execução

Após a compilação, o executável estará disponível no diretório `build`:

```bash
./build/VisualizadorDICOM.exe
```

---

## 📌 Observações

* O projeto foi desenvolvido seguindo boas práticas de **engenharia de software**, **organização de código** e **arquitetura modular**.
* Compatível com arquivos DICOM monocromáticos (grayscale).

---

## 🗺️ Roadmap – Melhorias Futuras

Este roadmap apresenta as funcionalidades planejadas para as próximas versões do **Saturnino.eng View**, com foco na evolução da **experiência do usuário**, **visualização de metadados DICOM** e **robustez da aplicação**.

---

### 🔹 Versão 1.1 – Interface do Usuário (UI/UX)
* [x] Redesign da interface principal
* [x] Atalhos de teclado
* [x] Feedback visual aprimorado

### 🔹 Versão 1.2 – Visualização de Metadados DICOM
* [x] Exibição de informações essenciais


---

## 🎯 Objetivo do Roadmap

O objetivo das próximas versões é transformar o **Saturnino.eng View** em um visualizador DICOM:

* Mais **intuitivo**
* Mais **informativo**
* Mais **próximo de ferramentas profissionais**, mantendo simplicidade e desempenho.

---
