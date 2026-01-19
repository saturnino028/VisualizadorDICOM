/**
 * @file DicomManager.h
 * @brief Definição da classe gerenciadora de operações DICOM.
 * @details Este arquivo define a interface para manipulação de arquivos DICOM,
 * servindo como uma camada de abstração (wrapper) simplificada para a biblioteca DCMTK.
 * @author Marco Antonio (Saturnino.eng)
 * @version 1.0
 */

#ifndef DICOMMANAGER_H
#define DICOMMANAGER_H

#include <QString>
#include <QImage>

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
};

#endif // DICOMMANAGER_H