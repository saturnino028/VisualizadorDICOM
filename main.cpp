/**
 * @file main.cpp
 * @brief Ponto de entrada da aplicação Visualizador DICOM.
 * @details Este arquivo configura a interface gráfica principal, gerencia a navegação entre telas
 * (Boas-vindas e Visualizador) e inicializa os codecs de descompressão DICOM necessários.
 * @author Marco Antonio (Saturnino.eng)
 * @version 1.0.1
 */

#include <QLabel>           // Para exibir textos (ex: "Saturnino.eng View")
#include <QScreen>          // Para obter informações sobre o monitor
#include <QStyle>           // Para acessar estilos e geometria do sistema
#include <QShortcut>        // Criar atalhos de teclado
#include <QMainWindow>      // A janela principal da aplicação
#include <QPushButton>      // Botões clicáveis
#include <QVBoxLayout>      // Organiza widgets verticalmente (um em cima do outro)
#include <QHBoxLayout>      // Organiza widgets horizontalmente (um ao lado do outro)
#include <QFileDialog>      // A janela de "Abrir Arquivo" do sistema operacional
#include <QMessageBox>      // Janelas de alerta (Pop-ups de erro)
#include <QApplication>     // Gerencia o fluxo da aplicação e configurações globais
#include <QGraphicsView>    // O "visualizador" da imagem (permite zoom/pan)
#include <QStackedWidget>   // Gerencia as "páginas" (Tela Inicial vs Visualizador)
#include <QGuiApplication>  // Classe base para aplicações com GUI
#include <QGraphicsScene>   // A "cena" onde a imagem é desenhada dentro do View
#include <QGraphicsPixmapItem> // O item que contém a imagem

// Includes dos Codecs de descompressão da DCMTK
#include "dcmtk/dcmjpeg/djdecode.h"  // Permite abrir DICOM comprimido em JPEG
#include "dcmtk/dcmjpls/djdecode.h"  // Permite abrir JPEG-LS (muito usado em Mamografia)
#include "dcmtk/dcmdata/dcrledrg.h"  // Permite abrir compressão RLE

// Gerenciador personalizado
#include "DicomManager.h" // Classe que faz a ponte entre o arquivo .dcm e o Qt

/**
 * @brief Função principal da aplicação.
 * * Responsável por:
 * 1. Registrar os decodificadores globais da DCMTK.
 * 2. Construir a interface gráfica (Janela, Stack de telas, Botões).
 * 3. Conectar a lógica de interação (Sinais e Slots).
 * 4. Executar o loop de eventos do Qt.
 * * @param argc Número de argumentos de linha de comando.
 * @param argv Vetor de argumentos de linha de comando.
 * @return int Código de saída da aplicação (0 para sucesso).
 */
