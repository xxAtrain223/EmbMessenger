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
        name##Exception(const std::string& message="") : \
            emb::BaseException(#name ": " + message) \
        {} \
    }

#define NEW_HOST_EX(name) \
    class name##HostException : public emb::HostException \
    { \
    public: \
        name##HostException(const std::string& message="") : \
            emb::HostException(#name ": " + message) \
        {} \
    }

#define NEW_DEVICE_EX(code, name) \
    class name : public emb::DeviceException \
    { \
    public: \
        name(const std::string& message = "") : \
            emb::DeviceException(code, #name ": " + message) \
        {} \
    }

#define NEW_CODE_EX(code) NEW_DEVICE_EX(emb::k##code, code##DeviceException)

namespace emb
{
    class BaseException : public std::runtime_error
    {
    public:
        BaseException(const std::string& message) :
            std::runtime_error(message) { }
    };

    class HostException : public BaseException
    {
    public:
        HostException(const std::string& message) :
            BaseException(message) { }
    };

    class DeviceException : public BaseException
    {
    private:
        uint8_t m_error_code;

    public:
        DeviceException(const uint8_t errorCode, const std::string& message) :
            m_error_code(errorCode), BaseException(message) { }

        inline uint8_t getErrorCode() const
        {
            return m_error_code;
        }
    };

    class ParameterReadErrorHostException : public HostException
    {
    private:
        uint8_t m_parameter_index;

    public:
        ParameterReadErrorHostException(const uint8_t parameterIndex) :
            m_parameter_index(parameterIndex), HostException("Error reading parameter " + std::to_string(parameterIndex)) { }

        inline uint8_t getParameterIndex() const
        {
            return m_parameter_index;
        }
    };
    
    NEW_HOST_EX(MessageIdReadError);
    NEW_HOST_EX(MessageIdInvalid);
    NEW_HOST_EX(CrcInvalid);
    NEW_HOST_EX(ExtraParameters);

    class ParameterReadErrorDeviceException : public DeviceException
    {
    private:
        uint8_t m_parameter_index;

    public:
        ParameterReadErrorDeviceException(const uint8_t parameterIndex) :
            m_parameter_index(parameterIndex), DeviceException(DataError::kParameter0ReadError + parameterIndex, "Error reading parameter " + std::to_string(parameterIndex)) { }

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
        ParameterInvalidDeviceException(const uint8_t parameterIndex) :
            m_parameter_index(parameterIndex), DeviceException(DataError::kParameter0Invalid + parameterIndex, "Parameter " + std::to_string(parameterIndex) + " is invalid") { }

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