/**
 * File: qtChatWidget.cpp
 *
 * History:
 * When      | Who            | What
 * ----------|----------------|------------------------------------------------
 * 25/10/2025| Tian-Qing Ye   | Created with assistance of Claude Sonnet 4.5
 */
#include "qtChatWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QScrollBar>
#include <QDateTime>
#include <QApplication>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QBrush>
#include <QColor>

uiChatWidget::uiChatWidget(const QString& title, const QString& welcomeMsg, int maxContextMessages, QWidget* parent)
	: QWidget(parent)
	, _maxContextMessages(maxContextMessages)
	, _chatHistoryDisplay(nullptr)
	, _chatInputBox(nullptr)
	, _sendButton(nullptr)
	, _progressBar(nullptr)
{
	// Create the UI
	createUI(title);

	// Add Assistant welcome message
	if (welcomeMsg.isEmpty())
		AppendChatMessage("Assistant", "Welcome! I'm your AI assistant. How can I help you today?");
	else
		AppendChatMessage("Assistant", welcomeMsg);

	AppendChatMessage("Assistant", QString("Note: The assistant will only remember up to %1 recent messages for context.").arg(_maxContextMessages));
}

uiChatWidget::~uiChatWidget()
{
}

void uiChatWidget::createUI(const QString& title)
{
	QVBoxLayout* mainLayout = new QVBoxLayout;
	setLayout(mainLayout);

	// Title/Instructions Label
	if (title.isEmpty() == false)
	{
		QLabel* titleLabel = new QLabel(title, this);
		titleLabel->setStyleSheet("font-weight: bold; font-size: 11pt; padding: 5px;");
		mainLayout->addWidget(titleLabel);
	}

	// Chat History Display Area
	_chatHistoryDisplay = new QTextEdit(this);
	_chatHistoryDisplay->setReadOnly(true);
	_chatHistoryDisplay->setPlaceholderText("Chat history will appear here...");
	_chatHistoryDisplay->setStyleSheet(
		"QTextEdit { "
		"   background-color: #f5f5f5; "
		"   border: 1px solid #cccccc; "
		"   border-radius: 4px; "
		"   padding: 8px; "
		"   font-family: 'Segoe UI', Arial, sans-serif; "
		"   font-size: 10pt; "
		"}"
	);
	mainLayout->addWidget(_chatHistoryDisplay, 1);

	// Progress Bar (initially hidden)
	_progressBar = new QProgressBar(this);
	_progressBar->setTextVisible(false);
	_progressBar->setRange(0, 0); // Indeterminate/busy mode
	_progressBar->setStyleSheet(
		"QProgressBar {"
		"   border: 2px solid #0078d4;"
		"   border-radius: 4px;"
		"   background-color: #e6e6e6;"
		"   height: 8px;"
		"   margin: 4px 0px;"
		"   text-align: center;"
		"}"
		"QProgressBar::chunk {"
		"   background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
		"       stop:0 #0078d4, stop:0.5 #106ebe, stop:1 #0078d4);"
		"   border-radius: 2px;"
		"   width: 20px;"
		"}"
	);
	_progressBar->setMaximumHeight(12);
	_progressBar->setMinimumHeight(12);
	_progressBar->setVisible(false);
	mainLayout->addWidget(_progressBar);

	// Input Area Container
	QWidget* inputContainer = new QWidget(this);
	QHBoxLayout* inputLayout = new QHBoxLayout;
	inputContainer->setLayout(inputLayout);
	inputLayout->setContentsMargins(0, 5, 0, 0);

	// Input Box
	_chatInputBox = new QLineEdit(inputContainer);
	_chatInputBox->setPlaceholderText("Type your query here and press Enter or click Send...");
	_chatInputBox->setStyleSheet(
		"QLineEdit { "
		"   padding: 8px; "
		"   border: 1px solid #cccccc; "
		"   border-radius: 4px; "
		"   font-size: 10pt; "
		"}"
	);
	inputLayout->addWidget(_chatInputBox, 1);

	// Send Button
	_sendButton = new QPushButton("Send", inputContainer);
	_sendButton->setStyleSheet(
		"QPushButton { "
		"   background-color: #0078d4; "
		"   color: white; "
		"   border: none; "
		"   border-radius: 4px; "
		"   padding: 8px 20px; "
		"   font-size: 10pt; "
		"   font-weight: bold; "
		"} "
		"QPushButton:hover { "
		"   background-color: #106ebe; "
		"} "
		"QPushButton:pressed { "
		"   background-color: #005a9e; "
		"} "
		"QPushButton:disabled { "
		"   background-color: #cccccc; "
		"   color: #666666; "
		"}"
	);
	_sendButton->setCursor(Qt::PointingHandCursor);
	_sendButton->setMinimumWidth(80);
	inputLayout->addWidget(_sendButton);

	mainLayout->addWidget(inputContainer);

	// Connect signals
	connect(_chatInputBox, &QLineEdit::returnPressed, this, &uiChatWidget::onSendButtonClicked);
	connect(_sendButton, &QPushButton::clicked, this, &uiChatWidget::onSendButtonClicked);
}

