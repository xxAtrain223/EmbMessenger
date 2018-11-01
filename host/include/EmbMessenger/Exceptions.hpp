#ifndef EMBMESSENGER_EXCEPTIONS_HPP
#define EMBMESSENGER_EXCEPTIONS_HPP

#include <cstdint>
#include <stdexcept>
#include <string>
#include "EmbMessenger/DataError.hpp"

#define NEW_EX(name) \
    class name##Exception : public emb::BaseException \
    { \
    public: \
        name##Exception(const std::string& message = "", std::shared_ptr<Command> command = nullptr) : \
            emb::BaseException(#name ": " + message, command) \
        {} \
    }

#define NEW_HOST_EX(name) \
    class name##HostException : public emb::HostException \
    { \
    public: \
        name##HostException(const std::string& message = "", std::shared_ptr<Command> command = nullptr) : \
            emb::HostException(#name "HostException: " + message, command) \
        {} \
    }

#define NEW_DEVICE_EX(code, name) \
    class name : public emb::DeviceException \
    { \
    public: \
        name(const std::string& message = "", std::shared_ptr<Command> command = nullptr) : \
            emb::DeviceException(code, #name "DeviceException: " + message, command) \
        {} \
    }

#define NEW_CODE_EX(code) NEW_DEVICE_EX(emb::k##code, code##DeviceException)

namespace emb
{
    class Command;

    class BaseException : public std::runtime_error
    {
        std::shared_ptr<Command> m_command;

    public:
        BaseException(const std::string& message, std::shared_ptr<Command> command = nullptr) :
            std::runtime_error(message), m_command(command) { }

        std::shared_ptr<Command> getCommand() const
        {
            return m_command;
        }
    };

    class HostException : public BaseException
    {
    public:
        HostException(const std::string& message, std::shared_ptr<Command> command = nullptr) :
            BaseException(message, command) { }
    };

    class DeviceException : public BaseException
    {
    private:
        uint8_t m_error_code;

    public:
        DeviceException(const uint8_t errorCode, const std::string& message, std::shared_ptr<Command> command = nullptr) :
            m_error_code(errorCode), BaseException(message, command) { }

        inline uint8_t getErrorCode() const
        {
            return m_error_code;
        }
    };

    class InitializingErrorHostException : public HostException
    {
    public:
        InitializingErrorHostException(const std::string& message) :
            HostException("InitializingErrorHostException: " + message, nullptr) { }
    };

    class ParameterReadErrorHostException : public HostException
    {
    private:
        uint8_t m_parameter_index;

    public:
        ParameterReadErrorHostException(const uint8_t parameterIndex, std::shared_ptr<Command> command = nullptr) :
            m_parameter_index(parameterIndex), HostException("ParameterReadErrorHostException: Error reading parameter " + std::to_string(parameterIndex), command) { }

        inline uint8_t getParameterIndex() const
        {
            return m_parameter_index;
        }
    };
    
    NEW_HOST_EX(MessageIdReadError);
    NEW_HOST_EX(MessageIdInvalid);
    NEW_HOST_EX(CrcInvalid);
    NEW_HOST_EX(ExtraParameters);
    NEW_HOST_EX(InvalidCommandTypeIndex);

    class ParameterReadErrorDeviceException : public DeviceException
    {
    private:
        uint8_t m_parameter_index;

    public:
        ParameterReadErrorDeviceException(const uint8_t parameterIndex, std::shared_ptr<Command> command = nullptr) :
            m_parameter_index(parameterIndex), DeviceException(DataError::kParameter0ReadError + parameterIndex, "ParameterReadErrorDeviceException: Error reading parameter " + std::to_string(parameterIndex), command) { }

        inline uint8_t getParameterIndex() const
        {
            return m_parameter_index;
        }
    };

    class ParameterInvalidDeviceException : public DeviceException
    {
    private:
        uint8_t m_parameter_index;

    public:
        ParameterInvalidDeviceException(const uint8_t parameterIndex, std::shared_ptr<Command> command = nullptr) :
            m_parameter_index(parameterIndex), DeviceException(DataError::kParameter0Invalid + parameterIndex, "ParameterInvalidDeviceException: Parameter " + std::to_string(parameterIndex) + " is invalid", command) { }

        inline uint8_t getParameterIndex() const
        {
            return m_parameter_index;
        }
    };

    NEW_CODE_EX(ExtraParameters);
    NEW_CODE_EX(OutOfPeriodicCommandSlots);
    NEW_CODE_EX(MessageIdReadError);
    NEW_CODE_EX(CommandIdReadError);
    NEW_CODE_EX(CrcReadError);
    NEW_CODE_EX(MessageIdInvalid);
    NEW_CODE_EX(CommandIdInvalid);
    NEW_CODE_EX(CrcInvalid);
}

#endif // EMBMESSENGER_EXCEPTIONS_HPP