/**
 * File: qtChatWidget.h
 *
 * History:
 * When      | Who           | What
 * ----------|---------------|------------------------------------------------------
 * 25/10/2025| Tian-Qing Ye  | Created with assistance of Claude Sonnet 4.5
 */
#ifndef QT_CHATWIDGET_H
#define QT_CHATWIDGET_H

#include <QWidget>
#include <QString>
#include <QList>
#include <QDateTime>

 // Forward declarations
class QTextEdit;
class QLineEdit;
class QPushButton;
class QProgressBar;

/**
 * \brief Structure to hold a single chat message
 */
struct ChatMessage
{
	QString timestamp;  // ISO format: "yyyy-MM-dd hh:mm:ss"
	QString sender;		// "You", "Assistant", "System"
	QString message;    // Message content
	QString role;       // "user", "assistant", "system" (OpenAI format)

	ChatMessage() = default;

	ChatMessage(const QString& ts, const QString& s, const QString& m, const QString& r)
		: timestamp(ts), sender(s), message(m), role(r) {
	}
};

/**
 * \brief A reusable chat widget with AI assistant integration
 *
 * This widget provides a chat interface with:
 * - Formatted message display with timestamps
 * - Input box with send button
 * - Progress indicator for async operations
 * - Chat history management (structured format)
 */
class uiChatWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * \brief Constructor
	 * \param title The title/header text for the chat widget
	 * \param maxContextMessages Maximum number of messages to keep in context (default: 20)
	 * \param parent Parent widget
	 */
	explicit uiChatWidget(const QString& title = "AI Assistant",
		const QString& welcomeMsg = "Hello. I am your assistant. How can I help you today?",
		int maxContextMessages = 20,
		QWidget* parent = nullptr);

	virtual ~uiChatWidget();

	/**
	 * \brief Append a message to the chat history display
	 * \param sender The sender name (e.g., "You", "Assistant", "System")
	 * \param message The message content
	 */
	void AppendChatMessage(const QString& sender, const QString& message);

	/**
	 * \brief Get the chat history as a list of messages
	 * \return QList of ChatMessage structures
	 */
	QList<ChatMessage> GetChatHistory() const { return _chatHistory; }

	/**
	 * \brief Set the entire chat history (useful for loading from file)
	 * \param history List of ChatMessage structures
	 */
	void SetChatHistory(const QList<ChatMessage>& history);

	/**
	 * \brief Clear the chat history
	 */
	void ClearChatHistory();

	/**
	 * \brief Build context messages suitable for OpenAI API
	 * \param maxMessages Maximum number of messages to include (-1 for all)
	 * \return QList of recent messages (user/assistant only, excludes system notifications)
	 */
	QList<ChatMessage> BuildContextMessages(int maxMessages = -1) const;

	//! Show the progress indicator
	void ShowProgressIndicator();

	//! Hide the progress indicator
	void HideProgressIndicator();

	/**
	 * \brief Get the current input text
	 * \return QString containing the input box text
	 */
	QString GetInputText() const;

	//! Clear the input box
	void ClearInput();

	/**
	 * \brief Enable or disable the input controls
	 * \param enabled true to enable, false to disable
	 */
	void SetInputEnabled(bool enabled);

	/**
	 * \brief Set the title/header text
	 * \param title The new title text
	 */
	void SetTitle(const QString& title);

signals:
	/**
	 * \brief Emitted when the user sends a message
	 * \param message The message text
	 */
	void messageSent(const QString& message);

private slots:
	void onSendButtonClicked();

private:
	Q_DISABLE_COPY(uiChatWidget)

		//! Chat history stored as list of messages
		QList<ChatMessage> _chatHistory;

	//! Maximum number of messages to send as context (to avoid token limits)
	int _maxContextMessages;

	// UI Components
	QTextEdit* _chatHistoryDisplay;
	QLineEdit* _chatInputBox;
	QPushButton* _sendButton;
	QProgressBar* _progressBar;

	//! Create and setup the UI
	void createUI(const QString& title);

	//! Helper: Convert sender name to role
	QString senderToRole(const QString& sender) const;
};

#endif // UI_ChatWidget_H