QString uiChatWidget::senderToRole(const QString& sender) const
{
	if (sender == "You" || sender == "User") {
		return "user";
	}
	else if (sender == "Assistant" || sender == "Bot") {
		return "assistant";
	}
	else {
		return "system";
	}
}

void uiChatWidget::onSendButtonClicked()
{
	QString userInput = _chatInputBox->text().trimmed();

	// Check if input is empty
	if (userInput.isEmpty())
		return;

	// Display user message
	AppendChatMessage("You", userInput);

	// Clear input box
	_chatInputBox->clear();

	// Emit signal so parent can handle the query
	emit messageSent(userInput);
}

void uiChatWidget::AppendChatMessage(const QString& sender, const QString& message)
{
	if (!_chatHistoryDisplay) return;

	// Create and store message
	QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	QString role = senderToRole(sender);

	ChatMessage chatMsg(timestamp, sender, message, role);
	_chatHistory.append(chatMsg);

	// Display in UI
	QTextCursor cursor = _chatHistoryDisplay->textCursor();
	cursor.movePosition(QTextCursor::End);

	// Format timestamp for display (time only)
	QString displayTime = timestamp.mid(11, 8); // Extract "hh:mm:ss"

	// Set format based on sender
	QTextCharFormat senderFormat;
	QTextCharFormat messageFormat;

	if (sender == "You") {
		senderFormat.setFontWeight(QFont::Bold);
		senderFormat.setForeground(QBrush(QColor("#0078d4")));
	}
	else if (sender == "Assistant" || sender == "Bot") {
		senderFormat.setFontWeight(QFont::Bold);
		senderFormat.setForeground(QBrush(QColor("#107c10")));
	}
	else if (sender == "System") {
		senderFormat.setFontWeight(QFont::Bold);
		senderFormat.setForeground(QBrush(QColor("#605e5c")));
		messageFormat.setFontItalic(true);
	}
	else {
		senderFormat.setFontWeight(QFont::Bold);
	}

	messageFormat.setForeground(QBrush(QColor("#323130")));

	// Add separator if not the first message
	if (!_chatHistoryDisplay->toPlainText().isEmpty()) {
		cursor.insertText("\n");
	}

	// Insert sender and timestamp
	cursor.setCharFormat(senderFormat);
	cursor.insertText(QString("[%1] %2:\n").arg(displayTime).arg(sender));

	// Insert message
	cursor.setCharFormat(messageFormat);
	cursor.insertText(message + "\n");

	// Scroll to bottom
	_chatHistoryDisplay->verticalScrollBar()->setValue(_chatHistoryDisplay->verticalScrollBar()->maximum());
}

