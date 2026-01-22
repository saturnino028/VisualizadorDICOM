/**
 * @file main.cpp
 * @brief Ponto de entrada da aplicação Visualizador DICOM.
 * @details Este arquivo configura a interface gráfica principal, gerencia a navegação entre telas
 * (Boas-vindas e Visualizador) e inicializa os codecs de descompressão DICOM necessários.
 * @author Marco Antonio (Saturnino.eng)
 * @version 1.2.0
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
#include <QGraphicsScene>   // A "cena" onde a imagem é desenhada dentro do View
#include <QProgressDialog> // Para a janela de "Aguarde"
#include <QGuiApplication>  // Classe base para aplicações com GUI
#include <QGraphicsPixmapItem> // O item que contém a imagem

// Includes dos Codecs de descompressão da DCMTK
#include "dcmtk/dcmjpeg/djdecode.h"  // Permite abrir DICOM comprimido em JPEG
#include "dcmtk/dcmjpls/djdecode.h"  // Permite abrir JPEG-LS (muito usado em Mamografia)
#include "dcmtk/dcmdata/dcrledrg.h"  // Permite abrir compressão RLE

// Gerenciador personalizado
#include "DicomManager.h" // Classes que fazem a ponte entre o arquivo .dcm e o Qt

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
    
    // CORREÇÃO 1: Removemos as margens externas da página para o preto encostar na borda da janela
    QVBoxLayout *viewerLayout = new QVBoxLayout(viewerPage);
    viewerLayout->setContentsMargins(0, 0, 0, 0); 
    viewerLayout->setSpacing(0);

    // Container para sobrepor Labels na GraphicsView
    QWidget *viewContainer = new QWidget;
    
    QGridLayout *overlayLayout = new QGridLayout(viewContainer);

    int m = 10; // Margem interna para o texto não colar na borda da tela
    overlayLayout->setContentsMargins(m, m, m, m); // Margem interna para o texto não colar na borda da tela

    // 1. O Visualizador (Fica na camada de fundo)
    QGraphicsScene *scene = new QGraphicsScene();
    QGraphicsView *view = new QGraphicsView(scene);
    view->setDragMode(QGraphicsView::ScrollHandDrag); 
    view->setBackgroundBrush(Qt::black);              
    
    // CORREÇÃO 2: Remove a borda e as barras de rolagem (Scrollbars)
    // Isso impede que a barra branca apareça e corte o texto
    view->setFrameShape(QFrame::NoFrame); 
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // O view ocupa tudo (Row 0, Col 0, RowSpan 3, ColSpan 2)
    overlayLayout->addWidget(view, 0, 0, 3, 2); 

    // 2. Configuração dos Labels de Overlay
    QString overlayStyle = "QLabel { color: #f1c40f; font-weight: bold; font-size: 14px; background: transparent; }";

    // Canto Superior Esquerdo (Info Paciente)
    QLabel *lblTopLeft = new QLabel("");
    lblTopLeft->setStyleSheet(overlayStyle);
    lblTopLeft->setAttribute(Qt::WA_TransparentForMouseEvents); 
    overlayLayout->addWidget(lblTopLeft, 0, 0, Qt::AlignTop | Qt::AlignLeft);

    // Canto Superior Direito (Info Instituição/Data)
    QLabel *lblTopRight = new QLabel("");
    lblTopRight->setStyleSheet(overlayStyle);
    lblTopRight->setAlignment(Qt::AlignRight); // Garante alinhamento à direita
    lblTopRight->setAttribute(Qt::WA_TransparentForMouseEvents);
    overlayLayout->addWidget(lblTopRight, 0, 1, Qt::AlignTop | Qt::AlignRight);

    // Canto Inferior Direito (Info Técnica)
    QLabel *lblBottomRight = new QLabel("");
    lblBottomRight->setStyleSheet(overlayStyle);
    lblBottomRight->setAlignment(Qt::AlignRight); // Garante alinhamento à direita
    lblBottomRight->setAttribute(Qt::WA_TransparentForMouseEvents);
    overlayLayout->addWidget(lblBottomRight, 2, 1, Qt::AlignBottom | Qt::AlignRight);
    
    // Adiciona o container montado à página
    viewerLayout->addWidget(viewContainer);

    // Barra de Ferramentas Inferior
    QHBoxLayout *toolsLayout = new QHBoxLayout();
    
    QPushButton *btnOpenAnother = new QPushButton("Abrir Outro");
    QPushButton *btnZoomIn = new QPushButton("Zoom (+)");
    QPushButton *btnZoomOut = new QPushButton("Zoom (-)");
    QPushButton *btnFit = new QPushButton("Resetar");
    QPushButton *btnBack = new QPushButton("Voltar ao Início");
    QPushButton *btnToggleInfo = new QPushButton("Mostrar Metadados (On)");

    btnToggleInfo->setCheckable(true); // Transforma em botão de ligar/desligar
    btnToggleInfo->setChecked(true);   // Começa ligado (texto visível)

    // Estilização dos botões da barra
    QString toolBtnStyle = "padding: 8px 15px; font-weight: bold; border-radius: 4px; background-color: #ecf0f1;";
    btnOpenAnother->setStyleSheet(toolBtnStyle);
    btnZoomIn->setStyleSheet(toolBtnStyle);
    btnZoomOut->setStyleSheet(toolBtnStyle);
    btnFit->setStyleSheet(toolBtnStyle);
    btnToggleInfo->setStyleSheet(toolBtnStyle);
    btnBack->setStyleSheet("padding: 8px 15px; color: white; background-color: #e74c3c; border-radius: 4px;");
    

    toolsLayout->addWidget(btnOpenAnother);
    toolsLayout->addStretch(); // Espaçador
    toolsLayout->addWidget(btnToggleInfo);
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

    // Lambda para abrir arquivo
    auto openDicomAction = [&window, stackedWidget, scene, view, lblTopLeft, lblTopRight, lblBottomRight]() {
        
        QString initialDir = QCoreApplication::applicationDirPath() + "/../ArquivosDesafio";
        if (!QDir(initialDir).exists()) {
             initialDir = QCoreApplication::applicationDirPath() + "/..";
        }

        QString path = QFileDialog::getOpenFileName(
            &window, "Abrir DICOM", initialDir, "Arquivos DICOM (*.dcm);;Todos os Arquivos (*)"
        );

        if (!path.isEmpty()) {
            // [1] Feedback Visual
            QApplication::setOverrideCursor(Qt::WaitCursor);
            QProgressDialog progress("Processando imagem e metadados...", nullptr, 0, 0, &window);
            progress.setWindowTitle("Aguarde");
            progress.setWindowModality(Qt::WindowModal);
            progress.setMinimumDuration(0);
            progress.show();
            QCoreApplication::processEvents(); 

            // [2] Trabalho Pesado (Carrega Imagem + Metadados)
            QImage img = DicomManager::loadDicomImage(path);
            DicomMetadata meta = DicomManager::extractMetadata(path); // <--- NOVO

            // [3] Remove Feedback
            progress.close();
            QApplication::restoreOverrideCursor();

            if (!img.isNull()) {
                scene->clear(); 
                scene->setSceneRect(-10000, -10000, 20000, 20000); 

                QGraphicsPixmapItem *item = scene->addPixmap(QPixmap::fromImage(img));
                item->setOffset(-img.width() / 2.0, -img.height() / 2.0);

                view->fitInView(item, Qt::KeepAspectRatio);
                view->scale(0.95, 0.95); 
                view->centerOn(0, 0);

                // --- ATUALIZAÇÃO DO OVERLAY ---
                if (meta.isValid) {
                    lblTopLeft->setText(QString("NOME: %1\nID: %2\nMOD: %3")
                                        .arg(meta.patientName)
                                        .arg(meta.patientID)
                                        .arg(meta.modality));

                    lblTopRight->setText(QString("%1\nDATA: %2")
                                         .arg(meta.institution)
                                         .arg(meta.studyDate));

                    lblBottomRight->setText(QString("DIM: %1").arg(meta.dimensions));
                } else {
                    lblTopLeft->setText("METADADOS INDISPONÍVEIS");
                    lblTopRight->clear();
                    lblBottomRight->clear();
                }

                stackedWidget->setCurrentIndex(1); 
            } else {
                QMessageBox::critical(&window, "Erro", "Falha ao processar imagem DICOM.");
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
    
    // Mostrar/esconder texto
    QObject::connect(btnToggleInfo, &QPushButton::toggled, 
        [btnToggleInfo, lblTopLeft, lblTopRight, lblBottomRight](bool checked) {
            
            // Define a visibilidade baseada no estado do botão
            lblTopLeft->setVisible(checked);
            lblTopRight->setVisible(checked);
            lblBottomRight->setVisible(checked);

            // Muda o texto do botão para dar feedback ao usuário
            if (checked) {
                btnToggleInfo->setText("Mostrar Metadados (On)");
            } else {
                btnToggleInfo->setText("Mostrar Metadados (Off)");
            }
        }
    );

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
        view->centerOn(0,0); // Centraliza a imagem após o reset
    });

    // 5. Atalho para Info (Ctrl + I)
    QShortcut *shortcutInfo = new QShortcut(QKeySequence("Ctrl+I"), &window);
    QObject::connect(shortcutInfo, &QShortcut::activated, [btnToggleInfo]() {
        btnToggleInfo->toggle(); 
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