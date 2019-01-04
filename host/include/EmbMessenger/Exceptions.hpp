#ifndef EMBMESSENGER_EXCEPTIONS_HPP
#define EMBMESSENGER_EXCEPTIONS_HPP

#include <cstdint>
#include <stdexcept>
#include <string>
#include "EmbMessenger/DataError.hpp"

#define NEW_EX(name)                                                        \
    class name : public emb::host::BaseException                            \
    {                                                                       \
    public:                                                                 \
        name(const ExceptionSource source, const std::string& message = "",          \
             std::shared_ptr<emb::host::Command> command = nullptr) :       \
            emb::host::BaseException(source, #name ": " + message, command) \
        {                                                                   \
        }                                                                   \
    }

#define NEW_EX_SOURCE(name, source)                                                                    \
    class name : public emb::host::BaseException                                                       \
    {                                                                                                  \
    public:                                                                                            \
        name(const std::string& message = "", std::shared_ptr<emb::host::Command> command = nullptr) : \
            emb::host::BaseException(emb::host::ExceptionSource::source, #name ": " + message, command)         \
        {                                                                                              \
        }                                                                                              \
    }

namespace emb
{
    namespace host
    {
        class Command;

        enum class ExceptionSource
        {
            Host,
            Device
        };

        class BaseException : public std::runtime_error
        {
            std::shared_ptr<Command> m_command;
            ExceptionSource m_source;

        public:
            BaseException(const ExceptionSource source, const std::string& message, std::shared_ptr<Command> command = nullptr) :
                std::runtime_error(message),
                m_source(source),
                m_command(command)
            {
            }

            std::shared_ptr<Command> getCommand() const
            {
                return m_command;
            }

            ExceptionSource getSource() const
            {
                return m_source;
            }
        };

        class ParameterReadError : public BaseException
        {
        private:
            uint8_t m_parameter_index;

        public:
            ParameterReadError(const ExceptionSource source, const uint8_t parameterIndex,
                               std::shared_ptr<Command> command = nullptr) :
                m_parameter_index(parameterIndex),
                BaseException(source, "ParameterReadError: Error reading parameter " + std::to_string(parameterIndex),
                              command)
            {
            }

            inline uint8_t getParameterIndex() const
            {
                return m_parameter_index;
            }
        };

        class ParameterInvalid : public BaseException
        {
        private:
            uint8_t m_parameter_index;

        public:
            ParameterInvalid(const uint8_t parameterIndex, std::shared_ptr<Command> command = nullptr) :
                m_parameter_index(parameterIndex),
                BaseException(ExceptionSource::Device,
                              "ParameterInvalid: Parameter " + std::to_string(parameterIndex) + " is invalid", command)
            {
            }

            inline uint8_t getParameterIndex() const
            {
                return m_parameter_index;
            }
        };

        NEW_EX(CrcInvalid);
        NEW_EX(CrcReadError);
        NEW_EX(ExtraParameters);
        NEW_EX(MessageIdReadError);

        NEW_EX_SOURCE(InitializationError, Host);
        NEW_EX_SOURCE(InvalidCommandTypeIndex, Host);
        NEW_EX_SOURCE(MessageIdInvalid, Host);

        NEW_EX_SOURCE(CommandIdReadError, Device);
        NEW_EX_SOURCE(CommandIdInvalid, Device);
        NEW_EX_SOURCE(OutOfPeriodicCommandSlots, Device);
    }  // namespace host
}  // namespace emb

#endif  // EMBMESSENGER_EXCEPTIONS_HPP