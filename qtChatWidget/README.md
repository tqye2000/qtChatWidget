# qtChatWidget

A reusable Qt widget for creating chat interfaces with AI assistant integration.

## Overview

`qtChatWidget` (`uiChatWidget` class) is a fully-featured, ready-to-use chat interface widget built with Qt 5.15+. It provides a polished chat UI with message history, formatted display, progress indicators, and built-in context management suitable for AI/chatbot applications.

## Features

- **📝 Formatted Message Display**
  - Color-coded messages by sender (User, Assistant, System)
  - Timestamps for each message
  - Automatic scrolling to latest message
  - Rich text formatting support

- **⚡ Interactive Input**
  - Text input box with placeholder text
  - Send button with hover effects
  - Enter key support for sending messages
  - Enable/disable controls during processing

- **🔄 Progress Indication**
- Animated progress bar for async operations
  - Show/hide functionality
  - Visual feedback during AI processing

- **💾 Chat History Management**
  - Structured message storage (timestamp, sender, content, role)
  - Export/import chat history
  - Context building for AI APIs (OpenAI format compatible)
  - Configurable context window size

## Requirements

- Qt 5.15+ (Core, GUI, Widgets modules)
- C++14 or later
- Windows, Linux, or macOS

## Integration

### 1. Add to Your Project

Copy the following files to your project:
```
qtChatWidget/
├── qtChatWidget.h
└── qtChatWidget.cpp
```

### 2. Qt Project Configuration

**For `.vcxproj` (Visual Studio):**
```xml
<ItemGroup>
  <QtMoc Include="qtChatWidget\qtChatWidget.h" />
  <ClCompile Include="qtChatWidget\qtChatWidget.cpp" />
</ItemGroup>
```

**For `.pro` (qmake):**
```qmake
HEADERS += qtChatWidget/qtChatWidget.h
SOURCES += qtChatWidget/qtChatWidget.cpp
```

**For `CMakeLists.txt`:**
```cmake
set(CMAKE_AUTOMOC ON)
add_executable(YourApp
    qtChatWidget/qtChatWidget.h
    qtChatWidget/qtChatWidget.cpp
    # ... other files
)
target_link_libraries(YourApp Qt5::Core Qt5::Gui Qt5::Widgets)
```

### 3. Include and Use

```cpp
#include "qtChatWidget/qtChatWidget.h"

// Create the widget
uiChatWidget* chatWidget = new uiChatWidget(
    "AI Assistant",    // Title
 "Hello! How can I help?", // Welcome message
    20, // Max context messages
 parentWidget
);

// Connect to message sent signal
connect(chatWidget, &uiChatWidget::messageSent, 
        this, &YourClass::onUserMessage);
```

## API Reference

### Constructor

```cpp
uiChatWidget(
    const QString& title = "AI Assistant",
    const QString& welcomeMsg = "Hello. I am your assistant. How can I help you today?",
    int maxContextMessages = 20,
    QWidget* parent = nullptr
);
```

**Parameters:**
- `title` - Header text displayed at the top of the widget
- `welcomeMsg` - Initial greeting message from the assistant
- `maxContextMessages` - Maximum number of messages to keep in context (for AI API calls)
- `parent` - Parent widget

### Key Methods

#### Message Management

```cpp
// Add a message to the chat
void AppendChatMessage(const QString& sender, const QString& message);

// Get full chat history
QList<ChatMessage> GetChatHistory() const;

// Set chat history (load from file)
void SetChatHistory(const QList<ChatMessage>& history);

// Clear all messages
void ClearChatHistory();

// Build context for AI API (last N user/assistant messages only)
QList<ChatMessage> BuildContextMessages(int maxMessages = -1) const;
```

#### UI Control

```cpp
// Show/hide progress indicator
void ShowProgressIndicator();
void HideProgressIndicator();

// Enable/disable input controls
void SetInputEnabled(bool enabled);

// Get/clear input text
QString GetInputText() const;
void ClearInput();

// Update title
void SetTitle(const QString& title);
```

### Signals

```cpp
// Emitted when user sends a message (clicks Send or presses Enter)
void messageSent(const QString& message);
```

### ChatMessage Structure

