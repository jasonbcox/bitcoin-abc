// Copyright (c) 2011-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_SPLASHSCREEN_H
#define BITCOIN_QT_SPLASHSCREEN_H

#include <QSplashScreen>
#include <functional>

#include <memory>

class NetworkStyle;

namespace interface {
class Handler;
class Node;
class Wallet;
};

/** Class for the splashscreen with information of the running client.
 *
 * @note this is intentionally not a QSplashScreen. Bitcoin Core initialization
 * can take a long time, and in that case a progress window that cannot be moved
 * around and minimized has turned out to be frustrating to the user.
 */
class SplashScreen : public QWidget {
    Q_OBJECT

public:
    explicit SplashScreen(interface::Node& node, Qt::WindowFlags f, const NetworkStyle *networkStyle);
    ~SplashScreen();

protected:
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

public Q_SLOTS:
    /** Slot to call finish() method as it's not defined as slot */
    void slotFinish(QWidget *mainWin);

    /** Show message and progress */
    void showMessage(const QString &message, int alignment,
                     const QColor &color);

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;

private:
    /** Connect core signals to splash screen */
    void subscribeToCoreSignals();
    /** Disconnect core signals to splash screen */
    void unsubscribeFromCoreSignals();
    /** Connect wallet signals to splash screen */
    void ConnectWallet(std::unique_ptr<interface::Wallet> wallet);

    QPixmap pixmap;
    QString curMessage;
    QColor curColor;
    int curAlignment;

    interface::Node& m_node;
    std::unique_ptr<interface::Handler> m_handler_init_message;
    std::unique_ptr<interface::Handler> m_handler_show_progress;
    std::unique_ptr<interface::Handler> m_handler_load_wallet;
    std::list<std::unique_ptr<interface::Wallet>> m_connected_wallets;
    std::list<std::unique_ptr<interface::Handler>> m_connected_wallet_handlers;
};

#endif // BITCOIN_QT_SPLASHSCREEN_H
