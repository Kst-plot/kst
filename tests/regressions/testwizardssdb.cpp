// SPDX-License-Identifier: GPL-2.0-or-later
#include <QtTest>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QtEndian>

#include "application.h"
#include "datasourceselector.h"
#include "datasourcepluginmanager.h"
#include "datawizard.h"
#include "document.h"
#include "objectstore.h"
#include "matrixdialog.h"
#include "scalardialog.h"
#include "stringdialog.h"

// Minimal postcard fixture: Subscription::Consumer = [1],
// Response::FieldList(empty) = [3,0], Response::LatestTime(None) = [4,0].
// Real client transport and plugin loading are exercised, not C API stubs.
class EmptyDatabase : public QTcpServer {
public:
  explicit EmptyDatabase(QObject *parent = nullptr) : QTcpServer(parent) {
    connect(this, &QTcpServer::newConnection, this, [this] {
      while (hasPendingConnections()) {
        auto *socket = nextPendingConnection();
        ++connections;
        auto bytes = std::make_shared<QByteArray>();
        auto stage = std::make_shared<int>(0);
        connect(socket, &QTcpSocket::readyRead, socket, [socket, bytes, stage] {
          bytes->append(socket->readAll());
          while (bytes->size() >= 4) {
            const auto length = qFromLittleEndian<quint32>(bytes->constData());
            if (bytes->size() < 4 + length) break;
            bytes->remove(0, 4 + length);
            const QByteArray response = (*stage)++ == 0 ? QByteArray::fromHex("01")
                : *stage == 2 ? QByteArray::fromHex("0300") : QByteArray::fromHex("0400");
            QByteArray packet(4, '\0');
            qToLittleEndian<quint32>(quint32(response.size()), packet.data());
            socket->write(packet + response);
          }
        });
        connect(socket, &QTcpSocket::disconnected, socket, &QObject::deleteLater);
      }
    });
  }
  int connections = 0;
};

class TestWizardSsdb : public QObject {
  Q_OBJECT
  Kst::ObjectStore *store() { return kstApp->mainWindow()->document()->objectStore(); }
  Kst::DataWizardPageDataSource *page(QWizard &wizard, const QString &endpoint) {
    auto *value = new Kst::DataWizardPageDataSource(store(), &wizard, endpoint);
    wizard.addPage(value);
    wizard.addPage(new QWizardPage);
    wizard.show();
    return value;
  }
  void verifyIncomplete(QWizard &wizard, Kst::DataWizardPageDataSource *value) {
    QVERIFY(!value->isComplete());
    QVERIFY(!value->dataSource());
    QVERIFY(!wizard.button(QWizard::NextButton)->isEnabled());
    QVERIFY(!value->findChild<QPushButton *>("_configureSource")->isEnabled());
    QVERIFY(!value->findChild<QComboBox *>("_updateBox")->isEnabled());
    QVERIFY(value->findChild<QLabel *>("_fileType")->text().isEmpty());
  }
private Q_SLOTS:
  void cleanup() { store()->clear(); }

  void refusedEndpointBehavesLikeMissingFile() {
    QTcpServer reserve;
    QVERIFY(reserve.listen(QHostAddress::LocalHost));
    const auto endpoint = QString("ssdb://127.0.0.1:%1").arg(reserve.serverPort());
    reserve.close();
    QWizard wizard;
    auto *value = page(wizard, endpoint);
    QSignalSpy changed(value, &Kst::DataWizardPageDataSource::completeChanged);
    QTRY_VERIFY(changed.count() > 0);
    verifyIncomplete(wizard, value);
    QVERIFY(store()->dataSourceList().isEmpty());
    auto *selector = value->findChild<Kst::DataSourceSelector *>("_url");
    QVERIFY(selector);
    changed.clear();
    selector->setFile("/kst-regression/nonexistent-file");
    verifyIncomplete(wizard, value);
    // Edit invalidates immediately; local validation never enables the page.
    QTRY_VERIFY(changed.count() > 0);
    verifyIncomplete(wizard, value);
  }

  void silentServerTimesOutWithoutFreezingWizard() {
    QTcpServer silent;
    QVERIFY(silent.listen(QHostAddress::LocalHost));
    QWizard wizard;
    auto *value = page(wizard, QString("ssdb://127.0.0.1:%1").arg(silent.serverPort()));
    QSignalSpy changed(value, &Kst::DataWizardPageDataSource::completeChanged);
    int ticks = 0;
    QTimer heartbeat;
    heartbeat.setInterval(10);
    connect(&heartbeat, &QTimer::timeout, this, [&] { ++ticks; });
    heartbeat.start();
    QElapsedTimer elapsed;
    elapsed.start();
    QTRY_VERIFY_WITH_TIMEOUT(silent.hasPendingConnections(), 2000);
    QTRY_VERIFY_WITH_TIMEOUT(changed.count() > 0, 6500);
    QVERIFY(elapsed.elapsed() >= 4900);
    QVERIFY(elapsed.elapsed() < 6500);
    QVERIFY(ticks > 50);
    verifyIncomplete(wizard, value);
    QVERIFY(store()->dataSourceList().isEmpty());
  }

