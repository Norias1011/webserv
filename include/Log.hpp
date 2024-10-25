#include <iostream>
#include <sstream>
#include <string>

// Définir les codes de couleurs ANSI
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

class Log {
public:
    enum LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

    static void  log(LogLevel level, const std::string &message) {
        switch (level) {
            case DEBUG:
                std::cout << BLUE << "[DEBUG] " << message << RESET << std::endl;
                break;
            case INFO:
                std::cout << GREEN << "[INFO] " << message << RESET << std::endl;
                break;
            case WARNING:
                std::cout << YELLOW << "[WARNING] " << message << RESET << std::endl;
                break;
            case ERROR:
                std::cout << RED << "[ERROR] " << message << RESET << std::endl;
                break;
        }
    }

    // Pour un seul paramètre
    template <typename T>
    static void logVar(LogLevel level, const std::string &message, T var1) {
        std::ostringstream oss;
        oss << replacePlaceholder(message, "{}") << var1;
        log(level, oss.str());
    }


private:
    // Fonction pour remplacer le premier "{}" dans le message
    static std::string replacePlaceholder(const std::string &message, const std::string &placeholder) {
        size_t pos = message.find(placeholder);
        if (pos != std::string::npos)
            return message.substr(0, pos) + message.substr(pos + placeholder.size());
        return message;
    }
};