int main(int argc, char *argv[]) {
    // --- 1. Registro de Codecs (Essencial para abrir imagens comprimidas - Padrão) ---
    DJDecoderRegistration::registerCodecs();     // Suporte a JPEG
    DJLSDecoderRegistration::registerCodecs();   // Suporte a JPEG-LS
    DcmRLEDecoderRegistration::registerCodecs(); // Suporte a RLE
    
    QApplication app(argc, argv); //Prepara o ambiente gráfico

    // Configuração da Janela Principal
    QMainWindow window;
    window.setWindowTitle("Saturnino.eng View - Versão 1.0.1");
    
    // 1. Pega a tela onde o mouse/app está (Tela Primária)
    QScreen *screen = QGuiApplication::primaryScreen();
    
    // 2. Pega a geometria disponível (Tamanho total - Barra de Tarefas)
    QRect screenGeometry = screen->availableGeometry();
    
    // 3. Define um tamanho padrão
    int width = screenGeometry.width();
    int height = screenGeometry.height();
    
    // 4. Centraliza esse retângulo na tela
    int x = (screenGeometry.width() - width) / 2;
    int y = (screenGeometry.height() - height) / 2;
    
    // Aplica o tamanho calculado para quando a janela não estiver maximizada
    window.setGeometry(x, y, width, height);

    // Centraliza a janela no monitor do usuário ao abrir
    /*
    window.setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            window.size(),
            QGuiApplication::primaryScreen()->availableGeometry()
        )
    );
    */
   
    window.setWindowFlags(Qt::Window);

    // QStackedWidget permite alternar entre a "Tela Inicial" e o "Visualizador"
    QStackedWidget *stackedWidget = new QStackedWidget;
    window.setCentralWidget(stackedWidget);

    // =========================================================
    // TELA 1: Boas-vindas (Welcome Screen)
    // =========================================================
    QWidget *welcomePage = new QWidget;
    QVBoxLayout *welcomeLayout = new QVBoxLayout(welcomePage);
    
    // Centralizar verticalmente o conteúdo
    welcomeLayout->addStretch(); 

    // Logo (Estilizado via CSS)
    QLabel *logoLabel = new QLabel("Saturnino.eng View");
    logoLabel->setStyleSheet("font-size: 48px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    welcomeLayout->addWidget(logoLabel, 0, Qt::AlignCenter);

    // Subtítulo
    QLabel *subTitle = new QLabel("Visualizador DICOM de Alta Performance");
    subTitle->setStyleSheet("font-size: 18px; color: #7f8c8d;");
    welcomeLayout->addWidget(subTitle, 0, Qt::AlignCenter);

    welcomeLayout->addSpacing(40);

    // Botão Principal "Abrir Arquivo"
    QPushButton *btnBigOpen = new QPushButton("📂 Abrir Arquivo DICOM");
    btnBigOpen->setCursor(Qt::PointingHandCursor);
    btnBigOpen->setFixedSize(300, 60); 
    btnBigOpen->setStyleSheet(
        "QPushButton { "
        "  background-color: #3498db; color: white; border-radius: 8px; font-size: 18px; font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #2980b9; }"
    );
    
    welcomeLayout->addWidget(btnBigOpen, 0, Qt::AlignCenter);
    welcomeLayout->addStretch(); 

    // =========================================================
    // TELA 2: Visualizador (Viewer Screen)
    // =========================================================
    QWidget *viewerPage = new QWidget;
    QVBoxLayout *viewerLayout = new QVBoxLayout(viewerPage);

    // Configuração do Graphics View Framework (Para Zoom e Pan eficientes)
    QGraphicsScene *scene = new QGraphicsScene();
    QGraphicsView *view = new QGraphicsView(scene);
    view->setDragMode(QGraphicsView::ScrollHandDrag); // Permite arrastar com o mouse
    view->setBackgroundBrush(Qt::black);              // Fundo padrão radiológico
    view->setStyleSheet("border: none;"); 
    viewerLayout->addWidget(view);

    // Barra de Ferramentas Inferior
    QHBoxLayout *toolsLayout = new QHBoxLayout();
    
    QPushButton *btnOpenAnother = new QPushButton("Abrir Outro");
    QPushButton *btnZoomIn = new QPushButton("Zoom (+)");
    QPushButton *btnZoomOut = new QPushButton("Zoom (-)");
    QPushButton *btnFit = new QPushButton("Resetar");
    QPushButton *btnBack = new QPushButton("Voltar ao Início");

    // Estilização dos botões da barra
    QString toolBtnStyle = "padding: 8px 15px; font-weight: bold; border-radius: 4px; background-color: #ecf0f1;";
    btnOpenAnother->setStyleSheet(toolBtnStyle);
    btnZoomIn->setStyleSheet(toolBtnStyle);
    btnZoomOut->setStyleSheet(toolBtnStyle);
    btnFit->setStyleSheet(toolBtnStyle);
    btnBack->setStyleSheet("padding: 8px 15px; color: white; background-color: #e74c3c; border-radius: 4px;");

    toolsLayout->addWidget(btnOpenAnother);
    toolsLayout->addStretch(); // Espaçador
    toolsLayout->addWidget(btnZoomIn);
    toolsLayout->addWidget(btnZoomOut);
    toolsLayout->addWidget(btnFit);
    toolsLayout->addWidget(btnBack);
    
    viewerLayout->addLayout(toolsLayout);

    // Adiciona as páginas ao Stack
    stackedWidget->addWidget(welcomePage); // Índice 0
    stackedWidget->addWidget(viewerPage);  // Índice 1
    stackedWidget->setCurrentIndex(0);     // Inicia na tela de boas-vindas

    // =========================================================
    // LÓGICA E CONEXÕES (Signals & Slots)
    // =========================================================

    // Lambda para abrir arquivo (utilizada pelos dois botões de abrir)
    auto openDicomAction = [&window, stackedWidget, scene, view]() {
        
        //Tenta abrir na pasta "ArquivosDesafio"
        QString initialDir = QCoreApplication::applicationDirPath() + "/../ArquivosDesafio";

        // Se a pasta "ArquivosDesafio" não existir, tenta apenas a raiz do projeto
        if (!QDir(initialDir).exists()) {
             initialDir = QCoreApplication::applicationDirPath() + "/..";
        }

        QString path = QFileDialog::getOpenFileName(
            &window, 
            "Abrir DICOM", 
            initialDir, 
            "DICOM Files (*.dcm);;Todos os Arquivos (*)"
        );

        if (!path.isEmpty()) {
            // Chama o DicomManager para processar o arquivo
            QImage img = DicomManager::loadDicomImage(path);
            
            if (!img.isNull()) {
                scene->clear(); 
                
                // 1. Define a "Mesa Infinita" para melhor visualização
                scene->setSceneRect(-10000, -10000, 20000, 20000);

                // 2. Adiciona a imagem
                QGraphicsPixmapItem *item = scene->addPixmap(QPixmap::fromImage(img));
                
                // 3. Define o ponto de origem da imagem
                item->setOffset(-img.width() / 2.0, -img.height() / 2.0);

                // 4. Ajusta o Zoom para a imagem caber na tela
                view->fitInView(item, Qt::KeepAspectRatio);
                
                // 5. Recua um pouco (95%) para dar uma margem estética
                view->scale(0.95, 0.95); 

                // Força a câmera a olhar exatamente para o centro (0,0)
                // Como definimos o offset da imagem acima, (0,0) é o centro da imagem.
                view->centerOn(0, 0);

                stackedWidget->setCurrentIndex(1); 
            } else {
                QMessageBox::critical(&window, "Erro", "Falha ao processar imagem.\nVerifique se o arquivo é um DICOM válido.");
            }
        }
    };

    // Conexões dos Botões
    QObject::connect(btnBigOpen, &QPushButton::clicked, openDicomAction);
    QObject::connect(btnOpenAnother, &QPushButton::clicked, openDicomAction);
    
    // Controles de Zoom
    QObject::connect(btnZoomIn, &QPushButton::clicked, [view]() { view->scale(1.25, 1.25); });
    QObject::connect(btnZoomOut, &QPushButton::clicked, [view]() { view->scale(0.8, 0.8); });
    
    // Resetar visualização (Fit to Screen)
    QObject::connect(btnFit, &QPushButton::clicked, [scene, view]() { 
        view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio); 
    });
    
    // Voltar para a Home
    QObject::connect(btnBack, &QPushButton::clicked, [stackedWidget, scene]() {
        scene->clear(); // Libera memória da imagem atual
        stackedWidget->setCurrentIndex(0);
    });

    // --- ATALHOS DE TECLADO (SHORTCUTS) ---
    // 1. Atalho para Abrir Arquivo (Ctrl + O)
    QShortcut *shortcutOpen = new QShortcut(QKeySequence("Ctrl+O"), &window);
    QObject::connect(shortcutOpen, &QShortcut::activated, openDicomAction);

    // 2. Atalho para Zoom In (Ctrl + +)
    QShortcut *shortcutZoomIn = new QShortcut(QKeySequence::ZoomIn, &window);
    QObject::connect(shortcutZoomIn, &QShortcut::activated, [view]() {
        view->scale(1.20, 1.20);
    });

    // 3. Atalho para Zoom Out (Ctrl + -)
    QShortcut *shortcutZoomOut = new QShortcut(QKeySequence::ZoomOut, &window);
    QObject::connect(shortcutZoomOut, &QShortcut::activated, [view]() {
        view->scale(0.8, 0.8);
    });
    
    // 4. Atalho para Resetar Zoom (Ctrl + 0)
    QShortcut *shortcutReset = new QShortcut(QKeySequence("Ctrl+0"), &window);
    QObject::connect(shortcutReset, &QShortcut::activated, [scene, view]() {
        view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
        view->centerOn(0,0); // Centraliza
    });

    window.show();

    // Executa a aplicação
    int result = app.exec();
    
    // Limpeza dos Codecs ao encerrar
    DJDecoderRegistration::cleanup();
    DJLSDecoderRegistration::cleanup();
    DcmRLEDecoderRegistration::cleanup();
    
    return result;
}