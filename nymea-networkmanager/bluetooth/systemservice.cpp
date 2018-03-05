#include "systemservice.h"
#include "bluetoothuuids.h"
#include "loggingcategory.h"
#include "loopd.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QLowEnergyDescriptorData>
#include <QLowEnergyCharacteristicData>

SystemService::SystemService(QLowEnergyService *service, QObject *parent) :
    QObject(parent),
    m_service(service)
{
    connect(m_service, &QLowEnergyService::characteristicChanged, this, &SystemService::characteristicChanged);
    connect(m_service, &QLowEnergyService::characteristicRead, this, &SystemService::characteristicChanged);
    connect(m_service, &QLowEnergyService::characteristicWritten, this, &SystemService::characteristicWritten);
    connect(m_service, &QLowEnergyService::descriptorWritten, this, &SystemService::descriptorWritten);
    connect(m_service, SIGNAL(error(QLowEnergyService::ServiceError)), this, SLOT(serviceError(QLowEnergyService::ServiceError)));

    connect(Loopd::instance()->snapdControl(), &SnapdControl::updateRunningChanged, this, &SystemService::onUpdateRunningChanged);
}

QLowEnergyService *SystemService::service()
{
    return m_service;
}

QLowEnergyServiceData SystemService::serviceData()
{
    QLowEnergyServiceData serviceData;
    serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceData.setUuid(systemServiceUuid);

    QLowEnergyDescriptorData clientConfigDescriptorData(QBluetoothUuid::ClientCharacteristicConfiguration, QByteArray(2, 0));

    // System commander characterisitc e081fed1-f757-4449-b9c9-bfa83133f7fc
    QLowEnergyCharacteristicData systemCommanderCharacteristicData;
    systemCommanderCharacteristicData.setUuid(systemCommanderCharacteristicUuid);
    systemCommanderCharacteristicData.setProperties(QLowEnergyCharacteristic::Write);
    systemCommanderCharacteristicData.setValueLength(0, 20);
    serviceData.addCharacteristic(systemCommanderCharacteristicData);

    // Response characterisitc e081fed2-f757-4449-b9c9-bfa83133f7fc
    QLowEnergyCharacteristicData systemResponseCharacteristicData;
    systemResponseCharacteristicData.setUuid(systemResponseCharacteristicUuid);
    systemResponseCharacteristicData.setProperties(QLowEnergyCharacteristic::Notify);
    systemResponseCharacteristicData.addDescriptor(clientConfigDescriptorData);
    systemResponseCharacteristicData.setValueLength(0, 20);
    serviceData.addCharacteristic(systemResponseCharacteristicData);
    return serviceData;

    // System update indicator characterisitc e081fed3-f757-4449-b9c9-bfa83133f7fc
    QLowEnergyCharacteristicData systemUpdateCharacteristicData;
    systemUpdateCharacteristicData.setUuid(systemUpdateCharacteristicUuid);
    systemUpdateCharacteristicData.setProperties(QLowEnergyCharacteristic::Read | QLowEnergyCharacteristic::Notify);
    systemUpdateCharacteristicData.addDescriptor(clientConfigDescriptorData);
    systemUpdateCharacteristicData.setValueLength(1, 1);
    systemUpdateCharacteristicData.setValue(QByteArray::number((int)Loopd::instance()->snapdControl()->updateRunning()));
    serviceData.addCharacteristic(systemUpdateCharacteristicData);
    return serviceData;
}

void SystemService::streamData(const QVariantMap &responseMap)
{
    QLowEnergyCharacteristic characteristic = m_service->characteristic(systemResponseCharacteristicUuid);
    if (!characteristic.isValid()) {
        qCWarning(dcBluetoothServer()) << "SystemService: System response characteristic not valid";
        return;
    }

    QByteArray data = QJsonDocument::fromVariant(responseMap).toJson(QJsonDocument::Compact) + '\n';
    qCDebug(dcBluetoothServer()) << "SystemService: Start streaming response data:" << data.count() << "bytes";

    int sentDataLength = 0;
    QByteArray remainingData = data;
    while (!remainingData.isEmpty()) {
        QByteArray package = remainingData.left(20);
        sentDataLength += package.count();
        m_service->writeCharacteristic(characteristic, package);
        remainingData = remainingData.remove(0, package.count());
    }

    qCDebug(dcBluetoothServer()) << "SystemService: Finished streaming response data";
}

QVariantMap SystemService::createResponse(const SystemService::SystemServiceCommand &command, const SystemService::SystemServiceResponse &responseCode)
{
    QVariantMap response;
    response.insert("c", (int)command);
    response.insert("r", (int)responseCode);
    return response;
}

