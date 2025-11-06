/**
 * File: DemoWindow.h
 * 
 * Demo window showcasing qtChatWidget features
 */
#ifndef DEMO_WINDOW_H
#define DEMO_WINDOW_H

#include <QWidget>

class QLabel;
class uiChatWidget;

/**
 * \brief Demo window showcasing qtChatWidget features
 */
class DemoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DemoWindow(QWidget* parent = nullptr);

private slots:
    void onMessageSent(const QString& message);
    void onSimulateResponse();
void onToggleProgress();
    void onShowHistory();
    void onShowContext();
    void onClearHistory();
    void onAddUserMessage();
    void onAddAssistantMessage();
    void onAddSystemMessage();

private:
    uiChatWidget* _chatWidget;
    QLabel* _statusLabel;
    int _messageCounter;
};

#endif // DEMO_WINDOW_H
