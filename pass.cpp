#include <QCoreApplication>
#include <QtWidgets>
#include <QFile>
#include <QTextStream>
#include <QCryptographicHash>
#include <QMessageBox>

class PasswordManager : public QWidget {
    Q_OBJECT

public:
    PasswordManager(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Password Manager");

        // Set up layout
        QVBoxLayout *layout = new QVBoxLayout(this);

        // Application password
        QLabel *appPasswordLabel = new QLabel("Enter Application Password:", this);
        layout->addWidget(appPasswordLabel);
        appPasswordInput = new QLineEdit(this);
        appPasswordInput->setEchoMode(QLineEdit::Password);
        layout->addWidget(appPasswordInput);

        // Load Button
        loadButton = new QPushButton("Load Passwords", this);
        layout->addWidget(loadButton);

        // Save Button
        saveButton = new QPushButton("Save Password", this);
        layout->addWidget(saveButton);

        // Username input
        QLabel *usernameLabel = new QLabel("Username:", this);
        layout->addWidget(usernameLabel);
        usernameInput = new QLineEdit(this);
        layout->addWidget(usernameInput);

        // Password input
        QLabel *passwordLabel = new QLabel("Password:", this);
        layout->addWidget(passwordLabel);
        passwordInput = new QLineEdit(this);
        passwordInput->setEchoMode(QLineEdit::Password);
        layout->addWidget(passwordInput);

        // Connect signals to slots
        connect(loadButton, &QPushButton::clicked, this, &PasswordManager::loadPasswords);
        connect(saveButton, &QPushButton::clicked, this, &PasswordManager::savePassword);

        // Set initial state
        loadButton->setEnabled(true);
        saveButton->setEnabled(false);
    }

private slots:
    void loadPasswords() {
        QString appPassword = appPasswordInput->text();
        if (appPassword.isEmpty()) {
            QMessageBox::warning(this, "Error", "Please enter the application password.");
            return;
        }

        QFile file("passwords.dat");
        if (!file.exists()) {
            QMessageBox::warning(this, "Error", "Password file not found.");
            return;
        }

        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, "Error", "Unable to open password file.");
            return;
        }

        QTextStream in(&file);
        QString encryptedData = in.readAll();
        QString decryptedData = decryptData(encryptedData, appPassword);

        // Split the data into lines and display them
        QStringList passwordEntries = decryptedData.split("\n", QString::SkipEmptyParts);
        for (const QString &entry : passwordEntries) {
            QStringList details = entry.split(",");
            if (details.size() == 2) {
                QString username = details[0];
                QString password = details[1];
                passwordList.append({username, password});
            }
        }

        // Enable saving passwords now
        saveButton->setEnabled(true);
    }

    void savePassword() {
        QString appPassword = appPasswordInput->text();
        if (appPassword.isEmpty()) {
            QMessageBox::warning(this, "Error", "Please enter the application password.");
            return;
        }

        // Collect the data from the fields
        QString username = usernameInput->text();
        QString password = passwordInput->text();
        if (username.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "Error", "Please enter both username and password.");
            return;
        }

        // Save the new password data
        passwordList.append({username, password});
        QString encryptedData = encryptData(passwordList);
        
        QFile file("passwords.dat");
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, "Error", "Unable to open file to save passwords.");
            return;
        }

        QTextStream out(&file);
        out << encryptedData;
        file.close();
    }

private:
    QLineEdit *appPasswordInput;
    QLineEdit *usernameInput;
    QLineEdit *passwordInput;
    QPushButton *loadButton;
    QPushButton *saveButton;

    QList<QPair<QString, QString>> passwordList;

    QString encryptData(const QList<QPair<QString, QString>> &passwordList) {
        QString data;
        for (const auto &entry : passwordList) {
            data.append(entry.first + "," + entry.second + "\n");
        }

        // Simple XOR encryption for demonstration
        QByteArray byteArray = data.toUtf8();
        for (int i = 0; i < byteArray.size(); ++i) {
            byteArray[i] ^= 0xFF;  // XOR each byte with 0xFF
        }

        return QString(byteArray);
    }

    QString decryptData(const QString &encryptedData, const QString &appPassword) {
        QByteArray byteArray = encryptedData.toUtf8();
        for (int i = 0; i < byteArray.size(); ++i) {
            byteArray[i] ^= 0xFF;  // XOR each byte with 0xFF
        }

        return QString(byteArray);
    }
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    PasswordManager w;
    w.show();
    return a.exec();
}

#include "main.moc"