```cpp
struct ChatMessage
{
    QString timestamp;  // ISO format: "yyyy-MM-dd hh:mm:ss"
    QString sender;     // "You", "Assistant", "System"
    QString message;    // Message content
    QString role;       // "user", "assistant", "system" (OpenAI format)
};
```

## Usage Examples

### Basic Chat Implementation

```cpp
void MyApp::setupChat()
{
    chatWidget = new uiChatWidget("AI Assistant", "Hello!", 20, this);
    
    connect(chatWidget, &uiChatWidget::messageSent,
            this, &MyApp::handleUserMessage);
          
    layout->addWidget(chatWidget);
}

void MyApp::handleUserMessage(const QString& message)
{
    // Disable input during processing
    chatWidget->SetInputEnabled(false);
    chatWidget->ShowProgressIndicator();
    
    // Send to AI API (pseudo-code)
    QList<ChatMessage> context = chatWidget->BuildContextMessages(10);
    aiService->sendQuery(message, context, [this](QString response) {
        // Display response
        chatWidget->AppendChatMessage("Assistant", response);
        chatWidget->HideProgressIndicator();
        chatWidget->SetInputEnabled(true);
    });
}
```

### Adding System Notifications

```cpp
// System messages appear in gray italic text
chatWidget->AppendChatMessage("System", "Connection established");
chatWidget->AppendChatMessage("System", "Model switched to GPT-4");
```

### Saving/Loading Chat History

```cpp
// Save to JSON
void MyApp::saveChatHistory()
{
    QList<ChatMessage> history = chatWidget->GetChatHistory();
    QJsonArray jsonArray;
    
    for (const ChatMessage& msg : history) {
        QJsonObject obj;
        obj["timestamp"] = msg.timestamp;
        obj["sender"] = msg.sender;
        obj["message"] = msg.message;
        obj["role"] = msg.role;
        jsonArray.append(obj);
    }
    
    // Save jsonArray to file...
}

// Load from JSON
void MyApp::loadChatHistory(const QJsonArray& jsonArray)
{
    QList<ChatMessage> history;
    
    for (const QJsonValue& val : jsonArray) {
        QJsonObject obj = val.toObject();
        ChatMessage msg(
            obj["timestamp"].toString(),
       obj["sender"].toString(),
            obj["message"].toString(),
            obj["role"].toString()
        );
     history.append(msg);
    }
    
    chatWidget->SetChatHistory(history);
}
```

### OpenAI API Integration

```cpp
void MyApp::sendToOpenAI(const QString& userMessage)
{
    // Get conversation context
    QList<ChatMessage> context = chatWidget->BuildContextMessages(10);
    
    // Build OpenAI API messages array
    QJsonArray messages;
    for (const ChatMessage& msg : context) {
        QJsonObject msgObj;
        msgObj["role"] = msg.role;        // "user" or "assistant"
        msgObj["content"] = msg.message;
        messages.append(msgObj);
    }
  
    // Add system prompt if needed
    QJsonObject systemMsg;
    systemMsg["role"] = "system";
    systemMsg["content"] = "You are a helpful assistant.";
    messages.prepend(systemMsg);
    
  // Send to OpenAI API...
}
```

## Styling

The widget uses modern styling with:
- Microsoft Fluent-inspired color scheme
- Smooth hover effects on buttons
- Professional typography (Segoe UI)
- Responsive layout

### Sender Color Coding

- **User messages**: Blue (#0078d4)
- **Assistant messages**: Green (#107c10)
- **System messages**: Gray (#605e5c, italic)

### Customization

You can customize the appearance by modifying the stylesheet in `createUI()` method or by applying external stylesheets to the widget.

## Thread Safety

⚠️ **Note**: This widget is not thread-safe. All UI operations must be performed on the main GUI thread. When receiving responses from async operations (network calls, AI APIs), ensure you emit signals or use `QMetaObject::invokeMethod` with `Qt::QueuedConnection` to update the UI.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Credits

**Created by**: Tian-Qing Ye  (email: tqye2006@gmail.com)  
**AI Assistant**: Claude Sonnet 4.5  
**Date**: November 25, 2025  
**Qt Version**: 5.15.2  
**C++ Standard**: C++14

## Support

For issues, questions, or contributions, please refer to the main project repository.

---

**Version**: 1.0  
**Qt Version**: 5.15.2+  
**C++ Standard**: C++14 or later
