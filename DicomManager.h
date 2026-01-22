/**
 * @file DicomManager.h
 * @brief Definição da classe gerenciadora de operações DICOM.
 * @details Este arquivo define a interface para manipulação de arquivos DICOM,
 * servindo como uma camada de abstração (wrapper) simplificada para a biblioteca DCMTK.
 * @author Marco Antonio (Saturnino.eng)
 * @version 1.2.0
 */

#ifndef DICOMMANAGER_H
#define DICOMMANAGER_H

#include <QString>
#include <QImage>

/**
 * @struct DicomMetadata
 * @brief Estrutura de dados para armazenar metadados essenciais extraídos do arquivo DICOM.
 * @details Esta estrutura agrupa informações do paciente e do exame para exibição em overlay.
 */
struct DicomMetadata {
    QString patientName;  ///< Nome do Paciente (Tag 0010,0010)
    QString patientID;    ///< ID do Paciente (Tag 0010,0020)
    QString studyDate;    ///< Data do Estudo (Tag 0008,0020)
    QString modality;     ///< Modalidade (CT, MR, CR, etc) (Tag 0008,0060)
    QString institution;  ///< Nome da Instituição (Tag 0008,0080)
    QString dimensions;   ///< Dimensões da imagem (Colunas x Linhas)
    bool isValid = false; ///< Flag para indicar se a extração foi bem-sucedida
};

/**
 * @class DicomManager
 * @brief Classe utilitária estática para carregar e processar imagens médicas.
 *
 * A DicomManager isola a complexidade da biblioteca DCMTK (Dicom Toolkit).
 * Ela é responsável por abrir o arquivo, aplicar transformações de janela (contraste/brilho),
 * e converter os dados brutos de pixel para QImage.
 */
class DicomManager {
public:
    /**
     * @brief Carrega uma imagem DICOM do sistema de arquivos.
     *
     * Este método realiza a leitura dos metadados e dos pixels, aplica
     * as correções de fotometria e gera uma imagem pronta para renderização.
     *
     * @param path O caminho completo (absoluto ou relativo) para o arquivo .dcm.
     * @return QImage A imagem processada em escala de cinza (8-bit). 
     * Retorna uma imagem nula (QImage::isNull()) se o arquivo for inválido ou corrompido.
     */
    static QImage loadDicomImage(const QString &path);

    /**
     * @brief Extrai metadados textuais do arquivo DICOM (Overlay).
     * * Lê o cabeçalho do arquivo DICOM sem processar os pixels da imagem,
     * tornando a operação extremamente rápida.
     * * @param path O caminho completo para o arquivo .dcm.
     * @return DicomMetadata Estrutura contendo as tags principais (Nome, Data, etc).
     */
    static DicomMetadata extractMetadata(const QString &path);
};

#endif // DICOMMANAGER_H