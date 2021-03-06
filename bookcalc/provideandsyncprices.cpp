#include <QFile>
#include <QDebug>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QFileInfo>

#include "provideandsyncprices.h"

ProvideAndSyncPrices::ProvideAndSyncPrices():mXmlWriter(), mXmlReader()
{
    mXmlWriter.setAutoFormatting(true);
}

void ProvideAndSyncPrices::SavePrices()
{

    QFile* file = new QFile(QCoreApplication::applicationDirPath() + "/prices.xml");

    if ( !file->open(QIODevice::WriteOnly) )
    {
        QMessageBox::warning(NULL, QObject::tr("Price saving:"),
                             QObject::tr("Cannot write file %1:\n%2.")
                             .arg(QString("prices.xml"))
                             .arg(file->errorString()));
        return;
    }

    mXmlWriter.setDevice(file);
    mXmlWriter.writeStartDocument();
    mXmlWriter.writeDTD("<!DOCTYPE BookPrice>");
    mXmlWriter.writeStartElement("ElementPrices");
    mXmlWriter.writeAttribute("version", "1.0");

    mXmlWriter.writeStartElement("ФорматКниги");
    for (auto& it : mBookPrices.format)
        mXmlWriter.writeTextElement(it.first, QString::number(it.second));
//        mXmlWriter.writeTextElement("А4", "1.5");
//        mXmlWriter.writeTextElement("А5", "1.0");
//        mXmlWriter.writeTextElement("А6", "0.8");
//        mXmlWriter.writeTextElement("Кишенькова", "1.2");
    mXmlWriter.writeEndElement();

    mXmlWriter.writeStartElement("Обкладинка");
    for (auto& it : mBookPrices.cover)
        mXmlWriter.writeTextElement(it.first, QString::number(it.second));
//        mXmlWriter.writeTextElement("Тверда", "5.0");
//        mXmlWriter.writeTextElement("М.яка", "3.0");
    mXmlWriter.writeEndElement();

    mXmlWriter.writeStartElement("Скріплення");
        mXmlWriter.writeTextElement("Скоба", "3.0");
        mXmlWriter.writeTextElement("Термобіндер", "3.0");
        mXmlWriter.writeTextElement("Твердий_Переплет", "3.0");
        mXmlWriter.writeTextElement("Пружинка", "3.0");
    mXmlWriter.writeEndElement();

//    mXmlWriter.writeEndElement();
    mXmlWriter.writeEndDocument();
    file->close();
}

void ProvideAndSyncPrices::ReceivePrices()
{
    QFile* file = new QFile(QCoreApplication::applicationDirPath() + "/prices.xml");

    if ( !file->open(QIODevice::ReadOnly) )
    {
        QMessageBox::warning(NULL, QObject::tr("Price saving:"),
                             QObject::tr("Cannot write file %1:\n%2.")
                             .arg(QString("prices.xml"))
                             .arg(file->errorString()));
        return;
    }
    mXmlReader.setDevice(file);
//    qDebug() << "is DTD" << mXmlReader.isDTD();

    mXmlReader.readNextStartElement();
    if (mXmlReader.name() == "ElementPrices")
    {
        while (mXmlReader.readNextStartElement())
        {
            if (mXmlReader.name() == "ФорматКниги")
            {
                while (mXmlReader.readNextStartElement())
                {
                    std::pair<QString, double> p(mXmlReader.name().toString(),
                                                 mXmlReader.readElementText().toDouble());
                    mBookPrices.format.insert(p);
                }
            } else if (mXmlReader.name() == "Обкладинка")
            {
                while (mXmlReader.readNextStartElement())
                {
                    std::pair<QString, double> p(mXmlReader.name().toString(),
                                                 mXmlReader.readElementText().toDouble());
                    mBookPrices.cover.insert(p);
                }
            }

            else if (mXmlReader.name() == "Скріплення")
            {
                while (mXmlReader.readNextStartElement())
                {
                    std::pair<QString, double> p(mXmlReader.name().toString(),
                                                 mXmlReader.readElementText().toDouble());
                    mBookPrices.fastening.insert(p);
                }
            }
        }
    }
    qDebug() << mXmlReader.errorString();
}

std::map<QString, double> &ProvideAndSyncPrices::GetBookPrices(ProvideAndSyncPrices::EBookPrices iKey)
{
    switch (iKey) {
    case eFormat:
        return mBookPrices.format; break;
    case eCover:
        return mBookPrices.cover; break;
    case eFastering:
        return mBookPrices.fastening; break;
    default:
        break;
    }
}
