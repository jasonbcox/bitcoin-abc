// Copyright (c) 2011-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "transactiondesc.h"

#include "bitcoinunits.h"
#include "guiutil.h"
#include "paymentserver.h"
#include "transactionrecord.h"

#include "chain.h"
#include "consensus/consensus.h"
#include "dstencode.h"
#include "interface/node.h"
#include "script/script.h"
#include "timedata.h"
#include "util.h"
#include "validation.h"
#include "wallet/db.h"
#include "wallet/finaltx.h"
#include "wallet/wallet.h"

#include <cstdint>
#include <string>

<<<<<<< HEAD
QString TransactionDesc::FormatTxStatus(const CWalletTx &wtx) {
    AssertLockHeld(cs_main);
    if (!CheckFinalTx(*wtx.tx)) {
        if (wtx.tx->nLockTime < LOCKTIME_THRESHOLD) {
            return tr("Open for %n more block(s)", "",
                      wtx.tx->nLockTime - chainActive.Height());
        } else {
            return tr("Open until %1")
                .arg(GUIUtil::dateTimeStr(wtx.tx->nLockTime));
        }
    } else {
        int nDepth = wtx.GetDepthInMainChain();
        if (nDepth < 0) {
            return tr("conflicted with a transaction with %1 confirmations")
                .arg(-nDepth);
        } else if (GetAdjustedTime() - wtx.nTimeReceived > 2 * 60 &&
                   wtx.GetRequestCount() == 0) {
            return tr("%1/offline").arg(nDepth);
        } else if (nDepth == 0) {
            return tr("0/unconfirmed, %1")
                       .arg((wtx.InMempool() ? tr("in memory pool")
                                             : tr("not in memory pool"))) +
                   (wtx.isAbandoned() ? ", " + tr("abandoned") : "");
        } else if (nDepth < 6) {
=======
QString TransactionDesc::FormatTxStatus(const interface::WalletTx& wtx, const interface::WalletTxStatus& status, bool inMempool, int numBlocks, int64_t adjustedTime)
{
    if (!status.is_final)
    {
        if (wtx.tx->nLockTime < LOCKTIME_THRESHOLD)
            return tr("Open for %n more block(s)", "", wtx.tx->nLockTime - numBlocks);
        else
            return tr("Open until %1").arg(GUIUtil::dateTimeStr(wtx.tx->nLockTime));
    }
    else
    {
        int nDepth = status.depth_in_main_chain;
        if (nDepth < 0)
            return tr("conflicted with a transaction with %1 confirmations").arg(-nDepth);
        else if (adjustedTime - status.time_received > 2 * 60 && status.request_count == 0)
            return tr("%1/offline").arg(nDepth);
        else if (nDepth == 0)
            return tr("0/unconfirmed, %1").arg((inMempool ? tr("in memory pool") : tr("not in memory pool"))) + (status.is_abandoned ? ", "+tr("abandoned") : "");
        else if (nDepth < 6)
>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files
            return tr("%1/unconfirmed").arg(nDepth);
        } else {
            return tr("%1 confirmations").arg(nDepth);
        }
    }
}

<<<<<<< HEAD
QString TransactionDesc::toHTML(CWallet *wallet, CWalletTx &wtx,
                                TransactionRecord *rec, int unit) {
=======
QString TransactionDesc::toHTML(interface::Node& node, interface::Wallet& wallet, TransactionRecord *rec, int unit)
{
    int numBlocks;
    int64_t adjustedTime;
    interface::WalletTxStatus status;
    interface::WalletOrderForm orderForm;
    bool inMempool;
    interface::WalletTx wtx = wallet.getWalletTxDetails(rec->hash, status, orderForm, inMempool, numBlocks, adjustedTime);

>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files
    QString strHTML;

    strHTML.reserve(4000);
    strHTML += "<html><font face='verdana, arial, helvetica, sans-serif'>";

<<<<<<< HEAD
    int64_t nTime = wtx.GetTxTime();
    Amount nCredit = wtx.GetCredit(ISMINE_ALL);
    Amount nDebit = wtx.GetDebit(ISMINE_ALL);
    Amount nNet = nCredit - nDebit;

    strHTML += "<b>" + tr("Status") + ":</b> " + FormatTxStatus(wtx);
    int nRequests = wtx.GetRequestCount();
    if (nRequests != -1) {
        if (nRequests == 0) {
=======
    int64_t nTime = wtx.time;
    CAmount nCredit = wtx.credit;
    CAmount nDebit = wtx.debit;
    CAmount nNet = nCredit - nDebit;

    strHTML += "<b>" + tr("Status") + ":</b> " + FormatTxStatus(wtx, status, inMempool, numBlocks, adjustedTime);
    int nRequests = status.request_count;
    if (nRequests != -1)
    {
        if (nRequests == 0)
>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files
            strHTML += tr(", has not been successfully broadcast yet");
        } else if (nRequests > 0) {
            strHTML += tr(", broadcast through %n node(s)", "", nRequests);
        }
    }
    strHTML += "<br>";

    strHTML += "<b>" + tr("Date") + ":</b> " +
               (nTime ? GUIUtil::dateTimeStr(nTime) : "") + "<br>";

    //
    // From
    //
<<<<<<< HEAD
    if (wtx.IsCoinBase()) {
        strHTML += "<b>" + tr("Source") + ":</b> " + tr("Generated") + "<br>";
    } else if (wtx.mapValue.count("from") && !wtx.mapValue["from"].empty()) {
        // Online transaction
        strHTML += "<b>" + tr("From") + ":</b> " +
                   GUIUtil::HtmlEscape(wtx.mapValue["from"]) + "<br>";
    } else {
=======
    if (wtx.is_coinbase)
    {
        strHTML += "<b>" + tr("Source") + ":</b> " + tr("Generated") + "<br>";
    }
    else if (wtx.value_map.count("from") && !wtx.value_map["from"].empty())
    {
        // Online transaction
        strHTML += "<b>" + tr("From") + ":</b> " + GUIUtil::HtmlEscape(wtx.value_map["from"]) + "<br>";
    }
    else
    {
>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files
        // Offline transaction
        if (nNet > Amount::zero()) {
            // Credit
            CTxDestination address =
                DecodeDestination(rec->address, wallet->chainParams);
            if (IsValidDestination(address)) {
<<<<<<< HEAD
                if (wallet->mapAddressBook.count(address)) {
                    strHTML +=
                        "<b>" + tr("From") + ":</b> " + tr("unknown") + "<br>";
                    strHTML += "<b>" + tr("To") + ":</b> ";
                    strHTML += GUIUtil::HtmlEscape(rec->address);
                    QString addressOwned =
                        (::IsMine(*wallet, address) == ISMINE_SPENDABLE)
                            ? tr("own address")
                            : tr("watch-only");
                    if (!wallet->mapAddressBook[address].name.empty()) {
                        strHTML += " (" + addressOwned + ", " + tr("label") +
                                   ": " +
                                   GUIUtil::HtmlEscape(
                                       wallet->mapAddressBook[address].name) +
                                   ")";
                    } else {
=======
                std::string name;
                isminetype ismine;
                if (wallet.getAddress(address, &name, &ismine))
                {
                    strHTML += "<b>" + tr("From") + ":</b> " + tr("unknown") + "<br>";
                    strHTML += "<b>" + tr("To") + ":</b> ";
                    strHTML += GUIUtil::HtmlEscape(rec->address);
                    QString addressOwned = ismine == ISMINE_SPENDABLE ? tr("own address") : tr("watch-only");
                    if (!name.empty())
                        strHTML += " (" + addressOwned + ", " + tr("label") + ": " + GUIUtil::HtmlEscape(name) + ")";
                    else
>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files
                        strHTML += " (" + addressOwned + ")";
                    }
                    strHTML += "<br>";
                }
            }
        }
    }

    //
    // To
    //
<<<<<<< HEAD
    if (wtx.mapValue.count("to") && !wtx.mapValue["to"].empty()) {
=======
    if (wtx.value_map.count("to") && !wtx.value_map["to"].empty())
    {
>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files
        // Online transaction
        std::string strAddress = wtx.value_map["to"];
        strHTML += "<b>" + tr("To") + ":</b> ";
<<<<<<< HEAD
        CTxDestination dest =
            DecodeDestination(strAddress, wallet->chainParams);
        if (wallet->mapAddressBook.count(dest) &&
            !wallet->mapAddressBook[dest].name.empty()) {
            strHTML +=
                GUIUtil::HtmlEscape(wallet->mapAddressBook[dest].name) + " ";
        }
=======
        CTxDestination dest = DecodeDestination(strAddress);
        std::string name;
        if (wallet.getAddress(dest, &name) && !name.empty())
            strHTML += GUIUtil::HtmlEscape(name) + " ";
>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files
        strHTML += GUIUtil::HtmlEscape(strAddress) + "<br>";
    }

    //
    // Amount
    //
<<<<<<< HEAD
    if (wtx.IsCoinBase() && nCredit == Amount::zero()) {
        //
        // Coinbase
        //
        Amount nUnmatured = Amount::zero();
        for (const CTxOut &txout : wtx.tx->vout) {
            nUnmatured += wallet->GetCredit(txout, ISMINE_ALL);
        }
        strHTML += "<b>" + tr("Credit") + ":</b> ";
        if (wtx.IsInMainChain()) {
            strHTML += BitcoinUnits::formatHtmlWithUnit(unit, nUnmatured) +
                       " (" +
                       tr("matures in %n more block(s)", "",
                          wtx.GetBlocksToMaturity()) +
                       ")";
        } else {
=======
    if (wtx.is_coinbase && nCredit == 0)
    {
        //
        // Coinbase
        //
        CAmount nUnmatured = 0;
        for (const CTxOut& txout : wtx.tx->vout)
            nUnmatured += wallet.getCredit(txout, ISMINE_ALL);
        strHTML += "<b>" + tr("Credit") + ":</b> ";
        if (status.is_in_main_chain)
            strHTML += BitcoinUnits::formatHtmlWithUnit(unit, nUnmatured)+ " (" + tr("matures in %n more block(s)", "", status.blocks_to_maturity) + ")";
        else
>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files
            strHTML += "(" + tr("not accepted") + ")";
        }
        strHTML += "<br>";
    } else if (nNet > Amount::zero()) {
        //
        // Credit
        //
        strHTML += "<b>" + tr("Credit") + ":</b> " +
                   BitcoinUnits::formatHtmlWithUnit(unit, nNet) + "<br>";
    } else {
        isminetype fAllFromMe = ISMINE_SPENDABLE;
<<<<<<< HEAD
        for (const CTxIn &txin : wtx.tx->vin) {
            isminetype mine = wallet->IsMine(txin);
            if (fAllFromMe > mine) {
                fAllFromMe = mine;
            }
        }

        isminetype fAllToMe = ISMINE_SPENDABLE;
        for (const CTxOut &txout : wtx.tx->vout) {
            isminetype mine = wallet->IsMine(txout);
            if (fAllToMe > mine) {
                fAllToMe = mine;
            }
=======
        for (isminetype mine : wtx.txin_is_mine)
        {
            if(fAllFromMe > mine) fAllFromMe = mine;
        }

        isminetype fAllToMe = ISMINE_SPENDABLE;
        for (isminetype mine : wtx.txout_is_mine)
        {
            if(fAllToMe > mine) fAllToMe = mine;
>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files
        }

        if (fAllFromMe) {
            if (fAllFromMe & ISMINE_WATCH_ONLY) {
                strHTML +=
                    "<b>" + tr("From") + ":</b> " + tr("watch-only") + "<br>";
            }

            //
            // Debit
            //
<<<<<<< HEAD
            for (const CTxOut &txout : wtx.tx->vout) {
                // Ignore change
                isminetype toSelf = wallet->IsMine(txout);
                if ((toSelf == ISMINE_SPENDABLE) &&
                    (fAllFromMe == ISMINE_SPENDABLE)) {
=======
            auto mine = wtx.txout_is_mine.begin();
            for (const CTxOut& txout : wtx.tx->vout)
            {
                // Ignore change
                isminetype toSelf = *(mine++);
                if ((toSelf == ISMINE_SPENDABLE) && (fAllFromMe == ISMINE_SPENDABLE))
>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files
                    continue;
                }

<<<<<<< HEAD
                if (!wtx.mapValue.count("to") || wtx.mapValue["to"].empty()) {
=======
                if (!wtx.value_map.count("to") || wtx.value_map["to"].empty())
                {
>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files
                    // Offline transaction
                    CTxDestination address;
                    if (ExtractDestination(txout.scriptPubKey, address)) {
                        strHTML += "<b>" + tr("To") + ":</b> ";
<<<<<<< HEAD
                        if (wallet->mapAddressBook.count(address) &&
                            !wallet->mapAddressBook[address].name.empty()) {
                            strHTML +=
                                GUIUtil::HtmlEscape(
                                    wallet->mapAddressBook[address].name) +
                                " ";
                        }
                        strHTML +=
                            GUIUtil::HtmlEscape(EncodeDestination(address));
                        if (toSelf == ISMINE_SPENDABLE) {
=======
                        std::string name;
                        if (wallet.getAddress(address, &name) && !name.empty())
                            strHTML += GUIUtil::HtmlEscape(name) + " ";
                        strHTML += GUIUtil::HtmlEscape(EncodeDestination(address));
                        if(toSelf == ISMINE_SPENDABLE)
>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files
                            strHTML += " (own address)";
                        } else if (toSelf & ISMINE_WATCH_ONLY) {
                            strHTML += " (watch-only)";
                        }
                        strHTML += "<br>";
                    }
                }

                strHTML +=
                    "<b>" + tr("Debit") + ":</b> " +
                    BitcoinUnits::formatHtmlWithUnit(unit, -1 * txout.nValue) +
                    "<br>";
                if (toSelf) {
                    strHTML +=
                        "<b>" + tr("Credit") + ":</b> " +
                        BitcoinUnits::formatHtmlWithUnit(unit, txout.nValue) +
                        "<br>";
                }
            }

            if (fAllToMe) {
                // Payment to self
<<<<<<< HEAD
                Amount nChange = wtx.GetChange();
                Amount nValue = nCredit - nChange;
                strHTML += "<b>" + tr("Total debit") + ":</b> " +
                           BitcoinUnits::formatHtmlWithUnit(unit, -1 * nValue) +
                           "<br>";
                strHTML += "<b>" + tr("Total credit") + ":</b> " +
                           BitcoinUnits::formatHtmlWithUnit(unit, nValue) +
                           "<br>";
=======
                CAmount nChange = wtx.change;
                CAmount nValue = nCredit - nChange;
                strHTML += "<b>" + tr("Total debit") + ":</b> " + BitcoinUnits::formatHtmlWithUnit(unit, -nValue) + "<br>";
                strHTML += "<b>" + tr("Total credit") + ":</b> " + BitcoinUnits::formatHtmlWithUnit(unit, nValue) + "<br>";
>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files
            }

            Amount nTxFee = nDebit - wtx.tx->GetValueOut();
            if (nTxFee > Amount::zero())
                strHTML += "<b>" + tr("Transaction fee") + ":</b> " +
                           BitcoinUnits::formatHtmlWithUnit(unit, -1 * nTxFee) +
                           "<br>";
        } else {
            //
            // Mixed debit transaction
            //
<<<<<<< HEAD
            for (const CTxIn &txin : wtx.tx->vin) {
                if (wallet->IsMine(txin)) {
                    strHTML +=
                        "<b>" + tr("Debit") + ":</b> " +
                        BitcoinUnits::formatHtmlWithUnit(
                            unit, -1 * wallet->GetDebit(txin, ISMINE_ALL)) +
                        "<br>";
                }
            }
            for (const CTxOut &txout : wtx.tx->vout) {
                if (wallet->IsMine(txout)) {
                    strHTML += "<b>" + tr("Credit") + ":</b> " +
                               BitcoinUnits::formatHtmlWithUnit(
                                   unit, wallet->GetCredit(txout, ISMINE_ALL)) +
                               "<br>";
=======
            auto mine = wtx.txin_is_mine.begin();
            for (const CTxIn& txin : wtx.tx->vin) {
                if (*(mine++)) {
                    strHTML += "<b>" + tr("Debit") + ":</b> " + BitcoinUnits::formatHtmlWithUnit(unit, -wallet.getDebit(txin, ISMINE_ALL)) + "<br>";
                }
            }
            mine = wtx.txout_is_mine.begin();
            for (const CTxOut& txout : wtx.tx->vout) {
                if (*(mine++)) {
                    strHTML += "<b>" + tr("Credit") + ":</b> " + BitcoinUnits::formatHtmlWithUnit(unit, wallet.getCredit(txout, ISMINE_ALL)) + "<br>";
>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files
                }
            }
        }
    }

    strHTML += "<b>" + tr("Net amount") + ":</b> " +
               BitcoinUnits::formatHtmlWithUnit(unit, nNet, true) + "<br>";

    //
    // Message
    //
<<<<<<< HEAD
    if (wtx.mapValue.count("message") && !wtx.mapValue["message"].empty()) {
        strHTML += "<br><b>" + tr("Message") + ":</b><br>" +
                   GUIUtil::HtmlEscape(wtx.mapValue["message"], true) + "<br>";
    }
    if (wtx.mapValue.count("comment") && !wtx.mapValue["comment"].empty()) {
        strHTML += "<br><b>" + tr("Comment") + ":</b><br>" +
                   GUIUtil::HtmlEscape(wtx.mapValue["comment"], true) + "<br>";
    }

    strHTML +=
        "<b>" + tr("Transaction ID") + ":</b> " + rec->getTxID() + "<br>";
    strHTML += "<b>" + tr("Transaction total size") + ":</b> " +
               QString::number(wtx.tx->GetTotalSize()) + " bytes<br>";
    strHTML += "<b>" + tr("Output index") + ":</b> " +
               QString::number(rec->getOutputIndex()) + "<br>";

    // Message from normal bitcoincash:URI (bitcoincash:123...?message=example)
    for (const std::pair<std::string, std::string> &r : wtx.vOrderForm) {
        if (r.first == "Message") {
            strHTML += "<br><b>" + tr("Message") + ":</b><br>" +
                       GUIUtil::HtmlEscape(r.second, true) + "<br>";
        }
    }
=======
    if (wtx.value_map.count("message") && !wtx.value_map["message"].empty())
        strHTML += "<br><b>" + tr("Message") + ":</b><br>" + GUIUtil::HtmlEscape(wtx.value_map["message"], true) + "<br>";
    if (wtx.value_map.count("comment") && !wtx.value_map["comment"].empty())
        strHTML += "<br><b>" + tr("Comment") + ":</b><br>" + GUIUtil::HtmlEscape(wtx.value_map["comment"], true) + "<br>";

    strHTML += "<b>" + tr("Transaction ID") + ":</b> " + rec->getTxHash() + "<br>";
    strHTML += "<b>" + tr("Transaction total size") + ":</b> " + QString::number(wtx.tx->GetTotalSize()) + " bytes<br>";
    strHTML += "<b>" + tr("Transaction virtual size") + ":</b> " + QString::number(GetVirtualTransactionSize(*wtx.tx)) + " bytes<br>";
    strHTML += "<b>" + tr("Output index") + ":</b> " + QString::number(rec->getOutputIndex()) + "<br>";

    // Message from normal bitcoin:URI (bitcoin:123...?message=example)
    for (const std::pair<std::string, std::string>& r : orderForm)
        if (r.first == "Message")
            strHTML += "<br><b>" + tr("Message") + ":</b><br>" + GUIUtil::HtmlEscape(r.second, true) + "<br>";
>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files

    //
    // PaymentRequest info:
    //
<<<<<<< HEAD
    for (const std::pair<std::string, std::string> &r : wtx.vOrderForm) {
        if (r.first == "PaymentRequest") {
=======
    for (const std::pair<std::string, std::string>& r : orderForm)
    {
        if (r.first == "PaymentRequest")
        {
>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files
            PaymentRequestPlus req;
            req.parse(
                QByteArray::fromRawData(r.second.data(), r.second.size()));
            QString merchant;
            if (req.getMerchant(PaymentServer::getCertStore(), merchant)) {
                strHTML += "<b>" + tr("Merchant") + ":</b> " +
                           GUIUtil::HtmlEscape(merchant) + "<br>";
            }
        }
    }

<<<<<<< HEAD
    if (wtx.IsCoinBase()) {
        quint32 numBlocksToMaturity = COINBASE_MATURITY + 1;
        strHTML +=
            "<br>" +
            tr("Generated coins must mature %1 blocks before they can be "
               "spent. When you generated this block, it was broadcast to the "
               "network to be added to the block chain. If it fails to get "
               "into the chain, its state will change to \"not accepted\" and "
               "it won't be spendable. This may occasionally happen if another "
               "node generates a block within a few seconds of yours.")
                .arg(QString::number(numBlocksToMaturity)) +
            "<br>";
=======
    if (wtx.is_coinbase)
    {
        quint32 numBlocksToMaturity = COINBASE_MATURITY +  1;
        strHTML += "<br>" + tr("Generated coins must mature %1 blocks before they can be spent. When you generated this block, it was broadcast to the network to be added to the block chain. If it fails to get into the chain, its state will change to \"not accepted\" and it won't be spendable. This may occasionally happen if another node generates a block within a few seconds of yours.").arg(QString::number(numBlocksToMaturity)) + "<br>";
>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files
    }

    //
    // Debug view
    //
<<<<<<< HEAD
    if (gArgs.GetBoolArg("-debug", false)) {
        strHTML += "<hr><br>" + tr("Debug information") + "<br><br>";
        for (const CTxIn &txin : wtx.tx->vin) {
            if (wallet->IsMine(txin)) {
                strHTML += "<b>" + tr("Debit") + ":</b> " +
                           BitcoinUnits::formatHtmlWithUnit(
                               unit, -1 * wallet->GetDebit(txin, ISMINE_ALL)) +
                           "<br>";
            }
        }
        for (const CTxOut &txout : wtx.tx->vout) {
            if (wallet->IsMine(txout)) {
                strHTML += "<b>" + tr("Credit") + ":</b> " +
                           BitcoinUnits::formatHtmlWithUnit(
                               unit, wallet->GetCredit(txout, ISMINE_ALL)) +
                           "<br>";
            }
        }
=======
    if (node.getLogCategories() != BCLog::NONE)
    {
        strHTML += "<hr><br>" + tr("Debug information") + "<br><br>";
        for (const CTxIn& txin : wtx.tx->vin)
            if(wallet.txinIsMine(txin))
                strHTML += "<b>" + tr("Debit") + ":</b> " + BitcoinUnits::formatHtmlWithUnit(unit, -wallet.getDebit(txin, ISMINE_ALL)) + "<br>";
        for (const CTxOut& txout : wtx.tx->vout)
            if(wallet.txoutIsMine(txout))
                strHTML += "<b>" + tr("Credit") + ":</b> " + BitcoinUnits::formatHtmlWithUnit(unit, wallet.getCredit(txout, ISMINE_ALL)) + "<br>";
>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files

        strHTML += "<br><b>" + tr("Transaction") + ":</b><br>";
        strHTML += GUIUtil::HtmlEscape(wtx.tx->ToString(), true);

        strHTML += "<br><b>" + tr("Inputs") + ":</b>";
        strHTML += "<ul>";

        for (const CTxIn &txin : wtx.tx->vin) {
            COutPoint prevout = txin.prevout;

            Coin prev;
<<<<<<< HEAD
            if (pcoinsTip->GetCoin(prevout, prev)) {
                strHTML += "<li>";
                const CTxOut &vout = prev.GetTxOut();
                CTxDestination address;
                if (ExtractDestination(vout.scriptPubKey, address)) {
                    if (wallet->mapAddressBook.count(address) &&
                        !wallet->mapAddressBook[address].name.empty()) {
                        strHTML += GUIUtil::HtmlEscape(
                                       wallet->mapAddressBook[address].name) +
                                   " ";
                    }
                    strHTML +=
                        QString::fromStdString(EncodeDestination(address));
=======
            if(node.getUnspentOutput(prevout, prev))
            {
                {
                    strHTML += "<li>";
                    const CTxOut &vout = prev.out;
                    CTxDestination address;
                    if (ExtractDestination(vout.scriptPubKey, address))
                    {
                        std::string name;
                        if (wallet.getAddress(address, &name) && !name.empty())
                            strHTML += GUIUtil::HtmlEscape(name) + " ";
                        strHTML += QString::fromStdString(EncodeDestination(address));
                    }
                    strHTML = strHTML + " " + tr("Amount") + "=" + BitcoinUnits::formatHtmlWithUnit(unit, vout.nValue);
                    strHTML = strHTML + " IsMine=" + (wallet.txoutIsMine(vout) & ISMINE_SPENDABLE ? tr("true") : tr("false")) + "</li>";
                    strHTML = strHTML + " IsWatchOnly=" + (wallet.txoutIsMine(vout) & ISMINE_WATCH_ONLY ? tr("true") : tr("false")) + "</li>";
>>>>>>> 5884558... Remove direct bitcoin calls from qt transaction table files
                }
                strHTML = strHTML + " " + tr("Amount") + "=" +
                          BitcoinUnits::formatHtmlWithUnit(unit, vout.nValue);
                strHTML =
                    strHTML + " IsMine=" +
                    (wallet->IsMine(vout) & ISMINE_SPENDABLE ? tr("true")
                                                             : tr("false")) +
                    "</li>";
                strHTML =
                    strHTML + " IsWatchOnly=" +
                    (wallet->IsMine(vout) & ISMINE_WATCH_ONLY ? tr("true")
                                                              : tr("false")) +
                    "</li>";
            }
        }

        strHTML += "</ul>";
    }

    strHTML += "</font></html>";
    return strHTML;
}
