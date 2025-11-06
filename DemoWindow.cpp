/**
 * File: DemoWindow.cpp
 *
 * Implementation of demo window showcasing qtChatWidget features
 */
#include "DemoWindow.h"
#include "qtChatWidget/qtChatWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>

DemoWindow::DemoWindow(QWidget* parent) : QWidget(parent), _messageCounter(0)
{
	setWindowTitle("qtChatWidget Demonstration");

	// Get the screen size and set initial size to 80% of it
	QSize screenSize = QGuiApplication::primaryScreen()->size();
	resize(screenSize.width() * 0.8, screenSize.height() * 0.8);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	// Title
	QLabel* titleLabel = new QLabel("qtChatWidget Demo Application", this);
	titleLabel->setStyleSheet("font-size: 14pt; font-weight: bold; padding: 10px;");
	titleLabel->setAlignment(Qt::AlignCenter);
	mainLayout->addWidget(titleLabel);

	QLabel* descLabel = new QLabel(
		"This demo shows the features of the qtChatWidget class:\n"
		"• Formatted chat display with timestamps\n"
		"• Message sending and receiving\n"
		"• Progress indicator for async operations\n"
		"• Chat history management and context building",
		this
	);
	descLabel->setStyleSheet("padding: 5px 10px; color: #605e5c;");
	mainLayout->addWidget(descLabel);

	// Create the chat widget
	_chatWidget = new uiChatWidget(
		"AI Assistant Demo",
		"Hello! I'm a simulated AI assistant. Try sending me messages and explore the demo buttons below!",
		20,
		this
	);
	mainLayout->addWidget(_chatWidget, 1);

	// Demo control buttons
	QGroupBox* controlGroup = new QGroupBox("Demo Controls", this);
	QVBoxLayout* controlLayout = new QVBoxLayout(controlGroup);

	// Row 1: Simulate response and progress
	QHBoxLayout* row1 = new QHBoxLayout();
	QPushButton* simulateBtn = new QPushButton("Simulate AI Response (2s delay)", this);
	simulateBtn->setToolTip("Shows progress indicator and simulates async AI response");
	connect(simulateBtn, &QPushButton::clicked, this, &DemoWindow::onSimulateResponse);
	row1->addWidget(simulateBtn);

	QPushButton* toggleProgressBtn = new QPushButton("Toggle Progress Indicator", this);
	toggleProgressBtn->setToolTip("Manually show/hide the progress bar");
	connect(toggleProgressBtn, &QPushButton::clicked, this, &DemoWindow::onToggleProgress);
	row1->addWidget(toggleProgressBtn);
	controlLayout->addLayout(row1);

	// Row 2: History management
	QHBoxLayout* row2 = new QHBoxLayout();
	QPushButton* showHistoryBtn = new QPushButton("Show Chat History", this);
	showHistoryBtn->setToolTip("Display chat history in console/debug output");
	connect(showHistoryBtn, &QPushButton::clicked, this, &DemoWindow::onShowHistory);
	row2->addWidget(showHistoryBtn);

	QPushButton* showContextBtn = new QPushButton("Show Context (last 5)", this);
	showContextBtn->setToolTip("Display context messages suitable for AI API");
	connect(showContextBtn, &QPushButton::clicked, this, &DemoWindow::onShowContext);
	row2->addWidget(showContextBtn);

	QPushButton* clearBtn = new QPushButton("Clear History", this);
	clearBtn->setToolTip("Clear all chat messages");
	connect(clearBtn, &QPushButton::clicked, this, &DemoWindow::onClearHistory);
	row2->addWidget(clearBtn);
	controlLayout->addLayout(row2);

	// Row 3: Add sample messages
	QHBoxLayout* row3 = new QHBoxLayout();
	QPushButton* addUserMsg = new QPushButton("Add User Message", this);
	connect(addUserMsg, &QPushButton::clicked, this, &DemoWindow::onAddUserMessage);
	row3->addWidget(addUserMsg);

	QPushButton* addAssistantMsg = new QPushButton("Add Assistant Message", this);
	connect(addAssistantMsg, &QPushButton::clicked, this, &DemoWindow::onAddAssistantMessage);
	row3->addWidget(addAssistantMsg);

	QPushButton* addSystemMsg = new QPushButton("Add System Message", this);
	connect(addSystemMsg, &QPushButton::clicked, this, &DemoWindow::onAddSystemMessage);
	row3->addWidget(addSystemMsg);
	controlLayout->addLayout(row3);

	mainLayout->addWidget(controlGroup);

	// Status label
	_statusLabel = new QLabel("Ready - Try typing a message or click the demo buttons", this);
	_statusLabel->setStyleSheet("padding: 5px; background-color: #f0f0f0; border-top: 1px solid #ccc;");
	mainLayout->addWidget(_statusLabel);

	// Connect chat widget signal
	connect(_chatWidget, &uiChatWidget::messageSent, this, &DemoWindow::onMessageSent);
}