void SystemService::characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    // Command
    if (characteristic.uuid() == systemCommanderCharacteristicUuid) {
        // Check if currently reading
        if (m_readingInputData) {
            m_inputDataStream.append(value);
        } else {
            m_inputDataStream.clear();
            m_readingInputData = true;
            m_inputDataStream.append(value);
        }

        // If command finished
        if (value.endsWith('\n')) {
            QJsonParseError error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(m_inputDataStream, &error);
            if (error.error != QJsonParseError::NoError) {
                qCWarning(dcBluetoothServer()) << "SystemService: Got invalid json object" << m_inputDataStream;
                m_inputDataStream.clear();
                m_readingInputData = false;
                return;
            }

            qCDebug(dcBluetoothServer()) << "SystemService: Got command stream" << jsonDocument.toJson();

            processCommand(jsonDocument.toVariant().toMap());

            m_inputDataStream.clear();
            m_readingInputData = false;
        }

        // Limit possible data stream to prevent overflow
        if (m_inputDataStream.length() >= 20 * 1024) {
            m_inputDataStream.clear();
            m_readingInputData = false;
            return;
        }
    }
}

void SystemService::characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qCDebug(dcBluetoothServer()) << "SystemService: Characteristic read" << characteristic.uuid().toString() << value;
}

void SystemService::characteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qCDebug(dcBluetoothServer()) << "SystemService: Characteristic written" << characteristic.uuid().toString() << value;
}

void SystemService::descriptorRead(const QLowEnergyDescriptor &descriptor, const QByteArray &value)
{
    qCDebug(dcBluetoothServer()) << "SystemService: Descriptor read" << descriptor.uuid().toString() << value;
}

void SystemService::descriptorWritten(const QLowEnergyDescriptor &descriptor, const QByteArray &value)
{
    qCDebug(dcBluetoothServer()) << "SystemService: Descriptor written" << descriptor.uuid().toString() << value;
}

void SystemService::serviceError(const QLowEnergyService::ServiceError &error)
{
    QString errorString;
    switch (error) {
    case QLowEnergyService::NoError:
        errorString = "No error";
        break;
    case QLowEnergyService::OperationError:
        errorString = "Operation error";
        break;
    case QLowEnergyService::CharacteristicReadError:
        errorString = "Characteristic read error";
        break;
    case QLowEnergyService::CharacteristicWriteError:
        errorString = "Characteristic write error";
        break;
    case QLowEnergyService::DescriptorReadError:
        errorString = "Descriptor read error";
        break;
    case QLowEnergyService::DescriptorWriteError:
        errorString = "Descriptor write error";
        break;
    case QLowEnergyService::UnknownError:
        errorString = "Unknown error";
        break;
    default:
        errorString = "Unhandled error";
        break;
    }

    qCWarning(dcBluetoothServer()) << "SystemService: Error:" << errorString;
}

void SystemService::processCommand(const QVariantMap &request)
{
    if (!request.contains("c")) {
        qCWarning(dcBluetoothServer()) << "SystemService: Invalid request. Command value missing.";
        streamData(createResponse(SystemServiceCommandPushAuthentication, SystemServiceResponseInvalidValue));
        return;
    }

    bool commandIntOk;
    int command = request.value("c").toInt(&commandIntOk);
    if (!commandIntOk) {
        qCWarning(dcBluetoothServer()) << "SystemService: Invalid request. Could not convert method to interger.";
        streamData(createResponse(SystemServiceCommandPushAuthentication, SystemServiceResponseInvalidValue));
        return;
    }

    // Process method
    switch (command) {
    case SystemServiceCommandPushAuthentication:
        commandPressPushButton();
        break;
    default:
        qCWarning(dcBluetoothServer()) << "SystemService: Invalid request. Unknown command" << command;
        streamData(createResponse(SystemServiceCommandPushAuthentication, SystemServiceResponseInvalidCommand));
        break;
    }
}

void SystemService::commandPressPushButton()
{
    if (!m_service) {
        qCWarning(dcBluetoothServer()) << "SystemService: Could not press push button. Service not valid.";
        return;
    }

    QLowEnergyCharacteristic characteristic = m_service->characteristic(systemResponseCharacteristicUuid);
    if (!characteristic.isValid()) {
        qCWarning(dcBluetoothServer()) << "SystemService: System response characteristic not valid";
        return;
    }

    if (!Loopd::instance()->nymeaService()->available()) {
        streamData(createResponse(SystemServiceCommandPushAuthentication, SystemServiceResponsePushServiceUnavailable));
        return;
    }

    Loopd::instance()->nymeaService()->pushButtonPressed();

    streamData(createResponse(SystemServiceCommandPushAuthentication, SystemServiceResponseSuccess));
}

void SystemService::onUpdateRunningChanged(const bool &running)
{
    if (!m_service) {
        qCWarning(dcBluetoothServer()) << "SystemService: Could not set system update running characteristic. Service not valid.";
        return;
    }

    QLowEnergyCharacteristic characteristic = m_service->characteristic(systemUpdateCharacteristicUuid);
    if (!characteristic.isValid()) {
        qCWarning(dcBluetoothServer()) << "SystemService: Could not set system update running characteristic. Characteristic not valid";
        return;
    }

    qCDebug(dcBluetoothServer()) << "SystemService: Notify system update running changed:" << (running ? "running" : "finished");
    m_service->writeCharacteristic(characteristic, running ? QByteArray::fromHex("01") : QByteArray::fromHex("00"));
    return;
}
