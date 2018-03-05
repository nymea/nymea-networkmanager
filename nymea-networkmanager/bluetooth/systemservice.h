#ifndef SYSTEMSERVICE_H
#define SYSTEMSERVICE_H

#include <QObject>
#include <QLowEnergyService>
#include <QLowEnergyServiceData>

class SystemService : public QObject
{
    Q_OBJECT
public:
    enum SystemServiceCommand {
        SystemServiceCommandInvalid = -1,
        SystemServiceCommandPushAuthentication = 0x00
    };
    Q_ENUM(SystemServiceCommand)

    enum SystemServiceResponse {
        SystemServiceResponseSuccess                = 0x00,
        SystemServiceResponseUnknownError           = 0x01,
        SystemServiceResponseInvalidCommand         = 0x02,
        SystemServiceResponseInvalidValue           = 0x03,
        SystemServiceResponsePushServiceUnavailable = 0x04,
    };
    Q_ENUM(SystemServiceResponse)

    explicit SystemService(QLowEnergyService *service, QObject *parent = nullptr);

    QLowEnergyService *service();

    static QLowEnergyServiceData serviceData();

private:
    QLowEnergyService *m_service = nullptr;

    bool m_readingInputData = false;
    QByteArray m_inputDataStream;

    void streamData(const QVariantMap &responseMap);
    QVariantMap createResponse(const SystemServiceCommand &command, const SystemServiceResponse &responseCode = SystemServiceResponseSuccess);


private slots:
    // Service
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void descriptorRead(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
    void descriptorWritten(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
    void serviceError(const QLowEnergyService::ServiceError &error);

    // Commands
    void processCommand(const QVariantMap &request);

    // Push button authentication
    void commandPressPushButton();

    void onUpdateRunningChanged(const bool &running);
};

#endif // SYSTEMSERVICE_H
