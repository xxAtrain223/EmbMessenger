#ifndef EMBMESSENGER_EXCEPTIONS_HPP
#define EMBMESSENGER_EXCEPTIONS_HPP

#include <cstdint>
#include <stdexcept>
#include <string>
#include "EmbMessenger/DataError.hpp"

#define NEW_EMB_EX(name)                                                                                            \
    class name : public emb::host::BaseException                                                                    \
    {                                                                                                               \
    public:                                                                                                         \
        /** @brief Construct a new name exception. @param source Source of the exception @param message Message for \
            the exception @param command Command that was running when an error occurred */                         \
        name(const ExceptionSource source, const std::string& message = "",                                         \
             std::shared_ptr<emb::host::Command> command = nullptr) :                                               \
            emb::host::BaseException(source, #name ": " + message, command)                                         \
        {                                                                                                           \
        }                                                                                                           \
    };

#define NEW_EMB_EX_SOURCE(name, source)                                                                 \
    class name : public emb::host::BaseException                                                        \
    {                                                                                                   \
    public:                                                                                             \
        /** @brief Construct a new name exception. @param message Message for the exception             \
            @param command Command that was running when an error occurred */                           \
        name(const std::string& message = "", std::shared_ptr<emb::host::Command> command = nullptr) :  \
            emb::host::BaseException(emb::host::ExceptionSource::source, #name ": " + message, command) \
        {                                                                                               \
        }                                                                                               \
    };

namespace emb
{
    namespace host
    {
        class Command;

        /**
         * @brief Describes the source of the exception thrown.
         */
        enum class ExceptionSource
        {
            Host,   /// The exception originated on the host
            Device  /// The exception originated on the device
        };

        /**
         * @brief Base exception for anything that EmbMessenger will throw.
         */
        class BaseException : public std::runtime_error
        {
            std::shared_ptr<Command> m_command;
            ExceptionSource m_source;

        public:
            /**
             * @brief Construct a new Base Exception exception.
             *
             * @param source Source of the exception
             * @param message Message for the exception
             * @param command Command that was running when an error occurred
             */
            BaseException(const ExceptionSource source, const std::string& message,
                          std::shared_ptr<Command> command = nullptr) :
                std::runtime_error(message),
                m_source(source),
                m_command(command)
            {
            }

            /**
             * @brief Get the Command that was running when an error occurred.
             *
             * @return Command that was running when an error occurred
             */
            std::shared_ptr<Command> getCommand() const
            {
                return m_command;
            }

            /**
             * @brief Get the Source of the exception.
             *
             * @return Source of the exception
             */
            ExceptionSource getSource() const
            {
                return m_source;
            }
        };

        /**
         * @brief Exception for when an error occurrs while reading a parameter.
         * 
         * e.g. User tried to a float as uint8_t.
         */
        class ParameterReadError : public BaseException
        {
        private:
            uint8_t m_parameter_index;

        public:
            /**
             * @brief Construct a new Parameter Read Error exception.
             *
             * @param source Source of the exception
             * @param parameterIndex Index of the parameter that couldn't be read
             * @param command Command that was running when an error occurred
             */
            ParameterReadError(const ExceptionSource source, const uint8_t parameterIndex,
                               std::shared_ptr<Command> command = nullptr) :
                m_parameter_index(parameterIndex),
                BaseException(source, "ParameterReadError: Error reading parameter " + std::to_string(parameterIndex),
                              command)
            {
            }

            /**
             * @brief Get the index of the parameter that couldn't be read.
             *
             * @return Parameter index
             */
            inline uint8_t getParameterIndex() const
            {
                return m_parameter_index;
            }
        };

        /**
         * @brief Exception for when parameter is invalid
         * 
         * e.g. parameter is out of bounds, `[](int16_t val){ return val > -16 && val < 128; }`, where the parameter is 255.
         */
        class ParameterInvalid : public BaseException
        {
        private:
            uint8_t m_parameter_index;

        public:
            /**
             * @brief Construct a new Parameter Invalid object
             *
             * @param parameterIndex Index of the parameter that couldn't be read
             * @param command Command that was running when an error occurred
             */
            ParameterInvalid(const uint8_t parameterIndex, std::shared_ptr<Command> command = nullptr) :
                m_parameter_index(parameterIndex),
                BaseException(ExceptionSource::Device,
                              "ParameterInvalid: Parameter " + std::to_string(parameterIndex) + " is invalid", command)
            {
            }

            /**
             * @brief Get the index of the parameter that couldn't be read.
             *
             * @return Parameter index
             */
            inline uint8_t getParameterIndex() const
            {
                return m_parameter_index;
            }
        };

        /**
         * @brief Exception for CRC Read Error.
         * 
         * In theory this should never be thrown.
         * The CRC should only be read after all parameters have been read.
         * So if the CRC is expected, then you have extra parameters.
         */
        NEW_EMB_EX(CrcReadError);

        /**
         * @brief Exception for an invalid CRC.
         * 
         * One or more bytes got corrupted in transit.
         */
        NEW_EMB_EX(CrcInvalid);

        /**
         * @brief Exception for Message ID Read Error.
         * 
         * An error occurred when reading the message ID.
         */
        NEW_EMB_EX(MessageIdReadError);

        /**
         * @brief Exception for Extra Parameters.
         * 
         * Not all parameters were read.
         */
        NEW_EMB_EX(ExtraParameters);

        /**
         * @brief Exception for an Initialization Error.
         * 
         * e.g. Timed out while establishing a connection.
         */
        NEW_EMB_EX_SOURCE(InitializationError, Host);

        /**
         * @brief Exception for an Invalid Command Type Index.
         * 
         * Occurs when a command gets sent using the nontemplate version of EmbMessenger::send and the command did not set Command::m_type_index.
         */
        NEW_EMB_EX_SOURCE(InvalidCommandTypeIndex, Host);

        /**
         * @brief Exception for an Invalid Message ID.
         * 
         * A message sent by the device doesn't have a command to receive it.
         */
        NEW_EMB_EX_SOURCE(MessageIdInvalid, Host);

        /**
         * @brief Exception for an Unregistered Command.
         * 
         * Ensure the Command Type is registered with the EmbMessenger.
         */
        NEW_EMB_EX_SOURCE(UnregisteredCommand, Host);

        /**
         * @brief Exception for Command ID Read Error.
         * 
         * An error occurred while reading the Command ID.
         */
        NEW_EMB_EX_SOURCE(CommandIdReadError, Device);

        /**
         * @brief Exception for an Invalid Command ID.
         * 
         * Occurs when a command is not registered on the device.
         */
        NEW_EMB_EX_SOURCE(CommandIdInvalid, Device);

        /**
         * @brief Exception for when the device is Out of Periodic Command Slots.
         * 
         * Occurs when too many periodic commands are registered.
         */
        NEW_EMB_EX_SOURCE(OutOfPeriodicCommandSlots, Device);
    }  // namespace host
}  // namespace emb

#endif  // EMBMESSENGER_EXCEPTIONS_HPP