void uiChatWidget::SetChatHistory(const QList<ChatMessage>& history)
{
	_chatHistory = history;

	// Rebuild display
	if (_chatHistoryDisplay) {
		_chatHistoryDisplay->clear();

		for (const ChatMessage& msg : _chatHistory) {
			QTextCursor cursor = _chatHistoryDisplay->textCursor();
			cursor.movePosition(QTextCursor::End);

			// Extract time from timestamp
			QString displayTime = msg.timestamp.mid(11, 8);

			// Set format based on sender
			QTextCharFormat senderFormat;
			QTextCharFormat messageFormat;

			if (msg.sender == "You") {
				senderFormat.setFontWeight(QFont::Bold);
				senderFormat.setForeground(QBrush(QColor("#0078d4")));
			}
			else if (msg.sender == "Assistant" || msg.sender == "Bot") {
				senderFormat.setFontWeight(QFont::Bold);
				senderFormat.setForeground(QBrush(QColor("#107c10")));
			}
			else if (msg.sender == "System") {
				senderFormat.setFontWeight(QFont::Bold);
				senderFormat.setForeground(QBrush(QColor("#605e5c")));
				messageFormat.setFontItalic(true);
			}
			else {
				senderFormat.setFontWeight(QFont::Bold);
			}

			messageFormat.setForeground(QBrush(QColor("#323130")));

			// Add separator
			if (!_chatHistoryDisplay->toPlainText().isEmpty()) {
				cursor.insertText("\n");
			}

			// Insert sender and timestamp
			cursor.setCharFormat(senderFormat);
			cursor.insertText(QString("[%1] %2:\n").arg(displayTime).arg(msg.sender));

			// Insert message
			cursor.setCharFormat(messageFormat);
			cursor.insertText(msg.message + "\n");
		}

		// Scroll to bottom
		_chatHistoryDisplay->verticalScrollBar()->setValue(_chatHistoryDisplay->verticalScrollBar()->maximum());
	}
}

QList<ChatMessage> uiChatWidget::BuildContextMessages(int maxMessages) const
{
	QList<ChatMessage> contextMessages;

	// Use provided maxMessages or fall back to member variable
	int limit = (maxMessages > 0) ? maxMessages : _maxContextMessages;

	// Count user/assistant messages only
	int userAssistantCount = 0;
	for (const ChatMessage& msg : _chatHistory) {
		if (msg.role == "user" || msg.role == "assistant") {
			userAssistantCount++;
		}
	}

	// Calculate how many to skip
	int skipCount = (userAssistantCount > limit) ? (userAssistantCount - limit) : 0;
	int currentCount = 0;

	// Build context list (skip system messages, limit to recent messages)
	for (const ChatMessage& msg : _chatHistory) {
		if (msg.role == "user" || msg.role == "assistant") {
			if (currentCount >= skipCount) {
				contextMessages.append(msg);
			}
			currentCount++;
		}
	}

	return contextMessages;
}

void uiChatWidget::ShowProgressIndicator()
{
	if (_progressBar) {
		_progressBar->setVisible(true);
		_progressBar->setRange(0, 0); // Ensure indeterminate mode
		QApplication::processEvents(); // Force immediate repaint
	}
}

void uiChatWidget::HideProgressIndicator()
{
	if (_progressBar) {
		_progressBar->setVisible(false);
		QApplication::processEvents(); // Force immediate repaint
	}
}

QString uiChatWidget::GetInputText() const
{
	return _chatInputBox ? _chatInputBox->text() : QString();
}

void uiChatWidget::ClearInput()
{
	if (_chatInputBox) {
		_chatInputBox->clear();
	}
}

void uiChatWidget::SetInputEnabled(bool enabled)
{
	if (_chatInputBox) {
		_chatInputBox->setEnabled(enabled);
	}
	if (_sendButton) {
		if (enabled) {
			_sendButton->setText("Send");
		}
		else {
			_sendButton->setText("Wait...");
		}
		_sendButton->setEnabled(enabled);
	}
}

void uiChatWidget::SetTitle(const QString& title)
{
	// Find the title label (first child of the layout)
	QLabel* titleLabel = findChild<QLabel*>();
	if (titleLabel) {
		titleLabel->setText(title);
	}
}

void uiChatWidget::ClearChatHistory()
{
	// Clear history
	_chatHistory.clear();

	// Clear display
	if (_chatHistoryDisplay) {
		_chatHistoryDisplay->clear();
	}

	// Add welcome message back
	AppendChatMessage("System", "Welcome! I'm your AI assistant. How can I help you today?");
	AppendChatMessage("System", QString("Note: The assistant will remember up to %1 recent messages for context.").arg(_maxContextMessages));
}