void DemoWindow::onMessageSent(const QString& message)
{
	_statusLabel->setText(QString("Message sent: \"%1\"").arg(message));
	qDebug() << "User message:" << message;

	// Disable input while "processing"
	_chatWidget->SetInputEnabled(false);
	_chatWidget->ShowProgressIndicator();

	// Simulate async AI processing with a timer
	QTimer::singleShot(1500, this, [this, message]() {
		// Simulate AI response
		QString response = QString("I received your message: \"%1\". This is a simulated response!")
			.arg(message);
		_chatWidget->AppendChatMessage("Assistant", response);

		_chatWidget->HideProgressIndicator();
		_chatWidget->SetInputEnabled(true);
		_statusLabel->setText("Response received - Ready for next message");
		});
}

void DemoWindow::onSimulateResponse()
{
	_statusLabel->setText("Simulating async AI response...");
	_chatWidget->SetInputEnabled(false);
	_chatWidget->ShowProgressIndicator();

	QTimer::singleShot(2000, this, [this]() {
		_chatWidget->AppendChatMessage("Assistant",
			"This is a simulated async response that took 2 seconds to 'process'.");
		_chatWidget->HideProgressIndicator();
		_chatWidget->SetInputEnabled(true);
		_statusLabel->setText("Simulation complete");
		});
}

void DemoWindow::onToggleProgress()
{
	static bool progressVisible = false;
	progressVisible = !progressVisible;

	if (progressVisible) {
		_chatWidget->ShowProgressIndicator();
		_statusLabel->setText("Progress indicator shown");
	}
	else {
		_chatWidget->HideProgressIndicator();
		_statusLabel->setText("Progress indicator hidden");
	}
}

void DemoWindow::onShowHistory()
{
	QList<ChatMessage> history = _chatWidget->GetChatHistory();

	QString historyText = QString("=== Chat History (%1 messages) ===\n").arg(history.size());
	for (const ChatMessage& msg : history) {
		historyText += QString("[%1] %2 (%3): %4\n")
			.arg(msg.timestamp)
			.arg(msg.sender)
			.arg(msg.role)
			.arg(msg.message);
	}

	qDebug().noquote() << historyText;

	QMessageBox::information(this, "Chat History",
		QString("Chat history has %1 messages.\nCheck the console/debug output for details.")
		.arg(history.size()));

	_statusLabel->setText(QString("Displayed %1 messages in console").arg(history.size()));
}

void DemoWindow::onShowContext()
{
	QList<ChatMessage> context = _chatWidget->BuildContextMessages(5);

	QString contextText = QString("=== Context Messages (last 5 user/assistant) ===\n");
	for (const ChatMessage& msg : context) {
		contextText += QString("[%1] %2: %3\n")
			.arg(msg.role)
			.arg(msg.sender)
			.arg(msg.message);
	}

	qDebug().noquote() << contextText;

	QMessageBox::information(this, "Context Messages",
		QString("Built context with %1 messages (user/assistant only).\nCheck the console for details.")
		.arg(context.size()));

	_statusLabel->setText(QString("Built context with %1 messages").arg(context.size()));
}

void DemoWindow::onClearHistory()
{
	_chatWidget->ClearChatHistory();
	_statusLabel->setText("Chat history cleared");
	_messageCounter = 0;
}

void DemoWindow::onAddUserMessage()
{
	_messageCounter++;
	_chatWidget->AppendChatMessage("You",
		QString("This is demo user message #%1").arg(_messageCounter));
	_statusLabel->setText("Added user message");
}

void DemoWindow::onAddAssistantMessage()
{
	_messageCounter++;
	_chatWidget->AppendChatMessage("Assistant",
		QString("This is demo assistant response #%1. I can help you with various tasks!").arg(_messageCounter));
	_statusLabel->setText("Added assistant message");
}

void DemoWindow::onAddSystemMessage()
{
	_chatWidget->AppendChatMessage("System",
		"This is a system notification message. It appears in italic with gray color.");
	_statusLabel->setText("Added system message");
}