  void emptyDatabaseAdoptsOneConnectionAndEnablesNext() {
    EmptyDatabase database;
    QVERIFY(database.listen(QHostAddress::LocalHost));
    QWizard wizard;
    auto *value = page(wizard, QString("ssdb://127.0.0.1:%1").arg(database.serverPort()));
    QTRY_VERIFY_WITH_TIMEOUT(value->isComplete(), 2500);
    QVERIFY(wizard.button(QWizard::NextButton)->isEnabled());
    QVERIFY(value->dataSource()->isValid());
    QVERIFY(value->dataSource()->isEmpty());
    QCOMPARE(value->dataSourceFieldList(), QStringList({"INDEX", "TIME"}));
    QCOMPARE(database.connections, 1);
    QCOMPARE(store()->dataSourceList().count(), 1);
  }

  void editAwayAndCancelCannotPublishStaleSources() {
    QTcpServer silent;
    QVERIFY(silent.listen(QHostAddress::LocalHost));
    QWizard wizard;
    auto *value = page(wizard, QString("ssdb://127.0.0.1:%1").arg(silent.serverPort()));
    QTRY_VERIFY_WITH_TIMEOUT(silent.hasPendingConnections(), 2000);
    auto *oldSocket = silent.nextPendingConnection();
    QVERIFY(oldSocket);
    QTemporaryFile local;
    QVERIFY(local.open());
    local.write("0 1\n1 2\n2 3\n");
    local.flush();
    value->findChild<Kst::DataSourceSelector *>("_url")->setFile(local.fileName());
    // Local validation must not wait for the five-second SSDB worker.
    QTRY_VERIFY_WITH_TIMEOUT(value->isComplete(), 2000);
    QCOMPARE(value->dataSource()->fileName(), local.fileName());
    oldSocket->abort();
    QCOMPARE(store()->dataSourceList().count(), 1);
    wizard.reject();
  }

  void closingWizardDiscardsLateSuccessfulReplies() {
    QTcpServer delayed;
    QVERIFY(delayed.listen(QHostAddress::LocalHost));
    QWizard wizard;
    auto *value = page(wizard, QString("ssdb://127.0.0.1:%1").arg(delayed.serverPort()));
    QTRY_VERIFY_WITH_TIMEOUT(delayed.hasPendingConnections(), 2000);
    auto *socket = delayed.nextPendingConnection();
    QVERIFY(socket);
    QSignalSpy disconnected(socket, &QTcpSocket::disconnected);
    wizard.reject();
    // A healthy reply arrives only after Cancel. It cannot create store/slave
    // objects or complete the hidden page, even though the wizard still exists.
    for (const auto &response : {QByteArray::fromHex("01"), QByteArray::fromHex("0300"), QByteArray::fromHex("0400")}) {
      QByteArray packet(4, '\0');
      qToLittleEndian<quint32>(quint32(response.size()), packet.data());
      socket->write(packet + response);
    }
    QTRY_VERIFY_WITH_TIMEOUT(disconnected.count() > 0, 2500);
    QVERIFY(store()->dataSourceList().isEmpty());
    verifyIncomplete(wizard, value);
  }

  void legacyDialogsSafelyRejectFailedLoads() {
    QTcpServer reserve;
    QVERIFY(reserve.listen(QHostAddress::LocalHost));
    const auto endpoint = QString("ssdb://127.0.0.1:%1").arg(reserve.serverPort());
    reserve.close();
    const auto validate = [&](auto *tab) {
      QVERIFY(tab);
      QSignalSpy changed(tab, SIGNAL(sourceChanged()));
      QVERIFY(QMetaObject::invokeMethod(tab, "fileNameChanged", Qt::DirectConnection, Q_ARG(QString, endpoint)));
      QTRY_VERIFY_WITH_TIMEOUT(changed.count() >= 2, 2000);
      QVERIFY(!tab->dataSource());
    };
    Kst::MatrixDialog matrix(nullptr, kstApp->mainWindow());
    validate(matrix.findChild<Kst::MatrixTab *>());
    Kst::ScalarDialog scalar(nullptr, kstApp->mainWindow());
    validate(scalar.findChild<Kst::ScalarTab *>());
    Kst::StringDialog string(nullptr, kstApp->mainWindow());
    validate(string.findChild<Kst::StringTab *>());
    QVERIFY(store()->dataSourceList().isEmpty());
  }
};

int main(int argc, char **argv) {
  QTemporaryDir settings;
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settings.path());
  Kst::Application application(argc, argv);
  Q_INIT_RESOURCE(icons);
  application.initMainWindow();
  TestWizardSsdb test;
  return QTest::qExec(&test, argc, argv);
}
#include "testwizardssdb.moc"