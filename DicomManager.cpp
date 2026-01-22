/**
 * @file DicomManager.cpp
 * @brief Implementação da classe responsável pelo carregamento e processamento de imagens DICOM.
 * @details Este arquivo contém a lógica de integração entre a biblioteca DCMTK (backend)
 * e o framework Qt (frontend), gerenciando a conversão de dados brutos para visualização.
 * @author Marco Antonio (Saturnino.eng)
 * @version 1.2.0
 */

#include "DicomManager.h"

#define HAVE_CONFIG_H // Definição necessária para compilação da DCMTK em ambiente Windows

#include <dcmtk/config/osconfig.h> 
#include <dcmtk/dcmimgle/dcmimage.h>

// Includes para leitura de Tags (Metadados)
#include <dcmtk/dcmdata/dctk.h>      // Acesso genérico
#include <dcmtk/dcmdata/dcfilefo.h>  // Manipulação de arquivo DICOM
#include <dcmtk/dcmdata/dcdeftag.h>  // Definições das Tags (DCM_PatientName...)

#include <QDebug>

/**
 * @brief Carrega um arquivo DICOM do disco e o converte para QImage.
 * * @details O método realiza as seguintes etapas críticas:
 * 1. Carrega o arquivo usando a classe DicomImage da DCMTK.
 * 2. Verifica a integridade e o status do arquivo.
 * 3. Aplica o "Window Level/Width" (Contraste/Brilho) lendo as tags do arquivo ou calculando automaticamente.
 * 4. Renderiza os dados para 8 bits (Escala de Cinza).
 * 5. Converte o buffer de memória para o formato QImage do Qt.
 * * @param path O caminho absoluto ou relativo para o arquivo .dcm.
 * @return QImage Uma imagem válida em formato Grayscale8 se o carregamento for bem-sucedido; 
 * caso contrário, retorna uma QImage nula (QImage::isNull() == true).
 */
QImage DicomManager::loadDicomImage(const QString &path) {
    // Tenta carregar o arquivo DICOM. 
    DicomImage *image = new DicomImage(path.toStdString().c_str());

    // Verifica se a imagem foi carregada e se o status é 'Normal'
    if (image == nullptr || image->getStatus() != EIS_Normal) {
        if (image != nullptr) delete image;
        return QImage(); // Retorna imagem vazia indicando erro
    }

    // --- Processamento de Contraste (Windowing) ---
    // Tenta aplicar o primeiro preset de janela (Window Center/Width) salvo no arquivo .
    // Isso garante que a visualização inicial seja a recomendada pelo radiologista/equipamento.
    if (!image->setWindow(0)) {
        // Se não houver presets, calcula o Min/Max dos pixels para garantir visibilidade.
        image->setMinMaxWindow();
    }

    const int width = image->getWidth();
    const int height = image->getHeight();
    
    // Extrai os dados de pixel renderizados em 8 bits (0-255).
    uint8_t *pixelData = (uint8_t *)(image->getOutputData(8));

    if (pixelData != nullptr) {
        // Cria a QImage a partir do buffer bruto.
        // width' como 4º parâmetro ajuda a definir o "bytesPerLine" ajudando a evitar 
        // falhas de segmentação (crash) se a largura da imagem não for múltiplo de 4 bytes.
        QImage result(pixelData, width, height, width, QImage::Format_Grayscale8);
        
        // Evita problemas de gerenciamento de memória fazendo uma cópia dos dados.
        QImage finalImage = result.copy();
        
        delete image; // Libera a memória alocada pelo DCMTK
        return finalImage;
    }

    // Limpeza em caso de falha na extração dos pixels
    delete image;
    return QImage();
}

/**
 * @brief Extrai metadados textuais do arquivo DICOM (Overlay).
 * @details Utiliza DcmFileFormat para ler apenas o Header do arquivo.
 * Realiza conversão de encoding (Latin1) para suportar acentuação e formata a data.
 * @param path O caminho absoluto ou relativo para o arquivo .dcm.
 * @return DicomMetadata Estrutura preenchida. Se falhar, retorna isValid = false.
 */
DicomMetadata DicomManager::extractMetadata(const QString &path) {
    DicomMetadata data;
    DcmFileFormat fileformat;

    // Carrega apenas a estrutura de dados (Header), sem carregar pixels (rápido)
    if (fileformat.loadFile(path.toStdString().c_str()).good()) {
        DcmDataset *dataset = fileformat.getDataset();
        OFString tempVal;

        // Lambda auxiliar para buscar tags com segurança e converter encoding
        auto getTag = [&](const DcmTagKey &tag) -> QString {
            if (dataset->findAndGetOFString(tag, tempVal).good()) {
                return QString::fromLatin1(tempVal.c_str());
            }
            return "N/A";
        };

        // Extração das Tags
        data.patientName = getTag(DCM_PatientName);
        data.patientID   = getTag(DCM_PatientID);
        data.studyDate   = getTag(DCM_StudyDate);
        data.modality    = getTag(DCM_Modality);
        data.institution = getTag(DCM_InstitutionName);

        // Formatação de Data (DICOM usa YYYYMMDD -> DD/MM/YYYY)
        if (data.studyDate.length() == 8) {
            data.studyDate = data.studyDate.mid(6, 2) + "/" + 
                             data.studyDate.mid(4, 2) + "/" + 
                             data.studyDate.left(4);
        }

        // Extração de Dimensões (Números inteiros)
        long cols = 0, rows = 0;
        dataset->findAndGetLongInt(DCM_Columns, cols);
        dataset->findAndGetLongInt(DCM_Rows, rows);
        data.dimensions = QString("%1 x %2 px").arg(cols).arg(rows);

        data.isValid = true;
    } else {
        qDebug() << "Erro ao ler metadados do arquivo:" << path;
        data.isValid = false;
    }

    return data;
}